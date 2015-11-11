/*! \file CSerialComCU.h
 *  \brief Specification de la classe relative au protocole de communication
 *  série de la charge utile.
 */

#ifndef _CSERIALCOMCU_H_
#define _CSERIALCOMCU_H_

// Headers Syslay
#include <CSerialCom.h>

// Headers locaux
#include "ProtoIcu.h"
#include "CChecksum.h"

/*! \addtogroup interfacecu */
/* @{ */

/*! \class CSerialComCU
 *  \brief Classe utilisee pour implementer le protocole de communication serie
 *  de la charge utile.
 *
 *  La classe CSerialComCU est utilisee pour mettre en place le protocole de
 *  synchronisation afin de permettre de delimiter explicitement les messages
 *  transmis dans le flux d'octets.
 * 
 *  La classe herite de la classe CSerialCom pour surcharger et utiliser les
 *  methodes de communication a travers un lien serie.
 */
class CSerialComCU : public CSerialCom
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/    

public:

protected:

private:

    /*!
     *   Defini les differents etats de la machine a etats utilisee pour
     *   traiter de la synchronisation de la reception du message.
     */
    enum
    {
        STATE_1_SYNC_START,
        /*!< Etat de réception du premier octet de synchronisation. */
        STATE_2_HEADER,
        /*!< Etat de réception du header. */
        STATE_3_MESSAGE,
        /*!< Etat de reception du message. */
        STATE_4_CHECKSUM,
        /*!< Etat de réception du checksum */
        STATE_5_SYNC_END,
        /*!< Etat de réception du dernier octet de synchronisation */
        STATE_6_ERROR,
        /*!< Etat de coontr du dernier octet de synchronisation */

    };

    /*!
     *    Variable de la machine à état de la réception
     */
    unsigned int m_uiState;

    /*!
     *    Variable comptabilisant les trames reçues
     */
    unsigned short m_usFrameCounter;

    /*!
     *    Variable donnant le nombre d'octets à lire
     */
    int m_iNbBytesToRead;

    /*!
     *    Variable indiquant le nombre courant d'octets lus
     */
    int m_iReadIndex;

    /*!
     *    Variable de stockage du checksum lu
     */
    short m_sReadChecksum;

    /*!
     *    Variable de stockage des informations de trame lues
     */
    eIcuProtoFrame m_ReadFrame;

    /*!
     *   Calculateur de checksum
     */
    CChecksum m_Checksum;

    /*------------------------------------------------------------------------*/
    /* Méthodes d'instance                                                    */
    /*------------------------------------------------------------------------*/            
    
public:

    /*!
     *   \brief Constructeur de la classe.
     */
    CSerialComCU();

    /*!
     *   \brief Destructeur de la classe.
     */
    virtual ~CSerialComCU();
        
    /*!
     *   \brief Indique la presence d'octets a lire.
     */
    void ReceivePendingMsg( int iCnx );

    /*!
     *   \brief Permet de sélectionner le type de checksum à utiliser
     */
    void SetChecksumType( unsigned int uiChecksumType );

    /*!
     *   \brief Permet l'émission d'un message vers la charge utile
     */
    void Send( const char* pcData, int iSize );


protected:

private:

};

/* @} */

#endif //_CSERIALCOMCU_H_
