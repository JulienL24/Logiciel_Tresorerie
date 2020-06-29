/**
\file transaction.h
\date 10/06/2020
\version 1
\brief  Fichier d'entête pour les transactions

Ce fichier permet de documenter l'architecture (classes,méthodes) de la partie Transaction de notre projet.
**/


#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "compte.h"
#include<QString>
#include <QTextStream>
#include<iostream>
#include <vector>
#include <QDate>


using namespace std;
class Compte;

/**
    \class TresorerieException
    \brief Génération d'Exception

    Permet de centraliser la gestion des exceptions générées par l'exécution des différentes fonctions du programme
**/
class TresorerieException {
    //! Message à afficher pour l'exception
    //! \brief Message d'erreur
    QString info;
public:
    //! Initialise l'attribut info avec le message d'erreur récupéré
    //! \brief Constructeur prenant le message à afficher
    TresorerieException(const QString& message) :info(message) {}
    //! Retourne le message
    //! \brief Accesseur de info
    QString getInfo() const { return info; }
};

class Transaction{
    friend class TransactionApplication;
    //! Boolean qui indique si la transaction est rapprochée ou non.
    //! \brief Indique dans quel état se situe la transaction.
    bool rapproche=false;
    //! \brief Date de la transaction
    //! Indique la date à laquelle s'est déroulée la transaction
    QDate date;
    //! \brief ID de la transaction.
    //! L'ID permet de différencier deux transactions, en effet plusieurs transactions peuvent prendre le même titre et la même reférence ainsi que la même date.
    unsigned int id;
    //! \brief Référence de la transaction.
    //! Numéro de chèque ou de facture de la transaction. Elle peut être identique pour plusieurs transactions étant donné que ceci mélange les chèques et les factures.
    QString ref;
    //! \brief Titre de la transaction.
    //! Nom donné à la transaction.
    QString titre;
    //! \brief Listes des Compte intervenant dans la transaction, avec le crédit/débit associé.
    //! Vecteur de tuples englobant un Compte ainsi que son crédit et son débit.
    std::vector<std::tuple<Compte*,double,double>> listCompte;
    //! \brief Constructeur de recopie de Transaction.
    //! Constructeur de recopie du Transaction à partir d'une autre instance de Transaction.
    Transaction(const Transaction& t)=default;
    //! Permet d'assigner aux attributs d'une Transaction les attributs d'une autre Transaction définie en paramètres.\n
    //! \brief Operateur =
    Transaction& operator=(const Transaction& t)=default;
    //! Constructeur de Transaction.\n Il prend en argument la date de la Transaction, la référence,le titre,la description de la Transaction avec la liste de Compte, et l'ID.
    //! \brief Constructeur à 5 arguments.
    Transaction(QDate d,QString r,QString t,std::vector<std::tuple<Compte*,double,double>> l,int i):date(d),id(i),ref(r),titre(t),listCompte(l){}
    bool comptePresent(const Compte* c) const;
public:
    //! Positionne le boolean rapproche à vrai.
    //! \brief Mutateur de rapproche.
    void setRapproch(){rapproche=true;};
    //! Retourne l'état de rapproche.
    //! \brief Accesseur de rapproche.
    bool getRapproche() const {return rapproche;};
    //! Retourne la date de la Transaction.
    //! \brief Accesseur de date.
    QDate getDate()const {return date;};
    //! Modifie la valeur de l'attribut date.
    //! \brief Mutateur de date.
    void setDate(QDate d) {date=d;};
    //! Retourne la reference de la Transaction.
    //! \brief Accesseur de la référence.
    QString getRef()const {return ref;};
    //! Modifie la valeur de l'attribut ref.
    //! \brief Mutateur de la référence.
    void setRef(const QString& r) {ref=r;};
    //! Retourne la reference de la Transaction.
    //! \brief Accesseur de la référence.
    QString getTitre() const{return titre;};
    //! Modifie la valeur de l'attribut titre.
    //! \brief Mutateur de titre.
    void setTitre(const QString& t) {titre=t;}
    //! Retourne l'ID de la Transaction.
    //! \brief Accesseur de l'ID.
    unsigned int getid() const{return id;}
    //! Retourne la liste de Compte de la Transaction.
    //! \brief Accesseur de la liste de Compte.
    std::vector<std::tuple<Compte*,double,double>>& getListCompte() {return listCompte;};
    //! Retourne une QStringList remplie avec les noms des Compte impliqués dans la Transaction
    //! \brief Permet de renseigner le nom des Comptes impliqués dans la Transaction.
    QStringList nomlistecompte() const;
    //! Permet de dédoubler la Transaction à l'aide du constructeur de recopie et de renvoyer un pointeur sur le double.
    //! \brief Dédouble la Transaction.
    Transaction* dedouble();
};

