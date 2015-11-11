/*! \file CAscDtu.cpp
 *  \brief Implementation de la classe ASC DTU
 */
 
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/12/11  
//	- Prise en compte de BL-FI-2315
//	- Modification de la methode Init()
//					
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 06/02/12  
//	- Prise en compte de BL-FI-2404
//	- Modification de la methode IFAWReceivePulse()
//					
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//  - Prise en compte de BL-DM-2474
//      - Remise � niveau de la documentation Doxygen
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//  - Prise en compte de BL-DM-2471
//	- Modification de la methode Init()
//	- Modification de la methode GetSpecificConfig()
//	- Modification de la methode IBASReceiveTc()
//  - Modification de la methode IFAWReceivePulse()
//	- Modification de la methode SendDebugTm()
//////////////////////////////////////////////////////////////////////////////// 

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  Auteur : Vincent ROLLY - CIO
//	- Prise en compte de BL-DM-2470
//	- Modification de la m�thode CAscDtu()
//	- Rajout de l'inclusion du fichier "Version.h" contenant la version du driver
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  Auteur : R�mi THONNERIEUX - CIO
//  - Prise en compte de BL-FI-4222
//      - Modification de la m�thode Init()
//      - Modification de la m�thode IReceiveMsgExt()
//      - Modification de la m�thode IBASReceivePPS()
//      - Modification de la methode IFAWReceivePulse()
//      - Modification de la m�thode IBASReceiveTimeResync()
//      - Ajout variable NB_PPS_RECV_TIMEOUT
//      - Ajout variable PPS_LSOT
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//  Auteur : R�mi THONNERIEUX - CIO
//  - Prise en compte de BL-FI-4929
//      - Modification de la m�thode IReceiveMsgExt()
//  - Prise en compte de BL-DM-4197
//		- Modification de la m�thode IBASReceiveTc()
//		- Modfiication de la m�thode SendDebugTm()
//		- Modification de la m�thode Init()
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 07/10/14
//  Auteur : R�mi THONNERIEUX - CIO
//  - Prise en compte de BL-DM-XXXX
//      - Modification de la m�thode StockBroadcastAttitudeFrame()
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 16/03/15
//  Auteur : R�mi THONNERIEUX - CIO
//	- Prise en compte de BL-FI5575
//		- Modification de la m�thode GetSpecificConfig()
////////////////////////////////////////////////////////////////////////////////
 
#include "CAscDtu.h"
#include "CConv.h"
#include <gulliver.h>
#include <math.h>
#include "IfCPUCard.h"
#include "IfStockage.h"
#include "Version.h"

/*! \addtogroup ascdtu */
/* @{ */

//#define CASCDTU_DEBUG
/// @cond CASCDTU_DEBUG 
/*! \def CASCDTU_DEBUG
 *    Selecteur pour activer ou inhiber l'affichage de trace sur la console
 */
/// @endcond

//#define CASCDTU_DEBUG_COOR
/// @cond CASCDTU_DEBUG_COOR 
/*! \def CASCDTU_DEBUG_COOR
 *    Selecteur pour activer ou inhiber l'affichage de trace sur la console
 */
/// @endcond

//#define CASCDTU_DEBUG_TIME
/// @cond CASCDTU_DEBUG_TIME 
/*! \def CASCDTU_DEBUG_TIME
 *    Selecteur pour activer ou inhiber l'affichage de trace sur la console
 */
/// @endcond

/*!
 *   Portee : classe CAscDtu\n
 *   Identifiant du pulse utilisee pour cadencer les traitements du driver.
 */
const int TIMER_PULSE = CBASDrv::FIRST_PULSE_FREE;

/*!
 *   Portee : classe CAscDtu\n
 *   Identifiant du pulse utilisee pour notifier la presence de donnees sur
 *   la voie de reception des messages emis par le DTU.
 */
const int RCV_MSG_PULSE = CBASDrv::FIRST_PULSE_FREE + 1;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour fixer la duree d'emission de la TM de debug
 */
const int DEBUG_TM_DURATION_TIME = 60 * 3; //3 minutes d'�mission

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour definir le nombre de cycle a partir duquel le
 *   DTU est detectee comme perdue. Au bout de 5s l'ASC DTU est considere comme
 *   perdu.
 */	
const int NB_CYCLE_CPT_LIFE = 5;

/*!
 *   Portee : classe CAscDtu\n
 *   Taille maximale d'un message pouvant etre emis. La taille est fixee par
 * 	 le driver de telecommunication.
 */
const int SIZE_MAX_MSG = 243;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour indiquer qu'une option de parametrage n'est pas
 *   utilisee.
 */
const unsigned int OPTION_NOT_USED = 0xFFFFFFFF;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour indiquer que le CHU ou le DPU n'est pas en vie
 */
const unsigned int CHU_DPU_NOT_ALIVE = 0;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour fixer la valeur du compteur de vie d'un CHU et
 *   du DPU
 */
const unsigned int CHU_DPU_CPT_LIFE = 5;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour fixer la valeur du residus lorsque l'attitude est
 *   invalide
 */
const unsigned char RESIDUS_NOK = 255;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour positionner un validant a valide 
 */	
const unsigned char VAL = 0x01;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour positionner un validant a invalide 
 */	
const unsigned char INVAL = 0x00;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour definir au bout de combien de PPS recu le message
 *   de resynchronisation par PPS est emis au DPU.
 */	
const unsigned int NB_PPS_RECV_BEFORE_SYNC = 10;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour definir au bout de combien de temps le PPS est
 *   declaree perdu..
 */ 
const unsigned int NB_PPS_RECV_TIMEOUT = 2;

/*!
 *   Portee : classe CAscDtu\n
 *   Constante utilisee pour definir la valeur corredpondant a une perte PPS
 */ 
const unsigned int PPS_LOST = 0;

/* @} */


/******************************************************************************/
/*																			  */
/*							METHODES PUBLIC 								  */
/*																			  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CAscDtu					                                                  */
/*----------------------------------------------------------------------------*/
/*! Effectue l'initialisation de la taille maximale d'un message encapsule dans
 *  le protocole et alloue la connexion.
 *  connexion.
 *
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//	- BL-DM-2470
//		-Appel de la m�thode FAWSetVersion permettant de param�trer le num�ro de
//		 version du driver
//
////////////////////////////////////////////////////////////////////////////////
CAscDtu::CAscDtu() : CBASDrv()
{
	//D�clenchement du timer (toutes les secondes)						
	const uint64_t DECLENCHEMENT_TIMER = 1000000000;	
	
	//Utilisation du timer sans r�p�tition	
	const uint64_t SANS_REPETITION = 0;	
			
	//Initialisation de la variable temps du timer utilis� pour le r�veil
	//p�riodique		
    m_itime.nsec = DECLENCHEMENT_TIMER;	//D�lai avant le d�clenchement du timer    
    m_itime.interval_nsec = SANS_REPETITION; //Pas de d�clenchement p�riodique	
    
	//Initialise les donnees du driver
	Init();  	

	// Param�trage de la version du driver
    FAWSetVersion(APP_VERSION);  
}

/*----------------------------------------------------------------------------*/
/* ~CAscDtu				                                                  	  */
/*----------------------------------------------------------------------------*/
/*! Pas de traitement.
 *
 */
/*----------------------------------------------------------------------------*/
CAscDtu::~CAscDtu()
{
}


