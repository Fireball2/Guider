/*! \file CSerialComProto.cpp
 *  \brief Implementation de la classe relative au protocole de communication
 *  serie avec l'ASC DTU.
 */
 
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/12/11  
//	- Prise en compte de BL-FI-2315
//	- Modification de la methode CSerialComProto()
//					
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//  - Prise en compte de BL-DM-2474
//      - Remise à niveau de la documentation Doxygen
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 25/02/14
//  Auteur : Rémi THONNERIEUX - CIO
//  - Prise en compte de BL-FI-5577
//      - Modification de la methode ReceivePendingMsg()
////////////////////////////////////////////////////////////////////////////////
 
#include "CSerialComProto.h"
#include "CLog.h"
#include <gulliver.h>

/*! \addtogroup ascdtu */
/* @{ */


//#define CSERIALCOMPROTO_DEBUG
/// @cond CSERIALCOMPROTO_DEBUG 
/*! \def CSERIALCOMPROTO_DEBUG
 *    Selecteur pour activer ou inhiber l'affichage de traces sur la console
 */
/// @endcond

//#define CSERIALCOMPROTO_ERR_DEBUG
/// @cond CSERIALCOMPROTO_ERR_DEBUG 
/*! \def CSERIALCOMPROTO_ERR_DEBUG
 *    Selecteur pour activer ou inhiber l'affichage d'erreur sur la console
 */			  
/// @endcond

//#define CSERIALCOMPROTO_TIME_DEBUG
/// @cond CSERIALCOMPROTO_TIME_DEBUG
/*! \def CSERIALCOMPROTO_TIME_DEBUG
 *    Selecteur pour activer ou inhiber l'affichage de traces liees a la datation
 *    sur la console
 */
/// @endcond		  

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir le premier octet de synchronisation
 *   d'une telemesure
 */
const unsigned char PACKET_TM_ID_LOW = 0x0F;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir le premier octet de synchronisation
 *   d'une telecommande
 */
const unsigned char PACKET_TC_ID_LOW = 0x1F;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir le deuxieme octet de synchronisation
 */
const unsigned char PACKET_ID_HIGH =0x91;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir la valeur de la version PU pour une
 *   telemesure
 */
const unsigned char PUS_VERSION_TM = 0x10;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir la valeur de la version PU pour une
 *   telecommande
 */
const unsigned char PUS_VERSION_TC = 0x19;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir la valeur de la segmentation
 */
const unsigned short SEGMENTATION_VALUE = 0xC000;
		
/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir la valeur maximale du compteur
 */
const unsigned short FRAME_COUNT_MAX = 0x3FF;		

/* @} */


/******************************************************************************/
/*																			  */
/*							METHODES PUBLIC 								  */
/*																			  */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* CSerialComProto			                                                  */
/*----------------------------------------------------------------------------*/
/*! Effectue l'initialisation des donnees de la classe.
 *
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/12/11  
//	- BL-FI-2315 : - Initialisation de la variable m_usTcFrameCount
//					
////////////////////////////////////////////////////////////////////////////////
CSerialComProto::CSerialComProto() : CSerialCom()
{	
	//Constante utilisee pour définir la valeur d'un octet spare
	const char SPARE_VALUE = 0x00;
	
	//Par défaut, l'état initial est l'état de réception des données.
	m_uiState = STATE_1;
	m_iPtrBuffer = 0;
	
	//Par défaut, la synchronisation n'est pas activée.
	m_bSync = false;
		
	//Initialisation du compteur de sequence en TM
	m_usTmFrameCount = 0;
	
	//Initialisation du compteur de sequence en TC
	m_usTcFrameCount = 0;	
	
	//Initialise la table de calcul du CRC.
	InitCrcTable();
	
	//Preinitialisation de l'entete d'une trame a emetrre au DTU
	m_pucBufferSend.sHeader.cPacketIDLow = PACKET_TC_ID_LOW;
	m_pucBufferSend.sHeader.cPacketIDHigh = PACKET_ID_HIGH;
	m_pucBufferSend.sTcData.sTcHeader.cPusVersion = PUS_VERSION_TC;
	m_pucBufferSend.sTcData.sTcHeader.cSpare = SPARE_VALUE;	
}

/*----------------------------------------------------------------------------*/
/* ~CSerialComProto			                                                  */
/*----------------------------------------------------------------------------*/
/*! Pas de traitement.
 *
 */