class TransactionApplication{
    //! \brief Numero de la dernière Transaction crée.
    //! S'incrémente à la création d'une Transaction.
    unsigned int nb=0;
    //! \brief Vecteur contenant toutes les Transaction.
    vector<Transaction*> listTransaction;
    //!	\brief Fichier de chargement/sauvegarde des Transaction de la session actuelle.
    QString filename = "tmpBaseTransaction.txt";
    //! \brief Constructeur par défaut de la classe TransactionApplication.
    TransactionApplication()=default;
    //! \brief Destructeur de la classe TransactionApplication.
    //! Destructeur de la classe TransactionApplication.\n Appelle la méthode save pour sauvegarder les informations sur les Transaction.
    ~TransactionApplication(){savetrans();};
    //! \brief Constructeur de recopie de la classe TransactionApplication (effacé).
    TransactionApplication(TransactionApplication& m)=delete;
    //! \brief Opérateur d'affectation de la classe TransactionApplication (effacé).
    TransactionApplication& operator=(TransactionApplication& m)=delete;
    /**
       \struct Handler
       \brief Design Pattern Singleton

       Implémentation du design Pattern Singleton avec possibilité de supprimer/recréer l'instance.

    **/
    struct Handler{
     //! \brief Instance de TransactionApplication.
     TransactionApplication* instance;
     //! Constructeur de Handler initialisant l'instance de TransactionApplication à nullptr.
     //! \brief Constructeur de Handler.
     Handler():instance(nullptr){}
     //! Appelle l'instance de TransactionApplication.
     //! \brief Destructeur de Handler.
     ~Handler(){delete instance;}
    };
    //! \brief Singleton de TransactionApplication
    static Handler handler;
public:
    //! Retourne l'instance courante de TransactionApplication.\n Si celle-ci n'existe pas, la fonction en crée une et la retourne.
    //! \brief Création de TransactionApplication.
    static TransactionApplication& getTransactionApplication();
    //! Supprime l'instance courante de TransactionApplication.
    //! \brief Suppression de TransactionApplication.
    static void libererTransactionApplication();
    //! \brief Accesseur de la liste des Transaction.
    //! Renvoie une référence sur la liste des Transaction de ce TransactionApplication.
    vector<Transaction*>& getTransactionListe(){return listTransaction;};
    //! \brief Renvoie un pointeur sur la Transaction d'identifiant donné.
    //! Renvoie un pointeur sur la Transaction d'identifiant précisé en paramètres si elle existe et un pointeur nul sinon.
    Transaction* getTransaction(unsigned int id);
     //! \brief Accesseur de l'attribut nb.
    unsigned int getnb() const {return nb;};
    //! \brief Permet l'incrementation de nb et son renvoie.
    unsigned int usenb(){nb++;return (nb-1);}
    //! \brief Modificateur de l'attribut filename.
    //! Modifie la valeur de l'attribut filename avec la valeur donnée en argument.
    void setFilename(const QString& f="tmpBaseTransaction.txt") { filename = f; }
    //! \brief Accesseur de l'attribut filename.
    //! Retourne un QString de la valeur de l'attribut filename.
    QString getFilename(){ return filename;}
    //! \brief Charge les Transaction de la session.
    //! Ouvre le fichier texte donné en argument et charge les Transaction contenues dans le fichier.
    void load(const QString& f);
    //! \brief Sauvegarde les Transaction de la session.
    //! Ouvre le fichier texte donné en argument et sauvegarde les Transaction contenues dans ce TransactionApplication.
    void savetrans() const;
    //! \brief Crée une nouvelle Transaction.
    //! Crée une nouvelle Transaction d'attributs donnés en paramètres et renvoie un pointeur sur cette dernière. Le boolean permet d'indiquer si on veut un changement sur les comptes associées à la transaction(créditer/débiter compte).
    Transaction* creerTransaction(vector<tuple<Compte*,double,double>>,QDate date, const QString& ref, const QString& titre,unsigned int n,bool changeCompte=true);
    //! \brief Verifie qu'une Transaction est correctement déclarée.
    //!  Verifie qu'une Transaction est correctement déclarée en vérifiant que la somme des débits est égale à la somme des crédits, qu'il y ait plus d'un Compte pour la Transaction et que les Compte impliqués existent.\n De plus, cette fonction factorise la présence multiple d'un Compte et transforme les débits/crédits négatifs.
    bool verifTransac(vector<tuple<Compte*,double,double>>& l);
    //! \brief Supprime une Transaction.
    //!	Supprime une Transaction si elle existe et n'est pas rapprochée en annulant ses actions sur les soldes des Compte impliqués.
    void suppTrans(Transaction* t);
    /**
        \class FilterIterator
        \brief Parcours les Transaction d'un Compte choisi.

        Permet d'itérer sur les Transaction spécifiques à un Compte choisi.
    **/
    class FilterIterator { //
        friend class TransactionApplication;
        //! Instance de TransactionApplication sur laquelle on parcourt les Transaction.
        //! \brief Instance de TransactionApplication courante.
        const TransactionApplication& instance= TransactionApplication::getTransactionApplication();
        //! \brief Position de la Transaction sur lequel pointe l'itérateur
        unsigned int index=0;
        //! Permet de selectionner le mode de parcours des Transaction, lorsque le boolean est a false on ne prend pas en compte les Transactions rapprochées.
        //! \brief Selection du mode de parcours des Transaction.
        bool Rapprocher;
        //! \brief Compte dont on veut parcourir les Transaction.
        const Compte& compte;
        //! Permet de positionner l'itérateur sur la prochaine Transaction du Compte courant.
        //! \brief Participe à l'itération sur la prochaine Transaction.
        void goToTheNextTransaction(){
            if(Rapprocher==true){
                while(index!=instance.listTransaction.size() && instance.listTransaction[index]->comptePresent(&compte)==false) // on navigue dans toutes les transactions
                    index++;
            }
            else{
                while(index!=instance.listTransaction.size() && (instance.listTransaction[index]->comptePresent(&compte)==false || instance.listTransaction[index]->rapproche==true)) // on navigue seulement dans les non rapprochés
                    index++;
            }
        }
        //! Constructeur de FilterIterator.\n Il prend en argument 2 paramètres, une reference sur le Compte choisi et un boolean indiquant le mode de parcours.
        //! \brief Constructeur à 2 arguments.
        FilterIterator(const Compte& c, bool m):Rapprocher(m),compte(c){goToTheNextTransaction();}
   public:
        //! Renvoie vrai si toutes les Transaction que l'on souhaite parcourir l'ont été.
        //! \brief Fonction testant la fin du parcours.
        bool isDone() const {return index==instance.listTransaction.size();}
        //! Permet de positionner l'itérateur sur la prochaine Transaction du Compte courant, seulement si il reste une Transaction à parcourir.
        //! \brief Itère sur la prochaine Transaction.
        void next(){
            if(isDone()) throw TresorerieException("erreur, next sur un iterateur en fin de sequence d'iteration");
            index++; goToTheNextTransaction();
        }
        //! Renvoie une référence sur la Transaction sur laquelle l'itérateur est positionné.
        //! \brief Renvoie une référence sur la Transaction courante.
        Transaction& currentItem() const{
            if(isDone())
                throw TresorerieException("erreur, indirection sur un iterateur en fin de sequence d'iteration");
            return *instance.listTransaction[index];
        }
    };
    //! \brief Renvoie un FilterIterator construit avec les paramètres donnés.
    FilterIterator getFilterIterator(const Compte& c, int m){return FilterIterator(c,m);}
};

#endif // TRANSACTION_H
