/*! \file CChecksum.h
 *  \brief Spécification de la classe de calcul de sommes de contrôle
 */

#ifndef _CCHECKSUM_H_
#define _CCHECKSUM_H_

/*! \addtogroup interfacecu */
/* @{ */

/*! \class CInterfaceCU
 *  \brief Classe permettant de calculer les checksum supportées par le driver
 *         interface charge utile
 */
class CChecksum
{
    /*------------------------------------------------------------------------*/
    /* Attributs d'instance                                                   */
    /*------------------------------------------------------------------------*/

public:

    /*!
     *   Définition des types de checksums implémentés
     */
    enum
    {
        NO_CHECKSUM,
        /*!< Aucune chsecksum sélectionnée */
        CHECKSUM_SUM16,
        /*!< Checksum de type CRC16 */
        CHECKSUM_XOR8
        /*!< Checksum de type Xor */
    };


protected:

private:

	/*!
	 *    Indique le type de checksum utilisé
	 */
	unsigned int m_uiChecksumType;

	/*------------------------------------------------------------------------*/
	/* Méthodes d'instance                                                    */
	/*------------------------------------------------------------------------*/

public:

	/*!
	 *   \brief Constructeur de la classe.
	 */
	CChecksum();

	/*!
	 *   \brief Destructeur de la classe.
	 */
	virtual ~CChecksum();

	/*!
	 *   \brief Calcule le checksum d'un tableau d'octet.
	 */
	unsigned short Calculate( const unsigned char * ucByteTab, int iSize ) const;

	/*!
	 *   \brief Permet de sélectionner le type de checksum à utiliser
	 */
	void SetType( unsigned int uiType );

    /*!
     *   \brief Donne le nombre d'octets utilisés pour le stockage du checksum
     */
    unsigned int GetChecksumSize();


protected:

private:

    /*!
     *   \brief Calcule le checksum XOR8 d'un tableau d'octet.
     */
    static unsigned short CalculateXor8( const unsigned char * ucByteTab, int iSize );

    /*!
     *   \brief Calcule le checksum SUM16 d'un tableau d'octet.
     */
    static unsigned short CalculateSum16( const unsigned char * ucByteTab, int iSize );

};

/* @} */

#endif //_CCHECKSUM_H_
