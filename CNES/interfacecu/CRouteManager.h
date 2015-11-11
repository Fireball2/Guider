/*! \file CRouteManager.h
 *  \brief Sp�cification de la classe de gestion du routage
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
 *  Cette classe impl�mente une table de routage des messages applicatifs qui
 *  permet de connaitre l'identifiant de message correspondant � un identifiant
 *  de driver donn�.
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
     *  \brief D�finition de la table de routage
     */
    typedef std::map<unsigned int, unsigned int> TRoutingMap;

    /*!
     *  \brief Table de routage
     */
    TRoutingMap m_Routes;

    /*------------------------------------------------------------------------*/
    /* M�thodes d'instance                                                    */
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
     *  \brief M�thode de d�finition d'une route
     */
	bool AddRoute( unsigned int uiDriverId, unsigned int uiMsgId );

    /*!
     *  \brief M�thode de recherche d'une route
     */
	bool FindRoute( unsigned int uiDriverId, unsigned int * puiMsgId ) const;

    /*!
     *  \brief M�thode donnant le nombre de routes d�finies
     */
	unsigned int GetNbRoutes() const;

protected:

private:

};

/* @} */

#endif // _CROUTEMANAGER_H_
