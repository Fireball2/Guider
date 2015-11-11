/*! \file CInterfaceCU.h
 *  \brief Spécification de la classe principale du driver ICU
 *
 */

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//  Auteur : Rémi THONNERIEUX - CIO Informatique Industrielle
//  - Prise en compte de la BL-FI-5913
//      - Modification du type de la varialbe m_iCuActivityCounter
//                  
////////////////////////////////////////////////////////////////////////////////

#ifndef _CINTERFACECU_H_
#define _CINTERFACECU_H_

// Headers C++
#include <string>

// Headers Syslay
#include <CBASDrv.h>
#include <CCnx.h>
#include <IReceiveProtMsg.h>

// Headers locaux
#include "IfInterfaceCU.h"
#include "CRouteManager.h"
#include "CChecksum.h"

/*! \addtogroup interfacecu */

/* @{ */

/*! \class CInterfaceCU
 *  \brief Classe utilisée pour implementer le driver Interface Charge Utile.
 *
 *  La classe CInterfaceCU est utilisée pour interfacer une charge utile.
 *  La classe herite de la classe CBASDrv pour surcharger et utiliser les 
 *  méthodes d'un driver et de la classe IReceiveProtMsg pour recevoir les 
 *  messages de la charge utile.
 * 
 *  La classe utilise les services d'emission de telemesures et de reception
 *  de telecommandes pour s'interfacer avec la supervision.
 * 
 *  La classe utilise le service de reception de messages d'application pour 
 *  recevoir les messages émis par l'I/O driver ISaGRAF et les drivers tiers.
 * 
 *  La classe utilise le stockage de donnees pour les messages échangés 
 *  entre la charge utile et le driver.
 * 
 *  Les parametres de configuration du driver sont:
 *  - PeriodeTMDebug qui fournit la periode d'émission en secondes de la TM de
 *    debug
 *  - Interface qui permet de sélectionner le type de communication utilisée
 *    pour interfacer la charge utile
 *  - ActivationStockage qui permet d'indiquer que le stockage est activé
 *  - ActivationRoutage qui permet d'indiquer que le routage est activé
 *  - IdRootDriverX où X est un numéro, permet d'énumérer une liste
 *    d'identifiants de driver à router
 *  - IdRootMsgX où X est un numéro, permet d'indiquer l'identifiant de
 *    message correspondant à l'identifiant de driver cité précédemment
 *  - TimeoutCU qui permet de sélectionner le timeout de la charge utile
 *  - TypeChecksum qui permet de choisir le type de checksum à utiliser
 *    (SUM16 ou XOR)
 *  
 *  Il existe également des paramètres de configuration propres au type
 *  d'interface choisi :
 * 
 *  Pour une liaison série :
 *  - NumCom qui permet d'indiquer le numéro du port série utilisé
 *  - Bauderate qui permet de sélectionner la vitesse d'émission des données
 *  - NbBitsStop qui permet d'indiquer le nombre de bits de stop
 *  - Parite qui permet d'indiquer la parité
 * 
 *  Pour une liaison UDP/IP :
 *  - AdresseIpDistante qui permet de sélectionner l'adresse IP de la charge
 *    utile
 *  - PortUdpLocal qui permet d'indiquer le port utilisé en réception
 *  - PortUdpDistant qui permet d'indiquer le port utilisé en émission
 * 
 *  Les telemesures émises sont:
 *  - La télémesure de debug qui fournit des données de diagnostic de la liaison
 *    avec la charge utile
 * 
 *  Les télécommandes reçues sont les télécommandes de base (Debug et RazDebug)
 */
class CInterfaceCU : public CBASDrv, public IReceiveProtMsg
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/    
    
