#ifndef FENETRE_TRANSACTION_H
#define FENETRE_TRANSACTION_H
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
#include <QCalendarWidget>
#include <QDebug>
#include "compte.h"
#include "transaction.h"
#include "fenetre_compte.h"

//! \class MaFenetreTrans
//! \brief Fenêtre pour visualiser les Transaction d'un CompteAbstrait.
class MaFenetreTrans: public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe MaFenetreTrans.\n Prend en argument le QDialog parent.
    explicit MaFenetreTrans(QDialog *parent = nullptr);
    //! \brief Permet de créer une fenêtre MaFenetreAgTransNR qui agrandit la Transaction en argument.
    void AgrandirTransNR(Transaction* t);
    //! \brief Met à jour l'affichage des Transaction après une modification ou un ajout dans une autre fenêtre.
    void refresh();
    //! \brief Accesseur d'un pointeur sur le QLabel Transactions.
    QLabel* getlab(){return Transactions;}
    //! \brief Accesseur d'un pointeur sur MaFenetreCompte parente.
    MaFenetreCompte* getpere(){return pere;}
    //! \brief Modificateur du pointeur sur MaFenetreCompte parente.
    void setPere(MaFenetreCompte* mf){pere=mf;}
    //! \brief Modificateur du pointeur sur le CompteAbstrait cp.
    void setCompte(CompteAbstrait* c){cp=c;}

signals:
public slots:
    //! \brief Ouvre une fenetre permettant d'agrandir la Transaction, d'afficher les CompteAbstrait participants et de choisir, dans le cas où
    //!  elle n'est pas rapprochée de la modifier ou de la supprimer.
    void menu(int,int);
private:
    //! \brief QLabel qui affiche un message dans la fenêtre actuelle.
    QLabel* Transactions;
    //! \brief QTableWidget d'affichage des Transaction sur un CompteAbstrait.
    QTableWidget* vueTransactions;
    //! \brief QVBoxLayout de mise en forme des Transaction.
    QVBoxLayout* transactions_layout;
    //! \brief CompteAbstrait duquel on veut afficher les Transaction.
    CompteAbstrait* cp;
    //! \brief MaFenetreCompte depuis laquelle on ouvre cette MaFenetreTrans.
    MaFenetreCompte* pere;
};
//! \brief Permet l'affichage d'une fenetre MaFenetreAgTransR aggrandissant les Transaction rapprochées.
void AgrandirTransR(Transaction* t);
//! \brief Permet l'affichage d'une fenêtre MaFenetreTrans affichant une interface pour visualiser les Transaction d'un CompteAbstrait donné en argument.
void AfficherTrans(CompteAbstrait* cp);

//! \class MaFenetreRapprochement
//! \brief Ouvre une fenêtre lorsque l'utilisateur souhaite rapprocher un CompteAbstrait.
class MaFenetreRapprochement : public QDialog{
Q_OBJECT
public:
    //! \brief Permet l'affichage d'une fenêtre MaFenetreRapprochement affichant une interface pour rapprocher un CompteAbstrait donné en argument.
    friend void AfficherRapp(CompteAbstrait* cp);
    //! \brief Constructeur de la classe MaFenetreRapprochement.
    explicit MaFenetreRapprochement(QDialog *parent=nullptr);

signals:
public slots:
    //! \brief Fait le rapprochement du CompteAbstrait.
    void lancerRapprochement();
    //! \brief
    void menu(int,int);

private:
    //! \brief QLabel qui affiche un message dans la fenêtre actuelle.
    QLabel* Transactions;
    //! \brief QTableWidget d'affichage des Transaction sur un CompteAbstrait.
    QTableWidget* vueTransactions;
    //! \brief QTableWidget d'affichage des soldes avant et après rapprochement du CompteAbstrait.
    QTableWidget* vueSolde;
    //! \brief QVBoxLayout de mise en page de la fenêtre actuelle.
    QVBoxLayout* transactions_layout;
    //! \brief Bouton de validation du rapprochement.
    QPushButton* buttonRapp;
    //! \brief Bouton d'annulation du rapprochement.
    QPushButton* retour;
    //! \brief Pointeur sur le CompteAbstrait à rapprocher.
    Compte* concerne;
};
//! \brief Permet l'affichage d'une fenêtre MaFenetreRapprochement affichant une interface pour rapprocher un CompteAbstrait donné en argument.
void AfficherRapp(CompteAbstrait* cp);
//! \class MaFenetreAgTransR
//! \brief Fenêtre d'affichage des Transaction rapprochées.
class MaFenetreAgTransR: public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe MaFenetreAgTransR.
    explicit MaFenetreAgTransR(QDialog *parent = nullptr);
    //! \brief Accesseur du QLabel Transaction.
    QLabel* getLab(){return Transactions;}
    //! \brief Accesseur de l'attribut vueTransactions.
    QTableWidget* getvue(){return vueTransactions;}

private:
    //! \brief QLabel qui affiche un message dans la fenêtre actuelle.
    QLabel* Transactions;
    //! \brief QTableWidget d'affichage des Transaction rapprochées sur un CompteAbstrait.
    QTableWidget* vueTransactions;
    //! \brief QVBoxLayout de mise en page de la fenêtre actuelle.
    QVBoxLayout* transactions_layout;
};