/*----------------------------------------------------------------------------*/
/* Start					                                                  */
/*----------------------------------------------------------------------------*/
/*! Initialise le driver. Initialise un evenement pour reveiller periodique
 *  le driver. Initialise un deuxieme evenement pour etre notifie de la 
 *  presence de donnees sur le lien de communication. Enfin, demarre le driver.
 *
 *	\param       [in] argc : Nombre d'arguments dans la ligne de commande.
 *	\param       [in] argv[] : Tableau d'arguments de type string.
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
void CAscDtu::Start( int argc, char *argv[] )
{	
	//Initialisation du driver
	BASInit( argc, argv );	
    	
	//Cr�ation d'un �v�nement utilis� pour g�n�rer un r�veil p�riodique				
	m_EventTimer = FAWCreatePulse( TIMER_PULSE, 0 );
    	
	//Cr�ation d'un �v�nement utilis� pour signaler la pr�sence de donn�es � 
	//lire.				
	m_EventRcvData = FAWCreatePulse( RCV_MSG_PULSE, 0 );	    	
		
	//Lancement du driver
	BASStart();				
}


/******************************************************************************/
/*																			  */
/*							METHODES PRIVATE 								  */
/*																			  */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Init					                                                  	  */
/*----------------------------------------------------------------------------*/
/*! Effectue l'initialisation des variables membres et l'initialisation des
 *  variables g�rant les traitements p�riodiques.
 *
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/12/11  
//	- BL-FI-2315 : - Initialisation de la variable m_uiAscDtuLive
//					
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/11/12
//	- BL-DM-2471 : - Utilisation d'un objet timer de type CPeriodicManage 
//	  				 permettant de param�trer le cadencement de 
//					 l'�mission des TM Debug.
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  - BL-FI-4222 : - Modification de la periode d'emission du message de resync
//                 au DTU, passage de 1 minutes � 10s et ajout de l'init du flag
//                 de validite du PPS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//	- BL-DM-4197 : - Initialisation du compteur m_uiNbMsgRcvFromEGSENok
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::Init()
{					
	//Periode d'�mission de la t�l�mesure de debug par d�faut
	const int DEBUG_TM_SEND_DEFAULT_RATE = 10;
	
	//Periode d'emission de la t�l�mesure d'attitude des CHU.
	const int ATTITUDE_TM_SEND = 5;
	
	//Emission toutes les 10s par defaut
	const int SYNCHRO_MSG_DURATION_TIME = 10;
    
    //Periode d'�mission de la t�l�mesure vers le sol
    const int DEBUG_TM_ALWAYS_SEND = 0;
		
	int iChu; //Variable utilisee pour acceder aux differents CHU	
	
	//Initialisation des variables membres.
	m_pConfig = NULL;		
	m_pShareMem = NULL;	
	m_pShareMemSem = NULL;
	
	//Le canal �v�nementiel utilis�.
	m_uiChannelId = CHANNEL_EVT_MIN;
	        			
	//Initialisation de la p�riode par d�faut de la TM de debug
	m_SendDataTmDebugRate.Init( DEBUG_TM_SEND_DEFAULT_RATE);		
	
    //Initialisation de la dur�e d'�mission par d�faut de la TM de debug
    m_SaveAndSendDataTmDebugRate.Init(  DEBUG_TM_ALWAYS_SEND, 
                                        DEBUG_TM_DURATION_TIME);
	
	//Par d�faut la TM de debug n'est pas �mise.
	m_SaveAndSendDataTmDebugRate.SetInhibition();
	
	//Initialisation de l'emission de la TC de synchronisation horaire au DTU
	m_SendClockSyncMsg.Init( SYNCHRO_MSG_DURATION_TIME  );
	    
    //Initialisation des compteurs de maintenance
    m_uiNbAscDtuLost = 0;
    m_uiNbMsgSendToEGSE = 0;
    m_uiNbMsgRcvFromEGSE = 0;
    m_uiNbMsgRcvFromEGSENok = 0;
    uiNbRebuiltErrMsgFromEGSE = 0;
    
    //Initialisation des donnees de debug de la classe connexion
    m_Cnx.ResetDebugData();	
        
    //Par defaut l'ASC DTU est en vie
    m_bAscDtuAlive = false;  
    m_uiAscDtuLive = NB_CYCLE_CPT_LIFE;    
    
    //Initialise l'�tat de la gestion en reception des sous messages EGSE
	m_iEgseBufferState = FIRST_SS_MSG_STATE;  
	m_iEgseBufferCpt = 0;
	
	//Par defaut la date n'est pas forcee
	m_bDateIsForced = false;

	//Par defaut on initialise la variable au maximum	
    m_uiPpsCpt = NB_PPS_RECV_BEFORE_SYNC;
	
    //Initialisation du compteur de vie de chaque CHU � invalide
    //Invalidation du residus
    //Inhibition de l'emission de la trame d'attitude de chaque CHU
    for( iChu = 0; iChu < NB_CHU; iChu++ )
    {   
	    m_ChuState[iChu].uiCptLive = CHU_DPU_NOT_ALIVE; 
	    m_ChuState[iChu].ucResidus = RESIDUS_NOK;
	    m_ChuState[iChu].bSendAttitude = false;
    }	
        
    //Initialisation de la p�riode d'�mission de l'attitude.
    m_SendAttitudeTm.Init( ATTITUDE_TM_SEND ); 
    
    //Initialisation de la validit� du PPS, par defaut valide
    m_uiPpsAlive = NB_PPS_RECV_TIMEOUT;               
}

/*----------------------------------------------------------------------------*/
/* GetSpecificConfig		                                                  */
/*----------------------------------------------------------------------------*/
/*! 
 *
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/11/12
//	- BL-DM-2471 : - Param�trage de l'objet CPeriodicManage pour les �missions
//					 des TM Debug
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 16/03/15
//	- BL-DM-5575 : - Utilisation de la m�thode StringToPosInt pour les parametres
//	strictement positif
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::GetSpecificConfig()	
{			
	const string PERIODE_EMISSION_TM_DEBUG = "PeriodeTMDebug";	
		
	const string ID_DRV = "DriverId";	
	
	const string IDENTIFIANT_COM = "NumCom";		
	
	const int DTU_BAUDERATE = 115200;
	
	//Premier identifiant de driver autorise
	const unsigned int FIRST_DRIVERID_ALLOWED = 30;

	//Dernier identifiant de driver autorise
	const unsigned int LAST_DRIVERID_ALLOWED = 100;
	
	//Variable utilis�e pour acc�der � la configuration de la CPU
	eCPUConf * pShareMem = NULL; 
	
	//Variable utilisee pour acceder a la memoire partagee de la CPU
	CShareMemory ShareMemory;
	
		
	string strSerialComName = "/dev/ser"; //Nom du port device utilise
			
	string strValue;	//Variable tampon utilis�e pour r�cup�rer les param�tres	
	
	//On r�cup�re la p�riode d'�mission de la t�l�mesure de debug en seconde
	//si elle est d�fini
	strValue = m_pConfig->GetParamValue( PERIODE_EMISSION_TM_DEBUG );
	
	if (strValue.empty() == false )
	{
		m_SendDataTmDebugRate.Init( CConv::StringToPosInt( strValue, LOC ));
	}
		
	//On r�cup�re l'identifiant du driver vers lequel les messages d'attitudes
	//des CHU doivent �tre envoy�s si ce dernier est d�fini.
	strValue = m_pConfig->GetParamValue( ID_DRV );
	
	if (strValue.empty() == false )
	{
		m_uiDrvIdToSendChuAtt = CConv::StringToPosInt( strValue, LOC );
		
		//On verifie que l'identifiant de driver est bien dans la plage 
		//autorise
		if( ( m_uiDrvIdToSendChuAtt < FIRST_DRIVERID_ALLOWED ) || 
		    ( m_uiDrvIdToSendChuAtt > LAST_DRIVERID_ALLOWED ) )
		{
			CLog::LogAndRaiseException( LOC, ERR_ASCDTU_INVALID_DRIVERID,
		    							_SLOG_ERROR, m_uiDrvIdToSendChuAtt );
		}			
	}
	else
	{
		m_uiDrvIdToSendChuAtt = OPTION_NOT_USED;
	}			
	    	
	//On r�cup�re le nom du port COM utilise 
	strValue = m_pConfig->GetParamValue( IDENTIFIANT_COM );		
	
	if ( strValue.empty() == true )
	{
		//Le num�ro de port n'est pas initialis�, on leve une exception
		CLog::LogAndRaiseException( LOC, ERR_ASCDTU_INVALID_COM, _SLOG_ERROR );
	}

	//Initialise le nom du device au format "dev/ser<x>" ou x est le numero du
	//port.
	strSerialComName = strSerialComName + strValue;										
	
	//On fixe le nom du port de communication utilis�
	m_Cnx.SetSerialComName( strSerialComName.c_str() );		
	
	//On fixe la configuration de la communication s�rie
	m_Cnx.SetBaudRate( DTU_BAUDERATE );
	m_Cnx.SetNbStop( CSerialCom::ONE_STOP_BIT );
	m_Cnx.SetParity( CSerialCom::NO_PARITY );
	
	//Installe le handler pour r�ceptionner les messages de fa�on asynchrone
	m_Cnx.InstallReceiveHandler( &m_EventRcvData, this );
	
	//On recupere la configuration du PPS au niveau de la memoire partagee de la
	//CPU pour savoir si la synchronisation horaire s'effectue avec ou sans PPS
	pShareMem = ( eCPUConf * ) ShareMemory.Open( SHAREMEM_CPU,
												 SHAREMEM_CPU_SIZE );
	
	//Mise � jour des donn�es
	m_bPPSUsed = pShareMem->bPpsUsed;
	
	//On ferme la m�moire partag�e
	ShareMemory.Close();		
}



/*----------------------------------------------------------------------------*/
/* OpenShareMem				                                                  */
/*----------------------------------------------------------------------------*/
/*! Ouvre la memoire partagee avec ISaGRAF et initialise les donnees de la
 *  memoire. Initialise le semaphore d'acces a la memoire.
 *
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
void CAscDtu::OpenShareMem()
{		
	string strShareMemoryName;	//Variable contenant le nom de la m�moire
								//partag�e
								
	int iChu; //Variable utilisee pour acceder aux differents CHU
		
	//Cr�ation du nom de la m�moire partag�e en fonction d'une base fixe et de
	//l'identifiant du driver
	strShareMemoryName = SHAREMEM_ASCDTU;
	
	strShareMemoryName = strShareMemoryName + "_" + 
						 CConv::IntToString( ( BASGetConfDrv() )->GetDrvId() );	
	
	//Cr�ation de la m�moire partag�e et mapping de la m�moire dans l'espace
	//adressable du driver.
	m_pShareMem = ( eIfApp * ) m_ShareMemory.Create( strShareMemoryName.c_str(),
													 SHAREMEM_ASCDTU_SIZE );
		
    //R�cup�ration du s�maphore d'acc�s � la m�moire partag�e.
    m_pShareMemSem = m_ShareMemory.GetShareMemSem();    
		   
    //Initialisation des donn�es de chaque CHU.
    for( iChu = 0; iChu < NB_CHU; iChu++ )
    {   
	    m_pShareMem->Chu[iChu].ucValidity = INVAL;
	    m_pShareMem->Chu[iChu].dQ0 = 0.0;
	    m_pShareMem->Chu[iChu].dQ1 = 0.0;
	    m_pShareMem->Chu[iChu].dQ2 = 0.0;
	    m_pShareMem->Chu[iChu].dQ3 = 0.0;
	    m_pShareMem->Chu[iChu].uiSec = 0;
	    m_pShareMem->Chu[iChu].uiNsec = 0;
    }
}

/*----------------------------------------------------------------------------*/
/* IFAWConf					                                                  */
/*----------------------------------------------------------------------------*/
/*! Effectue la configuration du driver puis recupere la configuration du driver.
 *  Effectue la configuration specifique du driver.
 *
 *  \return      true : La configuration s'est bien passe
 *  \return      false : La configuration n'a pu etre faite
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
bool CAscDtu::IFAWConf()
{
	bool 	bCr; 	//Compte rendu de la fonction
	
	bCr = CBASDrv::IFAWConf();
		
	//On effectue la r�cup�ration de la configuration et l'ouverture de la
	//memoire partagee si pas de probleme	
	if( bCr == true )
	{
		//R�cup�ration de la configuration du driver
		m_pConfig = BASGetConfDrv();	
		
		//Effectue la configuration specifique du driver
		GetSpecificConfig();		
		
		//Ouvre la m�moire partag�e
		OpenShareMem();
		
	}		
		
	return bCr;
}


/*----------------------------------------------------------------------------*/
/* IFAWDeconf				                                                  */
/*----------------------------------------------------------------------------*/
/*! Deconfigure le driver et desalloue la connexion.
 *
 *  \return      true : La deconfiguration s'est bien passe
 *  \return      false : La deconfiguration n'a pu etre faite
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
bool CAscDtu::IFAWDeconf()
{		
	bool bCr; //Compte rendu de la m�thode		
    		
	//Fermeture de la m�moire partag�e.
	m_ShareMemory.Close();			
		
	//Deconfigure le driver.
	bCr = CBASDrv::IFAWDeconf();
		
	//Initialise les donnees du driver
	Init();	
	
	return bCr;
}


/*----------------------------------------------------------------------------*/
/* IFAWRun					                                                  */
/*----------------------------------------------------------------------------*/
/*! Lance le timer pour reveiller periodiquement le dirver et ouvre la 
 *  connexion.
 *
 *  \return      true : Le passage dans l'etat run s'est bien passe
 *  \return      false : Le passage dans l'etat run n'est pas passe
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
bool CAscDtu::IFAWRun()
{
    int iCrQnx = EOK; //Compte rendu de fonctions systemes QNX	
    
    string strChannelName; //Nom du canal evenementiel	    			    
		    								   
	//Cr�ation d'un timer bas� sur la g�n�ration de l'�v�nement afin de pouvoir
	//g�n�rer un pulse toutes les secondes
	m_TimerId = TimerCreate( CLOCK_MONOTONIC, &m_EventTimer );	
		
	if( m_TimerId == QNXINTERNALERROR )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}				

	//Le timer est lanc�    								   
    iCrQnx = TimerSettime(m_TimerId, 0, &m_itime, NULL);
    
	if( iCrQnx == QNXINTERNALERROR )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}				
		
	//Ouverture du canal �v�nementiel de stockage
	strChannelName = GetChanelName();	
	
	m_Stockage.OpenChanel( m_uiChannelId, strChannelName );
	
	//On ouvre la connexion
	m_Cnx.Open();	
						
	return true;
}


/*----------------------------------------------------------------------------*/
/* IFAWStop					                                                  */
/*----------------------------------------------------------------------------*/
/*! Fixe le validant de chaque CHU a invalide (plus mis a jour par le driver),
 *  detruit le timer periodique et ferme la connexion
 *
 *  \return      true : Le passage dans l'etat run s'est bien passe
 *  \return      false : Le passage dans l'etat run n'est pas passe
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
bool CAscDtu::IFAWStop()
{	
    int iCrQnx = EOK; //Compte rendu de fonctions systemes QNX		    
	    	
	int iChu; //Variable utilisee pour acceder aux differents CHU

	//Prise du s�maphore d'acc�s � la m�moire partag�e
	iCrQnx = sem_wait( m_pShareMemSem );
	
	if( iCrQnx != EOK )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}	
		   
	//Initialisation des validants des CHU � invalide car ils ne sont plus
	//significatifs puisque le driver ne les met plus � jour.
    for( iChu = 0; iChu < NB_CHU; iChu++ )
    {   
	    m_pShareMem->Chu[iChu].ucValidity = INVAL;
    }
    
    //Lib�ration du s�maphore d'acc�s � la m�moire partag�e
	iCrQnx = sem_post( m_pShareMemSem );
	
	if( iCrQnx != EOK )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}
				    	
    //Le timer est d�truit afin de ne plus g�n�rer l'�v�nement
	iCrQnx = TimerDestroy( m_TimerId );
	
	if( iCrQnx == QNXINTERNALERROR )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}			
			
	//On ferme la connexion
	m_Cnx.Close();	
	
	//Fermeture du canal �v�nementiel de stockage
	m_Stockage.CloseChanel( m_uiChannelId );
		
	return true;	
}


/*----------------------------------------------------------------------------*/
/* IBASReceiveGpsDate			                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee sur reception du message d'heure GPS. L'heure
 *  GPS re�u est sauvegardee afin d'etre utilisee dans le cas de la 
 *  synchronisation avec PPS.
 *
 *	\param   	[in] pGpsDate : Date recue
 */