/*----------------------------------------------------------------------------*/
CSerialComProto::~CSerialComProto()
{
}

/*----------------------------------------------------------------------------*/
/* ReceivePendingMsg                                               	  		  */
/*----------------------------------------------------------------------------*/
/*!  Notifie la presence d'octets a lire sur la liaison serie.
* 
*   Elle appelle la methode de base pour signaler que tous les octets vont etre
*   lus puis lit le flux d'octets afin d'extraire les messages. Chaque message
*   extrait est transmis a la couche application par appel de la methode
*   RecvSerialMsg();
* 
*	\param       [in] iCnx : Identifiant de connexion
*/		
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 25/02/14  
//	- BL-FI-5577 : - Ajout de l'incrementation du compteur m_uiNbLostMsg lors
//	de la detection d'un desequencement
//					
////////////////////////////////////////////////////////////////////////////////		
void CSerialComProto::ReceivePendingMsg( int iCnx )
{		
	const int ONE_BYTE = 1;	//Constante utilisee pour lire un seul octet
		
	const int ZERO_BYTE = 0;	//Constante utilisee pour verifier s'il n'y a
								//plus de donnees à lire
								
	//Constante utilisee pour lire le nombre d'octets restants afin de recuperer
	//tous les headers.
	const int NB_BYTE_FOR_ALL_HEADER = 14; 
	
	//Constante utilisee pour recuperer la valeur du compteur
	const unsigned short FRAME_COUNT_MASK = 0x3FF;
	
	//Constante utilisee pour recuperer la valeur de la segmentation
	const unsigned short SEGMENTATION_MASK = 0xC000;
				
	int iSize = ONE_BYTE; //Variable utilisee pour contenir le nombre d'octet lu.
		
	//Indique que tous les octets de la liaison serie vont etre lus
	CSerialCom::ReceivePendingMsg( iCnx );
		
	while( iSize != ZERO_BYTE )
	{				
		//On traite les donnees en fonction de l'etat de la synchronisation.
		switch( m_uiState )
		{
			//Dans cet etat on lit le premier octet de synchronisation			
			case STATE_1 :
			{
				//On pointe en début du buffer
				m_iPtrBuffer = 0;
				
				//Lecture d'un octet
				iSize = Read( &( ( char * ) &m_pucBufferRecv ) [m_iPtrBuffer],
							   ONE_BYTE );
				
				//Si la taille est differente de 1 octet, alors on reste dans
				//l'état 1
				if( iSize != ONE_BYTE )
				{
					break;
				}
				
				//Si c'est l'octet de synchronisation alors on passe dans 
				//l'etat 2. Sinon on reste dans cet etat.
				if( m_pucBufferRecv.sHeader.cPacketIDLow == PACKET_TM_ID_LOW )
				{					
					m_uiState = STATE_2;	
					
					//Incrémentation du pointeur sur le buffer
					m_iPtrBuffer++;				
				}				

#ifdef CSERIALCOMPROTO_ERR_DEBUG	
				else
				{	
					printf( "Packet Id Low error, char=%x\n", 
							m_pucBufferRecv.sHeader.cPacketIDLow ); 
				}
#endif
				
				break;
			}
			
			//Dans cet etat on lit le deuxieme octet de synchronisation			
			case STATE_2 :
			{
				//Lecture d'un octet
				iSize = Read( &( ( char * ) &m_pucBufferRecv ) [m_iPtrBuffer],
							  ONE_BYTE );
				
				//Si la taille est differente de 1 octet, alors on reste dans 
				//l'etat courant
				if( iSize != ONE_BYTE )
				{
					break;
				}
				
				//Si c'est l'octet de synchronisation alors on passe dans 
				//l'etat 3. Sinon on part dans l'etat 1
				if( m_pucBufferRecv.sHeader.cPacketIDHigh == PACKET_ID_HIGH )
				{					
					m_uiState = STATE_3;
					
					//Incrémentation du pointeur sur le buffer
					m_iPtrBuffer++;
					
					//Nombre d'octet à lire
					m_iDataLeftToRead = NB_BYTE_FOR_ALL_HEADER;					
				}
				else
				{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
					printf( "Packet Id High error, char=%x\n", 
							m_pucBufferRecv.sHeader.cPacketIDHigh ); 
#endif

					m_uiState = STATE_1;
					m_bSync = false;
				}
				
				break;
			}					
				
			//Dans cet etat on lit l'ensemble des headers		
			case STATE_3 :
			{
				//Lecture du nombre d'octets restant
				iSize = Read( &( ( char * ) &m_pucBufferRecv ) [m_iPtrBuffer],
							  m_iDataLeftToRead );
				
				//Mise à jour du pointeur dans le buffer
				m_iPtrBuffer = m_iPtrBuffer + iSize;
				
				//Si le nombre d'octets lus n'est pas égal au nombre attendu
				//alors on met à jour le nombre d'octets restant et on reste
				//dans l'etat courant
				if( iSize != m_iDataLeftToRead )
				{
					m_iDataLeftToRead = m_iDataLeftToRead - iSize;					
					break;
				}
				
				//Sinon on traite l'ensemble des headers. En mode debug, on
				//verifie la valeur de tous les champs des headers. En mode
				//release, on se base uniquement sur le checksum.
				else
				{					
					//On vérifie que la segmentation est bien a 1
					if( ( ENDIAN_RET16( m_pucBufferRecv.sHeader.usSegAndCounter ) 
						& SEGMENTATION_MASK ) != SEGMENTATION_VALUE )
					{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf ( "Segmentation error, %x\n",
								 ENDIAN_RET16( m_pucBufferRecv.sHeader.usSegAndCounter ) );
#endif
						
						//Trame incorrecte.
						m_uiNbRecvNok++;
						
						m_uiState = STATE_1;
						m_bSync = false;	
						break;																
					}	
					
					//On verifie qu'il n'y a pas eu de perte de TM
					if( ( ENDIAN_RET16( m_pucBufferRecv.sHeader.usSegAndCounter ) 
						& FRAME_COUNT_MASK ) != m_usTmFrameCount )
					{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf ( "TM lost. IdInt=%d, ", m_usTmFrameCount ); 
#endif					

						//Reinitialisation du compteur de trames
						m_usTmFrameCount = 
							ENDIAN_RET16( m_pucBufferRecv.sHeader.usSegAndCounter ) 
							& FRAME_COUNT_MASK;	
							
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf ( "IdDtu=%d\n", m_usTmFrameCount ); 
#endif					
							
						//Trame perdu
						m_uiNbLostMsg++;																								
					}

#ifdef CSERIALCOMPROTO_DEBUG											
					printf("Count frame = %d\n", m_usTmFrameCount );
#endif
					//Incrémentation du compteur pour la prochaine trame
					m_usTmFrameCount++;
					
					//Si le compteur a depasse la valeur maximale, on le
					//reinitialise.
					if( m_usTmFrameCount > FRAME_COUNT_MAX )
					{
						m_usTmFrameCount = 0;
					}																				
					
					//Calcule de la taille de la trame.
					//Il s'agit du nombre d'octets fourni + un octet 
					//+ un octet de spare si le resultat est impaire.					
					m_usTmFrameSize = 
						ENDIAN_RET16( m_pucBufferRecv.sHeader.usSizePacket ) + 1;
						
#ifdef CSERIALCOMPROTO_DEBUG											
					printf("Size frame = %d\n", m_usTmFrameSize );
#endif
						
					//Le nombre d'octets est toujours paire. Il faut peut etre
					//ajouter la lecture d'un octet spare suplementaire
					if( m_usTmFrameSize % 2 )
					{
						m_usTmFrameSize++;
						
#ifdef CSERIALCOMPROTO_DEBUG					
						printf ( "Spare byte added\n" );
#endif						
					}
					
					//On verifie la taille de la TM a lire qui doit etre comprise
					//entre une taille maximale et une taille minimale qui 
					//correspond a aucune donnee (checksum pris en compte)
					if( ( m_usTmFrameSize > sizeof( sAscDtuTmData ) ) ||
						( m_usTmFrameSize < ( sizeof( sAscDtuTmHeader ) 
											+ sizeof( unsigned short ) ) ) )
					{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf( "TM Size error, size=%d\n", m_usTmFrameSize ); 
#endif	
					
						//Trame incorrecte.
						m_uiNbRecvNok++;
						
						m_uiState = STATE_1;
						m_bSync = false;
						break;																		
					}										
					
					//On vérifie que le PUS est bien à 1
					if( m_pucBufferRecv.sTmData.sTmHeader.cPusVersion != PUS_VERSION_TM )
					{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf ( "Pus version error\n" );
#endif
						
						//Trame incorrecte.
						m_uiNbRecvNok++;
						
						m_uiState = STATE_1;
						m_bSync = false;
						break;						
					}																							
						
					//Le nombre d'octets restants a lire ne doit pas prendre
					//en compte la taille du header de TM qui a deja ete lue.
					m_iDataLeftToRead = m_usTmFrameSize - sizeof( sAscDtuTmHeader );
												
					//Passage dans l'etat suivant
					m_uiState = STATE_4;													
				}
			}	
															
			//Dans cet état on lit le reste de la trame DTU.
			case STATE_4 :
			{				
				//Lecture du nombre d'octets restant
				iSize = Read( &( ( char * ) &m_pucBufferRecv ) [m_iPtrBuffer],
							  m_iDataLeftToRead );
				
				//On met à jour le nombre d'octets contenu dans le message
				m_iPtrBuffer = m_iPtrBuffer + iSize;
					
				//Si le nombre d'octets lus n'est pas égal au nombre attendu
				//alors on met à jour le nombre d'octets restant et on reste
				//dans l'etat courant
				if( iSize != m_iDataLeftToRead )
				{
					m_iDataLeftToRead = m_iDataLeftToRead - iSize;
				}								
				
				//Sinon on traite le message reçu, on reinitialise le pointeur
				//sur le buffer, le nombre d'octet à lire restant puis on 
				//retourne dans l'état 1
				else
				{						
					unsigned short usCrc; //Valeur du CRC retourne							
					
					unsigned short usCrcSizeFrame = sizeof( sAscDtuHeader ) + m_usTmFrameSize - sizeof( unsigned short );					
					
					char * pBuffer = ( char * ) &m_pucBufferRecv;						
					
					//On calcule le CRC de toute la trame sans integrer le CRC en fin de trame					
					usCrc = CalculateChecksum( pBuffer, usCrcSizeFrame );
							  		
					//Le résultat du crc doit etre egal à celui dans le message.   							
					if( usCrc == ENDIAN_RET16( *( ( unsigned short * ) &pBuffer[usCrcSizeFrame] ) ) ) 
					{												
						//Transmission du message.
						RecvSerialMsg( iCnx, ( char * ) &m_pucBufferRecv,
									   m_iPtrBuffer );									   
															   						
						m_uiNbRecvOk++;
						
						//On est synchronisé
						m_bSync = true;
					}
					else
					{
#ifdef CSERIALCOMPROTO_ERR_DEBUG					
						printf( "Checksum error, CrcCal=%x, CrcRead=%x\n", usCrc, 
						 ENDIAN_RET16( *( ( unsigned short * ) &pBuffer[usCrcSizeFrame - sizeof( unsigned short )] ) ) );						
#endif
						//Trame incorrecte.
						m_uiNbRecvNok++;
						
						//Erreur de checksum
						m_uiNbCheckSumErr++;
						
						//On considère que l'on est désynchronisé
						m_bSync = false;
					}
					
					//Reinitialisation des variables membres						
					m_iPtrBuffer = 0;
					m_uiState = STATE_1;					
				}
			}						
		}
	}		
}


