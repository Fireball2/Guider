/*! \file ProtoIcu.h
 *  \brief Définition des données et structures utilisées dans le cadre
 *         du protocole de communication avec la charge utile.
 */

#ifndef _PROTOICU_H_
#define _PROTOICU_H_

// Headers locaux
#include "IfInterfaceCU.h"

//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas créer d'espace libre entre les données
#pragma pack(push, Alignment1Byte, 1)

/*! \addtogroup interfacecu */
/* @{ */

/*!
 *   Structure définissant le contenu du header utilisé dans le protocole de
 *   communication avec la charge utile
 */
typedef struct
{
    unsigned short usFrameCounter;
    /*!< Compteur de trame */
    unsigned short usMsgSize;
    /*!< Taille de trame */
    unsigned char ucMsgId;
    /*!< Identifiant de trame */
} eIcuProtoHeader;

/*!
 *   Structure définissant la partie utile de la trame utilisée dans le
 *   protocole de commmunication avec la charge utile
 */
typedef struct
{
    eIcuProtoHeader Header;
    /*!< Segment header de la trame */
    char tcMsg[SIZE_ICU_MSG_MAX];
    /*!< Segment message de la trame */
    short sReserved;
    /*!< Réserve d'octets supplémentaires pour le stockage du checksum.
     *   Ce champ n'est pas destiné à être utilisé directement. */
} eIcuProtoFrame;

/* @} */

//On réinstalle le type d'alignement préalablement sauvegardé
#pragma pack(pop, Alignment1Byte)


#endif // _PROTOICU_H_
