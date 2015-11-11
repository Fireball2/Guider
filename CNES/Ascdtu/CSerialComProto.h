/*! \file CSerialComProto.h
 *  \brief Specification de la classe relative au protocole de communication
 *  serie avec l'ASC DTU.
 */
  
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//  - Prise en compte de BL-DM-2474
//      - Remise à niveau de la documentation Doxygen
////////////////////////////////////////////////////////////////////////////////
  
#ifndef _CSERIALCOMPROTO_H_
#define _CSERIALCOMPROTO_H_

#include "CSerialCom.h"
#include "CPerf.h"

/*! \addtogroup ascdtu */
/* @{ */

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir le nombre de donnees maximum dans une
 * 	 trame de TM.
 */
const unsigned int MAX_DATA_TM = 2030;

/*!
 *   Portee : classe CSerialComProto\n
 *   Constante utilisee pour definir le nombre de donnees maximum dans une
 * 	 trame de TC.
 */
const unsigned int MAX_DATA_TC = 2036;


/*! \class CSerialComProto
 *  \brief Classe utilisee pour implementer le protocole de communication serie
 *  avec l'ASC DTU.
 *
 *  La classe CSerialComProto est utilisee pour mettre en place le protocole de
 *  synchronisation afin de permettre de delimiter explicitement les messages
 *  transmis dans le flux d'octets.
 * 
 *  La classe herite de la classe CSerialCom pour surcharger et utiliser les
 *  methodes de communication a travers un lien serie.
 */
class CSerialComProto : public CSerialCom
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/	

	public:		
			
	/*!
	*   Defini les types et sous types des differents messages de TM
	*/
	enum
	{
		TYPE_TM_ATTITUDE = 5,
		/*!< Type de la TM d'attitude. */
		TYPE_TM_SYNCTIME = 9,
		/*!< Type de la TM de synchronisation horaire. */
		SUBTYPE_TM_ATTITUDE = 1,
		/*!< Sous type de la TM d'attitude. */
		SUBTYPE_TM_SYNCTIME = 131,
		/*!< Sous type de la TM de synchronisation horaire. */
	};
	
	/*!
	*   Defini les types et sous types des differents messages de TC
	*/
	enum
	{
		TYPE_TC_TIME = 9,
		/*!< Type de la TC de synchronisation horaire. */
		SUBTYPE_TC_TIME = 129,
		/*!< Sous type de la TC de synchronisation horaire. */
	};	

