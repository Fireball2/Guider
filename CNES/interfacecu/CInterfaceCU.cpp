/*! \file CInterfaceCU.cpp
 *  \brief Impl�mentation de la classe principale du driver ICU
 */
 
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//  Auteur : R�mi THONNERIEUX - CIO Informatique Industrielle
//  - Prise en compte de la BL-FI-5913
//      - Modification de la m�thode Init()
//		- Modification de la m�thode IReceiveMsgExt()
//		- Modification de la m�thode IFAWRun()
//		- Modification de la m�thode IFAWReceivePulse()
//		- Modification de la m�thode SendDebugTm()               
////////////////////////////////////////////////////////////////////////////////

 ////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 16/12/2015
//  Auteur : Thomas FOURCROY - CIO
//  - Prise en compte de BL-FI-6084
//      - Modification de la fonction IBASReceiveMsg()
////////////////////////////////////////////////////////////////////////////////

// Headers Syslay
#include "CConv.h"

// Headers C
#include <cstdio> // printf(), snprintf()
#include <ctime>  // clock_gettime(), gmtime_r()

// Headers Syslay
#include <IfStockage.h>
#include <services.h>

// Headers locaux
#include "ProtoIcu.h"
#include "CUdpCu.h"
#include "CSerialComCU.h"
#include "Version.h"
#include "CInterfaceCU.h"


/*! \addtogroup interfacecu */
/* @{ */

// /@cond ICU_DEBUG
/*! \def ICU_DEBUG
 *    S�lecteur pour activer ou inhiber l'affichage de trace sur la console
 */
// /@endcond
//#define ICU_DEBUG

/*!
 *   Portee : classe CInterfaceCU\n
 *   Constante utilis�e pour fixer la dur�e d'�mission de la TM de debug
 */
const int DEBUG_TM_DURATION_TIME = 60 * 3; // 3 minutes d'�mission

/*!
 *   Portee : classe CInterfaceCU\n
 *   Identifiant du pulse utilis� pour cadencer les traitements du driver.
 */
const int TIMER_PULSE = CBASDrv::FIRST_PULSE_FREE;

/*!
 *   Portee : classe CInterfaceCU\n
 *   Identifiant du pulse utilis� pour notifier la pr�sence de messages emis
 *   par la charge utile.
 */
const int RCV_MSG_PULSE = CBASDrv::FIRST_PULSE_FREE + 1;

/*!
 *   Portee : classe CInterfaceCU\n
 *   Constante utilis�e pour detecter la perte de la CU (en secondes)
 */ 
const unsigned int DEFAULT_CU_TIME_OUT = 5;

/*!
 *   Portee : classe CInterfaceCU\n
 *   Nombre maximal de drivers dont les messages peuvent �tre rout�s
 */
const unsigned int NB_MAX_ROUTAGE = 10;

/* @} */

using namespace std;

/******************************************************************************/
/*                                                                            */
/*                          METHODES PUBLIC                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CInterfaceCU                                                               */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la construction de l'instance.
 *
 *  Traitements effectu�s :
 *  - initialisation des donnees du driver
 *  - initialisation de la variable timer
 *  - initialisation de la version du driver
 */
/*----------------------------------------------------------------------------*/
CInterfaceCU::CInterfaceCU() : CBASDrv()
{
    // D�clenchement du timer (toutes les secondes)
    const uint64_t DECLENCHEMENT_TIMER = 1000000000;
    
    // Utilisation du timer sans r�p�tition
    const uint64_t SANS_REPETITION = 0;
    
    // Initialise les donnees du driver
    Init();

    // Initialisation de la variable temps du timer utilis� pour le r�veil
    // p�riodique
    m_itime.nsec = DECLENCHEMENT_TIMER; // D�lai avant le d�clenchement du timer
    m_itime.interval_nsec = SANS_REPETITION; // Pas de d�clenchement p�riodique
    
    // Initialisation du numero de version du driver
	FAWSetVersion( APP_VERSION );
}

/*----------------------------------------------------------------------------*/
/* ~CInterfaceCU                                                              */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la destruction de l'instance.
 *
 */
/*----------------------------------------------------------------------------*/
CInterfaceCU::~CInterfaceCU()
{
    // Rien � faire
}

/*----------------------------------------------------------------------------*/
/* Start                                                                      */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode initialise le driver puis cr�e un �v�nement pour reveiller
 *  periodiquement le driver. Ensuite, le driver est d�marr�.
 *
 *  \param       [in] argc : Nombre d'arguments dans la ligne de commande.
 *  \param       [in] argv[] : Tableau d'arguments de type string.
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
void CInterfaceCU::Start( int argc, char *argv[] )
{
    // Initialisation du driver
    BASInit( argc, argv );
    
    // Cr�ation d'un �v�nement utilis� pour g�n�rer un r�veil p�riodique
    m_Event = FAWCreatePulse( TIMER_PULSE, 0 );
    
    // Cr�ation d'un �v�nement utilis� pour signaler la pr�sence de donn�es �
    // lire.
    m_EventRcvData = FAWCreatePulse( RCV_MSG_PULSE, 0 );
    
    // Lancement du driver
    BASStart();
}

/******************************************************************************/
/*                                                                            */
/*                          METHODES PRIVATE                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Init                                                                       */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue l'initialisation des variables membres et
 *  l'initialisation des variables g�rant les traitements p�riodiques.
 *
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//	- BL-FI-5913 : - Modification du type de la variable m_iCuActivityCounter
////////////////////////////////////////////////////////////////////////////////
void CInterfaceCU::Init()
{
    // Periode d'�mission de la t�l�mesure de debug par d�faut
    const int DEBUG_TM_SEND_DEFAULT_RATE = 10;
    
    // Periode d'�mission de la t�l�mesure vers le sol
    const int DEBUG_TM_ALWAYS_SEND = 0;

    // Initialisation des variables membres.
    m_pConfig = NULL;
    m_pCnx = NULL;
    m_pShareMemSem = NULL;
    m_pShareMem = NULL;
    m_TimerId = 0;
    m_iCuActivityCounter = 0;

    // Initialisation de la p�riode par d�faut de la TM de debug
    m_SendDataTmDebugRate.Init( DEBUG_TM_SEND_DEFAULT_RATE );

    // Initialisation de la p�riode par d�faut de la TM de debug
    m_SaveAndSendDataTmDebugRate.Init( DEBUG_TM_ALWAYS_SEND,
                                       DEBUG_TM_DURATION_TIME );
    
    // Par d�faut la TM de debug n'est pas �mise.
    m_SaveAndSendDataTmDebugRate.SetInhibition();
    
    // Par d�faut, aucune checksum n'est s�lectionn�e
    m_Param.ucChecksumType = CChecksum::NO_CHECKSUM;
    
    // Par d�faut, le time out de la CU est �gale � sa constante
    m_Param.uiTimeOutCu = DEFAULT_CU_TIME_OUT;
    m_Param.bCheckCuActivity = true;

    // Par d�faut, on utilise pas le stockage
    m_Param.bStockageUsed = false;

    // Initialisation des compteurs de diagnostic
    m_uiNbRoutedMsgOk = 0;
    m_uiNbRoutedMsgNok = 0;
    m_uiNbLostCu = 0;
    m_uiNbSaturations = 0;
    m_uiNbStoredMsg = 0;

    // Initialisation du compteur de trames �mises
    m_usSentFrameCounter = 0;
}

