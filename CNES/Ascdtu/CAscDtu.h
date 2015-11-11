/*! \file CAscDtu.h
 *  \brief Specification de la classe ASC DTU
 */
 
#ifndef _CASCDTU_H_
#define _CASCDTU_H_

#include <CBASDrv.h>
#include "IfAscDtu.h"
#include "CPeriodicManage.h"
#include "CSerialComProto.h"

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//  - Prise en compte de BL-DM-2471
//      -Rajout d'un objet de type CPeriodicManage permettant de cadencer les
//       émissions des TM Debug
//      -Modification du prototype de la fonction SendDebugTm
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
// Date : 17/04/13
//  Auteur : Rémi THONNERIEUX - CIO
//  - BL-FI-4222 : - Ajout d'un flag permettant d'identifier la perte du PPS
//                  m_iPpsAlive
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//  Auteur : Rémi THONNERIEUX - CIO
//  - BL-DM-4197 : - Modification de la structure eDebugMsg (dont ucValidant n°5)
//					et ajout de la variable m_uiNbMsgRcvFromEGSENok
////////////////////////////////////////////////////////////////////////////////

/*! \addtogroup ascdtu */
/* @{ */

/*! \class CAscDtu
 *  \brief Classe utilisee pour implementer le driver du senseur stellaire DTU
 *
 *  La classe CAscDtu est utilisee pour interfacer le senseur stellaire DTU
 *  La classe herite de la classe CBASDrv pour surcharger et utiliser les
 *  methodes d'un driver et elle herite aussi de la classe IReceiveProtMsg pour
 *  traiter de la recetion de messages issus de l'ASC DTU.
 * 
 *  La classe utilise les services d'emission de telemesures et de reception
 *  de telecommandes pour s'interfacer avec la supervision.
 * 
 *  La classe utilise la classe CSerialComProto afin de mettre en place le
 *  protocole de communication avec le DTU et permettre l'emission et la 
 *  reception de messages.
 * 
 *  La classe utilise les methodes de notification de synchronisation horaire 
 *  afin de transmettre le message d'heure GPS une fois que le PPS a ete recu.
 *
 *  La classe utilise le stockage de donnees pour stocker au sein d'un canal
 *  permanent les messages d'attitudes des CHU.
 * 
 *  La classe utilise l'emission de messages d'application afin de transmettre
 *  les messages d'attitudes des CHU a un driver.
 * 
 *  Les parametres de configuration du driver sont:
 *  - NumCom qui fournit l'identifiant du port serie a utiliser pour la
 *    communication avec le DPU.
 *  - PeriodeTMDebug qui fournit la periode d'emission en seconde de
 *    la TM de debug
 *  - DriverId qui fournit l'identifiant du driver destine a recevoir l'attitude
 *    des CHU
 *  - CanalPermanentId qui fournit l'identifiant du canal permanent de 
 *    stockage des attitudes des CHU.
 *  
 *  Les telemesures emises sont:
 *  - La telemesure de debug qui fournit des informations sur l'attitude 
 *    de chaque CHU ainsi que des informations generales de communication avec
 *    le DPU.
 * 
 *  Les telecommandes recues sont:
 *  - Le passage en intervention pour l'emission de la TM de debug
 *  - La reinitialisation des donnees de debug
 *  - les messages emis par l'EGSE a destination du DPU
 */