/*----------------------------------------------------------------------------*/
/* RouteMsg					                                                  */
/*----------------------------------------------------------------------------*/
/*! Traite du routage d'un message complet (header et données) emis pas l'EGSE.
 *  Le message est tout simplement emis au DPU.
 *
 *	\param       [in] pcBuffer : Tableau d'octets a transmettre au DPU
 *	\param       [in] iSize : Nombre d'octets a transmettre au DPU
 */
/*----------------------------------------------------------------------------*/
void CSerialComProto::RouteMsg( char * pcBuffer, unsigned int iSize )
{	
	//Emission du message route au DTU
	Send( pcBuffer, iSize );
	
	m_uiNbSendOk++;
}


/*----------------------------------------------------------------------------*/
/* SetTime					                                                  */
/*----------------------------------------------------------------------------*/
/*! Transmet le message AscTimeTC au DPU afin de traiter la synchronisation
 *  horaire de ce dernier. Si l'heure transmise n'est pas initialisee, il 
 *  s'agit d'un message de synchronisation sans PPS. Dans ce cas on transmet
 *  directement la date et l'heure du calculateur. Dans le cas contraire, on
 *  transmet l'heure fournie.
 *
 *	\param       [in] pPpsTime : Date en mode PPS
 */
/*----------------------------------------------------------------------------*/
void CSerialComProto::SetTime( struct timespec * pPpsTime )
{	
	//Constantes utilisees pour definir la configuration de la TC AscTimeTC
	const unsigned short CORRELATE_PPS = 				0x02;	
	const unsigned short TRANSMITE_ASCSYNCTIMETM = 		0x04;	
	const unsigned short LAT_IN_ABSOLUTE_TIME = 		0x08;		
	
	//Constante definissant la configuration en mode sans PPS
	const unsigned short TIME_INFO_PPS_NOT_USED = 
				( TRANSMITE_ASCSYNCTIMETM | LAT_IN_ABSOLUTE_TIME );

	//Constante definissant la configuration en mode avec PPS	
	const unsigned short TIME_INFO_PPS_1_IS_USED = 
		( CORRELATE_PPS | TRANSMITE_ASCSYNCTIMETM | LAT_IN_ABSOLUTE_TIME );
		
	int iCrQnx; //Variable compte rendu Qnx
	
	struct timespec NewTime;
	
	unsigned short usTimeNs;
	
	_uint64 uint64NSec; //variable utilisée pour passer en fraction de secondes	
	
	//Donnee concernant la mise a jour de l'heure du DTU
	sTcTimeData TcDtuTimeData;		
	
	//Si la date n'est pas fournie, on prend celle du calculateur.	
	if( pPpsTime == NULL )
	{
		//On indique au DTU que la mise à l'heure n'utilise pas le PPS
		TcDtuTimeData.usInfo = ENDIAN_RET16( TIME_INFO_PPS_NOT_USED );
		
		//On recupere la date courante
		iCrQnx = clock_gettime( CLOCK_REALTIME, &NewTime );
		
		if( iCrQnx == QNXINTERNALERROR )
		{
			CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
		}	
		
#ifdef CSERIALCOMPROTO_DEBUG
		printf("Send a not PPS time TC to the DTU\n");
#endif
								 									
	}
	else
	{
		//On indique au DTU que la mise à l'heure utilise le PPS
		TcDtuTimeData.usInfo = ENDIAN_RET16( TIME_INFO_PPS_1_IS_USED ); 
		
		//On sauvegarde les informations de date. Seul le nombre de secondes
		//est significatif.
		NewTime.tv_sec = pPpsTime->tv_sec;
		NewTime.tv_nsec = 0;
		
#ifdef CSERIALCOMPROTO_DEBUG
		printf("Send a PPS time TC to the DTU\n");
#endif

	}
	
	//On calcule le nombre de fraction de secondes.
	uint64NSec = NewTime.tv_nsec;
	uint64NSec = uint64NSec * 65536 / 1000000000;		
	usTimeNs = ( unsigned short ) uint64NSec;
	
#ifdef CSERIALCOMPROTO_TIME_DEBUG
	printf("SetTimeTc : nombre sec=%d\n", NewTime.tv_sec );
	printf("SetTimeTc : nombre sec=%d\n", usTimeNs );
#endif		
		
	//Mise à jour de la variable temps compatible avec celle du DTU.
	TcDtuTimeData.uiTimeNbSec = ENDIAN_RET32( NewTime.tv_sec );			
	TcDtuTimeData.usTimeNbFracSec = ENDIAN_RET16( usTimeNs );
		
	//On emet le message de synchronisation horaire au DTU
	SendMsg( TYPE_TC_TIME, SUBTYPE_TC_TIME, ( char * ) &TcDtuTimeData,
			 sizeof( TcDtuTimeData ) );
}
			   
			   
			   