/*----------------------------------------------------------------------------*/
/* IReceiveMsgExt                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception d'un message en provenance de la
 *  charge  utile.
 *
 *  Traitements effectu�s :
 *  - mise � jour du compteur de vie contr�lant l'activit� de la charge utile
 *  - ajout d'un message dans la pile de message � disposition de l'automate
 *    � condition que la pile ne soit pas satur�e.
 *  - stockage du message, si la fonctionnalit� est activ�e
 *
 *  Aucune v�rification n'est faite sur la taille iSize du buffer, ni sur la
 *  coh�rence du contenu de la trame, cela ayant d�j� �t� fait dans la classe
 *  appelante.
 *
 *  \param       [in] iCnx : Identifiant fourni a ReceivePendingMsg()
 *  \param       [in] pBuffer : Buffer contenant la TC recue
 *  \param       [in] iSize : Taille de la TC recue
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//	- BL-FI-5913 : - Modification du type de la variable m_iCuActivityCounter
////////////////////////////////////////////////////////////////////////////////
void CInterfaceCU::IReceiveMsgExt( int iCnx, char * pBuffer, int iSize )
{
    int iCrQnx = 0;     // Compte rendu de fonction QNX

    bool bCr = true;    // Variable indiquant si le message est valide

    // Pointeur sur le contenu de la trame re�ue
    eIcuProtoFrame * pFrame = (eIcuProtoFrame *) pBuffer;

#ifdef ICU_DEBUG
    printf( "Message ICU recu : taille=%d\n", iSize );
#endif

    // R�initialisation du compteur d'activit� de la charge
    m_iCuActivityCounter = m_Param.uiTimeOutCu;

    // On indique que le driver est sans erreur car la connexion est active
    FAWSetLevelErr( DRV_NO_ERR );
    
    // Prise du s�maphore d'acc�s � la m�moire partag�e
    iCrQnx = sem_wait( m_pShareMemSem );
    
    if( iCrQnx != EOK )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
    }

    // On s'assure que la pile de messages n'est pas satur�e
    if( m_pShareMem->IcuIn.ucNbMessages >= NB_ICU_MSG_MAX )
    {
        // On incr�mente le compteur de saturations
        m_uiNbSaturations++;

        bCr = false;
    }
    else
    {
        // Index dans la pile de message
        unsigned int uiIndex = m_pShareMem->IcuIn.ucNbMessages;

        // Copie du message dans la m�moire partag�e
        memcpy( m_pShareMem->IcuIn.tMessages[uiIndex].tcData,
                pFrame->tcMsg, pFrame->Header.usMsgSize );

        // Mise � jour de l'identifiant de message
        m_pShareMem->IcuIn.tMessages[uiIndex].Header.usId =
                pFrame->Header.ucMsgId;
        
        // Mise � jour de la taille du message
        m_pShareMem->IcuIn.tMessages[uiIndex].Header.usSize =
                pFrame->Header.usMsgSize;

        // On incr�mente le nombre de messages dans la pile
        m_pShareMem->IcuIn.ucNbMessages++;
    }
    
    // Lib�ration du s�maphore d'acc�s � la m�moire partag�e
    iCrQnx = sem_post( m_pShareMemSem );
    
    if( iCrQnx != EOK )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );      
    }
    
    // On stocke le message si la fonctionnalit� est activ�e
    if( m_Param.bStockageUsed == true )
    {
        StoreMsg( pFrame->tcMsg, pFrame->Header.usMsgSize,
                      pFrame->Header.ucMsgId, FLOW_FROM_CU );
    }
}

/*----------------------------------------------------------------------------*/
/* GetSpecificConfig                                                          */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la r�cuperation du param�trage du driver.
 *
 *  Param�tres r�cup�r�s :
 *  - type d'interface de communication avec la charge utile et configuration
 *    de cette interface
 *  - p�riode d'emission de la telemesure de debug
 *    et initialise les periodes d'emission de la telemesure de debug.
 *  - activation du stockage des message
 *  - activation du routage du message et r�cup�ration de la liste des drivers
 *    � router ainsi que leur identifiant de message associ�
 *
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
void CInterfaceCU::GetSpecificConfig()    
{
    // Constantes utilis�es pour le nommage des param�tres � r�cup�rer
    const string PERIODE_EMISSION_TM_DEBUG = "PeriodeTMDebug";
    const string ACTIVATION_ROUTAGE = "ActivationRoutage";
    const string TIME_OUT = "TimeoutCU";
    const string TYPE_CHECKSUM = "TypeChecksum";
    const string ACTIVATION_STOCKAGE = "ActivationStockage";
    const string TYPE_PROTOCOLE = "Interface";
    const string SERIAL_PROTOCOL = "COM";
    const string UDP_PROTOCOL = "UDP";

    string strValue;  // Variable tampon utilis�e pour r�cup�rer les param�tres
    
    // On r�cup�re la p�riode d'�mission de la t�l�mesure de debug en seconde
    strValue = m_pConfig->GetParamValue( PERIODE_EMISSION_TM_DEBUG );
    
    if( strValue.empty() == false )
    {
        m_SendDataTmDebugRate.Init( CConv::StringToPosInt( strValue, LOC ) );
    }
    
    // On v�rifie si le routage est utilis�
    strValue = m_pConfig->GetParamValue( ACTIVATION_ROUTAGE );
    
    if( strValue.empty() == false )
    {
    	if( CConv::StringToBool( strValue, LOC ) == true )
    	{
    		// On r�cup�re le param�trage du routage
    		GetRouteConfig();
    	}
    }
    
    // On r�cup�re le time out de la charge utile
    strValue = m_pConfig->GetParamValue( TIME_OUT );
    
    if( strValue.empty() == false )
    {
        m_Param.uiTimeOutCu = CConv::StringToPosInt( strValue, LOC );

        // Si le temps est nul, l'�tat de connexion n'est pas surveill�
        if( m_Param.uiTimeOutCu == 0 )
        {
            m_Param.bCheckCuActivity = false;
        }
    }
    
    // On r�cup�re le type de checksum utilis�
    strValue = m_pConfig->GetParamValue( TYPE_CHECKSUM );
    
    if( strValue.empty() == false )
    {
        if( strValue == "XOR" )
        {
            m_Param.ucChecksumType = CChecksum::CHECKSUM_XOR8;
        }
        else if( strValue == "SUM16" )
        {
            m_Param.ucChecksumType = CChecksum::CHECKSUM_SUM16;
        }
        else
        {
            // Le type de checksum n'est pas connu, on log une erreur
            CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_CHECKSUM, _SLOG_ERROR );
        }
    }
    else
    {
        // Le type de checksum n'est pas fourni, on log une erreur
        CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_CHECKSUM, _SLOG_ERROR );
    }
    
    // Param�trage de l'algorithme de calcul du checksum
    m_Checksum.SetType( m_Param.ucChecksumType );

    // On r�cup�re l'indicateur d'activation du stockage
    strValue = m_pConfig->GetParamValue( ACTIVATION_STOCKAGE );
    
    if( strValue.empty() == false )
    {
        m_Param.bStockageUsed = CConv::StringToBool( strValue, LOC );
    }

    // On r�cup�re le type protocole de communication utilis�
    strValue = m_pConfig->GetParamValue( TYPE_PROTOCOLE );

    // La connexion se fait en s�rie
    if( strValue == SERIAL_PROTOCOL )
    {
        m_pCnx = CreateComCnx();
    }
    // La connexion se fait en UDP
    else if( strValue == UDP_PROTOCOL )
    {
        m_pCnx = CreateUdpCnx();
    }
    // Type de protocole inconnu, on l�ve une exception
    else
    {
        CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_SERIAL_ID, _SLOG_ERROR );
    }
}