class CAscDtu : public CBASDrv, public IReceiveProtMsg
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/	

	public:	
	
	/*!
	*   Erreurs levees par la classe
	*/
	enum eClassError
	{	
		ERR_ASCDTU_INVALID_COM = RANGE_APP_LOW,
	    /*!< 10000 : Port invalide. */	
	    ERR_ASCDTU_INVALID_CHANNEL_ID,
	    /*!< 10001 : Identifiant du canal invalide. */	
	    ERR_ASCDTU_INVALID_DRIVERID, 
	    /*!< 10002 : Identifiant du driver invalide. */	
	    ERR_ASCDTU_BAD_TC_SIZE,
	    /*!< 10003 : Reception d'une telecommande de taille invalide. */   
	};
	
	/*!
	*   Etats possibles de reception des sous messages
	*/
	enum
	{
		FIRST_SS_MSG_STATE = 0,
		 /*!< Etat dans lequel on attend le premier sous message */
		OTHER_SS_MSG_STATE,
		/*!< Etat dans lequel on attend les sous messages suivants */
	};

	/*!
	*   Etats possibles du protocole de gestion des longs messages
	*/
	enum
	{
		OTHER_SS_MSG = 0x00,
		/*!< Sous message different du premier et du dernier */
		FIRST_SS_MSG = 0x40,
		/*!< Premier sous message */
		LAST_SS_MSG = 0x80,
		/*!< Dernier sous message apres un suite de sous messages*/
		FIRST_AND_LAST_SS_MSG = 0xC0,
		/*!< Premier et dernier sous message */
	};
						
	protected:
	
	private:	
	
	/*!
	*   Identifiant des TM du driver
	*/
	enum
	{
		TM_DEBUG_ID,
		/*!< ID = 0 : TM contenant les donnees de debug du driver. */
	};
	
	/*!
	*   Identifiant des TC du driver confmanager
	*/
	enum
	{
		TC_DEBUG = 0,
		/*!< ID = 0 : Telecommande d'intervention */
		TC_RESET_DEBUG,
		/*!< ID = 1 : Telecommande de reset des compteurs de debug */		
	};	
		
	/*!
	*  \brief Structure utilisee pour superviser un CHU
	*/			
	typedef struct
	{		
		unsigned char ucResidus;
		/*!< Residus d'un CHU */
		unsigned int uiCptLive;
		/*!< Compteur de vie d'un CHU */				
		bool bSendAttitude;
		/*!< Compteur d'emission de l'attitude */	
	}
	sChuState;	
	
//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas créer d'espace libre entre les données
#pragma pack(push, Alignment1Byte, 1)
	
	/*!
	*  \brief Structure contenant les donnees de la telemesure de debug
	*/		
	typedef struct
	{		
		double dQ0ChuA;
		/*!< Quaternion 0 du CHU A*/	
		double dQ1ChuA;
		/*!< Quaternion 1 du CHU A*/	
		double dQ2ChuA;
		/*!< Quaternion 2 du CHU A*/	
		double dQ3ChuA;
		/*!< Quaternion 3 du CHU A*/	
		double dQ0ChuB;
		/*!< Quaternion 0 du CHU B*/	
		double dQ1ChuB;
		/*!< Quaternion 1 du CHU B*/	
		double dQ2ChuB;
		/*!< Quaternion 2 du CHU B*/	
		double dQ3ChuB;
		/*!< Quaternion 3 du CHU B*/			
		double dQ0ChuC;
		/*!< Quaternion 0 du CHU C*/	
		double dQ1ChuC;
		/*!< Quaternion 1 du CHU C*/	
		double dQ2ChuC;
		/*!< Quaternion 2 du CHU C*/	
		double dQ3ChuC;
		/*!< Quaternion 3 du CHU C*/	
		double dQ0ChuD;
		/*!< Quaternion 0 du CHU D*/	
		double dQ1ChuD;
		/*!< Quaternion 1 du CHU D*/	
		double dQ2ChuD;
		/*!< Quaternion 2 du CHU D*/	
		double dQ3ChuD;
		/*!< Quaternion 3 du CHU D*/		
		unsigned char ucValidant;
		/*!< Bit 0 : Etat de la communication avec l'ASC DTU */				
		/*!< Bit 1 : Etat du validant du CHU A */	
		/*!< Bit 2 : Etat du validant du CHU B */	
		/*!< Bit 3 : Etat du validant du CHU C */	
		/*!< Bit 4 : Etat du validant du CHU D */	
		/*!< Bit 5 : Etat de validité du PPS */
		unsigned char ucResidusChuA;
		/*!< Residus du CHU A */	
		unsigned char ucResidusChuB;
		/*!< Residus du CHU B */	
		unsigned char ucResidusChuC;
		/*!< Residus du CHU C */	
		unsigned char ucResidusChuD;
		/*!< Residus du CHU D */
		unsigned int uiNbCnxLost;
		/*!< Nombre de fois que la communication a ete perdue avec le DPU */
		unsigned int uiNbMsgSendToDPU;
		/*!< Nombre de messages emis vers le DPU */
		unsigned int uiNbMsgRcvFromDPU;
		/*!< Nombre de messages recus du DPU */
		unsigned int uiNbMsgRcvFromDPUNok;
		/*!< Nombre de messages recus du DPU en erreur*/
		unsigned int uiNbDeSeqFromDPU;
		/*!< Nombre de séquencement sur reception de message DPU*/
		unsigned int uiNbChecksumErrFromDPU;
		/*!< Nombre de messages recus de l'EGSE rejetes sur checksum*/
		unsigned int uiNbMsgSendToEGSE;
		/*!< Nombre de messages emis vers l'EGSE */
		unsigned int uiNbMsgRcvFromEGSE;
		/*!< Nombre de messages recus de l'EGSE */
		unsigned int uiNbMsgRcvFromEGSENok;
		/*!< Nombre de messages recus de l'EGSE en erreur*/
		unsigned int uiNbRebuiltErrMsgFromEGSE;
		/*!< Nombre de reconstruction de messages en erreur recus de l'EGSE*/
	} eDebugMsg;	
	
