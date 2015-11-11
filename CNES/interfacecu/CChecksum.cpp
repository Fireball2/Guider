/*! \file CChecksum.cpp
 *  \brief Implémentation de classe de calcul de sommes de contrôle
 */

#include "CChecksum.h"

/*! \addtogroup interfacecu */

/* @{ */

/* @} */

/******************************************************************************/
/*                                                                            */
/*                          METHODES PUBLIC                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CChecksum                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette méthode effectue la construction de l'instance
 *
 *  \warning     Aucun.
 */
/*----------------------------------------------------------------------------*/
CChecksum::CChecksum()
{
    m_uiChecksumType = NO_CHECKSUM;
}

/*----------------------------------------------------------------------------*/
/* ~CChecksum                                                                 */
/*----------------------------------------------------------------------------*/
/*! Cette méthode effectue la destruction de l'instance
 *
 *  \warning     Aucun.
 */
/*----------------------------------------------------------------------------*/
CChecksum::~CChecksum()
{
    // Rien à faire
}

/*----------------------------------------------------------------------------*/
/* Calculate                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette méthode effectue le calcul du checksum sur le buffer d'octets fourni
 *  en entrée.
 *
 *  \param       [in] ucByteTab : Buffer de données.
 *  \param       [in] iSize : Nombre d'octets dans le buffer.
 *  \return      Valeur du checksum
 */
/*----------------------------------------------------------------------------*/
unsigned short CChecksum::Calculate( const unsigned char * ucByteTab, int iSize ) const
{
    short sChecksumCalculated = 0;

    // Choix de l'algorithme de calcul de somme
    switch( m_uiChecksumType )
    {
        case CHECKSUM_SUM16 :

            sChecksumCalculated = CalculateSum16( ucByteTab, iSize );

            break;

        case CHECKSUM_XOR8 :

            sChecksumCalculated = CalculateXor8( ucByteTab, iSize );

            break;

        default:

            break;
    }
    
    return sChecksumCalculated;
}

/*----------------------------------------------------------------------------*/
/* SetType                                                                    */
/*----------------------------------------------------------------------------*/
/*! Cette méthode permet de paramétrer le type de checksum à utiliser
 *
 *  Les types de checksum supportés sont :
 *  - CHECKSUM_SUM16
 *  - CHECKSUM_XOR8
 *
 *  \param       [in] uiType : Type de checksum à utiliser
 */
/*----------------------------------------------------------------------------*/
void CChecksum::SetType( unsigned int uiType )
{
    m_uiChecksumType = uiType;
}

/*----------------------------------------------------------------------------*/
/* GetChecksumSize                                                            */
/*----------------------------------------------------------------------------*/
/*! Cette méthode donne le nombre d'octets utilisés pour le stockage du checksum
 *
 *  \retval     Nombre d'octets utilisés pour le stockage du checksum (max 2)
 */
/*----------------------------------------------------------------------------*/
unsigned int CChecksum::GetChecksumSize()
{
    const unsigned int SIZEOF_CHECKSUM_SUM16 = 2;
    const unsigned int SIZEOF_CHECKSUM_XOR8 = 1;

    unsigned int uiSize = 0;

    switch( m_uiChecksumType )
    {
        case CHECKSUM_SUM16 :

            uiSize = SIZEOF_CHECKSUM_SUM16;
            break;

        case CHECKSUM_XOR8 :

            uiSize = SIZEOF_CHECKSUM_XOR8;
            break;

        default:

            break;
    }

    return uiSize;
}


/******************************************************************************/
/*                                                                            */
/*                          METHODES PRIVATE                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* CalculateXor8                                                              */
/*----------------------------------------------------------------------------*/
/*! Cette méthode effectue le calcul du checksum XOR8 sur le buffer d'octets
 *  fourni en entrée.
 *
 *  \param       [in] ucByteTab : Buffer de données.
 *  \param       [in] iSize : Nombre d'octets dans le buffer.
 *  \return      Valeur du checksum
 */
/*----------------------------------------------------------------------------*/
unsigned short CChecksum::CalculateXor8( const unsigned char *ucByteTab, int iSize )
{
    unsigned int uiChecksumCalculated = 0;

    //Pour chaque caractères, on fait un OU Exclusif entre la checksum
    //calculée et la valeur ascii du caractère
    for( int i = 0; i < iSize; i++ )
    {
        uiChecksumCalculated ^= ucByteTab[i];
    }

    return (unsigned short) uiChecksumCalculated;
}

/*----------------------------------------------------------------------------*/
/* CalculateSum16                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette méthode effectue le calcul du checksum SUM16 sur le buffer d'octets
 *  fourni en entrée.
 *
 *  \param       [in] ucByteTab : Buffer de données.
 *  \param       [in] iSize : Nombre d'octets dans le buffer.
 *  \return      Valeur du checksum
 */
/*----------------------------------------------------------------------------*/
unsigned short CChecksum::CalculateSum16( const unsigned char *ucByteTab, int iSize )
{
    unsigned int uiSum = 0;

    bool bAddHighByte = true;

    unsigned int uiChecksumCalculated = 0;

    for( int i = 0; i < iSize; i++ )
    {
        if( bAddHighByte )
        {
            uiSum += ( ( ucByteTab[i] ) << 8 ) ^ 0xFF00;
            bAddHighByte = false;
        }
        else
        {
            uiSum += ( ucByteTab[i] ) ^ 0x00FF;
            bAddHighByte = true;
        }
    }

    if( bAddHighByte == false )
    {
        uiSum += 0xFF;
    }

    uiChecksumCalculated = ( ( uiSum >> 16 ) & 0xFFFF ) + ( uiSum & 0xFFFF );
    uiChecksumCalculated = ( ( uiChecksumCalculated >> 16 ) & 0xFFFF ) +
                           ( uiChecksumCalculated & 0xFFFF );

    return (unsigned short) uiChecksumCalculated;
}