/*----------------------------------------------------------------------------*/
/* OpenShareMem                                                               */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode ouvre la m�moire partag�e avec l'automate ISaGRAF et
 *  initialise le semaphore qui en arbitre les acc�s concurrents.
 *
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
void CInterfaceCU::OpenShareMem()
{           
    string strShmName;  // Variable contenant le nom de la m�moire partag�e
    
    // Cr�ation du nom de la m�moire partag�e en fonction d'une base fixe et de
    // l'identifiant du driver
    strShmName = string( SHAREMEM_ICU ) + "_" +
                 CConv::IntToString( ( BASGetConfDrv() )->GetDrvId() );
    
    // Cr�ation de la m�moire partag�e et mapping de la m�moire dans l'espace
    // adressable du driver.
    m_pShareMem = (eIfApp *) m_ShareMemory.Create( strShmName.c_str(),
                                                    SHAREMEM_ICU_SIZE );
    
    // R�cup�ration du s�maphore d'acc�s � la m�moire partag�e.
    m_pShareMemSem = m_ShareMemory.GetShareMemSem();
}

/*----------------------------------------------------------------------------*/
/* IFAWConf                                                                   */
/*----------------------------------------------------------------------------*/
/*! Effectue la configuration du driver puis recupere la configuration du 
 *  driver.
 *  Effectue la configuration specifique du driver puis ouvre la memoire 
 *  partagee.
 *
 *  \retval      true : La configuration s'est bien passe
 *  \retval      false : La configuration n'a pu etre faite
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::IFAWConf()
{
    bool bCr = true;    // Compte rendu de la fonction
    
    bCr = CBASDrv::IFAWConf();

    // On effectue la r�cup�ration de la configuration et l'ouverture de la
    // memoire partagee si pas de probleme
    if( bCr == true )
    {
        // R�cup�ration de la configuration du driver
        m_pConfig = BASGetConfDrv();

        // Effectue la configuration specifique du driver
        GetSpecificConfig();
        
        // Ouvre la m�moire partag�e
        OpenShareMem();
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* IFAWDeconf                                                                 */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode ferme la m�moire partag�e puis d�configure le driver.
 *
 *  \retval      true : La deconfiguration s'est bien passe
 *  \retval      false : La deconfiguration n'a pu etre faite
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::IFAWDeconf()
{
    bool bCr = true; // Compte rendu de la m�thode

    // Fermeture de la m�moire partag�e.
    m_ShareMemory.Close();

    // Destruction de la connexion
    delete m_pCnx;
    
    // D�configuration du driver
    bCr = CBASDrv::IFAWDeconf();
    
    return bCr;
}

/*----------------------------------------------------------------------------*/
/* IFAWRun                                                                    */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode lance le timer pour r�veiller periodiquement le driver,
 *  ouvre la connexion avec la charge utile et le canal de stockage
 *
 *  \retval      true : Le passage dans l'etat run a r�ussi
 *  \retval      false : Le passage dans l'etat run a �chou�
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//	- BL-FI-5913 : - Modification du type de la variable m_iCuActivityCounter
////////////////////////////////////////////////////////////////////////////////
bool CInterfaceCU::IFAWRun()
{
    int iCrQnx = EOK; // Compte rendu de fonctions systemes QNX
                                                   
    string strChannelName;   // Variable contenant le nom du canal de stockage
    
    // Cr�ation d'un timer bas� sur la g�n�ration de l'�v�nement afin de pouvoir
    // g�n�rer un pulse toutes les secondes
    m_TimerId = TimerCreate( CLOCK_MONOTONIC, &m_Event );
    
    if( m_TimerId == QNXINTERNALERROR )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
    }

    // Le timer est lanc�
    iCrQnx = TimerSettime( m_TimerId, 0, &m_itime, NULL );
    
    if( iCrQnx == QNXINTERNALERROR )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
    }
    
    // On ouvre la connexion
    m_pCnx->Open();
    
    // Initialisation du compteur de vie de la CU
    m_iCuActivityCounter = m_Param.uiTimeOutCu;
    
    // On ouvre le canal de stockage s'il est utilis�
    if( m_Param.bStockageUsed == true )
    {
        //Ouverture du canal �v�nementiel de stockage
        strChannelName = GetEventChannelName();

        m_Stockage.OpenChanel( CHANNEL_EVT_MIN, strChannelName );
    }

#ifdef ICU_DEBUG
    printf( "Driver ICU demarre\n" );
#endif

    return true;
}

