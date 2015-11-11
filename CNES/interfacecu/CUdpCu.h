/*! \file CUdpCu.h
 *  \brief Sp�cification de la classe relative au protocole de communication
 *  UDP de la charge utile.
 */

#ifndef _CUDPCU_H_
#define _CUDPCU_H_

// Headers Syslay
#include <CUdp.h>

// Headers locaux
#include "ProtoIcu.h"
#include "CChecksum.h"


/*! \addtogroup interfacecu */
/* @{ */

/*! \class CUdpCu
 *  \brief Classe utilis�e pour impl�menter le protocole de communication udp
 *  de la charge utile.
 * 
 *  La classe h�rite de la classe CUdp pour surcharger et utiliser les
 *  m�thodes de communication � travers un lien udp.
 */
class CUdpCu : public CUdp, public IReceiveProtMsg
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/    

public:

protected:

private:
    
    /*!
     *  Interface de notification utilisee pour la reception des messages.
     */
    IReceiveProtMsg * m_pIReceiveProtMsg;

    /*!
     *  Variable comptabilisant les trames re�ues
     */
    unsigned short m_usFrameCounter;

    /*!
     *  Calculateur de checksum
     */
    CChecksum m_Checksum;
    
    /*------------------------------------------------------------------------*/
    /* Methodes d'instance                                                    */
    /*------------------------------------------------------------------------*/            
    
public:

    /*!
     *   \brief Constructeur de la classe.
     */
    CUdpCu();

    /*!
     *   \brief Destructeur de la classe.
     */
    virtual ~CUdpCu();

    /*!
     *   \brief Permet de s�lectionner le type de checksum � utiliser
     */
    void SetChecksumType( unsigned int uiChecksumtype );

    /*!
     *   \brief R�ception d'un message issu de la connexion
     */
    void IReceiveMsgExt( int iCnx, char * pBuffer, int iSize );
    
    /*!
     *   \brief Installe le m�canisme de notification de reception de messages
     *   utilise lorsque la socket fonctionne en mode non bloquant.
     */
    void InstallReceiveHandler( int iSizeMsgMax,
                                const struct sigevent * sigEvent,
                                IReceiveProtMsg * pIReceiveProtMsg );

protected:

private:

};

/* @} */

#endif //_CUDPCU_H_
