/*! \file ProtoIcu.h
 *  \brief D�finition des donn�es et structures utilis�es dans le cadre
 *         du protocole de communication avec la charge utile.
 */

#ifndef _PROTOICU_H_
#define _PROTOICU_H_

// Headers locaux
#include "IfInterfaceCU.h"

//On sauvegarde l'alignement courant et on force un alignement
//sur 1 octet afin de en pas cr�er d'espace libre entre les donn�es
#pragma pack(push, Alignment1Byte, 1)

/*! \addtogroup interfacecu */
/* @{ */

/*!
 *   Structure d�finissant le contenu du header utilis� dans le protocole de
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
 *   Structure d�finissant la partie utile de la trame utilis�e dans le
 *   protocole de commmunication avec la charge utile
 */
typedef struct
{
    eIcuProtoHeader Header;
    /*!< Segment header de la trame */
    char tcMsg[SIZE_ICU_MSG_MAX];
    /*!< Segment message de la trame */
    short sReserved;
    /*!< R�serve d'octets suppl�mentaires pour le stockage du checksum.
     *   Ce champ n'est pas destin� � �tre utilis� directement. */
} eIcuProtoFrame;

/* @} */

//On r�installe le type d'alignement pr�alablement sauvegard�
#pragma pack(pop, Alignment1Byte)


#endif // _PROTOICU_H_
