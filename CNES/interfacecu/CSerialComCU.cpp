/*! \file CSerialComCU.cpp
 *  \brief Impl�mentation de la classe relative au protocole de communication
 *  s�rie de la charge utile.
 */

// Headers C
#include <errno.h>
#include <sys/uio.h>
#include <fcntl.h>

// Headers Syslay
#include <CLog.h>

// Headers locaux
#include "CSerialComCU.h"

/*! \addtogroup interfacecu */
/* @{ */

// /@cond COMICU_DEBUG
/*! \def COMICU_DEBUG
 *    S�l�cteur pour activer ou inhiber l'affichage de trace sur la console
 */
// /@endcond
//#define COMICU_DEBUG

/*!
 *   Portee : classe CSerialComCU\n
 *   Premier octet de synchronisation
 */ 
const char OCTET_SYNCHRO_START = 0x55;

/*!
 *   Portee : classe CSerialComCU\n
 *   Deuxi�me octet de synchronisation
 */ 
const char OCTET_SYNCHRO_END = 0xAA;

/* @} */


/******************************************************************************/
/*                                                                            */
/*                          METHODES PUBLIC                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CSerialComCU                                                               */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la construction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CSerialComCU::CSerialComCU() : CSerialCom()
{
    // Etat initial du parseur de trames
    m_uiState = STATE_1_SYNC_START;

    // Intialisation du compteur de trames re�ues valides ou non
    m_usFrameCounter = 0;

    // Initialisation des variables de travail du parseur
    m_iNbBytesToRead = 0;
    m_iReadIndex = 0;

    //On initialise les donn�es � lire dans chaque message
    m_sReadChecksum = 0;
}

/*----------------------------------------------------------------------------*/
/* ~CSerialComCU                                                              */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la destruction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CSerialComCU::~CSerialComCU()
{
	// Rien � faire
}

/*----------------------------------------------------------------------------*/
/* ReceivePendingMsg                                                          */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception de donn�es sur la liaison serie.
 *
 *  Elle appelle la methode de base pour signaler que tous les octets vont etre
 *  lus puis lit le flux d'octets afin d'extraire le message.
 *
 *   La lecture fonctionne sur le principe d'une machine � �tat :
 *       -Etat 1 : On attend le premier octet de synchronisation
 *       -Etat 2 : On lit le header de la trame qui contient la taille du message
 *                 l'identifiant du message et le compteur de trame.
 *       -Etat 3 : On lit le contenu du message (autant d'octets qu'indiqu�s par
 *                 le header)
 *       -Etat 4 : On lit le checksum (1 ou 2 octets selon le type de checksum)
 *       -Etat 5 : On lit l'octet de synchronisation de fin de trame, puis on
 *                 v�rifie la validit� de la trame
 *       -Etat 6 : On transite dans cet �tat lorsqu'une anomalie se pr�sente
 *                 au cours de l'un des �tats pr�c�dent.
 *
 *   \param       [in] iCnx : Identifiant de connexion
 */
