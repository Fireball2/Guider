/*! \file CRouteManager.cpp
 *  \brief Implémentation de la classe de gestion du routage
 */

// Headers Syslay
#include <CLog.h>

// Headers locaux
#include "CRouteManager.h"

/*! \addtogroup interfacecu */

/* @{ */

/*!
 *   Portée : classe CRouteManager\n
 *   Constante utilisée pour définir le plus petit identifiant de driver autorisé
 */
const unsigned int FIRST_DRIVERID_ALLOWED = 30;

/*!
 *   Portée : classe CRouteManager\n
 *   Constante utilisée pour définir le plus grand identifiant de driver autorisé
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
/*! Cette méthode effectue l'initialisation de l'instance
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
/*! Cette méthode effectue la destruction de l'instance
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
/*! Cette méthode permet de définir une route pour l'identifiant de driver
 *  donné en argument.
 *
 *  La méthode échoue dans les cas suivants :
 *  - la route a déjà été définie pour l'identifiant de driver donné
 *  - l'identifiant de driver est en dehors de la plage autorisée
 *
 *  \param    [in] uiDriverId : Identifiant du driver
 *  \param    [in] uiMsgId : Identifiant du message
 *
 *  \retval   true : succès de l'ajout de la route
 *  \retval   false : échec de l'ajout de la route
 */
/*----------------------------------------------------------------------------*/
bool CRouteManager::AddRoute( unsigned int uiDriverId, unsigned int uiMsgId )
{
    // Par défaut, on indique l'échec de l'ajout du routage
    bool bCr = false;

    if( ( uiDriverId >= FIRST_DRIVERID_ALLOWED ) &&
    	( uiDriverId <= LAST_DRIVERID_ALLOWED ) )
    {
        // Recherche de l'identifiant dans la table de routage
        TRoutingMap::const_iterator it = m_Routes.find( uiDriverId );

		// Ok, la routage n'est pas encore défini pour cet identifiant
		if( it == m_Routes.end() )
		{
		    // Ajout du routage
			m_Routes[uiDriverId] = uiMsgId;

			// On indique le succès de l'ajout du routage
			bCr = true;
		}
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* FindRoute                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette méthode recherche la route définie pour l'identifiant de driver passé
 *  en argument et si celle-ci existe fournit l'identifiant de message associé.
 *
 *  \param    [in] uiDriverId : Identifiant du driver
 *  \param    [out] puiMsgId : Identifiant du message si la route existe
 *
 *  \retval   true : une route définie pour l'identifiant de driver existe
 *  \retval   false : aucune route définie pour l'identifiant de driver
 */
/*----------------------------------------------------------------------------*/
bool CRouteManager::FindRoute( unsigned int uiDriverId, unsigned int * puiMsgId ) const
{
    // Par défaut, on indique que le routage n'est pas défini
	bool bCr = false;

    // Recherche de l'identifiant dans la table de routage
    TRoutingMap::const_iterator it = m_Routes.find( uiDriverId );

    // OK, le routage existe pour cet identifiant
    if( it != m_Routes.end() )
    {
        // Récupération de l'identifiant de message
        *puiMsgId = it->second;

        // On indique que le routage a été trouvé
        bCr = true;
    }

    return bCr;
}

/*----------------------------------------------------------------------------*/
/* GetNbRoutes                                                                */
/*----------------------------------------------------------------------------*/
/*! Cette méthode retourne le nombre de routes définies
 *
 *  \return   Nombre de routes définies
 */
/*----------------------------------------------------------------------------*/
unsigned int CRouteManager::GetNbRoutes() const
{
	return m_Routes.size();
}
