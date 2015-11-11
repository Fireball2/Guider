/*! \file CInterfaceCU.h
 *  \brief Sp�cification de la classe principale du driver ICU
 *
 */

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//  Auteur : R�mi THONNERIEUX - CIO Informatique Industrielle
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
 *  \brief Classe utilis�e pour implementer le driver Interface Charge Utile.
 *
 *  La classe CInterfaceCU est utilis�e pour interfacer une charge utile.
 *  La classe herite de la classe CBASDrv pour surcharger et utiliser les 
 *  m�thodes d'un driver et de la classe IReceiveProtMsg pour recevoir les 
 *  messages de la charge utile.
 * 
 *  La classe utilise les services d'emission de telemesures et de reception
 *  de telecommandes pour s'interfacer avec la supervision.
 * 
 *  La classe utilise le service de reception de messages d'application pour 
 *  recevoir les messages �mis par l'I/O driver ISaGRAF et les drivers tiers.
 * 
 *  La classe utilise le stockage de donnees pour les messages �chang�s 
 *  entre la charge utile et le driver.
 * 
 *  Les parametres de configuration du driver sont:
 *  - PeriodeTMDebug qui fournit la periode d'�mission en secondes de la TM de
 *    debug
 *  - Interface qui permet de s�lectionner le type de communication utilis�e
 *    pour interfacer la charge utile
 *  - ActivationStockage qui permet d'indiquer que le stockage est activ�
 *  - ActivationRoutage qui permet d'indiquer que le routage est activ�
 *  - IdRootDriverX o� X est un num�ro, permet d'�num�rer une liste
 *    d'identifiants de driver � router
 *  - IdRootMsgX o� X est un num�ro, permet d'indiquer l'identifiant de
 *    message correspondant � l'identifiant de driver cit� pr�c�demment
 *  - TimeoutCU qui permet de s�lectionner le timeout de la charge utile
 *  - TypeChecksum qui permet de choisir le type de checksum � utiliser
 *    (SUM16 ou XOR)
 *  
 *  Il existe �galement des param�tres de configuration propres au type
 *  d'interface choisi :
 * 
 *  Pour une liaison s�rie :
 *  - NumCom qui permet d'indiquer le num�ro du port s�rie utilis�
 *  - Bauderate qui permet de s�lectionner la vitesse d'�mission des donn�es
 *  - NbBitsStop qui permet d'indiquer le nombre de bits de stop
 *  - Parite qui permet d'indiquer la parit�
 * 
 *  Pour une liaison UDP/IP :
 *  - AdresseIpDistante qui permet de s�lectionner l'adresse IP de la charge
 *    utile
 *  - PortUdpLocal qui permet d'indiquer le port utilis� en r�ception
 *  - PortUdpDistant qui permet d'indiquer le port utilis� en �mission
 * 
 *  Les telemesures �mises sont:
 *  - La t�l�mesure de debug qui fournit des donn�es de diagnostic de la liaison
 *    avec la charge utile
 * 
 *  Les t�l�commandes re�ues sont les t�l�commandes de base (Debug et RazDebug)
 */
class CInterfaceCU : public CBASDrv, public IReceiveProtMsg
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/    
    
public:

	/*!
	 *   Erreurs lev�es par la classe
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
		/*!< 10008 : Param�trage du routage invalide. */
		ERR_ICU_PARAM_IP,
		/*!< 10009 : IP de la charge utile non renseign�e */
		ERR_ICU_PARAM_SERIAL_ID,
		/*!< 10010 : Port com non renseign�e */
		ERR_ICU_PARAM_PARITY,
		/*!< 10011 : Parit� non renseign�e */
		ERR_ICU_PARAM_NBSTOP,
		/*!< 10012 : Bit de stop non renseign�e */
		ERR_ICU_PARAM_COM_TYPE,
		/*!< 10013 : Protocole de communication non renseign� */
		ERR_ICU_PARAM_CHECKSUM,
		/*!< 10014 : Type de checksum non renseign� */
        ERR_ICU_INVALID_ICU_MSG_SIZE,
        /*!< 10015 : La taille du message applicatif recu est invalide */
        ERR_ICU_UNHANDLED_APP_MSG,
        /*!< 10016 : R�ception d'un message applicatif d'identifiant inconnu */
        ERR_ICU_PARAM_STOCK_ID,
        /*!< 10017 : Identifiant du canal permanent invalide */
        ERR_ICU_UNHANDLED_PULSE,
        /*!< 10018 : R�ception d'un pulse d'identifiant inconnu */
	};
        
