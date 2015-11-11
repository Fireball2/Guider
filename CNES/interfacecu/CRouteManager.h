/*! \file CRouteManager.h
 *  \brief Spécification de la classe de gestion du routage
 *
 */

#ifndef _CROUTEMANAGER_H_
#define _CROUTEMANAGER_H_

/*! \addtogroup interfacecu */
/* @{ */

// Headers C++
#include <map>

/*! \class CRouteManager
 *  \brief Classe de gestion de la table de routage des messages applicatifs
 *
 *  Cette classe implémente une table de routage des messages applicatifs qui
 *  permet de connaitre l'identifiant de message correspondant à un identifiant
 *  de driver donné.
 */
class CRouteManager
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/

public:

protected:

private:

    /*!
     *  \brief Définition de la table de routage
     */
    typedef std::map<unsigned int, unsigned int> TRoutingMap;

    /*!
     *  \brief Table de routage
     */
    TRoutingMap m_Routes;

    /*------------------------------------------------------------------------*/
    /* Méthodes d'instance                                                    */
    /*------------------------------------------------------------------------*/

public:

    /*!
     *  \brief Constructeur de la classe
     */
	CRouteManager();

    /*!
     *  \brief Destructeur de la classe
     */
	~CRouteManager();

    /*!
     *  \brief Méthode de définition d'une route
     */
	bool AddRoute( unsigned int uiDriverId, unsigned int uiMsgId );

    /*!
     *  \brief Méthode de recherche d'une route
     */
	bool FindRoute( unsigned int uiDriverId, unsigned int * puiMsgId ) const;

    /*!
     *  \brief Méthode donnant le nombre de routes définies
     */
	unsigned int GetNbRoutes() const;

protected:

private:

};

/* @} */

#endif // _CROUTEMANAGER_H_