/*----------------------------------------------------------------------------*/
/* IFAWStop                                                                   */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e � l'arr�t du driver.
 *
 *  Traitements effectu�s :
 *  - destruction du timer de r�veil p�riodique du driver
 *  - fermeture du canal de stockage.
 *
 *  \retval      true : Le passage dans l'etat stop s'est bien passe
 *  \retval      false : Le passage dans l'etat stop n'est pas passe
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::IFAWStop()
{
    int iCrQnx = EOK; // Compte rendu de fonctions systemes QNX

    // Le timer est d�truit afin de ne plus g�n�rer l'�v�nement
    iCrQnx = TimerDestroy( m_TimerId );
    
    if( iCrQnx == QNXINTERNALERROR )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );      
    }
    
    // On ferme le canal de stockage si l'option est configur�e
    if( m_Param.bStockageUsed == true )
    {
        m_Stockage.CloseChanel( CHANNEL_EVT_MIN );
    }

	// On ferme la connexion
    if( m_pCnx != NULL )
    {
    	m_pCnx->Close();
    }

    return true;
}

/*----------------------------------------------------------------------------*/
/* IFAWReceivePulse                                                           */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception d'un pulse.
 * 
 *  Si c'est un pulse timer, on v�rifie si la TM debug doit �tre envoy�e, on 
 *  d�cr�mente la vie du driver, et on relance le timer.
 * 
 *  Si c'est un pulse de r�ception de message de la charge utile, on appelle la
 *  m�thode ReceivePendingMsg de la connexion pour que le message soit lu.
 *
 *  \param       [in] iCode : Code du pulse.
 *  \param       [in] iValue : Valeur associee au pulse.
 *  \retval      true : Le pulse a �t� pris en compte.
 *  \retval      false : Le pulse n'a pas �t� pris en compte.
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 12/03/14
//  - BL-FI-4222 : - Modification de la gestion du timeout
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//	- BL-FI-5913 : - Modification du type de la variable m_iCuActivityCounter
//				   - Modification de la constante NO_ACTIVITY
//				   - Modification de la gestion du timeout
////////////////////////////////////////////////////////////////////////////////
bool CInterfaceCU::IFAWReceivePulse( int iCode, int iValue )
{
    const int NO_ACTIVITY = 0; // Constante indiquant l'inactivit�
                                        // de la connexion CU

    int iCrQnx = EOK; // Retour de fonction QNX
    
    bool bCr = true; // Indique que le pulse a �t� pris en compte
    
    bool bNoEmissionDebug = false; // Flag d'�mission de la TM de DEBUG

    // Dispatch du code du pulse
    switch( iCode )
    {
        // Pulse de r�veil p�riodique
        case TIMER_PULSE :

            // On v�rifie l'�tat du flag NoEmission de la t�l�mesure de debug
            bNoEmissionDebug = !m_SaveAndSendDataTmDebugRate.IsNow();

            // On v�rifie si l'�mission de la t�l�mesure de diagnostic s'effecue
            // maintenant
            if( m_SendDataTmDebugRate.IsNow() == true )
            {
                SendDebugTm( bNoEmissionDebug );
            }
            // Verification de la vie de la charge utile
            if( ( m_Param.bCheckCuActivity == true ) &&
                ( m_iCuActivityCounter >= NO_ACTIVITY ) )
            {              
                // Si le compteur tombe � z�ro alors la connexion est perdue
                if( m_iCuActivityCounter == NO_ACTIVITY )
                {
                    // On indique que l'�tat driver est en d�faut majeur
                    FAWSetLevelErr( DRV_ERR_MAJ );

                    // On incr�mente le nombre de connexions perdues
                    m_uiNbLostCu++;
                }
		        // D�cr�mentation du compteur de vie
		        m_iCuActivityCounter--;        
            }

            // Le timer est relanc� apr�s chaque pulse.
            iCrQnx = TimerSettime( m_TimerId, 0, &m_itime, NULL );

            if( iCrQnx == QNXINTERNALERROR )
            {
                CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
            }

            break;

        // Pulse message de la charge utile
        case RCV_MSG_PULSE :

            if( m_pCnx != NULL )
            {
                // On indique � la connexion qu'un message a �t� re�u
                m_pCnx->ReceivePendingMsg( iCode );
            }

            break;

        // Pulse inconnu
        default:

            // Le le pulse est inconnu, on log un warning
            CLog::Log( LOC, ERR_ICU_UNHANDLED_PULSE, _SLOG_WARNING );

            bCr = false;
            break;
    }
    
    return bCr;
}

/*----------------------------------------------------------------------------*/
/* IBASReceiveTc                                                              */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appel�e sur r�ception d'une t�l�commande. La t�l�commande
 *  est analys�e afin de v�rifier qu'elle est correcte puis elle est prise en
 *  compte. Toute t�l�commande incorrecte est rejet�e.
 *
 *  Les t�l�commandes prises en compte sont:
 *      - Abonnement � la t�l�commande de debug
 *      - R�initialisation des donn�es de debug.
 *
 *  \param       [in] iIdMsg : Identifiant de la t�l�commande.
 *  \param       [in] bStationSol : Type de station sol �mettrice
 *  \param       [in] iSize : Taille utile du message de t�l�commande.
 *  \param       [in] pData : Message de t�l�commande.
 *  \retval      true : La t�l�commande a �t� prise en compte.
 *  \retval      false : La t�l�commande n'a pas �t� prise en compte.
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::IBASReceiveTc( int iIdMsg, bool bStationSol, int iSize,
                                  char * pData )
{
    // Taille en octet des t�l�commandes
    const int TC_DEBUG_SIZE = 0; 
    const int TC_RAZDEBUG_SIZE = 0;     
    
    bool bCr = true;    // Compte rendu de fonction

    // Seules les t�l�commandes en provenance de la supervision sont trait�es
    if( bStationSol != SUPERVISOR_STATION )
    { 
        CLog::Log( LOC, ERR_ICU_NOT_SUPERVISOR_STATION_TC, _SLOG_WARNING );
                        
        return false;
    }

    // On traite la TC en fonction de son identifiant
    switch( iIdMsg )
    {
        // T�l�commande de debug
        case TC_DEBUG_ID :

            // V�rification de la taille de la TC
            if( iSize != TC_DEBUG_SIZE )
            {
                CLog::Log( LOC, ERR_ICU_BAD_TC_SIZE,
                           _SLOG_WARNING, iIdMsg, iSize );

                bCr = false;
            }
            else
            {

                //  Forcage de l'�mission de la TM Debug au prochain pulse
                m_SendDataTmDebugRate.ForceNow();

                // D�sinhibition de la TM de Debug
                m_SaveAndSendDataTmDebugRate.SetInhibitionOnDelay();
            }

            break;

        // T�l�commande de reset des donn�es de diagnostic
        case TC_RAZDEBUG_ID :

            // V�rification de la taille de la TC
            if( iSize != TC_RAZDEBUG_SIZE )
            {
                CLog::Log( LOC, ERR_ICU_BAD_TC_SIZE,
                           _SLOG_WARNING, iIdMsg, iSize );

                bCr = false;
            }
            else
            {
                // R�initialisation des donn�es li�es � la connexion
                m_pCnx->ResetDebugData();

                // R�initialisation du nombre de perte de la charge utile.
                m_uiNbLostCu = 0;
                
                // R�initialisation du nombre de messages re�us du driver tiers
                m_uiNbRoutedMsgOk = 0;
                
                // R�initialisation du nombre de messages root�s en �chec
                m_uiNbRoutedMsgNok = 0;
                
                // R�initialisation du nombre de saturation de la pile
                m_uiNbSaturations = 0;

                // R�initialisation du nombre de messages stock�s
                m_uiNbStoredMsg = 0;
            }

            break;

        // T�l�commande d'identifiant inconnu
        default :
            CLog::Log( LOC, ERR_ICU_UNKNOWN_TC, _SLOG_WARNING, iIdMsg );
            
            bCr = false;
            
            break;
    }
    
    return bCr;             
}