//! \class MaFenetreAgTransNR
//! \brief Fenêtre d'affichage des Transaction rapprochées et des actions possibles.
class MaFenetreAgTransNR: public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe MaFenetreAgTransNR.
    explicit MaFenetreAgTransNR(QDialog *parent = nullptr);
    //! \brief Accesseur du QLabel Transactions.
    QLabel* getLab(){return Transactions;}
    //! \brief Accesseur de l'attribut vueTransactions.
    QTableWidget* getvue(){return vueTransactions;}
    //! \brief Accesseur de la Transaction concerne.
    Transaction* getTrans(){return concerne;}
    //! \brief Modificateur de la Transaction concerne.
    void setTrans(Transaction* t){concerne=t;}
    //! \brief Accesseur d'un pointeur sur MaFenetreCompte parente.
    MaFenetreTrans* getpere(){return pere;}
    //! \brief Modificateur du pointeur sur la MaFenetreCompte parente.
    void setPere(MaFenetreTrans* mf){pere=mf;}
    //! \brief Accesseur du QPushButton de suppression.
    QPushButton* getSupp(){return Supp;}
    //! \brief Met à jour les informations sur une Transaction après modification.
    void refresh(Transaction* t);
    //! \brief Accesseur du QPushButton de modification.
    QPushButton* getModif(){return Modif;}

signals:
public slots:
    //! \brief Supprime une Transaction en annulant les débits et crédits sur les CompteAbstrait impliqués.
    void SuppTrans();
    //! \brief Modifie une Transaction, son nom ou les montants ou les CompteAbstrait impliqués.
    void modifTrans();
private:
    //! \brief QLabel qui affiche un message dans la fenêtre actuelle.
    QLabel* Transactions;
    //! \brief QTableWidget d'affichage des Transaction sur un CompteAbstrait.
    QTableWidget* vueTransactions;
    //! \brief Bouton de modification de la Transaction.
    QPushButton* Modif;
    //! \brief Bouton de supression de la Transaction.
    QPushButton* Supp;
    //! \brief QVBoxLayout de mise en page de la fenêtre actuelle.
    QVBoxLayout* transactions_layout;
    //! \brief Pointeur sur la Transaction concernée.
    Transaction* concerne;
    //! \brief Pointeur sur la fenêtre MaFenetreTrans parente.
    MaFenetreTrans* pere;
};

//! \class MaFenetreEnCours
//! \brief Fenêtre d'affichage d'une Transaction en cours de création.
class MaFenetreEnCours: public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe MaFenetreEnCours.
    explicit MaFenetreEnCours(QDialog *parent = nullptr);
    //! \brief Accesseur du QLabel Transactions.
    QLabel* getLab(){return Transactions;}
    //! \brief Accesseur du QTableWidget des CompteAbstrait impliqués.
    QTableWidget* getvue(){return vueTransactions;}
    //! \brief Accesseur de la MaFenetreCompte parente.
    MaFenetreCompte* getpere(){return pere;}
    //! \brief Modificateur de la MaFenetreCompte parente.
    void setPere(MaFenetreCompte* mf){pere=mf;}
    //! \brief Acceseur du QPushButton de rajout d'un CompteAbstrait.
    QPushButton* getCont(){return continuer;}
    //! \brief Acceseur du QPushButton d'annulation de création de la Transaction.
    QPushButton* getArr(){return arreter;}
    //! \brief Acceseur du QPushButton de création de la Transaction.
    QPushButton* getCre(){return cre;}
    //! \brief Acceseur du QPushButton de modification de la Transaction.
    QPushButton* getmod(){return modif;}

signals:
public slots:
    //! \brief Permet de modifier les valeurs associées à un CompteAbstrait dans une Transaction en creation qui est erronnée.
    void modif_cpt(int,int);
    //! \brief Permet l'annulation de la création de la Transaction.
    void cancel();
    //! \brief Permet la création de la Transaction.
    void creer();
    //! \brief Permet la modification de la Transaction en cours de création.
    void modifier();
    //! \brief Permet de rajouter un CompteAbstrait à impliquer lors de la création de la Transaction.
    void m_continuer();
private:
    //! \brief QLabel permettant d'afficher un message dans la fenêtre actuelle.
    QLabel* Transactions;
    //! \brief QLabel permettant d'afficher les CompteAbstrait impliqués dans la Transaction en cours de création dans la fenêtre actuelle.
    QTableWidget* vueTransactions;
    //! \brief QVBoxLayout permettant de gérer la mise en page des éléments dans la fenêtre actuelle.
    QVBoxLayout* transactions_layout;
    //! \brief QPushButton de rajout d'un CompteAbstrait dans la Transaction en cours.
    QPushButton* continuer;
    //! \brief QPushButton d'annulation de la création de la Transaction.
    QPushButton* arreter;
    //! \brief QPushButton de création de la Transaction.
    QPushButton* cre;
    //! \brief Fenêtre MaFenetreCompte parente de la fenêtre actuelle.
    MaFenetreCompte* pere;
    //! \brief QPushButton de modification de la Transaction.
    QPushButton* modif;
};

//! \brief Classe FenDate.\n
//! Permet de choisir une date en présentant un calendrier.
class FenDate: public QDialog{
Q_OBJECT
public:
    //! \brief Constructeur de la classe FenDate.\n
    //! Constructeur de la classe FenDate. Prend en argument le QDialog parent.
    explicit FenDate(QDialog *parent = nullptr);
    //! \brief Renvoie la QDate sélectionnée.\n
    //! Revoie la valeur de la QDate sélectionnée sur le calendarWidget.
    QDate selectedDate() const;
private:
    //! \brief QCalendarWidget qui affiche le calendrier.
    QCalendarWidget *calendarWidget;
    //! \brief Fenêtre d'affichage du calendrier.
    QVBoxLayout* aff;
    //! \brief Bouton de validation de la QDate.
    QPushButton* valider;
    //! \brief QWidget de la fenêtre d'affichage du choix de la date.
    QWidget* widget;
};
#endif // FENETRE_TRANSACTION_H

