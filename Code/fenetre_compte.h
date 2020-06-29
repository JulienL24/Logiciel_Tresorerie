#ifndef DEF_MAFENETRE
#define DEF_MAFENETRE
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QHeaderView>
#include <QPushButton>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include "compte.h"
#include "transaction.h"


//!\class MaFenetreCompte
//! \brief Classe de Fenêtre principale de l'interface / Hérite de QWidget.
//! Elle permet de montrer les différents CompteAbstrait dans un tableau, d'afficher les boutons qui réalisent les actions suivantes : création,modification et suppression de comptes et de transactions, affichage des comptes, génération de bilans et de comptes de résultat.

class MaFenetreCompte : public QWidget {
Q_OBJECT
public:
    friend class MaFenetreAgTransNR;
    friend void refresh(QString b);
    //! \brief Constructeur de la classe MaFenetre3.
    //! Constructeur de la classe MaFenetre3, affiche les CompteAbstrait et les boutons.
    explicit MaFenetreCompte(QWidget *parent = nullptr);
    //! \brief Affiche les Transaction effectuées vers/depuis ce CompteAbstrait.
    void AfficherTrans(CompteAbstrait* cp);
    //! \brief Modificateur de la valeur du booléen Cont.
    void setFalseC(bool t){Cont=t;}
    //! \brief Modificateur de la valeur du booléen Cre.
    void setFalseCr(bool t){Cre=t;}
    //! \brief Modificateur de la valeur du booléen chgt.
    void setfchgt(bool t){chgt = t;}
    //! \brief Renvoie une reference sur le dernier CompteVirtuel parcouru.
    vector<CompteAbstrait*>& getDernierCompte(){return dernierCompte;}
    //! \brief Renvoie une reference sur le CompteApplication.
    CompteApplication& getRacine(){return *racine;}
    //! \brief Modificateur du CompteApplication racine.
    void setRacine(CompteApplication& ca){racine=&ca;}
    //! \brief Accesseur de l'attribut contanant les informations sur un Transaction à créer.
    vector<tuple<Compte*,double,double>>&  getv() {return v;}
    //! \brief Efface les CompteAbstrait de l'interface.
    void reset()
    {
        this->racine=&(CompteApplication::getCompteApplication());
        dernierCompte.clear();
        dernierCompte.push_back(racine);
        sic=0;
        refresh(racine);
    }

signals:

public slots:
    //! \brief Charge un nouveau fichier de Compte ou de Transaction.
    //! Permet de charger un nouveau fichier contenant des Compte ou des Transaction.
    void charge();
    //! \brief Affichage d'un Type de CompteAbstrait.
    //! Affiche les CompteAbstrait d'un certain Type défini en argument.
    void affichage_ensemble();
    //! \brief Sert à raffraichir les ComptesAbstraits affichés en fonction de la position dans l'arborescence.
    //! Ce slot permet de raffraichir l'affichage de la QTablewidget en parcourant l'arborescence des CompteAbstrait.
    void AvancerHierarchie(int,int);
    //! \brief Actualise la vue des CompteAbstrait selon le CompteAbstrait parent.
    //! Met à jour les CompteAbstrait visibles selon la position dans l'arborescence des CompteAbstrait.
    void refresh(CompteAbstrait* cp);
    //! \brief Création d'un CompteAbstrait.
    //! Permet de créer un CompteAbstrait de parent le CompteVirtuel dans lequel on se trouve au moment de la création.
    void creation();
    //! \brief Menu des actions sur les CompteAbstrait.
    //! Détaille la liste des actions possibles sur un CompteAbstrait : suppression, modification du nom, affichage des transactions et rapprocher.
    void menu(int,int);
    //! \brief Renvoie à l'affichage du dernier CompteVirtuel affiché précédemment.
    //! Renvoie à l'affichage du dernier CompteVirtuel enregistré dans la variable dernierCompte à la dernière position et le retire du vecteur dernierCompte.
    void retour_arriere();
    //! \brief Création de Transaction.
    //! Ouvre la fenêtre pour créer une Transaction, demande à l'utilisateur de rentrer les informations sur la Transaction et la crée si elle est valide.
    void creationT();
    void clotureLivre();
    //! \brief Création du bilan.
    //! Génere une fenêtre qui affiche le bilan de l'exercice à une date saisie sur l'interface et crée le bilan au format web dans le fichier "bilan.html".
    void genereBilan();
    //! \brief Création du compte de résultat.
    //! Génere une fenêtre qui affiche le compte de résultat de l'exercice entre deux dates saisies sur l'interface et crée le compte de résultat au format web dans le fichier "compte_resultat.html".
    void genereCompteResultat();
private :
    //! \brief Informations concernant une Transaction à créer.
    vector<tuple<Compte*,double,double>> v;
    //! \brief CompteApplication racine de l'arborescence des CompteAbstrait.
    CompteApplication* racine = &(CompteApplication::getCompteApplication());
    //! \brief Bouton de choix du Type.
    QPushButton* listetype;
    //! \brief Bouton de création de CompteAbstrait (appelle la méthode création() ).
    QPushButton* m_creer;
    //! \brief Bouton de retour en arrière (appelle la méthode retour_arrière() ).
    QPushButton* retour;
    //! \brief Bouton de création de Transaction (appelle la méthode créationT() ).
    QPushButton* t_creer;
    //! \brief Bouton de clôture du livre (appelle la méthode clotureLivre() ).
    QPushButton* cloture;
    //! \brief Bouton de chargement d'un fichier de CompteAbstrait et/ou de Transaction.
    QPushButton* charger;
    //! \brief Bouton de génération du bilan (appelle la méthode genereBilan() ).
    QPushButton* bilan;
    //! \brief Bouton de génération du compte de résultat (appelle la méthode genereCompteResultat() ).
    QPushButton* compte_resultat;
    //! \brief QHBoxLayout des colonnes pour l'affichage des CompteAbstrait de titres "Nom", "Num" et "Solde".
    QHBoxLayout* compte_layout;
    //! \brief QHBoxLayout des lignes pour l'affichage des CompteAbstrait un par un.
    QHBoxLayout* couche;
    //! \brief QTableWidget contenant les informations des CompteAbstrait.
    QTableWidget* vueComptes;
    //! \brief QLabel de présentation de la vue des CompteAbstrait.
    QLabel* transfertsl;
    QVBoxLayout* final;
    QVBoxLayout* fin;
    //! \brief Vecteur contenant les CompteAbstrait parcourus (sert dans le cas du retour en arrière).
    vector<CompteAbstrait*> dernierCompte;
    //! \brief Enregistre le type du dernier CompteAbstrait.
    Type dernier_type=Type::M;
    //! \brief Nombre de CompteAbstrait dans l'arborescence, sert à l'identification unique des CompteAbstrait.
    int sic=0;
    //! \brief Booléen qui indique vrai si on continue de modifier une Transaction.
    bool Cont=true;
    //! \brief Booléen qui indique faux si on est en train de créer une Transaction.
    bool Cre=true;
    //! \brief
    bool estmodif=false;
    //! \brief Booléen qui indique vrai si il y a eu un changement lors de la création d'une Transaction.
    bool chgt = true;
};