public:

	/*!
	 *   Erreurs levées par la classe
	 */
	enum
	{
		ERR_ICU_PTR_NULL = RANGE_APP_LOW,
		/*!< 10000 : Pointeur de la connexion NULL. */
		ERR_ICU_INVALID_PORT,
		/*!< 10001 : Port invalide. */
		ERR_ICU_UNKNOWN_TC,
		/*!< 10002 : Reception d'une telecommande inconnue. */
		ERR_ICU_BAD_TC_SIZE,
		/*!< 10003 : Reception d'une telecommande de taille invalide. */
		ERR_ICU_NOT_SUPERVISOR_STATION_TC,
		/*!< 10004 : Reception d'une telecommande ne provenant pas de la
		 * supervision. */
		ERR_ICU_NOT_APP_MSG,
		/*!< 10005 : Reception d'un message de type autre que application.*/
		ERR_ICU_DESYNC,
		/*!< 10006 : Desynchronisation en reception de messages de la CU. */
		ERR_ICU_INVALID_APP_MSG,
		/*!< 10007 : Le message applicatif recu est invalide */
		ERR_ICU_PARAM_REMOTE_DRIVER_ID,
		/*!< 10008 : Paramétrage du routage invalide. */
		ERR_ICU_PARAM_IP,
		/*!< 10009 : IP de la charge utile non renseignée */
		ERR_ICU_PARAM_SERIAL_ID,
		/*!< 10010 : Port com non renseignée */
		ERR_ICU_PARAM_PARITY,
		/*!< 10011 : Parité non renseignée */
		ERR_ICU_PARAM_NBSTOP,
		/*!< 10012 : Bit de stop non renseignée */
		ERR_ICU_PARAM_COM_TYPE,
		/*!< 10013 : Protocole de communication non renseigné */
		ERR_ICU_PARAM_CHECKSUM,
		/*!< 10014 : Type de checksum non renseigné */
        ERR_ICU_INVALID_ICU_MSG_SIZE,
        /*!< 10015 : La taille du message applicatif recu est invalide */
        ERR_ICU_UNHANDLED_APP_MSG,
        /*!< 10016 : Réception d'un message applicatif d'identifiant inconnu */
        ERR_ICU_PARAM_STOCK_ID,
        /*!< 10017 : Identifiant du canal permanent invalide */
        ERR_ICU_UNHANDLED_PULSE,
        /*!< 10018 : Réception d'un pulse d'identifiant inconnu */
	};
        
protected:
            
private:
        
	/*!
	 *   Identifiants des télémesures du driver
	 */
	enum
	{
		TM_DEBUG_ID = 1
		/*!< ID = 1 : TM de debug */
	};

	/*!
	 *   Identifiants des télécommandes du driver
	 */
	enum
	{
		TC_DEBUG_ID = 0,
		/*!< ID = 0 : TC d'intervention */
		TC_RAZDEBUG_ID,
		/*!< ID = 1 : TC de réinitialisation des données de debug */
	};

	/*!
	 *   Identifiant du sens du flux pour le stockage
	 */
	enum
	{
		FLOW_TO_CU = 0,
		/*!< Identifiant d'un flux émis par la charge utile */
		FLOW_FROM_CU = 1
		/*!< Identifiant d'un flux reçu par la charge utile */
	};

    /*!
     *   Identifiant indiquant l'état de connexion avec la charge utile
     */
	enum
	{
	    CU_CONNECTION_UNKNOWN = 0,
	    /*!< Etat de la connexion inconnu car non contrôlée */
	    CU_CONNECTION_LOST = 1,
	    /*!< Etat de la connexion OK */
        CU_CONNECTION_ALIVE = 2,
	    /*!< Etat de la connexion KO */
	};
    