protected:
            
private:
        
	/*!
	 *   Identifiants des t�l�mesures du driver
	 */
	enum
	{
		TM_DEBUG_ID = 1
		/*!< ID = 1 : TM de debug */
	};

	/*!
	 *   Identifiants des t�l�commandes du driver
	 */
	enum
	{
		TC_DEBUG_ID = 0,
		/*!< ID = 0 : TC d'intervention */
		TC_RAZDEBUG_ID,
		/*!< ID = 1 : TC de r�initialisation des donn�es de debug */
	};

	/*!
	 *   Identifiant du sens du flux pour le stockage
	 */
	enum
	{
		FLOW_TO_CU = 0,
		/*!< Identifiant d'un flux �mis par la charge utile */
		FLOW_FROM_CU = 1
		/*!< Identifiant d'un flux re�u par la charge utile */
	};

    /*!
     *   Identifiant indiquant l'�tat de connexion avec la charge utile
     */
	enum
	{
	    CU_CONNECTION_UNKNOWN = 0,
	    /*!< Etat de la connexion inconnu car non contr�l�e */
	    CU_CONNECTION_LOST = 1,
	    /*!< Etat de la connexion OK */
        CU_CONNECTION_ALIVE = 2,
	    /*!< Etat de la connexion KO */
	};
    
//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas cr�er d'espace libre entre les donn�es
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
		/*!< Nombre de messages rout�s */
		unsigned int uiNbRoutedMsgNok;
		/*!< Nombre de messages trop grands re�us depuis le driver tiers */
		unsigned int uiNbSaturations;
		/*!< Nombre de d�passement de la pile de message */
		unsigned int uiNbStoredMsg;
		/*!< Nombre de messages stock�s */
        unsigned char ucConnexionState;
        /*!< Etat de la connexion avec la charge utile */
	};

	/*!
	 *  Entete d'un message de stockage du driver interface charge utile
	 */
	struct eIcuStockageHeader
	{
		timespec Date;
		/*!< Date de r�ception du message */
		unsigned char ucFlowId;
		/*!<  Identifiant du sens du flux */
		unsigned short usSize;
		/*!<  Taille du message */
		unsigned char ucMsgId;
		/*!<  Identifiant du message */
	};

	/*!
	 *  Structure d�finissant le format des messages � stocker
	 */
	struct eIcuStockageMsg
	{
        unsigned short usSyncByte;
        /*!< Information de d�but de trame donnant le nombre d'octets stock�s */
		eIcuStockageHeader Header;
		/*!< Header du msg de stockage */
		char tcData[SIZE_ICU_MSG_MAX];
		/*!< Message � stocker */
	};