/*----------------------------------------------------------------------------*/
/* IBASReceiveMsg                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appel�e sur r�ception d'un message applicatif.
 *  Ce dernier peut provenir soit d'une VM, soit d'un driver tiers.
 *
 *  La m�thode effectue les traitements suivants :
 *  - v�rification de la validit� du message (taille, type, source)
 *  - appel de la m�thode de traitement du message sp�cialis�e adapapt�e �
 *   l'identifiant de la source �mettrice (VM ou driver tiers)
 *
 *  \param       [in] pMsg : Buffer contenant le message
 *  \param       [in] iSize : Taille utile du message
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 16/12/2015
//  Auteur : Thomas FOURCROY - CIO
//  - Prise en compte de BL-FI-6084
//      - Modification du calcul interm�diaire de la taille du message (ne prend
//			 plus en compte le header)
////////////////////////////////////////////////////////////////////////////////
bool CInterfaceCU::IBASReceiveMsg( eMsgApp * pMsg, int iSize )
{
    const unsigned int FIRST_VM_ID = 1; // Premier identifiant de VM
    
    const unsigned int LAST_VM_ID = 8;  // Premier identifiant de VM

    bool bCr = true;                    // Compte rendu de la fonction

    // On v�rifie que la taille du message est valide. A ce stade, on ne peut
    // pas d�terminer si un message de taille nulle est invalide.
    if( iSize < 0 )
    {
        // La taille du message est invalide, on log une erreur
        CLog::Log( LOC, ERR_ICU_INVALID_APP_MSG, _SLOG_WARNING, iSize );

        bCr = false;
    }
    // On v�rifie que le message est applicatif
    else if( pMsg->Header.uiTypeMsg != APP_MSG )
    {
        // Le message n'est pas d'application, on log une erreur
        CLog::Log( LOC, ERR_ICU_NOT_APP_MSG, _SLOG_WARNING,
                   pMsg->Header.uiTypeMsg );

        bCr = false;
    }
    else
    {
        // On d�termine s'il s'agit d'un message �mis par une VM
        if( ( pMsg->Header.uiSrc >= FIRST_VM_ID ) &&
            ( pMsg->Header.uiSrc <= LAST_VM_ID ) )
        {
            bCr = OnReceiveMsgVm( pMsg->pData, iSize );
        }
        // Sinon il s'agit d'un message �mis par un driver tiers
        else
        {
            bCr = OnReceiveMsgDriver( pMsg->pData, iSize,
                                      pMsg->Header.uiSrc );
        }

        // Le contenu du message est invalide, on log une erreur
        if( bCr == false )
        {
            CLog::Log( LOC, ERR_ICU_INVALID_APP_MSG, _SLOG_WARNING, iSize,
                       pMsg->Header.uiSrc );
        }
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* OnReceiveMsgDriver                                                         */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception d'un message applicatif en
 *  provenance d'un driver tiers.
 *
 *  \param       [in] pData : Donn�es utiles du message applicatif
 *  \param       [in] iSize : Taille du message
 *  \param       [in] uiSrc : Identifiant du driver source
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::OnReceiveMsgDriver( const char * pData, int iSize,
                                       unsigned int uiSrc )
{
    bool bCr = true;          // Compte rendu de la m�thode

    unsigned int uiMsgId = 0; // Identifiant du message

    // On v�rifie que la taille du message est valide, sachant qu'une taille
    // nulle est valide
    if( ( iSize < 0 ) || ( iSize > SIZE_ICU_MSG_MAX ) )
    {
        bCr = false;
    }
    else
    {
        // Recherche du routage pour l'idenfiant de driver donn�
        bCr = m_RouteManager.FindRoute( uiSrc, &uiMsgId );

        // On v�rifie que le routage a �t� trouv�
        if( bCr == true )
        {
            // Incr�mentation du nombre de messages rout�s avec succ�s
            m_uiNbRoutedMsgOk++;

            // On demande l'�mission du message
            SendMsgCu( pData, iSize, uiMsgId );
        }
    }

    if( bCr == false )
    {
        // Incr�mentation du nombre d'�checs de routage
        m_uiNbRoutedMsgNok++;
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* OnReceiveMsgVm                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode est appel�e sur r�ception d'un message applicatif en
 *  provenance d'une VM.
 *
 *  \param       [in] pData : Donn�es utiles du message applicatif
 *  \param       [in] iSize : Taille du message
 */
/*----------------------------------------------------------------------------*/
bool CInterfaceCU::OnReceiveMsgVm( const char * pData, int iSize )
{
    bool bCr = true;                    // Compte rendu de la m�thode

    eIcuMsg * pMsg = (eIcuMsg *) pData; // Pointeur vers le message ICU

    // Calcul de la taille du message ICU sans le header
    iSize = iSize - sizeof( eIcuMsgHeader );

    // On v�rifie que la taille du message est valide, sachant qu'une taille
    // nulle est valide
    if( ( iSize < 0 ) || ( iSize > SIZE_ICU_MSG_MAX ) ||
        ( iSize != (int) pMsg->Header.usSize ) )
    {
        bCr = false;
    }
    else
    {
        // On demande l'�mission du message
        SendMsgCu( pMsg->tcData, iSize, pMsg->Header.usId );

        // On stocke le message (si la fonctionnalit� est activ�e)
        if( m_Param.bStockageUsed == true )
        {
            StoreMsg( pMsg->tcData, iSize, pMsg->Header.usId, FLOW_TO_CU );
        }
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* SendDebugTm                                                                */
/*----------------------------------------------------------------------------*/
/*! Cette methode est appel�e pour effectuer l'�mission de la t�l�mesure
 *  contenant les donnees de debug.
 *
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
//Suivi des modifications
//
// Date : 29/04/14
//	- BL-FI-5913 : - Modification du type de la variable m_iCuActivityCounter
//				   - Modification de la constante NO_ACTIVITY
////////////////////////////////////////////////////////////////////////////////
void CInterfaceCU::SendDebugTm( bool bNoEmission )
{
    const int NO_ACTIVITY = 0; // Constante indiquant l'inactivit�
                                        // de la connexion CU

    eMsgTmTc Msg; // Variable message
    
    eMsgTmDebug * pDebugMsg;  // Variable pointeur sur le message � �mettre
    
    pDebugMsg = (eMsgTmDebug *) Msg.pData; // Initialisation du pointeur de
                                           // message
    // Initialisation de l'entete de la TM
    Msg.HeaderTmTc.iIdMsg = TM_DEBUG_ID;
    Msg.HeaderTmTc.bStationSol = SUPERVISOR_STATION;        
    
    // Copie des informations de debug de communication
    pDebugMsg->uiNbRecvNok = m_pCnx->m_uiNbRecvNok;
    pDebugMsg->uiNbRecvOk = m_pCnx->m_uiNbRecvOk;
    pDebugMsg->uiNbSendNok = m_pCnx->m_uiNbSendNok;
    pDebugMsg->uiNbSendOk = m_pCnx->m_uiNbSendOk;
    pDebugMsg->uiNbLostMsg = m_pCnx->m_uiNbLostMsg;
    pDebugMsg->uiNbChecksumNok = m_pCnx->m_uiNbCheckSumErr;
    pDebugMsg->uiNbLostCu = m_uiNbLostCu;
    pDebugMsg->uiNbRoutedMsgOk = m_uiNbRoutedMsgOk;
    pDebugMsg->uiNbRoutedMsgNok = m_uiNbRoutedMsgNok;
    pDebugMsg->uiNbStoredMsg = m_uiNbStoredMsg;
    pDebugMsg->uiNbSaturations = m_uiNbSaturations;
    
    // On indique l'�tat de la connexion si celui-ci est surveill�
    if( m_Param.bCheckCuActivity == true )
    {
        if( m_iCuActivityCounter <= NO_ACTIVITY )
        {
            pDebugMsg->ucConnexionState = CU_CONNECTION_LOST;
        }
        else
        {
            pDebugMsg->ucConnexionState = CU_CONNECTION_ALIVE;
        }
    }
    else
    {
        // Sinon on indique qu'il n'est pas connu
        pDebugMsg->ucConnexionState = CU_CONNECTION_UNKNOWN;
    }

    // Emission du message de t�l�mesure
    m_Telecom.SendTm( Msg, sizeof( eMsgTmDebug ), TM_PRIORITY, bNoEmission );
}

/*----------------------------------------------------------------------------*/
/* SendMsgCu                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode �met un message vers la charge utile.
 *
 *  \param       [in] pcMsg : Tableau d'octets � emettre.
 *  \param       [in] uiSize : Nombre d'octets dans le tableau.
 *  \param       [in] uiId : Identifiant du message
 *  \warning     Exception lev�e si un probleme survient
/*----------------------------------------------------------------------------*/
void CInterfaceCU::SendMsgCu( const char * pcMsg, unsigned int uiSize,
                              unsigned int uiId )
{
    eIcuProtoFrame Frame; // Contenu de la trame � envoyer

    short sChecksum = 0;  // Valeur du checksum

    unsigned int uiFrameSize = 0; // Taille de la trame

    unsigned int uiChecksumSize = 0; // Taille du checksum

    // On renseigne les informations de l'en-t�te
    Frame.Header.ucMsgId = (unsigned char) uiId;
    Frame.Header.usMsgSize = (unsigned int) uiSize;
    Frame.Header.usFrameCounter = m_usSentFrameCounter;

    // Copie des donn�es utiles du message
    memcpy( Frame.tcMsg, pcMsg, uiSize );

    // Calcul de la taille de la trame sans compter le checksum
    uiFrameSize = sizeof( eIcuProtoHeader ) + uiSize;

    // On calcule le checksum
    sChecksum = m_Checksum.Calculate( (unsigned char *) &Frame, uiFrameSize );

    uiChecksumSize = m_Checksum.GetChecksumSize();

    // On ajoute le checksum � la fin du message
    if( uiChecksumSize == sizeof( short ) )
    {
        *(short *) &Frame.tcMsg[uiSize] = sChecksum;
    }
    else
    {
        Frame.tcMsg[uiSize] = (char) sChecksum;
    }

    // Calcul de la taille totale de la trame en comptant le checksum
    uiFrameSize = uiFrameSize + uiChecksumSize;

    // Emission de la trame
    m_pCnx->Send( (char *) &Frame, uiFrameSize );

    // Incr�mentation du compteur de trames �mises
    m_usSentFrameCounter++;
}

/*----------------------------------------------------------------------------*/
/* CreateUdpCnx                                                               */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode r�cup�re le param�trage de l'interface UDP dans le fichier
 *  NPConf.xml puis cr�e une connexion de type UDP avec la charge utile
 *
 *  \retval      CCnx * : Pointeur sur la connexion creee
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
CCnx * CInterfaceCU::CreateUdpCnx()
{
    // D�finitions des constantes relatives au param�trage de la connexion
    const string LOCAL_UDP_NB_PORT = "PortUdpLocal";
    const string REMOTE_UDP_NB_PORT = "PortUdpDistant";
    const string IP_CHARGE_UTILE = "AdresseIPDistante";
        
    CUdpCu * pUdp = NULL; // Pointeur sur l'instance de la connexion
            
    string strValue;      // Variable string tampon
    
    // Instanciation de l'objet connexion UDP
    pUdp = new(nothrow) CUdpCu();
    
    if( pUdp == NULL )
    {
        CLog::LogAndRaiseException( LOC, ERR_ICU_PTR_NULL, _SLOG_ERROR );
    }           
    
    // R�cup�ration de l'adresse IP locale sur le r�seau t�l�com.
    strValue = BASGetTelecomAddrIP( LOCAL_ID );
    
    // Initialisation de l'adresse locale
    pUdp->SetLocalIPAddress( strValue );
    
    // R�cup�ration de l'adresse IP de la charge utile
    strValue = m_pConfig->GetParamValue( IP_CHARGE_UTILE );
    
    if( strValue.empty() == true )
    {
        // L'IP de la charge utile n'est pas renseign�, on quitte
        CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_IP, _SLOG_ERROR );
    }
    
    // Initialisation de l'adresse de la charge utile
    pUdp->SetRemoteIPAddress( strValue );
    
    // On r�cup�re le num�ro de port local
    strValue = m_pConfig->GetParamValue( LOCAL_UDP_NB_PORT );       
    
    if( strValue.empty() == true )
    {
        // Le num�ro de port n'est pas initialis�, on leve une exception
        CLog::LogAndRaiseException( LOC, ERR_ICU_INVALID_PORT, _SLOG_ERROR );
    }
    
    // Initialisation du port local
    pUdp->SetLocalPort ( CConv::StringToPosInt( strValue, LOC ) );
    
    // On r�cup�re le num�ro de port local
    strValue = m_pConfig->GetParamValue( REMOTE_UDP_NB_PORT );      
    
    if( strValue.empty() == true )
    {
        // Le num�ro de port n'est pas initialis�, on leve une exception
        CLog::LogAndRaiseException( LOC, ERR_ICU_INVALID_PORT, _SLOG_ERROR );
    }
    
    // Initialisation du port distant
    pUdp->SetRemotePort( CConv::StringToPosInt( strValue, LOC ) );
    
    // On s�lectionne le type de checksum
    pUdp->SetChecksumType( m_Param.ucChecksumType );
    
    // Installe le handler pour r�ceptionner les messages de fa�on asynchrone
    pUdp->InstallReceiveHandler( sizeof( eIcuProtoFrame ), &m_EventRcvData, this );
    
    return pUdp;
}

/*----------------------------------------------------------------------------*/
/* CreateComCnx                                                               */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode r�cup�re le param�trage de l'interface s�rie dans le fichier
 *  NPConf.xml puis cr�e une connexion de type s�rie avec la charge utile
 *
 *  \retval      CCnx * : Pointeur sur la connexion creee
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
CCnx * CInterfaceCU::CreateComCnx()
{
    // D�finitions des constantes relatives au param�trage de la connexion
    const string IDENTIFIANT_COM = "NumCom";
    const string BAUDRATE = "Bauderate";
    const string PARITY = "Parite";
    const string NBSTOP = "NbBitsStop";
    const string PARITYNO = "aucune";
    const string PARITYEVEN = "paire";
    const string PARITYODD = "impaire";
    const string ONESTOP = "1"; 
    const string TWOSTOP = "2";
    const int DEFAULT_BAUDRATE_VALUE = 9600;
    
    CSerialComCU * pCom = NULL;   // Pointeur sur l'instance de la connexion
    
    string strValue;              // Variable string tampon
    
    string strSerialComName = "/dev/ser"; // Nom du port device utilise
    
    // Instanciation de l'objet connexion UDP
    pCom = new(nothrow) CSerialComCU();
    
    if( pCom == NULL )
    {
        CLog::LogAndRaiseException( LOC, ERR_ICU_PTR_NULL, _SLOG_ERROR );
    }           
            
    // On r�cup�re le nom du port COM utilise
    strValue = m_pConfig->GetParamValue( IDENTIFIANT_COM );     
    
    if( strValue.empty() == true )
    {
        // Le num�ro de port n'est pas initialis�, on leve une exception
        CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_SERIAL_ID, _SLOG_ERROR );
    }

    // Initialise le nom du device au format "dev/ser<x>" ou x est le numero du
    // port.
    strSerialComName = strSerialComName + strValue;                                     

    // Initialisation du nom du port
    pCom->SetSerialComName( strSerialComName );
    
    // Initialisation de la vitesse de transfert si elle est definie
    strValue = m_pConfig->GetParamValue( BAUDRATE );        
    
    // Si elle est d�finie, on initialise la vitesse
    if( strValue.empty() == false )
    {       
        pCom->SetBaudRate( CConv::StringToPosInt( strValue.c_str(), LOC ) );
    }
    else
    {
        pCom->SetBaudRate( DEFAULT_BAUDRATE_VALUE );
    }
    
    // Initialisation de la parite si elle est definie
    strValue = m_pConfig->GetParamValue( PARITY );      
    
    // Si elle est d�finie, on initialise la parit�
    if( strValue.empty() == false )
    {       
        if( strValue == PARITYNO )
        {
            pCom->SetParity( CSerialCom::NO_PARITY );
        }
        else if( strValue == PARITYEVEN )
        {
            pCom->SetParity( CSerialCom::EVEN_PARITY );
        }
        else if( strValue == PARITYODD ) 
        {
            pCom->SetParity( CSerialCom::ODD_PARITY );
        }
        else
        {
            // Configuration inconnue. On l�ve une exception
            CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_PARITY,
                                        _SLOG_ERROR );
        }
    }
    
    // Initialisation du nombre de bits de stop si il est defini.
    strValue = m_pConfig->GetParamValue( NBSTOP );      
    
    // S'il est d�fini, on initialise le nombre de bits de stop
    if( strValue.empty() == false )
    {       
        if( strValue == ONESTOP )
        {
            pCom->SetNbStop( CSerialCom::ONE_STOP_BIT );
        }
        else if( strValue == TWOSTOP )
        {
            pCom->SetNbStop( CSerialCom::TWO_STOP_BIT );
        }
        else
        {
            // Configuration inconnue. On l�ve une exception
            CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_NBSTOP,
                                        _SLOG_ERROR );
        }
    }
    
    // On s�lectionne le type de checksum
    pCom->SetChecksumType( m_Param.ucChecksumType );
    
    // Installe le handler pour r�ceptionner les messages de fa�on asynchrone
    pCom->InstallReceiveHandler( &m_EventRcvData, this );
    
    return pCom;    
}

