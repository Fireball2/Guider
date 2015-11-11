/*! \file IfAscDtu.h
 *  \brief Interface du driver du senseur stellaire DTU
 */
 
#ifndef _IFASCDTU_H_
#define _IFASCDTU_H_

/*! \addtogroup ascdtu */
/* @{ */

/*! \def SHAREMEM_ASCDTU
 *  Prefixe de la memoire partagee de l'ASC DTU sachant que le nom de la memoire
 * 	partagee est ASCDTU_ISAGRAF<Id>, ou Id est l'identifiant d'application du 
 *  driver
 */
#define SHAREMEM_ASCDTU "/ASCDTU_ISAGRAF"

/*! \def NB_CHU
 *  Defini le nombre de CHU supporter par l'ASC DTU
 */ 
#define NB_CHU 4

/*! \def CHU_A
 *  Defini l'index dans le tableau pour acceder au CHUA
 */ 
#define CHU_A 0

/*! \def CHU_B
 *  Defini l'index dans le tableau pour acceder au CHUB
 */ 
#define CHU_B 1

/*! \def CHU_C
 *  Defini l'index dans le tableau pour acceder au CHUC
 */ 
#define CHU_C 2

/*! \def CHU_D
 *  Defini l'index dans le tableau pour acceder au CHUD
 */ 
#define CHU_D 3

/*!
*  \brief Structure contenant les donnees d'un CHU
*/	
typedef struct
{
	double dQ0;
	/*!< Quaternion 0 (scalaire) */	
	double dQ1;
	/*!< Quaternion 1 */	
	double dQ2;
	/*!< Quaternion 2 */	
	double dQ3;
	/*!< Quaternion 3 */	
	unsigned int uiSec;
	/*!< Nombre de secondes a laquelle l'acquisition a ete faite */	
    unsigned int uiNsec;
	/*!< Nombre de nanosecondes a laquelle l'acquisition a ete faite */	
	unsigned char ucValidity;
	/*!< Validant des donnees (true = valide) */	
} eChu;

/*!
*  \brief Structure de la donnee contenue dans la memoire partagee
*/	
typedef struct
{
	eChu	Chu[NB_CHU];
	/*!< Tableau contenant les attitudes des CHU */	
} eIfApp;

/*! \def SHAREMEM_ASCDTU_SIZE
 *  Taille de la memoire partagee utilisee pour stocker les donnees des 
 *  applications. Utilisation d'un define pour la compatibilte avec le C et le
 *  C++
 */
#define SHAREMEM_ASCDTU_SIZE sizeof( eIfApp )

/* @} */

#endif //_IFASCDTU_H_