/*----------------------------------------------------------------------------*/
/* ResetData				                                                  */
/*----------------------------------------------------------------------------*/
/*! Reinitialise les valeurs de debug.
 *
 */
/*----------------------------------------------------------------------------*/
void CSerialComProto::ResetDebugData()
{
	CSerialCom::ResetDebugData();
}   


/******************************************************************************/
/*																			  */
/*							METHODES PRIVATE 								  */
/*																			  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* InitCrcTable				                                                  */
/*----------------------------------------------------------------------------*/
/*! Realise l'initialisation de la table de checksum utilisee pour le calcul
 *  du checksum. Ce code est directement recupere de la documentation fournie
 *  par le DTU (ref ECSS--E--70--41A).
 *
 */
/*----------------------------------------------------------------------------*/
void CSerialComProto::InitCrcTable()
{
	//Variable de boucle
	unsigned int i;
	
	//Valeur calculée à enregistrer dans le tableau
	unsigned short tmp;

	for ( i=0; i<256; i++ )
	{
		tmp=0;
		if ((i & 1) != 0) tmp=tmp ^ 0x1021;
		if ((i & 2) != 0) tmp=tmp ^ 0x2042;
		if ((i & 4) != 0) tmp=tmp ^ 0x4084;
		if ((i & 8) != 0) tmp=tmp ^ 0x8108;
		if ((i & 16) != 0) tmp=tmp ^ 0x1231;
		if ((i & 32) != 0) tmp=tmp ^ 0x2462;
		if ((i & 64) != 0) tmp=tmp ^ 0x48C4;
		if ((i & 128) != 0) tmp=tmp ^ 0x9188;
		
		m_tuiCrcTable[i] = tmp;
	}
}

