/*! \file CChecksum.cpp
 *  \brief Impl�mentation de classe de calcul de sommes de contr�le
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
/*! Cette m�thode effectue la construction de l'instance
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
/*! Cette m�thode effectue la destruction de l'instance
 *
 *  \warning     Aucun.
 */
/*----------------------------------------------------------------------------*/
CChecksum::~CChecksum()
{
    // Rien � faire
}

/*----------------------------------------------------------------------------*/
/* Calculate                                                                  */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue le calcul du checksum sur le buffer d'octets fourni
 *  en entr�e.
 *
 *  \param       [in] ucByteTab : Buffer de donn�es.
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
/*! Cette m�thode permet de param�trer le type de checksum � utiliser
 *
 *  Les types de checksum support�s sont :
 *  - CHECKSUM_SUM16
 *  - CHECKSUM_XOR8
 *
 *  \param       [in] uiType : Type de checksum � utiliser
 */
/*----------------------------------------------------------------------------*/
void CChecksum::SetType( unsigned int uiType )
{
    m_uiChecksumType = uiType;
}

/*----------------------------------------------------------------------------*/
/* GetChecksumSize                                                            */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode donne le nombre d'octets utilis�s pour le stockage du checksum
 *
 *  \retval     Nombre d'octets utilis�s pour le stockage du checksum (max 2)
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
/*! Cette m�thode effectue le calcul du checksum XOR8 sur le buffer d'octets
 *  fourni en entr�e.
 *
 *  \param       [in] ucByteTab : Buffer de donn�es.
 *  \param       [in] iSize : Nombre d'octets dans le buffer.
 *  \return      Valeur du checksum
 */
/*----------------------------------------------------------------------------*/
unsigned short CChecksum::CalculateXor8( const unsigned char *ucByteTab, int iSize )
{
    unsigned int uiChecksumCalculated = 0;

    //Pour chaque caract�res, on fait un OU Exclusif entre la checksum
    //calcul�e et la valeur ascii du caract�re
    for( int i = 0; i < iSize; i++ )
    {
        uiChecksumCalculated ^= ucByteTab[i];
    }

    return (unsigned short) uiChecksumCalculated;
}

/*----------------------------------------------------------------------------*/
/* CalculateSum16                                                             */
/*----------------------------------------------------------------------------*/
/*! Cette m�thode effectue le calcul du checksum SUM16 sur le buffer d'octets
 *  fourni en entr�e.
 *
 *  \param       [in] ucByteTab : Buffer de donn�es.
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