//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas créer d'espace libre entre les données
#pragma pack(push, Alignment1Byte, 1)

	/*!
	 *  \brief Structure contenant les donnees de la telemesure de debug
	 */
	struct eMsgTmDebug
	{
		unsigned int uiNbSendOk;
		/*!< Nombre de trames emises sans erreur */
		unsigned int uiNbSendNok;
		/*!< Nombre de trames emises avec erreur */
		unsigned int uiNbRecvOk;
		/*!< Nombre de trames correctement recu */
		unsigned int uiNbRecvNok;
		/*!< Nombre de trames mal recu */
		unsigned int uiNbChecksumNok;
		/*!< Nombre de checksum en erreur */
		unsigned int uiNbLostMsg;
		/*!< Nombre de desequencements */
		unsigned int uiNbLostCu;
		/*!< Nombre de pertes de la charge utile */
		unsigned int uiNbRoutedMsgOk;
		/*!< Nombre de messages routés */
		unsigned int uiNbRoutedMsgNok;
		/*!< Nombre de messages trop grands reçus depuis le driver tiers */
		unsigned int uiNbSaturations;
		/*!< Nombre de dépassement de la pile de message */
		unsigned int uiNbStoredMsg;
		/*!< Nombre de messages stockés */
        unsigned char ucConnexionState;
        /*!< Etat de la connexion avec la charge utile */
	};

	/*!
	 *  Entete d'un message de stockage du driver interface charge utile
	 */
	struct eIcuStockageHeader
	{
		timespec Date;
		/*!< Date de réception du message */
		unsigned char ucFlowId;
		/*!<  Identifiant du sens du flux */
		unsigned short usSize;
		/*!<  Taille du message */
		unsigned char ucMsgId;
		/*!<  Identifiant du message */
	};

	/*!
	 *  Structure définissant le format des messages à stocker
	 */
	struct eIcuStockageMsg
	{
        unsigned short usSyncByte;
        /*!< Information de début de trame donnant le nombre d'octets stockés */
		eIcuStockageHeader Header;
		/*!< Header du msg de stockage */
		char tcData[SIZE_ICU_MSG_MAX];
		/*!< Message à stocker */
	};

//On réinstalle le type d'alignement préalablement sauvegardé
#pragma pack(pop, Alignment1Byte)

    /*!
     *  Structure définissant les paramètres du driver
     */
    struct eDriverParam
    {
        unsigned int uiTimeOutCu;
        /*!< Durée maximum avant la connexion soit considérée comme perdue */
        bool bCheckCuActivity;
        /*!< Indique si l'état de présence de la connexion doit être surveillé */
        bool bStockageUsed;
        /*!< Indique si les messages doivent être stockés */
        unsigned char ucChecksumType;
        /*!< Indique le type de checksum à utiliser */
    };

	/*!
	 *   Variable utilisée pour contenir la configuration du driver manager.
	 */
	CBASConfigDrv * m_pConfig;

	/*!
	 *   Variable de communication avec la charge utile
	 */
	CCnx * m_pCnx;

	/*!
	 *   Variable mémoire partagée du confmanager.
	 */
	CShareMemory m_ShareMemory;

	/*!
	 *   Sémaphore d'accès aux données présentes dans la mémoire partagée.
	 */
	sem_t * m_pShareMemSem;

	/*!
	 *   Pointeur sur la mémoire partagée.
	 */
	eIfApp * m_pShareMem;

	/*!
	 *   Timer utilise pour generer un evenement.
	 */
	int m_TimerId;

	/*!
	*   Variable utilisée pour definir le rythme du timer.
	*/
	struct _itimer m_itime;

	/*!
	 *   Variable evenement genere par le timer et associe a un pulse.
	 */
	sigevent m_Event;

	/*!
	 *   Variable evenement generee lorsque des donnees sont a lire.
	 */
	sigevent m_EventRcvData;

	/*!
	 *   Variable utilisée pour l'emission periodique de la TM de debug.
	 */
	CPeriodicManage m_SendDataTmDebugRate;

	/*!
	 *   Variable utilisée pour valider l'envoi des TM debug jusqu'au sol (sinon
	 * transmises pour stockage).
	 */
	CPeriodicManage m_SaveAndSendDataTmDebugRate;

	/*!
	 *   Variable utilisée pour l'émission periodique de l'état driver
	 */
	CPeriodicManage m_SendStateRate;

    /*!
     *   Instance de la classe de calcul de checksum
     */
    CChecksum m_Checksum;

    /*!
     *   Instance de la table de routage des messages
     */
    CRouteManager m_RouteManager;

    /*!
     *   Configuration du driver
     */
    eDriverParam m_Param;

	/*!
	 *   Variable compteur utilisée pour détecter la perte de la CU
	 */
	int m_iCuActivityCounter;

	/*!
	 *   Variable utilisée pour contenir le nombre de perte de la charge
	 *   utile
	 */
	unsigned int m_uiNbLostCu;

	/*!
	 *   Nombre de message arrivés sur une pile complète en entrée d'une VM
	 */
	unsigned int m_uiNbSaturations;

	/*!
	 *   Nombre de message issus du driver tiers routés
	 */
	unsigned int m_uiNbRoutedMsgOk;

	/*!
	 *   Nombre de mauvais messages issus des drivers tiers
	 */
	unsigned int m_uiNbRoutedMsgNok;

    /*!
     *   Nombre de messages stockés
     */
    unsigned int m_uiNbStoredMsg;
    
    /*!
     *    Variable comptabilisant les trames émises
     */
    unsigned short m_usSentFrameCounter;

    /*------------------------------------------------------------------------*/
    /* Méthodes d'instance                                                    */
    /*------------------------------------------------------------------------*/
    