//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas créer d'espace libre entre les données
#pragma pack(push, Alignment1Byte, 1)			
	
	/*!
	*  \brief Structure definissant la date
	*/			
	typedef struct
	{		
		unsigned int uiSeconds;
		/*!< Nombre de secondes */		
		unsigned short usSubSeconds;
		/*!< Sous secondes. Valeur a diviser par 65536 */
	}
	sTime;
	
	/*!
	*  \brief Structure definissant l'entete d'une trame ASC DTU
	*/			
	typedef struct
	{		
		unsigned char cPacketIDLow;
		/*!< Poids faible du mot de l'identifiant du paquet */		
		unsigned char cPacketIDHigh;
		/*!< Poids fort du mot de l'identifiant du paquet */
		unsigned short usSegAndCounter;
		/*!< Information contenant la segmentation et le compteur
		 * - Les 2 bits de poids forts definissent la segmentation (egaux a 1)
		 * - Les bits restants definissent le compteur de trames */	
		unsigned short usSizePacket;
		/*!< Nombre d'octets - 1 contenu dans le packet de donnees  */
	}
	sAscDtuHeader;
	
	/*!
	*  \brief Structure definissant l'entete des donnees de TM 
	*/			
	typedef struct
	{		
		unsigned char cPusVersion;
		/*!< Version du programme */		
		unsigned char cType;
		/*!< Identifiant du type de la TM */
		unsigned char cSubType;
		/*!< Identifiant du sous type de la TM */
		sTime sTimeStamp;
		/*!< Datation du paquet */
		char cSpare;
		/*!< Octet non utilise */
	}
	sAscDtuTmHeader;
	
	/*!
	*  \brief Structure definissant l'entete des donnees de TC 
	*/			
	typedef struct
	{		
		char cPusVersion;
		/*!< Version du programme */		
		char cType;
		/*!< Identifiant du type de la TM */
		char cSubType;
		/*!< Identifiant du sous type de la TM */
		char cSpare;
		/*!< Octet non utilise */
	}
	sAscDtuTcHeader;
		
	/*!
	*  \brief Structure definissant une trame de donnees de TM
	*/			
	typedef struct
	{		
		sAscDtuTmHeader sTmHeader;
		/*!< Entete d'une trame TM */			
		char cData[MAX_DATA_TM];
		/*!< Tableau de donnees */
		unsigned short usCheckSum;
		/*!< Checksum de la partie TM (hors header). */	
	}
	sAscDtuTmData;	
	
	/*!
	*  \brief Structure definissant une trame de donnees de TC
	*/			
	typedef struct
	{		
		sAscDtuTcHeader sTcHeader;
		/*!< Entete d'une trame TC */			
		char cData[MAX_DATA_TC];
		/*!< Tableau de donnees */
		unsigned short usCheckSum;
		/*!< Checksum de la partie TC (hors header). */	
	}
	sAscDtuTcData;
		
	/*!
	*  \brief Structure definissant une trame de TM
	*/			
	typedef struct
	{		
		sAscDtuHeader sHeader;
		/*!< Entete de toute trame DTU */
		sAscDtuTmData sTmData;		
		/*!< Donnees de la trame TM */			
	}
	sAscDtuTmFrame;	
	
	/*!
	*  \brief Structure definissant une trame de TC
	*/			
	typedef struct
	{		
		sAscDtuHeader sHeader;
		/*!< Entete de toute trame DTU */		
		sAscDtuTcData sTcData;		
		/*!< Donnees de la trame TC */	
	}
	sAscDtuTcFrame;	
	
	/*!
	*  \brief Structure definissant la telecommande de gestion du temps
	*/			
	typedef struct
	{
		unsigned short usInfo;
		/*!< Information sur l'interpretation de la TC */		
		unsigned int uiTimeNbSec;
		/*!< Nombre de secondes de la nouvelle heure*/		
		unsigned short usTimeNbFracSec;		
		/*!< Nombre de fraction de seconde de la nouvelle heure */	
	}
	sTcTimeData;
	
	/*!
	*  \brief Structure definissant la telemesure d'attitude.
	*/			
	typedef struct
	{		
		short cReportId;
		/*!< Information REPORT_ID */		
		int iQ1;
		/*!< 1er quaternion */		
		int iQ2;
		/*!< 2eme quaternion */
		int iQ3;
		/*!< 3eme quaternion */
		int iQ4;
		/*!< 4eme quaternion (il s'agit du scalaire)*/
		unsigned char ucFlags;
		/*!< Information generale sur la trame d'attitude */
		unsigned char ucResidual;
		/*!< Estimation de la precision */
		unsigned char ucLocks;
		/*!< Nombre d'objets reconnus durant la phase de lost in space */
		unsigned char ucObjetcs;
		/*!< Nombre d'objets dans l'image */
		unsigned char ucStarsFail;
		/*!< Dependant de la validite de l'attitude (Nombre d'etoiles/erreur) */		
		char cAGC[3];
		/*!< Parametre interne a l'ASC DTU */
		unsigned int uiNbSeconds;		
		/*!< Nombre de secondes dans le timestamp */
		unsigned short usNbSubSeconds;
		/*!< Nombre de sous secondes dans le timestamp */
	}
	sTmAttitudeData;			

    /*!
	*  \brief Structure definissant la telemesure de synchronisation horaire.
	*/			
	typedef struct
	{						
		unsigned int uiObetNbSeconds;		
		/*!< Nombre de secondes correspondant a une heure absolue */
		unsigned short usObetNbSubSeconds;
		/*!< Nombre de sous secondes correspondant a une heure absolue */
		unsigned int uiLatNbSeconds;		
		/*!< Nombre de secondes de l'horloge interne du DTU (relatif) */
		unsigned short usLatNbSubSeconds;
		/*!< Nombre de sous secondes de l'horloge interne du DTU (relatif) */
	}
	sTmSyncTimeData;
	
	