/*----------------------------------------------------------------------------*/
/* StoreMsg                                                                   */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode permet de stocker le contenu d'un message
 * 
 *  \param       [in] pcMsg : Message � stocker
 *  \param       [in] uiMsgSize : Taille du message � stocker
 *  \param       [in] uiMsgId : Identifiant du message
 *  \param       [in] ucFlowDirection : Sens du flux du message (�mis ou re�u 
 *                                      par la CU)
 */
/*----------------------------------------------------------------------------*/
void CInterfaceCU::StoreMsg( const char * pcMsg,
                             unsigned int uiMsgSize,
                             unsigned int uiMsgId,
                             unsigned char ucFlowDirection )
{
    // Message de stockage � envoyer au driver de stockage
    eStockageMsg StockageMsg;
    
    // Pointeur vers la partie utile du message
    eIcuStockageMsg * pIcuMsg = (eIcuStockageMsg *) StockageMsg.pData;

    int iCrQnx = 0;                  // Compte rendu de fonction qnx

    unsigned int uiPayloadSize = 0;  // Taille de la partie utile du message

    unsigned int uiStoredSize = 0;   // Nombre total d'octets stock�s
    
    // On r�cup�re la date courante
    iCrQnx = clock_gettime( CLOCK_REALTIME, &pIcuMsg->Header.Date );
    
    if( iCrQnx == QNXINTERNALERROR )
    {
        CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
    }

    // Taille de la partie utile du message
    uiPayloadSize = sizeof( pIcuMsg->Header ) + uiMsgSize;

    // Mise � jour du header
    pIcuMsg->usSyncByte = (unsigned short) uiPayloadSize;
    pIcuMsg->Header.ucFlowId = ucFlowDirection;
    pIcuMsg->Header.usSize = (unsigned short) uiMsgSize;
    pIcuMsg->Header.ucMsgId = (unsigned char) uiMsgId;

    // Copie du contenu du message
    memcpy( pIcuMsg->tcData, pcMsg, uiMsgSize );

    // Copie de l'information de taille en fin de trame
    memcpy( &pIcuMsg->tcData[uiMsgSize], &pIcuMsg->usSyncByte,
            sizeof( pIcuMsg->usSyncByte ) );

    // Nombre total d'octets stock�s
    uiStoredSize = uiPayloadSize + 2 * sizeof( pIcuMsg->usSyncByte );

    // On envoie les donn�es au driver de stockage
    m_Stockage.StockeData( CHANNEL_EVT_MIN, StockageMsg, uiStoredSize );

    // Incr�mentation du nombre de messages stock�s
    m_uiNbStoredMsg++;
}