/*----------------------------------------------------------------------------*/
void CAscDtu::IBASReceiveGpsDate( struct timespec * pGpsDate )
{
	//On sauvegarde le dernier message d'heure re�u. 
	m_LastGpsDate = *pGpsDate;
	
#ifdef CASCDTU_DEBUG
	printf("Received new GpsDate\n");
#endif		
}


/*----------------------------------------------------------------------------*/
/* IBASReceiveTimeResync		                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee pour notifier la resynchronisation du calculateur
 *  et fournir l'�tat de forcage de la datation du calculateur. Sur reception
 *  on sauvegarde l'etat de forcage et si la date est forcee ou que l'on
 *  n'utilise pas le mode PPS, alors on resynchronise le DPU
 *
 *	\param   	[in] bLocked : Date recue
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  - BL-FI-4222 : - Suppression du test PPS utilisee
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::IBASReceiveTimeResync( bool bLocked )
{
    //On sauvegarde l'�tat de forcage.
    m_bDateIsForced = bLocked;  
    
	//On �met le message si la date est forcee
	if( m_bDateIsForced == true )
	{
		m_SendClockSyncMsg.ForceNow();
	}	
	
#ifdef CASCDTU_DEBUG
	printf("Received new synchro msg. Locked=%d\n", bLocked );
#endif		
	
}


/*----------------------------------------------------------------------------*/
/* IBASReceivePPS				                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee sur reception du PPS. La methode effectue
 *  l'emission du message de synchronisation horaire dans une configuration
 *  utilisation du PPS avec la date du dernier message GPS recu si ce dernier
 *  message est considere comme valide.
 *  
 *  Le message est emis a un sous multiple du PPS fixe par la constante
 *  NB_PPS_RECV_BEFORE_SYNC afin
 *
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  - BL-FI-4222 : - Ajout d'un flag permettant d'identifier la perte du PPS
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::IBASReceivePPS()
{
	//On ne transmet le message pour prise en compte du PPS que toutes les
	//NB_PPS_RECV_BEFORE_SYNC secondes.
    
    // Validation du PPS
    m_uiPpsAlive = NB_PPS_RECV_TIMEOUT;
	
	//On decremente le compteur
	m_uiPpsCpt--;
	
	if ( m_uiPpsCpt <= 0  )
	{
		m_uiPpsCpt = NB_PPS_RECV_BEFORE_SYNC;
		
		//On ne transmet le message que si le DTU est operationnel
		if( m_bAscDtuAlive  == true )
		{
			m_Cnx.SetTime( &m_LastGpsDate );
		}
	}
	
#ifdef CASCDTU_DEBUG
	printf("Received new PPS msg\n" );
#endif		
}

/*----------------------------------------------------------------------------*/
/* IFAWReceivePulse				                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee sur reception d'un pulse. La methode traitant le
 *  pulse est appelee.
 *  On gere :
 * 	-- l'etat de vie du DPU
 *  -- l'etat de vie de chaque CHU
 * 	-- l'emission du message de synchronisation horaire (mode sans PPS et date forcee)
 *  -- l'emission du message d'attitude de chaque CHU (emis toutes les 5s)
 *  -- l'emission de la telemcsure de debug
 *  -- la reception de donnees emises par le DPU
 *
 *	\param       [in] iCode : Code du pulse.
 *	\param       [in] iValue : Valeur associee au pulse.
 *  \return      true : Le pulse a ete pris en compte.
 *  \return      false : Le pulse n'a pas ete pris en compte.
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 06/02/12  
//	- BL-FI-2404 : - Modification de la gestion du compteur de vie afin de
//					 ne decrementer le compteur que s'il est superieur a 0.
//					
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  - BL-DM-2471 : - Gestion du flag NoEmission des t�l�mesures de diagnostic,
//                  passage en parametre a la fonction SendDebugTm
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  - BL-FI-4222 : - Ajout d'un test pour la validation du signal PPS, si Ko
//                  alors on test l'envoi du message de resync.
////////////////////////////////////////////////////////////////////////////////
bool CAscDtu::IFAWReceivePulse( int iCode, int iValue )
{			
	int iCrQnx;	//Retour de fonction QNX
	
	int iChu; //Variable utilisee pour acceder aux differents CHU
    
    bool bNoEmissionDebug = false;  // Flag d'emission de la TM diagnostic
		
	//Si ce n'est ni le pulse timer, ni le pulse de reception de messages
	//on quitte.	
	if( iCode == TIMER_PULSE )
	{
		if( m_uiAscDtuLive != CHU_DPU_NOT_ALIVE )
		{
			//On decremente le compteur de vie
			m_uiAscDtuLive--;
		
			//Si le compteur de vie est nul alors on indique que l'ASC DTU est perdu
			if( m_uiAscDtuLive == CHU_DPU_NOT_ALIVE )
			{								
				//On indique que le GPS est perdu
				m_bAscDtuAlive = false;
				
				//Probl�me de communication avec l'ASC DTU. L'�tat du driver est 
			    //en d�faut majeur
			    FAWSetLevelErr( DRV_ERR_MAJ );
			    
			    //On incremente le nombre de perte de l'ASC DTU
			    m_uiNbAscDtuLost++;
			}	
		}
		
	    //On traite le compteur de vie de chaque CHU
	    for( iChu = 0; iChu < NB_CHU; iChu++ )
	    {   
		    if( m_ChuState[iChu].uiCptLive != CHU_DPU_NOT_ALIVE )
			{
				//On decrement le compteur de vie
				m_ChuState[iChu].uiCptLive--;
				
				//Si le compteur de vie est nulle, le CHU est perdu. Il faut
				//placer le validant des donnees d'automatismes a invalide et
				//on met le residus du CHU � valeur incorrecte.
				if( m_ChuState[iChu].uiCptLive == CHU_DPU_NOT_ALIVE )
				{
					int iCrQnx; //Compte rendu QNX
					
					//Prise du s�maphore d'acc�s � la m�moire partag�e pour acceder au
					//compteur de vie de chaque CHU.
					iCrQnx = sem_wait( m_pShareMemSem );
				
					if( iCrQnx != EOK )
					{
						CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
					}
					
					//On positionne le validant du CHU � invalide
					m_pShareMem->Chu[iChu].ucValidity = INVAL;
					
					//Lib�ration du s�maphore d'acc�s � la m�moire partag�e
					iCrQnx = sem_post( m_pShareMemSem );
					
					if( iCrQnx != EOK )
					{
						CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
					}					
				}
			}				
	    }	
        
        // Compteur de PPS, pour validation
        if ( m_uiPpsAlive > PPS_LOST )
        {
            m_uiPpsAlive--;
        }
	    
		//On verifie dans le cas ou le PPS n'est pas configur� ou perdu s'il faut envoyer
		//le message de synchronisation horaire a l'ASC DTU
		if( ( m_bDateIsForced == true ) ||
            ( m_bPPSUsed == false ) ||
            ( ( m_bPPSUsed == true ) && ( m_uiPpsAlive == PPS_LOST ) ) )
		{
			if( m_SendClockSyncMsg.IsNow() == true )
			{
				//On transmet une TC de synchronisation horaire a l'ASC DTU
				//si ce dernier est operationnel.
				if( m_bAscDtuAlive  == true )
				{
					m_Cnx.SetTime();
				}
			}
		}									    					
					
		//On verifie si l'emission des attitudes des CHU doivent etre emises
		if( m_SendAttitudeTm.IsNow() == true )
		{
			for( iChu = 0; iChu < NB_CHU; iChu++ )
	    	{
	    		//On place l'indicateur d'emission de la trame d'attitude.
				m_ChuState[iChu].bSendAttitude = true;
	    	}
		}	
        
        // On v�rifie l'�tat du flag NoEmission de la t�l�mesure de diagnostic
        bNoEmissionDebug = !m_SaveAndSendDataTmDebugRate.IsNow();
			
		//On v�rifie si l'�mission de la t�l�mesure de diagnostic s'effecue maintenant
		if( m_SendDataTmDebugRate.IsNow() == true )
		{				
			SendDebugTm(bNoEmissionDebug);
		}
				
		//Le timer est relanc� apr�s chaque pulse.
	    iCrQnx = TimerSettime(m_TimerId, 0, &m_itime, NULL);
	    
		if( iCrQnx == QNXINTERNALERROR )
		{
			CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
		}					 							
	}
	else if ( iCode == RCV_MSG_PULSE )
	{
		m_Cnx.ReceivePendingMsg( iCode );			
	}
	else
	{
		return false;
	}		
				
	return true;  
}


/*----------------------------------------------------------------------------*/
/* IReceiveMsgExt			                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette methode effectue la prise en compte des messages recus de l'ASC DTU.
 *  Il n'y a pas de verification de trames car elle sont faites par la classe
 *  CSerialComProto.
 * 
 *  La trame recue est analysee afin de verifier son type. S'il s'agit d'un 
 *  message de boot du DPU alors il est marque afin de ne pas etre pris en compte
 *  par le driver pour detecter l'etat operationnel du DPU. A ce stade le DUP 
 *  n'est pas encore pret pour communiquer. S'il s'agit d'une trame d'attitude,
 *  les methodes ProcessingAttitudeFrame() et StockBroadcastAttitudeFrame() sont
 *  appelees. Si la methode ProcessingAttitudeFrame() retourne true alors
 *  la trame doit etre transmise a l'EGSE (limitation du nombre de trames
 *  d'attitude emises a l'egse). S'il s'agit d'une trame de synchronisation
 *  horaire, alors on appelle la methode ProcessingSyncTimeFrame().
 *  
 *  Ensuite, tout message devant etre transmis a l'EGSE est decompose en sous
 *  messages de taille compatible pour etre emis a l'EGSE (le reconstruction est
 *  suporter par la station sol).
 * 
 *  Ensuite, pour tout message, hors messages de mises sous tension du DPU, on 
 *  considere que le DTU est present. s'il etait absent, on indique que ce dernier
 *  est present, on demande une resynchronisation horaire et on met a jour son 
 *  niveau d'erreur.
 * 
 *  Tout message de mise sous tension du DPU force le driver en defaut majeur
 *  afin d'indiquer que le DPU ete prealablement perdu.
 *
 *	\param       [in] iCnx : Identifiant fourni a ReceivePendingMsg()
 *	\param       [in] pBuffer : Buffer contenant la telemesure recu
 *	\param       [in] iSize : Taille de la telemsure recu
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 17/04/13
//  - BL-FI-4222 : - Suppresion de l'envoi d'une resynchronisation sur connexion
//                  DPU
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//  - BL-FI-4929 : - Sur r�ception un message de type power, on ne fait pas appel �
//					ProcessingAttitudeFrame et StockBroadcastAttitudeFrame
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::IReceiveMsgExt( int iCnx, char * pBuffer, int iSize )
{			
	//Nombre d'octets constituants le checksum
	const int CHECKSUM_SIZE = 2;
	
	//Correction de la taille lue dans le header (cf spec protocole du DTU)
	const int CORR_SIZE = 1;
	
	//Nombre d'octets a retirer pour en deduire la taille du message de donnees	
	const int DATA_SIZE_HEADER = sizeof( CSerialComProto::sAscDtuTmHeader )
										+ CHECKSUM_SIZE - CORR_SIZE;
	
	//Variable utilisee pour savoir si le message doit �tre emis � l'EGSE
	bool bSendToEgse = true;
	
	//Variable utilisee pour indiquer si le message re�u est de type mise sous
	//tension
	bool bPowerMsg = false;
	
	//Nombre d'octets contenu dans le message de donnees.
	int iDataSize = 0;
		
	//Variable utilisee pour verifier si la trame est une trame d'attitude
	CSerialComProto::sAscDtuTmFrame * pAscDtuTm = 
						( CSerialComProto::sAscDtuTmFrame * ) pBuffer;	
		
#ifdef CASCDTU_DEBUG	
	printf( "Received DTU frame, size=%d\n", iSize );
	
	printf( "Received type=%d, subtype=%d\n", pAscDtuTm->sTmData.sTmHeader.cType,
		    pAscDtuTm->sTmData.sTmHeader.cSubType );	
	
	CLog::PrintfDataInHexa( pBuffer, iSize );	
#endif	
	
	//Calcul de la taille effective du message de donnees
	iDataSize = ENDIAN_RET16( pAscDtuTm->sHeader.usSizePacket ) - DATA_SIZE_HEADER;
	
	//S'il s'agit d'une trame d'attitude, on la prend en compte.
	if( ( pAscDtuTm->sTmData.sTmHeader.cType == CSerialComProto::TYPE_TM_ATTITUDE ) &&
		( pAscDtuTm->sTmData.sTmHeader.cSubType == CSerialComProto::SUBTYPE_TM_ATTITUDE ) )
	{
		//Si la trame de telemesure recu est une trame de mise sous tension du 
		//DPU alors l'indique et on ne propage pas l'attitude
		if( iDataSize != sizeof( CSerialComProto::sTmAttitudeData ) )
		{
			//Le message est un message de sous tension du DPU.
			bPowerMsg = true;
		}
		else
		{
			//La trame d'attitude est analysee puis prise en compte
			bSendToEgse = ProcessingAttitudeFrame( pAscDtuTm->sTmData.cData, iDataSize );					
			
			//On effectue le stockage et la diffusion vers un driver de la telemesure
			//d'attitude.
			StockBroadcastAttitudeFrame( pBuffer, iSize );		
		}
	}
	
	//S'il s'agit d'une trame de synchronisation horaire, on la prend en compte.
	if( ( pAscDtuTm->sTmData.sTmHeader.cType == CSerialComProto::TYPE_TM_SYNCTIME ) &&
		( pAscDtuTm->sTmData.sTmHeader.cSubType == CSerialComProto::SUBTYPE_TM_SYNCTIME ) )
	{
		//La trame de synchronisation horaire est analysee puis prise en compte
		ProcessingSyncTimeFrame( pAscDtuTm->sTmData.cData, iDataSize );		
	}
						
	//On v�rifie si la trame doit �tre �mise � l'EGSE.
	if( bSendToEgse == true )
	{
		//Constante utilis�e pour indiquer qu'il n'y a plus de
		//donn�es � transmettre
		const int NO_DATA_TO_SEND = 0;
		
		//Identifiant de trale du premier sous message
		const int FIRST_SS_MSG_ID = 0;	
		
		//Variable utilis�e pour se d�placer dans le buffer de message
		//afin d'envoyer des petits messages compatibles avec la contrainte
		//de taille.
		char * pShortMsg = pBuffer;
		
		eMsgTmTc Msg; //Variable message	
			
		//Initialisation de l'entete de la TM
		Msg.HeaderTmTc.bStationSol = GROUND_STATION;				
		
		//Taille utilis�e pour la transmission du message
		int iSendSize;
	
		//Taille restante des donn�es � �mettre
		int iLeftSize = iSize; 
	
		//Initialisation du compteur de trame interne utilise pour la transmission de
		//messages � la station sol.
		unsigned char ucIdFrame = 0; 

		while( iLeftSize != NO_DATA_TO_SEND )
		{		
			//Calcul du nombre d'octets en transmettre.
			if( iLeftSize > SIZE_MAX_MSG )
			{		
				iSendSize = SIZE_MAX_MSG;
	
				//S'il s'agit du premier sous message emis
				if( ucIdFrame == FIRST_SS_MSG_ID )
				{
					Msg.HeaderTmTc.iIdMsg = FIRST_SS_MSG;								
				}
	
				else
				{
					Msg.HeaderTmTc.iIdMsg = OTHER_SS_MSG | ucIdFrame;
				}
			}
			else
			{
				iSendSize = iLeftSize;
	
				//S'il s'agit du premier sous message emis
				if( ucIdFrame == FIRST_SS_MSG_ID )
				{
					Msg.HeaderTmTc.iIdMsg = FIRST_AND_LAST_SS_MSG;
				}
	
				else
				{
					Msg.HeaderTmTc.iIdMsg = LAST_SS_MSG | ucIdFrame;
				}
			}
	
	#ifdef CASCDTU_DEBUG
			printf( "Route DTU frame, Id msg=%x, Size=%d\n", 
					Msg.HeaderTmTc.iIdMsg, iSendSize );
	#endif
						
			//Incrementation du compteur de frame.
			ucIdFrame++;
	
			//Calcule du nombre d'octets restant � transmettre
			iLeftSize = iLeftSize - iSendSize;
			
			//Copie la partie du message � �mettre.		
			memcpy( Msg.pData, pShortMsg, iSendSize );		
			
			//Emission du message de t�l�mesure	
			m_Telecom.SendTm( Msg, iSendSize, TM_PRIORITY );
			
			//Mise � jour du pointeur sur le nouveau s'il y
			//a des donn�es � �mettre.
			if( iLeftSize != NO_DATA_TO_SEND )
			{
				pShortMsg = pShortMsg + iSendSize;
			}
		}
		
		//On incremente le nombre de message emis a l'EGSE
		m_uiNbMsgSendToEGSE++;
	}									   
	
	//Si le message re�u est un message de sous tension du DPU, il n'est pas utilise
	//pour detecter l'etat operationnel du DPU car a ce stade le DPU n'est pas
	//entierement operationnel (il ne peut pas par exemple prendre en compte
	//une TC). 
	if( bPowerMsg == false )
	{				
		//A ce stade, la communication avec l'ASC DTU est operationnelle.
		
		//Si la communication avec l'ASC DTU etait perdue il faut forcer l'emission
		//de la synchronisation horaire, indiquer que la communication est 
		//operationnelle et que le driver n'est plus en d�faut.
		if( m_bAscDtuAlive == false )
		{
			m_bAscDtuAlive = true;
			
			//Le DPU est operationnel
			FAWSetLevelErr( DRV_NO_ERR );
		}
		
		//R�initialisation du compteur de vie du DTU
		m_uiAscDtuLive = NB_CYCLE_CPT_LIFE;	
	}
	else
	{
		//Le DPU a ete reinitialise. Il n'est pas operationnel et on l'indique.
		m_bAscDtuAlive = false;
			
		//Le DPU est considere en defaut
		FAWSetLevelErr( DRV_ERR_MAJ );	
		
		//On force volontairement le compteur a sans vie.
		m_uiAscDtuLive = CHU_DPU_NOT_ALIVE;
	}	
}


/*----------------------------------------------------------------------------*/
/* IBASReceiveTc				                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee sur reception d'une telecommande. La telecommande
 *  est analyse afin de verifier qu'elle est correct puis elle est prise en 
 *  compte. Toute telecommande incorrect est rejetee. Les telecommandes prises
 *  en compte sont:
 *  	- Passage en mode intervention.
 *  	- Reset des donnees de debug.
 * 		- Message emis par l'EGSE a transmettre au DPU
 *
 *	\param       [in] iIdMsg : Identifiant de la telecommande.
 *	\param       [in] bStationSol : Type de station sol a la source de la
 *                                          telecommande.
 *	\param       [in] iSize : Taille utile du message de telecommande.
 *	\param       [in] pData : Message de telecommande.
 *  \return      true : La telecommande a ete prise en compte.
 *  \return      false : La telecommande n'a pas ete prise en compte.
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 21/11/12
//	- BL-DM-2471 : - R�activation du timer qui permet le cadencement
//					 de l'�mission des TM Debug
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//	- BL-DM-4197 : - Gestion du compteur m_uiNbMsgRcvFromEGSENok
////////////////////////////////////////////////////////////////////////////////
bool CAscDtu::IBASReceiveTc( int iIdMsg, bool bStationSol, int iSize,
							   char * pData )
{
	//Nombre de donn�es par TC
	const int TC_DEBUG_SIZE = 0;	
	const int TC_RESET_DEBUG_SIZE = 0;		
	
	//Constante utilisee pour recuperer l'information protocole 
	const unsigned char GET_PROTOCOLE_INFO = 0xC0;

	//Constante utilisee pour recuperer l'information compteur de trames
	const unsigned char GET_CPT_FRAME_INFO = 0x3F;
	
	bool bCr = true;	//Compte rendu de fonction			
		
	//S'il s'agit d'une t�l�commande �mise par la station de contr�le et de 
	//supervision
	if( bStationSol == SUPERVISOR_STATION )
	{
		//On traite la TC en fonction de son identifiant
		switch( iIdMsg )
		{
			case TC_DEBUG :
			{
				if( iSize != TC_DEBUG_SIZE )
				{
					CLog::Log( LOC, ERR_ASCDTU_BAD_TC_SIZE,
							   _SLOG_ERROR, iIdMsg, iSize );										  
	
					bCr = false;
									
					break;		
				}
				
				//	Forcage de l'�mission de la TM Debug au prochain pulse
				m_SendDataTmDebugRate.ForceNow();
	
				// D�sinhibition de la TM de Debug
				m_SaveAndSendDataTmDebugRate.SetInhibitionOnDelay();
					
				break;
			}
				
			case TC_RESET_DEBUG :
			{
				//V�rification de la taille de la TC		
				if( iSize != TC_RESET_DEBUG_SIZE )
				{
					CLog::Log( LOC, ERR_ASCDTU_BAD_TC_SIZE,
							   _SLOG_WARNING, iIdMsg, iSize );										  
					
					bCr = false;
									
					break;		
				}		
							
			    //Initialisation des compteurs de maintenance
			    m_uiNbAscDtuLost = 0;
			    m_uiNbMsgSendToEGSE = 0;
			    m_uiNbMsgRcvFromEGSE = 0;
			    m_uiNbMsgRcvFromEGSENok = 0;
			    uiNbRebuiltErrMsgFromEGSE = 0;			    
			    
			    //Initialisation des donnees de debug de la classe connexion
			    m_Cnx.ResetDebugData();	
				
				break;
			}
		}
	}
			
	//Sinon, il s'agit d'un message issu de la station sol EGSE.
	else
	{
		//Variable utilisee pour recuperer l'information protocole
		unsigned char ucProtoInfo = iIdMsg & GET_PROTOCOLE_INFO;

		//Variable utilisee pour recuperer l'information compteur de trame
		unsigned char ucCptFrameInfo = iIdMsg & GET_CPT_FRAME_INFO;
		
		//Si on est en attente du premier sous message
		if( m_iEgseBufferState == FIRST_SS_MSG_STATE )
		{
			//S'il s'agit du premier sous message
			if( ucProtoInfo == FIRST_SS_MSG )
			{
				//On sauvegarde le compteur de trame qui doit etre �gal � 0
				m_iEgseBufferCpt = ucCptFrameInfo;					

				//On copie le sous message dans le buffer
				memcpy(m_cEgseBuffer, pData, iSize );

				//On met � jour le pointeur sur le buffer
				m_iEgseBufferPtr = iSize;

				//On se met en attente des autres messages
				m_iEgseBufferState = OTHER_SS_MSG_STATE;
				
#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. First SubMsg, Cpt=%d, Size=%d\n",
						m_iEgseBufferCpt, iSize );
#endif						
			}

			//S'il s'agit d'un message standalone on l'emet directement
			if( ucProtoInfo == FIRST_AND_LAST_SS_MSG )
			{				
				//Emission de la donn�e sur l'interface
				m_Cnx.RouteMsg( pData, iSize );
				
				m_uiNbMsgRcvFromEGSE++;
				
#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. Standealone Msg, Cpt=%d, Size=%d\n",
						m_iEgseBufferCpt, iSize );
#endif				
			}

			//S'il s'agit d'un autre protocole, on ne fait rien
		}

		//Si on est en attente d'un autre sous message
		if( m_iEgseBufferState == OTHER_SS_MSG_STATE )
		{
			//S'il s'agit d'un nouveau sous message
			if( ucProtoInfo == OTHER_SS_MSG )
			{
				//On increment le compteur de trame attendu
				m_iEgseBufferCpt++;					

				//Si les compteurs de trames sont differents on reinitialise
				//la gestion
				if( ucCptFrameInfo != m_iEgseBufferCpt )
				{
					m_iEgseBufferState = FIRST_SS_MSG_STATE;	
					
					uiNbRebuiltErrMsgFromEGSE++;					

#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. Cpt error, Cpt received=%d, Cpt=%d\n",
						ucCptFrameInfo, m_iEgseBufferCpt );
#endif
					return bCr;
				}

				//On copie le sous message dans le buffer de reception
				memcpy( &m_cEgseBuffer[m_iEgseBufferPtr], pData, iSize );

				//On met a jour le pointeur
				m_iEgseBufferPtr = m_iEgseBufferPtr + iSize;
				
#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. Other SubMsg, Cpt=%d, Size=%d\n",
						m_iEgseBufferCpt, iSize );
#endif			
	
			}

			//S'il s'agit du dernier sous message
			else if( ucProtoInfo == LAST_SS_MSG )
			{
				//On increment le compteur de trame attendu
				m_iEgseBufferCpt++;					

				//Si les compteurs de trames sont differents on reinitialise
				//la gestion
				if( ucCptFrameInfo != m_iEgseBufferCpt )
				{					
					m_iEgseBufferState = FIRST_SS_MSG_STATE;
					
					uiNbRebuiltErrMsgFromEGSE++;
					
#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. Cpt error, Cpt received=%d, Cpt=%d\n",
						ucCptFrameInfo, m_iEgseBufferCpt );
#endif					
					return bCr;
				}

				//On copie le sous message dans le buffer de reception
				memcpy( &m_cEgseBuffer[m_iEgseBufferPtr], pData, iSize );

				//On met a jour le pointeur
				m_iEgseBufferPtr = m_iEgseBufferPtr + iSize;

				//Emission de la donn�e sur l'interface
				m_Cnx.RouteMsg( m_cEgseBuffer, m_iEgseBufferPtr );
				
				m_uiNbMsgRcvFromEGSE++;
				
#ifdef CASCDTU_DEBUG	
				printf( "Received EGSE TC. Last SubMsg, Cpt=%d, Size=%d\n",
						m_iEgseBufferCpt, iSize );
						
				printf( "Send Msg, Total size=%d\n", m_iEgseBufferPtr );
#endif				

			}

			//Sinon, il s'agit d'un probleme et on reinitialise la gestion.
			else
			{
				m_uiNbMsgRcvFromEGSENok++;
				m_iEgseBufferState = FIRST_SS_MSG_STATE;
			}
		}		
	}
	
	return bCr;				
}


/*----------------------------------------------------------------------------*/
/* ProcessingAttitudeFrame		                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee pour prendre en compte une trame d'attitude d'un
 *  CHU emise par le DPU.
 *  Le fomrat du quaternion recu du DTU est au format anglosaxon (le sclalaire
 *  est a la fin). Lorsque ce dernier est sauvegardee dans la memoire partagee
 *  il est mis au format francais.
 *
 *	\param   	 [in] pBuffer : Message de telemesure de type attitude
 *	\param       [in] iSize : Taille de la telemesure recu
 *  \return      true : Le message est a envoyer a l'EGSE
 *  \return      false : Le message ne doit pas etre envoyee a l'EGSE
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
bool CAscDtu::ProcessingAttitudeFrame( char * pBuffer, int iSize )
{
	//Masque utilise pour recuperer l'identifiant du CHU
	const unsigned char CHU_MASK = 0x03;

	//Nombre de decalage a droite pour recuperer l'identifiant CHU sur les 
	//bits de poids faible
	const unsigned char CHU_DECALAGE = 4;
	
	//Valeur de conversion utilisee pour conevrtir le quaternion acqui en
	//double
	const double QUATERNION_CONV = 2147483647.0;
	
	int iCrQnx;	//Retour de fonction QNX							
	
#ifdef CASCDTU_DEBUG_COOR
	//Valeur de conversion pour passer des radian en degre
	const double RAD2DEG = 180.0 / 3.141592653589793238462643383279;

	//Variables ascension droite, declinaison, rotation.
	double dRightAscent = 0;
	double dDeclinaison = 0;
	double dRotation = 0;
#endif
	
	//Quaternion utiliser pour la conversion
	int iQuaternion;
		
	//Quaternion extrait de la trame d'attitude en double
	double dQ1, dQ2, dQ3, dQ4;
	
	//Variable utilisee pour recuperer l'identifiant du CHU emetteur de la trame
	unsigned char ucChuId;		
	
	//Variable utilisee pour savoir si l'attitude du CHU doit etre emise
	bool bSendAttitude = false;
	
	//Variables utilisees pour manipuler la date contenue dans la trame d'attitude
	//afin de la passer en date absolue.
	unsigned int uiSec;
	unsigned short usFracSec;		
	
	//Formatage de la trame d'attitude.
	CSerialComProto::sTmAttitudeData * pAttitudeFrame = 
							( CSerialComProto::sTmAttitudeData * ) pBuffer;
							
	//On recupere la date contenue dans la trame d'attitude et on la convertie
	//en date dtu. La date DTU est en fait un 48bit contenant le nombre de 1/65536s	
	uiSec = ENDIAN_RET32( pAttitudeFrame->uiNbSeconds );
	usFracSec = ENDIAN_RET16( pAttitudeFrame->usNbSubSeconds );
	
#ifdef CASCDTU_DEBUG	
	double dDpuTimeAtt;
	
	dDpuTimeAtt = usFracSec * 65536/1000000000;
	dDpuTimeAtt = dDpuTimeAtt + uiSec;
	
	printf("Date de la telemesure d'attitude = %f\n", dDpuTimeAtt );			
#endif
				
	//On recupere les quaternions
	iQuaternion = pAttitudeFrame->iQ1;
	ENDIAN_SWAP32( &iQuaternion );
	dQ1 = ( double) iQuaternion / QUATERNION_CONV;	 
		
	iQuaternion = pAttitudeFrame->iQ2;
	ENDIAN_SWAP32( &iQuaternion );
	dQ2 = ( double) iQuaternion / QUATERNION_CONV;
	
	iQuaternion = pAttitudeFrame->iQ3;
	ENDIAN_SWAP32( &iQuaternion );
	dQ3 = ( double) iQuaternion / QUATERNION_CONV;
	
	iQuaternion = pAttitudeFrame->iQ4;
	ENDIAN_SWAP32( &iQuaternion );
	dQ4 = ( double) iQuaternion / QUATERNION_CONV;
	
	//Recuperation de l'identifiant de la camera
	ucChuId = ( pAttitudeFrame->ucFlags >> CHU_DECALAGE ) & CHU_MASK;	
		
#ifdef CASCDTU_DEBUG
	printf( "Received an attitude frame : flags=%x, residus=%d, chu=%d\n", 
			pAttitudeFrame->ucFlags, pAttitudeFrame->ucResidual, ucChuId );	
			
	printf( "Q1=%f, Q2=%f, Q3=%f, Q4=%f\n", dQ1, dQ2, dQ3, dQ4 );
#endif
			
#ifdef CASCDTU_DEBUG_COOR
	//On effectue le calcul de l'ascension, la declinaison et la rotation si
	//le residus est correct.
	if( pAttitudeFrame->ucResidual != RESIDUS_NOK )
	{
		dRightAscent = atan2( ( dQ2 * dQ3 - dQ1 * dQ4 ), ( dQ1 * dQ3 + dQ2 * dQ4 ) );
		
		dDeclinaison = asin( dQ3 * dQ3 + dQ4 * dQ4 - dQ1 * dQ1 - dQ2 * dQ2 );
		
		dRotation = atan2( ( dQ2 * dQ3 + dQ1 * dQ4 ), ( dQ2 * dQ4 - dQ1 * dQ3 ) );
		
		//On passe en degr�
		dRightAscent = dRightAscent * RAD2DEG;
		
		dDeclinaison = dDeclinaison * RAD2DEG;
		
		dRotation = dRotation * RAD2DEG;
		
		printf( "Rac=%f, Dec=%f, Rot=%f\n", 
			    dRightAscent, dDeclinaison, dRotation );	
	}
#endif	

	//On sauvegarde la valeur du residus du CHU
	m_ChuState[ucChuId].ucResidus = pAttitudeFrame->ucResidual; 
	
	//On reinitialise le compteur de vie du CHU
	m_ChuState[ucChuId].uiCptLive = CHU_DPU_CPT_LIFE;
		
	//Prise du s�maphore d'acc�s � la m�moire partag�e pour mettre � jour
	//les donn�es � Isagraf
	iCrQnx = sem_wait( m_pShareMemSem );
	
	if( iCrQnx != EOK )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}	
		
	//Si le residus est bon on met � jour les donnees et le validant. On passe
	//au format fran�ais, le scalaire est situ� au d�but.
	if( pAttitudeFrame->ucResidual != RESIDUS_NOK )
	{
		//Constante utilisee pour passer des fractions de seconde au nanosecondes
		const _uint64 FRACSEC2NANOSEC = 1000000000 /65536;
		
		_uint64 uint64NSec; //variable utilis�e pour passer en nanosecondes

		//Mise � jour des quaternions
		m_pShareMem->Chu[ucChuId].dQ0 = dQ4;
		m_pShareMem->Chu[ucChuId].dQ1 = dQ1;
		m_pShareMem->Chu[ucChuId].dQ2 = dQ2;
		m_pShareMem->Chu[ucChuId].dQ3 = dQ3;

		//Mise � jour de la date
		m_pShareMem->Chu[ucChuId].uiSec = uiSec;
		//On transforme les fractions de secondes en nanoseconde
		uint64NSec = usFracSec;
		uint64NSec = uint64NSec * FRACSEC2NANOSEC;			
		m_pShareMem->Chu[ucChuId].uiNsec = ( unsigned int ) uint64NSec;

		//Mise � jour du validant
		m_pShareMem->Chu[ucChuId].ucValidity = VAL;
	}
	//Sinon, on invalide les donnees
	else
	{
		m_pShareMem->Chu[ucChuId].ucValidity = INVAL;
	}
	
	//Lib�ration du s�maphore d'acc�s � la m�moire partag�e
	iCrQnx = sem_post( m_pShareMemSem );
	
	if( iCrQnx != EOK )
	{
		CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );		
	}	
	
	//On verifie si la telemesute d'attitude du CHU doit �tre emise
	if( m_ChuState[ucChuId].bSendAttitude == true )
	{
		//On indique que l'attitude doit etre emise a l'EGSE
		bSendAttitude = true;
		
		//On indique que l'emission de l'attitude a ete prise en compte
		m_ChuState[ucChuId].bSendAttitude = false;
	}
		
	return bSendAttitude;
}


/*----------------------------------------------------------------------------*/
/* StockBroadcastAttitudeFrame		                                          */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee pour stocker sur fichier et/ou diffuser vers un
 *  driver la trame d'attitude complete (header + message d'attitude + CRC)
 * 
 *  Chaque option est indiquee par un parametre driver.
 * 
 *	\param   	 [in] pBuffer : Message de telemesure de type attitude
 *	\param       [in] iSize : Taille des donnees de telemesure recu
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 07/10/14
//	- BL-DM-XXXX
//		-Ajout des octets de synchronisation lors de l'enregistrement dans le
//		fichier de stockage
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::StockBroadcastAttitudeFrame( char * pBuffer, int iSize )
{
	//On v�rifie si l'attitude doit �tre �mise � un driver. Si c'est le cas,
	//on le fait.
	if( m_uiDrvIdToSendChuAtt != OPTION_NOT_USED )
	{
		eMsgApp Msg; //Variable emission de messages asynchrones entre drivers		
		
		//Initialisation de l'identifiant du driver et du type de message.
		Msg.Header.uiSrc = m_pConfig->GetDrvId();
		Msg.Header.uiTypeMsg = APP_MSG;
			
		//Copie des donnees du message
		memcpy( Msg.pData, pBuffer, iSize );		
		
		//On transmet le message au driver destinataire. On n'analyse pas le code
		//retoure car si la file est pleine on ne peut rien faire.
		FAWSendMsgAsync( m_uiDrvIdToSendChuAtt, &Msg, iSize, APP_PRIORITY );
	}	

	//On v�rifie s'il faut stocker l'attitude. Si c'est le cas, on le fait.
	if( m_uiChannelId != OPTION_NOT_USED )
	{
		//D�claration d'un message de stockage	
		eStockageMsg StockMsg;	
		
		// D�claration de l'entier de synchro
		unsigned short usSync = (unsigned short) iSize;
		
		// Ecriture de la taille de la TM en d�but de sotckage
		memcpy( &StockMsg.pData, &usSync, sizeof(usSync) );
		
		//Copie des donnees du message
		memcpy( &StockMsg.pData[sizeof(usSync)], pBuffer, iSize );	
		
		// Ecriture de la taille de la TM en fin de sotckage
		memcpy( &StockMsg.pData[sizeof(usSync)+iSize], &usSync, sizeof(usSync) );
		
		//Stockage du message 
		m_Stockage.StockeData( m_uiChannelId, StockMsg, iSize + sizeof(usSync) + sizeof(usSync) );		
	}
}		
		
		
/*----------------------------------------------------------------------------*/
/* ProcessingSyncTimeFrame		                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est utilisee uniquement dans un contexte de debug. Elle
 *  permet de recuperer la telemesure de syncrhonisation horaire emise par
 *  le DPU (AscSyncTimeTM) afin d'afficher en mode debug l'ecart en ns du LAT
 *
 *	\param   	[in] pBuffer : Message de telemesure de type
 *                                       synchronisation horaire
 *	\param       [in] iSize : Taille des donnees de telemesure recu
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
void CAscDtu::ProcessingSyncTimeFrame( char * pBuffer, int iSize )
{
#ifdef CASCDTU_DEBUG_TIME		
	
	static double m_dLastLatTime = 0;
	
	//Formatage de la trame d'attitude.
	CSerialComProto::sTmSyncTimeData * pSyncTimeFrame = 
							( CSerialComProto::sTmSyncTimeData * ) pBuffer;
						
	//Variables utilisees pour manipuler la date contenue dans la trame d'attitude
	unsigned int uiSec;
	unsigned short usNsec;
	double dDpuTime;	
				
	//On recupere le temps LAT qui est le temps interne au DPU
	uiSec = ENDIAN_RET32( pSyncTimeFrame->uiLatNbSeconds );
	usNsec = ENDIAN_RET16( pSyncTimeFrame->usLatNbSubSeconds );
	
	dDpuTime = usNsec * 65536;

	printf("Ecrat du lat Time (ns)= = %f\n", dDpuTime - m_dLastLatTime );
	
	m_dLastLatTime = dDpuTime;
	
#endif	
}


/*----------------------------------------------------------------------------*/
/* SendDebugTm					                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee pour effectuer l'emission de la telemesure 
 *  contenant les donnees de debug.
 *
 *  \param      [in]    bNoEmission : Envoi ou non de la TM au sol
 * 
 *  \warning     Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 03/08/12
//  - BL-DM-2471 : - Ajout du parametre bNoEmission, utilis� lors de l'appel
//                  � la m�thode SendTm.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 22/08/13
//  - BL-DM-4197 : - Ajout des donn�es PPS et erreurs comm a la trame de debug
////////////////////////////////////////////////////////////////////////////////
void CAscDtu::SendDebugTm(bool bNoEmission)
{
	const char CHU_ALIVE = 0x01;
	
	eMsgTmTc Msg; //Variable message	
	
	eDebugMsg * pDebugMsg = NULL;	//Variable pointeur sur le message � �mettre
		
	pDebugMsg = ( eDebugMsg * ) &Msg.pData; //Initialisation du pointeur de message	
	
	//Initialisation de l'entete de la TM
	Msg.HeaderTmTc.iIdMsg = TM_DEBUG_ID;
	Msg.HeaderTmTc.bStationSol = SUPERVISOR_STATION;

	//Initialisation des champs de la TM de debug
	
	//Initialisation des donnees de chaque CHU
	pDebugMsg->dQ0ChuA = m_pShareMem->Chu[CHU_A].dQ0;
	pDebugMsg->dQ1ChuA = m_pShareMem->Chu[CHU_A].dQ1;
	pDebugMsg->dQ2ChuA = m_pShareMem->Chu[CHU_A].dQ2;
	pDebugMsg->dQ3ChuA = m_pShareMem->Chu[CHU_A].dQ3;
	pDebugMsg->dQ0ChuB = m_pShareMem->Chu[CHU_B].dQ0;
	pDebugMsg->dQ1ChuB = m_pShareMem->Chu[CHU_B].dQ1;
	pDebugMsg->dQ2ChuB = m_pShareMem->Chu[CHU_B].dQ2;
	pDebugMsg->dQ3ChuB = m_pShareMem->Chu[CHU_B].dQ3;
	pDebugMsg->dQ0ChuC = m_pShareMem->Chu[CHU_C].dQ0;
	pDebugMsg->dQ1ChuC = m_pShareMem->Chu[CHU_C].dQ1;
	pDebugMsg->dQ2ChuC = m_pShareMem->Chu[CHU_C].dQ2;
	pDebugMsg->dQ3ChuC = m_pShareMem->Chu[CHU_C].dQ3;
	pDebugMsg->dQ0ChuD = m_pShareMem->Chu[CHU_D].dQ0;
	pDebugMsg->dQ1ChuD = m_pShareMem->Chu[CHU_D].dQ1;
	pDebugMsg->dQ2ChuD = m_pShareMem->Chu[CHU_D].dQ2;
	pDebugMsg->dQ3ChuD = m_pShareMem->Chu[CHU_D].dQ3;
	
	//Initialisation des validants
	pDebugMsg->ucValidant = 0;
	
	pDebugMsg->ucValidant = pDebugMsg->ucValidant | ( unsigned char ) m_uiPpsAlive;
	pDebugMsg->ucValidant = pDebugMsg->ucValidant << 1;
	
	if( m_ChuState[CHU_D].uiCptLive != CHU_DPU_NOT_ALIVE )
	{
		pDebugMsg->ucValidant = pDebugMsg->ucValidant | CHU_ALIVE;
	}
	pDebugMsg->ucValidant = pDebugMsg->ucValidant << 1;	

	if( m_ChuState[CHU_C].uiCptLive != CHU_DPU_NOT_ALIVE )
	{
		pDebugMsg->ucValidant = pDebugMsg->ucValidant | CHU_ALIVE;
	}
	pDebugMsg->ucValidant = pDebugMsg->ucValidant << 1;	

	if( m_ChuState[CHU_B].uiCptLive != CHU_DPU_NOT_ALIVE )
	{
		pDebugMsg->ucValidant = pDebugMsg->ucValidant | CHU_ALIVE;
	}		
	pDebugMsg->ucValidant = pDebugMsg->ucValidant << 1;	

	if( m_ChuState[CHU_A].uiCptLive != CHU_DPU_NOT_ALIVE )
	{		
		pDebugMsg->ucValidant = pDebugMsg->ucValidant | CHU_ALIVE;
	}	
	pDebugMsg->ucValidant = pDebugMsg->ucValidant << 1;	

	pDebugMsg->ucValidant = pDebugMsg->ucValidant | ( unsigned char ) m_bAscDtuAlive;

	//Initialisation des r�sidus
	pDebugMsg->ucResidusChuA = m_ChuState[CHU_A].ucResidus;
	pDebugMsg->ucResidusChuB = m_ChuState[CHU_B].ucResidus;
	pDebugMsg->ucResidusChuC = m_ChuState[CHU_C].ucResidus;
	pDebugMsg->ucResidusChuD = m_ChuState[CHU_D].ucResidus;
	
	//Initialisation des compteurs de diagnostic
	pDebugMsg->uiNbCnxLost = m_uiNbAscDtuLost;
	pDebugMsg->uiNbMsgSendToDPU = m_Cnx.m_uiNbSendOk;
	pDebugMsg->uiNbMsgRcvFromDPU = m_Cnx.m_uiNbRecvOk;
	pDebugMsg->uiNbChecksumErrFromDPU = m_Cnx.m_uiNbCheckSumErr;
	pDebugMsg->uiNbMsgRcvFromDPUNok = m_Cnx.m_uiNbRecvNok;
	pDebugMsg->uiNbDeSeqFromDPU = m_Cnx.m_uiNbLostMsg;
	pDebugMsg->uiNbMsgSendToEGSE = m_uiNbMsgSendToEGSE;
	pDebugMsg->uiNbMsgRcvFromEGSE = m_uiNbMsgRcvFromEGSE;
	pDebugMsg->uiNbMsgRcvFromEGSENok = m_uiNbMsgRcvFromEGSENok;
	pDebugMsg->uiNbRebuiltErrMsgFromEGSE = uiNbRebuiltErrMsgFromEGSE;
		
	//Emission du message de t�l�mesure	
	m_Telecom.SendTm( Msg, sizeof( eDebugMsg ), TM_PRIORITY, bNoEmission );		
}



/*----------------------------------------------------------------------------*/
/* GetImageName					                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appelee pour recuperer le nom du canal evenementiel de
 *  stockage
 *
 *  \return      Nom du fichier de stockage
 *	\warning	 Exception levee si un probleme survient 
 */
/*----------------------------------------------------------------------------*/
string CAscDtu::GetChanelName()
{
	const int NBCHARINDATE = 50; //Nombre de caract�res maximum dans le tableau
	
	string strChanelName;
	
	char pcChanelName[50];
	
	char pcFormat[] = "DTUASCATT_%y%m%d-%H%M%S";
	
	time_t TimeNow; //Variable utilis�e pour acqu�rir la date
	
	struct tm * pCalendarTime; //Variable utilis�e pour convertir la date en une date calendaire	
	
	//Acquisition de la date
	TimeNow = time( NULL );
	
	pCalendarTime = localtime( &TimeNow );
	
	strftime( pcChanelName, NBCHARINDATE, pcFormat, pCalendarTime );	
	
	strChanelName = pcChanelName;
	
	return strChanelName;
}