/*----------------------------------------------------------------------------*/
/* CalculateChecksum		                                                  */
/*----------------------------------------------------------------------------*/
/*! Calcule le checksum d'une chaine de caractere. Ce code est directement 
 *  recupere de la documentation fournie par le DTU (ref ECSS--E--70--41A)
 *
 *	\param       [in] pBuffer : chaine de caracteres
 *	\param       [in] iSize : nombre de caracteres
 *  \return      Retourne la valeur du checksum calcule
 */
/*----------------------------------------------------------------------------*/
unsigned short CSerialComProto::CalculateChecksum( char * pBuffer, int iSize )
{
	//Variable checksum à initialiser
	unsigned short Chk = 0xFFFF;
	
	//Variable compteur
	int j;
	
	for(j = 0; j < iSize; j++ )
	{
		Chk = ( (Chk << 8) & 0xFF00 )^m_tuiCrcTable [(((Chk >> 8)^pBuffer[j]) & 0x00FF)];
	}
	
	return Chk;
}

/*----------------------------------------------------------------------------*/
/* SendMsg					                                                  */
/*----------------------------------------------------------------------------*/
/*! Construit un message de type TC a envoyer au DTU. Les donnees fournies
 *  correspondent uniquement aux champs de donnees Data[0] a Data[N-1] sans 
 *  prendre en compte le header. La taille fournie est N. La methode reconstruit
 *  la trame en gerant en interne le compteur de sequence.
 *
 *	\param       [in] ucType : Type du message de TC a transmettre au DPU
 *	\param       [in] ucSubType : Sous type du message de TC a transmettre au DPU
 *	\param       [in] pcBuffer : Tableau d'octets a transmettre au DPU
 *	\param       [in] uiSize : Nombre d'octets a transmettre au DPU
 */
