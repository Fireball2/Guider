/*! \file interfacecu.cc
 *  \brief Impl�mentation du driver interface charge utile
 */
 
#include "CInterfaceCU.h"

/*! \mainpage Driver et I/O driver Interface charge utile
* 
*   <H1> Objet </H1>
* 
*   Le pr�sent document est le dossier de definition du driver et de l'I/O 
*   driver du driver Interface Charge Utile.
* 
*   Le driver interface une charge utile a travers un lien s�rie ou ethernet 
*   UDP/IP, s�lectionnable dans le fichier de configuration NPConf.xml.
* 
*   Le driver gere la communication avec la charge utile, la communication avec
*   la supervision et l'interface entre les I/O drivers ISaGRAF.
*   
*   Les I/O drivers permettent de fournir un tableau pouvant contenir jusqu'� 
*   10 messages, du driver vers une VM ou d'une VM vers le driver. Dans le sens
*   driver->VM, le tableau de message est �mis gr�ce � une m�moire partag�e. 
*   Dans le sens VM->driver, L'I/O driver enverra les messages du tableau un � 
*   un en utilisant la messagerie asynchrone de la couche syst�me.
* 
*   Le driver peut stocker les messages �chang�s avec la charge utile en 
*   utilisant les fonctionalit�s du driver de stockage. L'option doit �tre 
*   s�lectionn�e dans le fichier de configuration NPConf.xml.
* 
*   Si une liste d'identifiants de driver est fournie dans le fichier 
*   NPConf.xml, le driver Interface Charge Utile peut router les messages re�u 
*   de ces drivers vers la charge utile.
* 
*   Le schema suivant decrit les interfaces du driver et des I/O drivers 
*   associ�s.
* 
*   \image html "ICU.JPG"
* 
*/

/*! \defgroup interfacecu Driver Interface Charge Utile 
*   \brief Le Driver Interface Charge Utile a pour fonction d'interfacer une
*   charge utile quelconque via une liaison s�rie ou UDP/IP. 
*   
*   La classe CInterfaceCU du driver herite de la classe CBASDrv qui est la
*   classe dont herite tout driver. La classe CInterfaceCU specialise le driver 
*   et elle ajoute:
*       - la connexion a la charge utile par lien s�rie ou UDP.
*       - la r�ception des messages �mis par la charge utile et sa mise �
*         disposition au VM ISaGRAF
*       - La reception de messages d'application issus de drivers r�pertori�s
*         dans le fichier NPConf.xml et la transmission de ces messages � la 
*         Charge utile
*       - l'emission d'une telemesure de debug portant sur des informations
*         fonctionnelles de la connexion
*       - le srockage des messages �chang�s entre le driver et la charge utile
* 
*   Le driver s'appuie sur la classe CUdpCu h�rit�e de CUdp pour traiter la  
*   communication avec la charge utile dans le cas d'une liaison UDP/IP. 
*   Il s'appuie la classe CSerialComCu h�rit�e de CSerialCom dans le cas d'une 
*   liaison s�rie.
* 
*   Le driver communique avec le driver de stockage pour sauvegarder les 
*   messages �chang�s entre le driver et la charge utile.
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
    // Variable driver d'interface � une charge utile.
    CInterfaceCU Driver;
    
    // Le driver manager est lance
    Driver.Start( argc, argv );
    
    return EXIT_SUCCESS;
}

/* @} */
