/*! \file IfInterfaceCU.h
 *  \brief Interface du driver interface charge utile avec l'I/O driver associ�
 */

#ifndef _IFINTERFACECU_H_
#define _IFINTERFACECU_H_

/*! \addtogroup interfacecu */
/* @{ */

/*! \def DATA_VM
 *  Type de message issu de la VM Isagraf
 */
#define DATA_VM     1

/*! \def SHAREMEM_ICU
 *  Prefixe de la memoire partagee du driver charge utile sachant que le nom de
 *  la memoire partagee est ICU_ISAGRAF<Id>, ou Id est l'identifiant 
 *  d'application du driver
 */ 
#define SHAREMEM_ICU "/ICU"

/*! \def SIZE_ICU_MSG_MAX
 *  Macro utilis�e pour d�finir la taille maximum d'un message en nombre d'octets
 */
#define SIZE_ICU_MSG_MAX 100

/*! \def NB_ICU_MSG_MAX
 *  Macro utilis�e pour d�finir la nombre maximum de messages envoy�s �chang�s
 *  par cycle d'�criture ou de lecture de la VM
 */
#define NB_ICU_MSG_MAX 10

/*!
 *  \brief Structure de d�finition du header d'un message
 */
typedef struct
{
    unsigned short usSize;
    /*!< Taille du message */
    unsigned short usId;
    /*!< Identifiant du message */
} eIcuMsgHeader;

/*!
 *  \brief Structure de d�finition d'un message
 */
typedef struct
{
    eIcuMsgHeader Header;
    /*!< Header du message */
    char tcData[SIZE_ICU_MSG_MAX];
    /*!< Contenu du message */
} eIcuMsg;

/*!
 *  \brief Structure de d�finition d'une liste de messages
 */
typedef struct
{
    unsigned char ucNbMessages;
    /*!< Nombre de nouveaux messages � lire par la VM */
    eIcuMsg tMessages[NB_ICU_MSG_MAX];
    /*!< Donnees accessibles en lecture a Isagraf */
} eIcuMsgList;

/*!
 *  \brief Structure de d�finition du contenu de la m�moire partag�e
 */
typedef struct
{
    eIcuMsgList IcuIn;
    /*!< Informations accessibles en lecture par la VM */
    eIcuMsgList IcuOut;
    /*!< Informations accessibles en �criture par la VM */
} eIfApp;

/*! \def SHAREMEM_ICU_SIZE
 *  D�finition de la taille de la m�moire partag�e
 */ 
#define SHAREMEM_ICU_SIZE sizeof( eIfApp )

/* @} */

#endif //_IFINTERFACECU_H_