/*----------------------------------------------------------------------------*/
void CSerialComProto::SendMsg( unsigned char ucType, unsigned char ucSubType,
							   char * pcBuffer, unsigned int uiSize )
{				
	unsigned short usCrc; //Checksum a calculer
	
	//On vérifie que la taille est correcte
	if( uiSize > MAX_DATA_TC )
	{
#ifdef CSERIALCOMPROTO_DEBUG					
		printf ( "TC Size error, size=%d\n", iSize ); 
#endif	
		
		return;																		
	}
						
	//Initialisation de la segmentation et du compteur de trame
	m_pucBufferSend.sHeader.usSegAndCounter =
		ENDIAN_RET16( m_usTcFrameCount | SEGMENTATION_VALUE );
		
	//Incrementation du compteur avec gestion de la saturation
	m_usTcFrameCount++;
	
	if( m_usTcFrameCount > FRAME_COUNT_MAX )
	{
		m_usTcFrameCount = 0;
	}
	
	//Initialisation de la taille de la TC qui consiste à ajouter à la taille
	//fournie la taille du header de TC, la taille du CRC et à laquelle il faut
	//retirer un octet (contrainte de specification). D'où l'ajout d'un seul
	//octet.
	m_pucBufferSend.sHeader.usSizePacket = 
		ENDIAN_RET16( uiSize + sizeof( sAscDtuTcHeader ) + sizeof( char ) );
	
	//Il faut maintenant mettre à jour le header de TC
	m_pucBufferSend.sTcData.sTcHeader.cType = ucType;
	m_pucBufferSend.sTcData.sTcHeader.cSubType = ucSubType;
		
	//Puis il faut copier les donnees dans le buffer
	memcpy( m_pucBufferSend.sTcData.cData, pcBuffer, uiSize );
	
	//On calcule maintenant le CRC de toute la trame
	usCrc = CalculateChecksum( ( char * ) &m_pucBufferSend, 
				sizeof( sAscDtuHeader ) + sizeof( sAscDtuTcHeader ) + uiSize );
				
	//On met à jour le checksum dans la trame.
	*( ( unsigned short *) &m_pucBufferSend.sTcData.cData[uiSize] ) = 
	ENDIAN_RET16( usCrc );	
	
	//Emission du message sur la connexion
	Send( ( char * ) &m_pucBufferSend, 
	    sizeof( sAscDtuHeader ) + sizeof( sAscDtuTcHeader ) +
	    uiSize + sizeof( unsigned short ) );
	    		    		    
		//On incremente le nombre de messages emis au DPU
	m_uiNbSendOk++;	
}

/* @} */
