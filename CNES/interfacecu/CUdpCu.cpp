/*! \file CUdpCu.cpp
 *  \brief Impl�mentation de la classe relative au protocole de communication
 *  UDP de la charge utile.
 */
 
// Headers locaux
#include "CUdpCu.h"

/*! \addtogroup interfacecu */
/* @{ */

// /@cond CUDPCU_DEBUG
/*! \def CUDPCU_DEBUG
 *    Selecteur pour activer ou inhiber l'affichage de trace sur la console
 */
// /@endcond
//#define CUDPCU_DEBUG

/* @} */

/******************************************************************************/
/*                                                                            */
/*                          METHODES PUBLIC                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CUdpCu                                                                     */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la construction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CUdpCu::CUdpCu() : CUdp()
{
    m_pIReceiveProtMsg = NULL;
    m_usFrameCounter = 0;
}

/*----------------------------------------------------------------------------*/
/* ~CUdpCu                                                                    */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la destruction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CUdpCu::~CUdpCu()
{
	// Rien � faire
}

/*----------------------------------------------------------------------------*/
/* SetChecksumType                                                            */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode permet de s�lectionner le type de checksum � utiliser
 *
 *  \param       [in] uiChecksumtype : type de checksum utilis�e

 */
/*----------------------------------------------------------------------------*/
void CUdpCu::SetChecksumType( unsigned int uiChecksumtype )
{
    //On indique le type de checksum au calculateur
    m_Checksum.SetType( uiChecksumtype );
}

/*----------------------------------------------------------------------------*/
/* IReceiveMsgExt                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception d'un message par la classe de
 *  base CUdp.
 *
 *  Traitements effectu�s :
 *  - v�rification de la coh�rence du message re�u (taille, checksum)
 *  - v�rification du compteur de trame
 *  - transmission du message � la classe utilisatrice
 *
 *  \param       [in] iCnx : Identifiant fourni par le ReceivePendingMsg de CUdp
 *  \param       [in] pBuffer : Buffer contenant la trame recue
 *  \param       [in] iSize : Taille de la trame recue
 *  \warning     Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
void CUdpCu::IReceiveMsgExt( int iCnx, char * pBuffer, int iSize )
{
    unsigned short usCalculatedChecksum = 0;  //Checksum calcul�
    
    unsigned short usReadChecksum = 0;        //Checksum re�u
    
    unsigned int uiFrameSize = 0;             // Taille de trame calcul�e

    eIcuProtoFrame * pFrame = NULL; // Pointeur sur le contenu de la trame (hors checksum)
    
    // Taille du checksum (1 ou 2 octets)
    unsigned int uiChecksumSize = m_Checksum.GetChecksumSize();

    // Annulation de l'incr�mentation effectu�e par la m�thode appelante de
    // r�ception de la trame UDP
    m_uiNbRecvOk--;

    // On v�rifie que la taille du buffer est moins �gale � celle du header
    if( iSize < (int) sizeof( pFrame->Header ) )
    {
        // Incr�mentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // Extraction de la trame (hors checksum)
    pFrame = (eIcuProtoFrame *) pBuffer;

    // Calcul de la taille th�orique de la trame (hors checksum)
    uiFrameSize = sizeof( pFrame->Header ) + pFrame->Header.usMsgSize;

    // On v�rifie que la taille annonc�e n'exc�de pas la taille limite
    if( pFrame->Header.usMsgSize > SIZE_ICU_MSG_MAX )
    {
        // Incr�mentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // On v�rifie que la taille du buffer est coh�rente
    if( (unsigned int) iSize != uiFrameSize + uiChecksumSize )
    {
        // Incr�mentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // Extraction du checksum re�u
    if( uiChecksumSize <= sizeof( usReadChecksum ) )
    {
        memcpy( (void *) &usReadChecksum,
                (void *) &pBuffer[iSize - uiChecksumSize], uiChecksumSize );
    }

    // Calcul de la valeur th�orique du checkum
    usCalculatedChecksum = m_Checksum.Calculate(
            (unsigned char *) pFrame, uiFrameSize );

    // On v�rifie que le checkum re�u est coh�rent
    if( usCalculatedChecksum != usReadChecksum )
    {
#ifdef CUDPCU_DEBUG
        printf("Erreur checksum, attendu %02hX\n", usCalculatedChecksum);
#endif

        // Incr�mentation du compteur de trames invalides
        m_uiNbRecvNok++;

        // Incr�mentation du compteur de checksums invalides
        m_uiNbCheckSumErr++;

        return;
    }

    // On v�rifie que le compteur de trame est celui attendu
    if( pFrame->Header.usFrameCounter != m_usFrameCounter )
    {
        // Incr�mentation du compteur de trames perdues
        m_uiNbLostMsg++;

        // Rectification de la valeur du compteur de messages
        m_usFrameCounter = pFrame->Header.usFrameCounter;
    }

    // Incr�mentation du compteur interne de trames
    m_usFrameCounter++;

    // Incr�mentation du compteur de trames sans erreurs
    m_uiNbRecvOk++;

    // Envoi du message � l'interface de la classe utilisatrice
    m_pIReceiveProtMsg->IReceiveMsgExt( iCnx, pBuffer, iSize );
}

/*----------------------------------------------------------------------------*/
/* InstallReceiveHandler                                                      */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode permet d'nstaller le m�canisme de notification de r�ception
 *  des messages.
 *
 *  \param       [in] iSizeMsgMax : Taille du buffer de r�ception.
 *  \param       [in] sigEvent : Evenement de notification.
 *  \param       [in] pIReceiveProtMsg : Interface de notification
 *  \warning     Exception levee si un probleme survient
 */
/*----------------------------------------------------------------------------*/
void CUdpCu::InstallReceiveHandler( int iSizeMsgMax,
                                  const struct sigevent* sigEvent,
                                  IReceiveProtMsg *pIReceiveProtMsg )
{
    //On m�morise la structure
    m_pIReceiveProtMsg = pIReceiveProtMsg;
    
    //On appelle la m�thode de la classe de base
    //La structure IReceiveProtMsg envoy�e est l'instance de la classe CUdpCu
    CUdp::InstallReceiveHandler( iSizeMsgMax, sigEvent, this );
}