public:

	/*!
	 *   \brief Constructeur de la classe.
	 */
	CInterfaceCU();

	/*!
	 *   \brief Destructeur de la classe.
	 */
	~CInterfaceCU();

	/*!
	 *   \brief Lance le driver.
	 */
	void Start( int argc, char * argv[] );

protected:

private:

	/*!
	 *   \brief Initialise les donnees du driver.
	 */
	void Init();

	/*!
	 *   \brief Réception d'un message émis par charge utile
	 */
	void IReceiveMsgExt( int iCnx, char * pBuffer, int iSize );

	/*!
	 *   \brief Ouvre la mémoire partagee avec ISaGRAF.
	 */
	void OpenShareMem();

	/*!
	 *   \brief Récupère la configuration spécifique du confmanager.
	 */
	void GetSpecificConfig();

	/*!
	 *   \brief Méthode appelée pour demander au driver de se configurer.
	 */
	bool IFAWConf();

	/*!
	 *   \brief Méthode appelée pour demander au driver de se déconfigurer.
	 */
	bool IFAWDeconf();

	/*!
	 *   \brief Méthode appelée pour mettre en marche le driver.
	 */
	bool IFAWRun();

	/*!
	 *   \brief Méthode appelée pour arreter le driver.
	 */
	bool IFAWStop();

	/*!
	 *   \brief Notifie la réception d'un pulse.
	 */
	bool IFAWReceivePulse( int iCode, int iValue );

	/*!
	 *   \brief Notifie la réception d'une télécommande.
	 */
	bool IBASReceiveTc( int iIdMsg,
						bool bStationSol,
						int iSize,
						char * pData );

	/*!
	 *   \brief Notifie la reception d'un message d'application.
	 */
	bool IBASReceiveMsg( eMsgApp * peMsg, int iSize );

    /*!
     *   \brief Méthode appelée pour traiter les messages émis par un driver
     */
	bool OnReceiveMsgDriver( const char * pData, int iSize, unsigned int uiSrc );

    /*!
     *   \brief Méthode appelée pour traiter les messages émis par une VM
     */
	bool OnReceiveMsgVm( const char * pData, int iSize );

	/*!
	 *   \brief Effectue l'émission d'une TM de debug
	 */
	void SendDebugTm( bool bNoEmission );

    /*!
     *   \brief Emet un message vers la charge utile
     */
	void SendMsgCu( const char* pcMsg, unsigned int uiSize, unsigned int uiId );
	/*!
	 *   \brief Créé une connexion de type udp
	 */
	CCnx * CreateUdpCnx();

	/*!
	 *   \brief Créé une connexion de type série
	 */
	CCnx * CreateComCnx();

	/*!
	 *   \brief Formate un message avant de l'envoyer au driver de stockage
	 */
	void StoreMsg( const char * pcMsg, unsigned int uiMsgSize,
                   unsigned int uiMsgId, unsigned char ucFlowDirection );

	/*!
	 *   \brief Récupère la table de routage définie dans le fichier NPConf.xml
	 */
	void GetRouteConfig();

    /*!
     *   \brief Construit le nom du canal de stockage évènementiel
     */
	string GetEventChannelName() const;
};

/* @} */

#endif //_CINTERFACECU_H_
