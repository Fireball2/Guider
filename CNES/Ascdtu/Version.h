/*! \file App/Ascdtu/Version.h
 *  \brief Spécification de la gestion de la version driver
 */
 
#ifndef _VERSION_H_
#define _VERSION_H_

/*! \def APP_VERSION
*   Version du driver se présentant sous la forme Major.Minor.Build
*   Les évolutions du numéro de version du driver doivent suivre les 
*   consignes ci-dessous :
*       - Une incrémentation du paramètre « MAJOR » se fait lorsque le driver a 
*       subi d'importantes modifications (format des télémesures différentes,
*       changement de valeur de retour d'une méthode) par exemple.
*       - Une incrémentation du paramètre « MINOR » se fait lorsque le driver a 
*       reçu des modifications fonctionnelles (ajout ou suppression de méthodes
*       par exemple).
*       - Une incrémentation du paramètre « BUILD » signifie que le driver a 
*       reçu des corrections de Bug ou des changements non fonctionnels 
*       (modification docs Doxygen, rajout ou suppression de commentaires par
*       exemple).
*/
#define APP_VERSION "3.1.1"

#endif //_VERSION_H_