/*----------------------------------------------------------------------------*/        
void CSerialComCU::ReceivePendingMsg( int iCnx )
{
    const int ZERO_BYTE = 0; // Constante utilisee pour verifier s'il n'y a
                              // plus de donnees � lire

    const int ONE_BYTE = 1;  // Constante utilis�e pour r�cup�rer un caract�re
    
    int iNbBytes = ONE_BYTE;     //Variable indiquant combien d'octets ont ete lus

    char cReadByte = 0; // Variable utilisee pour stocker un octet lu
    
    short sCalculatedChecksum = 0;    //Checksum calculee
    
    // Appel de la fonction permettant de lire le port serie
    CSerialCom::ReceivePendingMsg( iCnx );
    
    // On boucle tant que des octets sont disponibles
    while( iNbBytes != ZERO_BYTE )
    {
        switch( m_uiState )
        {
            // Etat 1 : Attente de l'octet de synchronisation
            case STATE_1_SYNC_START:
            {
                // On tente de lire un octet
                iNbBytes = Read( &cReadByte, ONE_BYTE );
                
                if( iNbBytes != ONE_BYTE )
                {
                    break;
                }

                // S'il s'agit de l'octet de synchro, on passe � l'�tat suivant
                if( cReadByte == OCTET_SYNCHRO_START )
                {
                    m_iNbBytesToRead = sizeof( eIcuProtoHeader );
                    m_iReadIndex = 0;
                    m_uiState = STATE_2_HEADER;
                }
                
                break;
            }
            
            // Etat 2 : Lecture du header
            case STATE_2_HEADER:
            {
                // Pointeur sur la variable de stockage du header
                char * pcBuffer = (char*) &m_ReadFrame.Header;

                // On lit les octets jusqu'� ce que le header soit complet
                iNbBytes = Read( &pcBuffer[m_iReadIndex], m_iNbBytesToRead );

                if( iNbBytes == m_iNbBytesToRead )
                {
                    // V�rification de la coh�rence de la taille annonc�e,
                    // sachant qu'une taille nulle est valide
                    if( m_ReadFrame.Header.usMsgSize <= SIZE_ICU_MSG_MAX )
                    {
                        m_iNbBytesToRead = m_ReadFrame.Header.usMsgSize;
                        m_iReadIndex = 0;
                        m_uiState = STATE_3_MESSAGE;
                    }
                    else
                    {
                        m_uiState = STATE_6_ERROR;
                    }
                }
                else
                {
                    m_iNbBytesToRead -= iNbBytes;
                    m_iReadIndex += iNbBytes;
                }
                
                break;
            }
            
            // Etat 3 : Lecture du contenu du message
            case STATE_3_MESSAGE:
            {
                // On lit les octets jusqu'� ce que le header soit complet
                iNbBytes = Read( &m_ReadFrame.tcMsg[m_iReadIndex], m_iNbBytesToRead );

                if( iNbBytes == m_iNbBytesToRead )
                {
                    // Le nombre d'octets � lire d�pend du type de checksum
                    m_iNbBytesToRead = m_Checksum.GetChecksumSize();
                    m_iReadIndex = 0;
                    m_uiState = STATE_4_CHECKSUM;
                }
                else
                {
                    m_iNbBytesToRead -= iNbBytes;
                    m_iReadIndex += iNbBytes;
                }

                break;
            }
            
            // Etat 4 : Lecture du checksum
            case STATE_4_CHECKSUM:
            {
                // Pointeur sur la variable de stockage du checksum
                char * pcBuffer = (char *) &m_sReadChecksum;

                // On lit jusqu'� obtention de l'unique ou des deux octets de checksum
                iNbBytes = Read( &pcBuffer[m_iReadIndex], m_iNbBytesToRead );
                
                if( iNbBytes == m_iNbBytesToRead  )
                {
                    m_uiState = STATE_5_SYNC_END;
                }
                else
                {
                    m_iNbBytesToRead -= iNbBytes;
                    m_iReadIndex += iNbBytes;
                }
                
                break;
            }
            
            //Etat 5 : Lecture de l'octet de synchronisation terminal
            case STATE_5_SYNC_END:
            {
                //On lit le deuxi�me octet de synchro
                iNbBytes = Read( &cReadByte, ONE_BYTE );

                if( iNbBytes != ONE_BYTE )
                {
                    break;
                }

                // On v�rifie si le deuxi�me octet de synchro est bien pr�sent
                if( cReadByte != OCTET_SYNCHRO_END )
                {
                    m_uiState = STATE_6_ERROR;
                }
                else
                {
                    // Calcul du checksum attendu
                    sCalculatedChecksum = m_Checksum.Calculate(
                        (unsigned char *) &m_ReadFrame,
                        sizeof( m_ReadFrame.Header) + m_ReadFrame.Header.usMsgSize );

#ifdef COMICU_DEBUG
                    printf("checksum calcul = %02hX\n", sCalculatedChecksum);
                    printf("checksum lu     = %02hX\n", m_sReadChecksum);
#endif

                    // V�rification du checksum re�u
                    if( sCalculatedChecksum != m_sReadChecksum )
                    {
                        // Incr�mentation du nombre d'erreur de checksum
                        m_uiNbCheckSumErr++;

                        m_uiState = STATE_6_ERROR;
                    }
                    // V�rification du compteur
                    else
                    {
                        if( m_ReadFrame.Header.usFrameCounter != m_usFrameCounter )
                        {
                            // Incr�mentation du nombre de messages perdus
                            m_uiNbLostMsg++;

                            // R�synchronisation du compteur interne
                            m_usFrameCounter = m_ReadFrame.Header.usFrameCounter;
                        }

                        // Incr�mentation du compteur de messages valides
                        m_uiNbRecvOk++;
                        
                        // Envoi du message au driver
                        RecvSerialMsg( iCnx, (char *) &m_ReadFrame,
                                sizeof( m_ReadFrame.Header ) +
                                m_ReadFrame.Header.usMsgSize );

                        // Incr�mentation du compteur interne de trames
                        m_usFrameCounter++;

                        m_uiState = STATE_1_SYNC_START;
                    }
                }
                
                break;
            }

            //Etat 6 : Etat transitoire lors d'une erreur de synchonisation
            case STATE_6_ERROR:
            {
                // Incr�mentation du nombre de trames invalides
                m_uiNbRecvNok++;

                m_uiState = STATE_1_SYNC_START;
                break;
            }


            default:
                break;
        }
    }
}