//On r�installe le type d'alignement pr�alablement sauvegard�
#pragma pack(pop, Alignment1Byte)

    /*!
     *  Structure d�finissant les param�tres du driver
     */
    struct eDriverParam
    {
        unsigned int uiTimeOutCu;
        /*!< Dur�e maximum avant la connexion soit consid�r�e comme perdue */
        bool bCheckCuActivity;
        /*!< Indique si l'�tat de pr�sence de la connexion doit �tre surveill� */
        bool bStockageUsed;
        /*!< Indique si les messages doivent �tre stock�s */
        unsigned char ucChecksumType;
        /*!< Indique le type de checksum � utiliser */
    };

	/*!
	 *   Variable utilis�e pour contenir la configuration du driver manager.
	 */
	CBASConfigDrv * m_pConfig;

	/*!
	 *   Variable de communication avec la charge utile
	 */
	CCnx * m_pCnx;

	/*!
	 *   Variable m�moire partag�e du confmanager.
	 */
	CShareMemory m_ShareMemory;

	/*!
	 *   S�maphore d'acc�s aux donn�es pr�sentes dans la m�moire partag�e.
	 */
	sem_t * m_pShareMemSem;

	/*!
	 *   Pointeur sur la m�moire partag�e.
	 */
	eIfApp * m_pShareMem;

	/*!
	 *   Timer utilise pour generer un evenement.
	 */
	int m_TimerId;

	/*!
	*   Variable utilis�e pour definir le rythme du timer.
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
	 *   Variable utilis�e pour l'emission periodique de la TM de debug.
	 */
	CPeriodicManage m_SendDataTmDebugRate;

	/*!
	 *   Variable utilis�e pour valider l'envoi des TM debug jusqu'au sol (sinon
	 * transmises pour stockage).
	 */
	CPeriodicManage m_SaveAndSendDataTmDebugRate;

	/*!
	 *   Variable utilis�e pour l'�mission periodique de l'�tat driver
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
	 *   Variable compteur utilis�e pour d�tecter la perte de la CU
	 */
	int m_iCuActivityCounter;

	/*!
	 *   Variable utilis�e pour contenir le nombre de perte de la charge
	 *   utile
	 */
	unsigned int m_uiNbLostCu;

	/*!
	 *   Nombre de message arriv�s sur une pile compl�te en entr�e d'une VM
	 */
	unsigned int m_uiNbSaturations;

	/*!
	 *   Nombre de message issus du driver tiers rout�s
	 */
	unsigned int m_uiNbRoutedMsgOk;

	/*!
	 *   Nombre de mauvais messages issus des drivers tiers
	 */
	unsigned int m_uiNbRoutedMsgNok;

    /*!
     *   Nombre de messages stock�s
     */
    unsigned int m_uiNbStoredMsg;
    
    /*!
     *    Variable comptabilisant les trames �mises
     */
    unsigned short m_usSentFrameCounter;

    /*------------------------------------------------------------------------*/
    /* M�thodes d'instance                                                    */
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
	 *   \brief R�ception d'un message �mis par charge utile
	 */
	void IReceiveMsgExt( int iCnx, char * pBuffer, int iSize );

	/*!
	 *   \brief Ouvre la m�moire partagee avec ISaGRAF.
	 */
	void OpenShareMem();

	/*!
	 *   \brief R�cup�re la configuration sp�cifique du confmanager.
	 */
	void GetSpecificConfig();

	/*!
	 *   \brief M�thode appel�e pour demander au driver de se configurer.
	 */
	bool IFAWConf();

	/*!
	 *   \brief M�thode appel�e pour demander au driver de se d�configurer.
	 */
	bool IFAWDeconf();

	/*!
	 *   \brief M�thode appel�e pour mettre en marche le driver.
	 */
	bool IFAWRun();

	/*!
	 *   \brief M�thode appel�e pour arreter le driver.
	 */
	bool IFAWStop();

	/*!
	 *   \brief Notifie la r�ception d'un pulse.
	 */
	bool IFAWReceivePulse( int iCode, int iValue );

	/*!
	 *   \brief Notifie la r�ception d'une t�l�commande.
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
     *   \brief M�thode appel�e pour traiter les messages �mis par un driver
     */
	bool OnReceiveMsgDriver( const char * pData, int iSize, unsigned int uiSrc );

    /*!
     *   \brief M�thode appel�e pour traiter les messages �mis par une VM
     */
	bool OnReceiveMsgVm( const char * pData, int iSize );

	/*!
	 *   \brief Effectue l'�mission d'une TM de debug
	 */
	void SendDebugTm( bool bNoEmission );

    /*!
     *   \brief Emet un message vers la charge utile
     */
	void SendMsgCu( const char* pcMsg, unsigned int uiSize, unsigned int uiId );
	/*!
	 *   \brief Cr�� une connexion de type udp
	 */
	CCnx * CreateUdpCnx();

	/*!
	 *   \brief Cr�� une connexion de type s�rie
	 */
	CCnx * CreateComCnx();

	/*!
	 *   \brief Formate un message avant de l'envoyer au driver de stockage
	 */
	void StoreMsg( const char * pcMsg, unsigned int uiMsgSize,
                   unsigned int uiMsgId, unsigned char ucFlowDirection );

	/*!
	 *   \brief R�cup�re la table de routage d�finie dans le fichier NPConf.xml
	 */
	void GetRouteConfig();

    /*!
     *   \brief Construit le nom du canal de stockage �v�nementiel
     */
	string GetEventChannelName() const;
};

/* @} */

#endif //_CINTERFACECU_H_
