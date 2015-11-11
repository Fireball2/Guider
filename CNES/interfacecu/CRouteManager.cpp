/*! \file CRouteManager.cpp
 *  \brief Impl�mentation de la classe de gestion du routage
 */

// Headers Syslay
#include <CLog.h>

// Headers locaux
#include "CRouteManager.h"

/*! \addtogroup interfacecu */

/* @{ */

/*!
 *   Port�e : classe CRouteManager\n
 *   Constante utilis�e pour d�finir le plus petit identifiant de driver autoris�
 */
const unsigned int FIRST_DRIVERID_ALLOWED = 30;

/*!
 *   Port�e : classe CRouteManager\n
 *   Constante utilis�e pour d�finir le plus grand identifiant de driver autoris�
 */
const unsigned int LAST_DRIVERID_ALLOWED = 100;

/* @} */

using namespace std;


/******************************************************************************/
/*                                                                            */
/*                             METHODES PUBLIC                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CRouteManager                                                              */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue l'initialisation de l'instance
 *
 */
/*----------------------------------------------------------------------------*/
CRouteManager::CRouteManager()
{
	// Pas de traitement
}

/*----------------------------------------------------------------------------*/
/* ~CRouteManager                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue la destruction de l'instance
 *
 */
/*----------------------------------------------------------------------------*/
CRouteManager::~CRouteManager()
{
	// Pas de traitement
}

/*----------------------------------------------------------------------------*/
/* AddRoute                                                                   */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode permet de d�finir une route pour l'identifiant de driver
 *  donn� en argument.
 *
 *  La m�thode �choue dans les cas suivants :
 *  - la route a d�j� �t� d�finie pour l'identifiant de driver donn�
 *  - l'identifiant de driver est en dehors de la plage autoris�e
 *
 *  \param    [in] uiDriverId : Identifiant du driver
 *  \param    [in] uiMsgId : Identifiant du message
 *
 *  \retval   true : succ�s de l'ajout de la route
 *  \retval   false : �chec de l'ajout de la route
 */
/*----------------------------------------------------------------------------*/
bool CRouteManager::AddRoute( unsigned int uiDriverId, unsigned int uiMsgId )
{
    // Par d�faut, on indique l'�chec de l'ajout du routage
    bool bCr = false;

    if( ( uiDriverId >= FIRST_DRIVERID_ALLOWED ) &&
    	( uiDriverId <= LAST_DRIVERID_ALLOWED ) )
    {
        // Recherche de l'identifiant dans la table de routage
        TRoutingMap::const_iterator it = m_Routes.find( uiDriverId );

		// Ok, la routage n'est pas encore d�fini pour cet identifiant
		if( it == m_Routes.end() )
		{
		    // Ajout du routage
			m_Routes[uiDriverId] = uiMsgId;

			// On indique le succ�s de l'ajout du routage
			bCr = true;
		}
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* FindRoute                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode recherche la route d�finie pour l'identifiant de driver pass�
 *  en argument et si celle-ci existe fournit l'identifiant de message associ�.
 *
 *  \param    [in] uiDriverId : Identifiant du driver
 *  \param    [out] puiMsgId : Identifiant du message si la route existe
 *
 *  \retval   true : une route d�finie pour l'identifiant de driver existe
 *  \retval   false : aucune route d�finie pour l'identifiant de driver
 */
/*----------------------------------------------------------------------------*/
bool CRouteManager::FindRoute( unsigned int uiDriverId, unsigned int * puiMsgId ) const
{
    // Par d�faut, on indique que le routage n'est pas d�fini
	bool bCr = false;

    // Recherche de l'identifiant dans la table de routage
    TRoutingMap::const_iterator it = m_Routes.find( uiDriverId );

    // OK, le routage existe pour cet identifiant
    if( it != m_Routes.end() )
    {
        // R�cup�ration de l'identifiant de message
        *puiMsgId = it->second;

        // On indique que le routage a �t� trouv�
        bCr = true;
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* GetNbRoutes                                                                */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode retourne le nombre de routes d�finies
 *
 *  \return   Nombre de routes d�finies
 */
/*----------------------------------------------------------------------------*/
unsigned int CRouteManager::GetNbRoutes() const
{
	return m_Routes.size();
}