/*----------------------------------------------------------------------------*/
/* GetRouteConfig                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode r�cup�re la liste des drivers routables et les identifiants de
 *  messages associ�s.
 *
 *  \warning     Exception lev�e si un probl�me survient
 */
/*----------------------------------------------------------------------------*/
void CInterfaceCU::GetRouteConfig()
{
    // Nom de base des param�tres de routage
    const string KEYWORD_DRIVER = "IdentifiantDriverRoute";
    const string KEYWORD_MSG    = "IdentifiantMessageRoute";

	bool bCr = true;             // Variable compte-rendu de m�thode

    unsigned int uiIdDriver = 0; // Identifiant de driver

    unsigned int uiIdMsg = 0;    // Identifiant de message

    string strValue; // Variable tampon utilis�e pour r�cup�rer les param�tres

    string strParam; // Variable utilis�e pour constuire le nom du param�tre

    // On boucle sur l'indice des couples de param�tres de routage
    for( unsigned int uiIndex = 1; uiIndex <= NB_MAX_ROUTAGE; uiIndex++ )
    {
        // On construit le nom du param�tre IdentifiantDriverRoute<x>
        strParam = KEYWORD_DRIVER + CConv::IntToString( uiIndex );

        // On r�cup�re la valeur du param�tre dans le fichier de configuration
        strValue = m_pConfig->GetParamValue( strParam );
        
        // Si le param�tre n'est pas d�fini, on sort de la boucle
        if( strValue.empty() == true )
        {
            break;
        }

        // Extraction de l'identifiant de driver
        uiIdDriver = CConv::StringToPosInt( strValue, LOC );

        // On construit le nom du param�tre IdentifiantMessageRoute<x>
        strParam = KEYWORD_MSG + CConv::IntToString( uiIndex );

        // On r�cup�re la valeur du param�tre dans le fichier de configuration
        strValue = m_pConfig->GetParamValue( strParam );

        // Extraction de l'identifiant de message
        uiIdMsg = CConv::StringToPosInt( strValue, LOC );

        // On tente d'ajouter ce routage
        bCr = m_RouteManager.AddRoute( uiIdDriver, uiIdMsg );

        // Si l'ajout du routage �choue, c'est qu'il est invalide ou que
        // l'identifiant du driver a d�j� �t� d�fini
        if( bCr == false )
        {
            CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_REMOTE_DRIVER_ID,
                                        _SLOG_ERROR );
        }
    }

    // Si aucune route n'a �t� d�finie, on l�ve une exception
    if( m_RouteManager.GetNbRoutes() == 0 )
    {
        CLog::LogAndRaiseException( LOC, ERR_ICU_PARAM_REMOTE_DRIVER_ID,
                                    _SLOG_ERROR );
    }
}


