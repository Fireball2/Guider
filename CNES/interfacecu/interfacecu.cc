/*! \file interfacecu.cc
 *  \brief Implémentation du driver interface charge utile
 */
 
#include "CInterfaceCU.h"

/*! \mainpage Driver et I/O driver Interface charge utile
* 
*   <H1> Objet </H1>
* 
*   Le présent document est le dossier de definition du driver et de l'I/O 
*   driver du driver Interface Charge Utile.
* 
*   Le driver interface une charge utile a travers un lien série ou ethernet 
*   UDP/IP, sélectionnable dans le fichier de configuration NPConf.xml.
* 
*   Le driver gere la communication avec la charge utile, la communication avec
*   la supervision et l'interface entre les I/O drivers ISaGRAF.
*   
*   Les I/O drivers permettent de fournir un tableau pouvant contenir jusqu'à 
*   10 messages, du driver vers une VM ou d'une VM vers le driver. Dans le sens
*   driver->VM, le tableau de message est émis grâce à une mémoire partagée. 
*   Dans le sens VM->driver, L'I/O driver enverra les messages du tableau un à 
*   un en utilisant la messagerie asynchrone de la couche système.
* 
*   Le driver peut stocker les messages échangés avec la charge utile en 
*   utilisant les fonctionalités du driver de stockage. L'option doit être 
*   sélectionnée dans le fichier de configuration NPConf.xml.
* 
*   Si une liste d'identifiants de driver est fournie dans le fichier 
*   NPConf.xml, le driver Interface Charge Utile peut router les messages reçu 
*   de ces drivers vers la charge utile.
* 
*   Le schema suivant decrit les interfaces du driver et des I/O drivers 
*   associés.
* 
*   \image html "ICU.JPG"
* 
*/

/*! \defgroup interfacecu Driver Interface Charge Utile 
*   \brief Le Driver Interface Charge Utile a pour fonction d'interfacer une
*   charge utile quelconque via une liaison série ou UDP/IP. 
*   
*   La classe CInterfaceCU du driver herite de la classe CBASDrv qui est la
*   classe dont herite tout driver. La classe CInterfaceCU specialise le driver 
*   et elle ajoute:
*       - la connexion a la charge utile par lien série ou UDP.
*       - la réception des messages émis par la charge utile et sa mise à
*         disposition au VM ISaGRAF
*       - La reception de messages d'application issus de drivers répertoriés
*         dans le fichier NPConf.xml et la transmission de ces messages à la 
*         Charge utile
*       - l'emission d'une telemesure de debug portant sur des informations
*         fonctionnelles de la connexion
*       - le srockage des messages échangés entre le driver et la charge utile
* 
*   Le driver s'appuie sur la classe CUdpCu héritée de CUdp pour traiter la  
*   communication avec la charge utile dans le cas d'une liaison UDP/IP. 
*   Il s'appuie la classe CSerialComCu héritée de CSerialCom dans le cas d'une 
*   liaison série.
* 
*   Le driver communique avec le driver de stockage pour sauvegarder les 
*   messages échangés entre le driver et la charge utile.
*/

/*! \addtogroup interfacecu
/* @{ */
 
/*----------------------------------------------------------------------------*/
/* main                                                                       */
/*----------------------------------------------------------------------------*/
/*! Lance le driver interface charge utile en lui fournissant les parametres du 
 *  programme.
 *
 *  \param       [in] argc : Nombre d'arguments dans la ligne de commande.
 *  \param       [in] *argv[] : Tableau d'arguments de type string.
 *  \retval      EXIT_SUCCESS
 *  \warning     Aucun
 */
/*----------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
    // Variable driver d'interface à une charge utile.
    CInterfaceCU Driver;
    
    // Le driver manager est lance
    Driver.Start( argc, argv );
    
    return EXIT_SUCCESS;
}

/* @} */