//On réinstalle le type d'alignement préalablement sauvegardé
#pragma pack(pop, Alignment1Byte)

	protected:
	
	private:
	
	/*!
	*   Defini les differents etats de la machine a etats utilisee pour traiter
	*   de la synchronisation de la reception du message.
	*/
	enum
	{
		STATE_1,
		/*!< Etat de reception du premier octet de synchronisation. */
		STATE_2,
		/*!< Etat de reception du deuxieme octet de synchronisation. */
		STATE_3,
		/*!< Etat de reception des headers de la trame ASC DTU. */		
		STATE_4,
		/*!< Etat de reception des donnees et du checksum de la trame ASC DTU. */	
	};		
		
	/*!
	*   Variable utilisee pour indiquer l'état courant de la machine à états
	*   utilisée pour traiter de la ésynchronisation de la réception de
	*   messages.
	*/			
	unsigned char m_uiState;
	
	/*!
	*   Variable utilisee en tant que iterateur pour pointer le buffer de 
	*   reception de messages et le remplir a l'aide des octets recus.
	*/			
	int m_iPtrBuffer;

	/*!
	*   Variable utilisee pour lire le nombre de donnes restantes de la trame
	*   ASC DTU.
	*/			
	int m_iDataLeftToRead;
	
	/*!
	*   Variable utilisee pour connaitre l'etat de la synchronisation de la 
	*   liaison.
	*/				
	bool m_bSync;
		
	/*!
	*   Variable utilisee pour recevoir un message. La taille du buffer est 
	*   egale a la taille du plus grand message possible.
	*/			
	sAscDtuTmFrame m_pucBufferRecv;
	
	/*!
	*   Variable utilisee pour contenir la taille de la TM en cours de lecture.
	*/
	unsigned short m_usTmFrameSize;
	
	/*!
	*   Variable utilisee pour contenir le compteur courant de la trame de TM
	*/
	unsigned short m_usTmFrameCount;
	
	/*!
	*   Variable utilisee pour contenir la table de valeurs utilisees dans le
	*   calcul du CRC.
	*/
	unsigned int m_tuiCrcTable[256];
	
	/*!
	*   Variable utilisee pour transmettre un message au DTU. La taille du 
	*   buffer est egale a la taille du plus grand message possible.
	*/			
	sAscDtuTcFrame m_pucBufferSend;
	
	/*!
	*   Variable utilisee pour contenir le compteur courant de la trame de TC
	*/
	unsigned short m_usTcFrameCount;
	
			
    /*------------------------------------------------------------------------*/
    /* Methodes d'instance                                                    */
    /*------------------------------------------------------------------------*/			
    
	public:
	/*!
	*   \brief Constructeur de la classe.
	*/		
	CSerialComProto();
	
	/*!
	*   \brief Destructeur de la classe.
	*/				
	virtual ~CSerialComProto();	
		
	/*!
	*   \brief Indique la presence d'octets a lire.
	*/	
	void ReceivePendingMsg( int iCnx );		
	
	/*!
	*   \brief Route un message emis par l'EGSE au DTU
	*/	
	void RouteMsg( char * pcBuffer, unsigned int iSize );
	
	/*!
	*   \brief met a jour l'heure du DTU avec ou sans PPS
	*/
	void SetTime( struct timespec * pPpsTime = NULL );
	
	/*!
	*   \brief Reinitialise toutes les informations de debug.
	*/		
	void ResetDebugData();	
	
	private:
	
	/*!
	*   \brief Initialise la table de calcul du CRC
	*/	
	void InitCrcTable();
	
	/*!
	*   \brief Transmet un message au DTU
	*/	
	void SendMsg( unsigned char ucType, unsigned char ucSubType, char * pcBuffer,
			      unsigned int iSize );	
	
	/*!
	*   \brief Calcule le checksum d'une trame DTU
	*/	
	unsigned short CalculateChecksum( char * pBuffer, int iSize );		
};

/* @} */

#endif //_CSERIALCOMPROTO_H_