/*----------------------------------------------------------------------------*/
/* GetEventChannelName                                                        */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode permet de construire le nom du canal evenementiel de stockage
 *
 *  \return      Nom du fichier de stockage
 *  \warning     Exception levee si un probleme survient
 */
/*----------------------------------------------------------------------------*/
string CInterfaceCU::GetEventChannelName() const
{
    const int OFFSET_YEAR = 1900; // Offset � ajouter au champ date
    const int OFFSET_MONTH = 1;   // Offset � ajouter au champ mois

    // Sp�cification du format � utiliser pour la construction du nom
    const char FORMAT_SPEC[] = "ICU_%u%02u%02u-%02u%02u%02u";

    char tcName[30] = {0};   // Variable utilis�e pour stocker le nom

    struct timespec TimeNow; // Date courante au format EPOCH

    struct tm CalendarDate;  // Date courante au format calendaire

    int iCrQnx = 0;          // Retour de fonction QNX

    // Acquisition de la date courante
    iCrQnx = clock_gettime( CLOCK_REALTIME, &TimeNow );

    // Si une erreur survient, on l�ve une exception
    if( iCrQnx == QNXINTERNALERROR )
    {
         CLog::LogAndRaiseException( LOC, iCrQnx, _SLOG_ERROR );
    }

    // Conversion de la date au format calendaire
    (void) gmtime_r( &TimeNow.tv_sec, &CalendarDate );

    // Formattage du message
    iCrQnx = snprintf( tcName, sizeof( tcName ), FORMAT_SPEC,
                       CalendarDate.tm_year + OFFSET_YEAR, // ann�e
                       CalendarDate.tm_mon + OFFSET_MONTH, // mois
                       CalendarDate.tm_mday,               // jour
                       CalendarDate.tm_hour,               // heures
                       CalendarDate.tm_min,                // minutes
                       CalendarDate.tm_sec );              // secondes

    // Si une erreur survient, on l�ve une exception
    if( iCrQnx < 0 )
    {
         CLog::LogAndRaiseException( LOC, errno, _SLOG_ERROR );
    }

    return string( tcName );
}