//! \class MaFenetreDeplace.
//! \brief Fenêtre de déplacement d'un CompteAbstrait.

class MaFenetreDeplace : public QDialog {
Q_OBJECT
public:
    //! \brief Constructeur de MaFenetreDeplace.\n Prend en argument le QDialog parent.
    explicit MaFenetreDeplace(QDialog *parent = nullptr);
    //! \brief initialise MaFenetreDeplace.
    //! Cette méthode initialise MaFenetreDeplace permettant le déplacement d'un compte, avec les comptes fils du compte racine ayant le même type que le compte que l'on déplace.
    void init(Type);
    //! \brief Modifie la valeur du type actuel à celui donné en argument.
    void setType(Type t){actuel=t;}
    //! \brief Modifie la valeur du pointeur sur CompteAbstrait concerne à celui donné en argument.
    void setConcerne(CompteAbstrait* cp){concerne=cp;}
    //! \brief Accesseur de la fenêtre père de la fenêtre actuelle.
    MaFenetreCompte* getpere(){return pere;}
    //! \brief Modifie la valeur du pointeur sur la fenêtre père de la fenêtre actuelle.
    void setPere(MaFenetreCompte* mf){pere=mf;}

signals:
public slots:
    //! \brief Sert à raffraichir les ComptesAbstraits affichés en fonction de la position dans l'arborescence.
    //! Ce slot permet de raffraichir l'affichage de la QTablewidget en parcourant l'arborescence des CompteAbstrait.
    void AvancerHierarchie(int i,int j);
    //! \brief Renvoie à l'affichage du dernier CompteVirtuel affiché précédemment.
    //! Renvoie à l'affichage du dernier CompteVirtuel enregistré dans la variable dernierCompte à la dernière position et le retire du vecteur dernierCompte.
    void retour_arriere();
    //! \brief Actualise la vue des CompteAbstrait selon le CompteAbstrait parent.
    //! Cette méthode permet de rafraichir la qtablewidget lorsqu'on la modifie, on utilise souvent la rafraichissement de la table lorsque l'on modifie un element d'un compte ou que l'on retourne en arrière dans la hiérarchie.
    void refresh(CompteAbstrait* cp);
    //! \brief Valide le déplacement d'un CompteAbstrait.
    //! Déplace un CompteAbstrait vers l'emplacement défini dans l'arborescence des CompteAbstrait.
    void Deplacement();

private :
    //! \brief CompteApplication racine de l'arborescence des CompteAbstrait.
    CompteApplication* racine = &(CompteApplication::getCompteApplication());
    //! \brief QLabel qui indique les instructions pour déplacer un CompteAbstrait.
    QLabel* compte;
    //! \brief Bouton de validation du déplacement du CompteAbstrait.
    QPushButton* valider;
    //! \brief Bouton d'annulation du déplacement du CompteAbstrait.
    QPushButton* refuser;
    //! \brief Bouton de retour en arrière (appelle la méthode retour_arrière() ).
    QPushButton* retour;
    //! \brief QHBoxLayout des colonnes pour l'affichage des CompteAbstrait de titres "Nom", "Num" et "Solde".
    QHBoxLayout* compte_layout;
    //! \brief QHBoxLayout des lignes pour l'affichage des CompteAbstrait un par un.
    QVBoxLayout* couche;
    //! \brief QTableWidget contenant les informations des CompteAbstrait.
    QTableWidget* vueComptes;
    //! \brief Vecteur contenant les CompteAbstrait parcourus (sert dans le cas du retour en arrière).
    vector<CompteAbstrait*> dernierCompte;
    //! \brief Enregistre la valeur du CompteAbstrait actuel.
    Type actuel=Type::M;
    //! \brief Enregistre le type du dernier CompteAbstrait.
    Type dernier_type=Type::M;
    //! \brief Pointeur sur le CompteAbstrait à déplacer.
    CompteAbstrait* concerne;
    //! \brief Pointeur sur la fenêtre parente.
    MaFenetreCompte* pere;
};

//! \class MaFenetreType
//! \brief Fenêtre d'affichage des CompteAbstrait d'un certain Type.
class MaFenetreType:public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe MaFenetreType.
     explicit MaFenetreType(QDialog *parent = nullptr);

signals:

public slots:
    //! \brief Permet l'affichage de tous les CompteAbstrait d'u certain Type.
    void affichage(int);
    //! \brief Menu des actions sur les CompteAbstrait.
    //! Détaille la liste des actions possibles sur un CompteAbstrait : suppression, modification du nom, affichage des transactions et rapprocher.
    void menu(int,int);

private:
        //! \brief QLabel du type de CompteAbstrait.
        QLabel* type;
        //! \brief QHBoxLayout d'une ligne de CompteAbstrait.
        QHBoxLayout* ligne;
        //! \brief QComboBox du choix du type de CompteAbstrait.
        QComboBox* listetype;
        //! \brief QTableWidget qui affiche les informations sur les CompteAbstrait.
        QTableWidget* vueComptes;
        //! \brief QVBoxLayout d'une colonne de CompteAbstrait.
        QVBoxLayout* col;

};

#endif // FENETRE_H





