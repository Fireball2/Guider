/*! \file App/Ascdtu/Version.h
 *  \brief Sp�cification de la gestion de la version driver
 */
 
#ifndef _VERSION_H_
#define _VERSION_H_

/*! \def APP_VERSION
*   Version du driver se pr�sentant sous la forme Major.Minor.Build
*   Les �volutions du num�ro de version du driver doivent suivre les 
*   consignes ci-dessous :
*       - Une incr�mentation du param�tre � MAJOR � se fait lorsque le driver a 
*       subi d'importantes modifications (format des t�l�mesures diff�rentes,
*       changement de valeur de retour d'une m�thode) par exemple.
*       - Une incr�mentation du param�tre � MINOR � se fait lorsque le driver a 
*       re�u des modifications fonctionnelles (ajout ou suppression de m�thodes
*       par exemple).
*       - Une incr�mentation du param�tre � BUILD � signifie que le driver a 
*       re�u des corrections de Bug ou des changements non fonctionnels 
*       (modification docs Doxygen, rajout ou suppression de commentaires par
*       exemple).
*/
#define APP_VERSION "3.1.1"

#endif //_VERSION_H_
