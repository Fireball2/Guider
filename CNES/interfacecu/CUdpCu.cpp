/*! \file CUdpCu.cpp
 *  \brief Implémentation de la classe relative au protocole de communication
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
/*! Cette méthode effectue la construction de l'instance.
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
/*! Cette méthode effectue la destruction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CUdpCu::~CUdpCu()
{
	// Rien à faire
}

/*----------------------------------------------------------------------------*/
/* SetChecksumType                                                            */
/*----------------------------------------------------------------------------*/
/*! Cette méthode permet de sélectionner le type de checksum à utiliser
 *
 *  \param       [in] uiChecksumtype : type de checksum utilisée

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
/*! Cette méthode est appelée sur réception d'un message par la classe de
 *  base CUdp.
 *
 *  Traitements effectués :
 *  - vérification de la cohérence du message reçu (taille, checksum)
 *  - vérification du compteur de trame
 *  - transmission du message à la classe utilisatrice
 *
 *  \param       [in] iCnx : Identifiant fourni par le ReceivePendingMsg de CUdp
 *  \param       [in] pBuffer : Buffer contenant la trame recue
 *  \param       [in] iSize : Taille de la trame recue
 *  \warning     Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
void CUdpCu::IReceiveMsgExt( int iCnx, char * pBuffer, int iSize )
{
    unsigned short usCalculatedChecksum = 0;  //Checksum calculé
    
    unsigned short usReadChecksum = 0;        //Checksum reçu
    
    unsigned int uiFrameSize = 0;             // Taille de trame calculée

    eIcuProtoFrame * pFrame = NULL; // Pointeur sur le contenu de la trame (hors checksum)
    
    // Taille du checksum (1 ou 2 octets)
    unsigned int uiChecksumSize = m_Checksum.GetChecksumSize();

    // Annulation de l'incrémentation effectuée par la méthode appelante de
    // réception de la trame UDP
    m_uiNbRecvOk--;

    // On vérifie que la taille du buffer est moins égale à celle du header
    if( iSize < (int) sizeof( pFrame->Header ) )
    {
        // Incrémentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // Extraction de la trame (hors checksum)
    pFrame = (eIcuProtoFrame *) pBuffer;

    // Calcul de la taille théorique de la trame (hors checksum)
    uiFrameSize = sizeof( pFrame->Header ) + pFrame->Header.usMsgSize;

    // On vérifie que la taille annoncée n'excède pas la taille limite
    if( pFrame->Header.usMsgSize > SIZE_ICU_MSG_MAX )
    {
        // Incrémentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // On vérifie que la taille du buffer est cohérente
    if( (unsigned int) iSize != uiFrameSize + uiChecksumSize )
    {
        // Incrémentation du compteur de trames invalides
        m_uiNbRecvNok++;

        return;
    }

    // Extraction du checksum reçu
    if( uiChecksumSize <= sizeof( usReadChecksum ) )
    {
        memcpy( (void *) &usReadChecksum,
                (void *) &pBuffer[iSize - uiChecksumSize], uiChecksumSize );
    }

    // Calcul de la valeur théorique du checkum
    usCalculatedChecksum = m_Checksum.Calculate(
            (unsigned char *) pFrame, uiFrameSize );

    // On vérifie que le checkum reçu est cohérent
    if( usCalculatedChecksum != usReadChecksum )
    {
#ifdef CUDPCU_DEBUG
        printf("Erreur checksum, attendu %02hX\n", usCalculatedChecksum);
#endif

        // Incrémentation du compteur de trames invalides
        m_uiNbRecvNok++;

        // Incrémentation du compteur de checksums invalides
        m_uiNbCheckSumErr++;

        return;
    }

    // On vérifie que le compteur de trame est celui attendu
    if( pFrame->Header.usFrameCounter != m_usFrameCounter )
    {
        // Incrémentation du compteur de trames perdues
        m_uiNbLostMsg++;

        // Rectification de la valeur du compteur de messages
        m_usFrameCounter = pFrame->Header.usFrameCounter;
    }

    // Incrémentation du compteur interne de trames
    m_usFrameCounter++;

    // Incrémentation du compteur de trames sans erreurs
    m_uiNbRecvOk++;

    // Envoi du message à l'interface de la classe utilisatrice
    m_pIReceiveProtMsg->IReceiveMsgExt( iCnx, pBuffer, iSize );
}

/*----------------------------------------------------------------------------*/
/* InstallReceiveHandler                                                      */
/*----------------------------------------------------------------------------*/
/*! Cette méthode permet d'nstaller le mécanisme de notification de réception
 *  des messages.
 *
 *  \param       [in] iSizeMsgMax : Taille du buffer de réception.
 *  \param       [in] sigEvent : Evenement de notification.
 *  \param       [in] pIReceiveProtMsg : Interface de notification
 *  \warning     Exception levee si un probleme survient
 */
/*----------------------------------------------------------------------------*/
void CUdpCu::InstallReceiveHandler( int iSizeMsgMax,
                                  const struct sigevent* sigEvent,
                                  IReceiveProtMsg *pIReceiveProtMsg )
{
    //On mémorise la structure
    m_pIReceiveProtMsg = pIReceiveProtMsg;
    
    //On appelle la méthode de la classe de base
    //La structure IReceiveProtMsg envoyée est l'instance de la classe CUdpCu
    CUdp::InstallReceiveHandler( iSizeMsgMax, sigEvent, this );
}