/*----------------------------------------------------------------------------*/
/* SetChecksumType                                                            */
/*----------------------------------------------------------------------------*/
/*! Reinitialise les valeurs de debug.
 *
 *  \param       [in] uiChecksumType
 */
/*----------------------------------------------------------------------------*/
void CSerialComCU::SetChecksumType( unsigned int uiChecksumType )
{

    //On indique le type de checksum au calculateur
    m_Checksum.SetType( uiChecksumType );
}

/*----------------------------------------------------------------------------*/
/* Send                                                                       */
/*----------------------------------------------------------------------------*/
/*! Effectue une �mission d'un tableau d'octets sur le port serie.
 *
 *  La m�thode surcharge la m�thode virtuelle de la classe m�re CCnx.
 *
 *  Les octets de synchronisation de d�but et de fin pr�vus par le procotocole
 *  ICU sont ajout�s de part et d'autre des donn�es.
 *
 *  \param       [in] pcData : Tableau d'octets a emettre.
 *  \param       [in] iSize : Nombre d'octets dans le tableau.
 *  \warning     Exception lev�e si un probleme survient
 */
/*----------------------------------------------------------------------------*/
void CSerialComCU::Send( const char* pcData , int iSize )
{
    // Buffer utilis� pour contenir une trame munie des deux octets de synchro
    char tcSerialData[ sizeof( eIcuProtoFrame ) + 2 * sizeof( char ) ];

    // Nombre d'octets � �mettre
    int iSerialDataSize = 0;

    // On �crit le 1er octet de synchronisation
    tcSerialData[iSerialDataSize] = OCTET_SYNCHRO_START;
    iSerialDataSize = sizeof( char );

    // On copie les octets contenus dans la trame
    memcpy( &tcSerialData[iSerialDataSize], pcData, iSize );
    iSerialDataSize += iSize;

    // On �crit le 2�me octet de synchronisation
    tcSerialData[iSerialDataSize] = OCTET_SYNCHRO_END;
    iSerialDataSize += sizeof( char );

    // On envoi le message
    CSerialCom::Send( tcSerialData, iSerialDataSize );

    // Incr�mentation du nombre de messages envoy�s
    m_uiNbSendOk++;
}