//On réinstalle le type d'alignement préalablement sauvegardé
#pragma pack(pop, Alignment1Byte)
	
	/*!
	*   Variable utilisee pour contenir la configuration du driver.
	*/
	CBASConfigDrv * m_pConfig;	
	
	/*!
	*   Variable memoire partagee du driver.
	*/
	CShareMemory	m_ShareMemory;
		
	/*!
	*   Semaphore d'acces aux donnees presentes dans la memoire partagee.
	*/
	sem_t *	m_pShareMemSem;
	
	/*!
	*   Pointeur sur la memoire partagee.
	*/
	eIfApp * m_pShareMem;
	
	/*!
	*   Variable de communication avec l'ASC DTU
	*/
	CSerialComProto	 m_Cnx;
		
	/*!
	*   Timer utilise pour generer un evenement.
	*/		
	int m_TimerId;
							
	/*!
	*   Variable utilisee pour definir le rythme du timer.
	*/		
	struct _itimer m_itime;	
	
	/*!
	*   Variable evenement generee par le timer et associe a un pulse.
	*/		
	sigevent m_EventTimer;

	/*!
	*   Variable evenement generee lorsque des donnees sont a lire.
	*/		
	sigevent m_EventRcvData;
			
    /*!
    *   Variable utilisee pour l'envoi periodique de la TM debug.
    */      	
	CPeriodicManage m_SendDataTmDebugRate;

    /*!
    *   Variable utilisee pour valider l'envoi des TM debug jusqu'au sol (sinon
    * transmises pour stockage).
    */ 
	CPeriodicManage m_SaveAndSendDataTmDebugRate;
	
	/*!
	*   Variable utilisee pour l'emission periodique du message de 
	*   synchronisation horaire lorsque le PPS n'est pas utilise.
	*/
	CPeriodicManage m_SendClockSyncMsg;
	
	/*!
	*   Variable utilisee pour l'emission de la trame d'attitude
	*/
	CPeriodicManage m_SendAttitudeTm;
	
	/*!
	*   Variable utilisee pour indiquer si le PPS est utilise. Lorsque c'est
	*   le cas la variable est initialisee a true.
	*/
	bool m_bPPSUsed;
	
	/*!
	*   Variable utilisee pour contenir l'identifiant de driver vers lequel
	*   les messages d'attitude des CHU sont emis. Si aucun driver n'est defini
	*   la variable est initialisee a OPTION_NOT_USED;
	*/
	unsigned int m_uiDrvIdToSendChuAtt;
	
	/*!
	*   Variable utilisee pour contenir l'identifiant de canal de stockage
	*   evenementiel utilise pour stocker les messages d'attitudes.
	*/
	unsigned int m_uiChannelId;
	
	/*!
	*   Variable utilisee pour contenir la derniere date GPS recue.
	*/
	struct timespec m_LastGpsDate;
	
	/*!
	*   Variable utilisee pour indiquer si la date est forcee ou pas.
	*/
	bool m_bDateIsForced;
	
	/*!
	*   Variable utilisee pour indiquer si l'ASC DTU est en vie
	*/				
	bool m_bAscDtuAlive;
    
    /*!
    *   Variable utilisee pour indiquer si le PPS est en vie
    */              
    unsigned int m_uiPpsAlive;
	
	/*!
	*   Variable utilisee pour stocker le compteur de vie du DPU
	*/
	unsigned int m_uiAscDtuLive; 
	
	/*!
	*   Variable utilisee pour stocker les donnees de supervision des 4 CHU
	*/				
	sChuState m_ChuState[NB_CHU];
	
	/*!
	*   Variable utilisee pour compter le nombre de fois que l'ASC DTU a ete
	*   perdu
	*/				
	unsigned int m_uiNbAscDtuLost;
	
	/*!
	*   Variable utilisee pour compter le nombre de messages emis a l'EGSE
	*/
	unsigned int m_uiNbMsgSendToEGSE;
	
	/*!
	*   Variable utilisee pour compter le nombre de messages recus de l'EGSE
	*/
	unsigned int uiNbRebuiltErrMsgFromEGSE;
	
	/*!
	*   Variable utilisee pour compter le nombre de messages recus de l'EGSE
	*/
	unsigned int m_uiNbMsgRcvFromEGSE;
	
	/*!
	*   Variable utilisee pour compter le nombre de messages recus de l'EGSE
	* en erreur
	*/
	unsigned int m_uiNbMsgRcvFromEGSENok;
	
	/*!
	*   Buffer de reception des sous messages transmis par la station sol EGSE
	*/
	char m_cEgseBuffer[sizeof( CSerialComProto::sAscDtuTcData ) ];
	
	/*!
	*   Pointeur sur le buffer de reception des sous messages 
	*	transmis par la station sol EGSE
	*/
	int m_iEgseBufferPtr;

	/*!
	*   Etat de reception des sous messages (FIRST_SS_MSG ou OTHER_SS_MSG)
	*   transmis par la station sol EGSE
	*/
	int m_iEgseBufferState;

	/*!
	*   Compteur de trames en reception des sous messages transmis par 
	*   la station sol EGSE.
	*/
	int m_iEgseBufferCpt;
		
	/*!
	*   Compteur utilise pour emettre une TC de resynchronisation du DPU
	*   a un sous multiple du PPS
	*/
	unsigned int m_uiPpsCpt;
				
    /*------------------------------------------------------------------------*/
    /* Methodes d'instance                                                    */
    /*------------------------------------------------------------------------*/			
    
	public:
	
	/*!
	*   \brief Constructeur de la classe.
	*/	
	CAscDtu();
	
	/*!
	*   \brief Destructeur de la classe.
	*/
	virtual ~CAscDtu();
	
	/*!
	*   \brief Lance le driver.
	*/		
	void Start( int argc, char *argv[] );
	
	protected:
	
	private:
	
	/*!
	*   \brief Initialise les donnees du driver.
	*/			
	void Init();	
	
	/*!
	*   \brief Ouvre la memoire partagee avec ISaGRAF.
	*/			
	void OpenShareMem();
	
	/*!
	*   \brief Recupere la configuration specifique du confmanager.
	*/			
	void GetSpecificConfig();
	
	/*!
	*   \brief Methode appelee pour demander au driver de se configurer.
	*/		
	bool IFAWConf();

	/*!
	*   \brief Methode appelee pour demander au driver de se deconfigurer.
	*/		
	bool IFAWDeconf();
		
	/*!
	*   \brief Methode appelee pour mettre en marche le driver.
	*/		
	bool IFAWRun();

	/*!
	*   \brief Methode appelee pour arreter le driver.
	*/		
	bool IFAWStop();
			
	/*!
	*   \brief Notifie la reception d'un pulse.
	*/			
	bool IFAWReceivePulse( int iCode, int iValue );		
	
	/*!
	*   \brief Reception d'un message issu de la connexion
	*/			
	void IReceiveMsgExt( int iCnx, char * pBuffer, int iSize );	
	
	/*!
	*   \brief Notifie la reception du message d'heure GPS.
	*/	
	void IBASReceiveGpsDate( struct timespec * pGpsDate	);

	/*!
	*   \brief Notifie la resynchronisation du calculateur.
	*/	
	void IBASReceiveTimeResync( bool bLocked );
	
	/*!
	*   \brief Notifie la reception du PPS.
	*/		
	void IBASReceivePPS();
	
	/*!
	*   \brief Notifie la reception d'une telecommande.
	*/			
	bool IBASReceiveTc( int iIdMsg, bool bStationSol, int iSize, char * pData );
		
	/*!
	*   \brief Effectue l'emission d'une TM de debug
	*/			
	void SendDebugTm(bool bNoEmission);		
	
	/*!
	*   \brief Effectue la prise en compte d'une trame d'attitude emise par un
	*   CHU.
	*/
	bool ProcessingAttitudeFrame( char * pBuffer, int iSize );
	
	/*!
	*   \brief Effectue le stockage et la diffusion vers un driver de la 
	*   trame d'attitude
	*/
 	void StockBroadcastAttitudeFrame( char * pBuffer, int iSize );
 	 	
	/*!
	*   \brief Effectue la prise en compte d'une trame de synchronisation horaire
	*   emise par un DPU.
	*/
	void ProcessingSyncTimeFrame( char * pBuffer, int iSize );	
	
	/*!
	*   \brief Recupere le nom du canal de stockage evenementiel.
	*/
	string GetChanelName();
};

/* @} */

#endif //_CASCDTU_H_
