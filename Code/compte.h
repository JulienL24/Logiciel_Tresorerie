/**
\file compte.h
\date 06/06/2020
\version 1
\brief  Fichier d'entête pour les comptes

Ce fichier permet de documenter l'architecture (classes,méthodes) de la partie Compte de notre projet.
**/

#ifndef TRESORERIE_H
#define TRESORERIE_H

#include "transaction.h"
#include<QString>
#include <QTextStream>
#include<iostream>
#include <vector>
#include <QDate>
#include <QByteArray>


using namespace std;

/**
    \class TypeCompte
    \brief Types de compte

    Différents types de compte:\n A=Actif\n P=Passif\n R=Recette\n D=Dépense\n M=Manager(type spécial pour le CompteApplication)
**/

enum class Type{A,P,R,D,M};

//! Permet de convertir un Type de compte en un caractère
//! \brief Convertir Type en caractère
QString converType(Type t);


class Compte;
/**
   \class CompteAbstrait
   \brief Classe de compte abstrait

   Le compte abstrait permet de factoriser certaines méthodes communes à la fois au comptes réels et virtuels.\n
   Il se spécialise en comptes réels et en comptes virtuels, c'est donc une classe abstraite.

**/
class CompteAbstrait{
protected:
    //! Nom du CompteAbstrait.
    //! \brief Nom.
    QString nom;
    //! Numéro du CompteAbstrait.
    //! \brief Numero.
    unsigned int num;
    //! Type du CompteAbstrait (A,P,R,D,M).
    //! \brief Type.
    Type TypeCompte;
    //! CompteAbstrait parent (dans la hierarchie) du CompteAbstrait.
    //! \brief Parent.
    CompteAbstrait* pere=nullptr;
    //! Constructeur de compte abstrait.\n Il prend en argument le nom du compte, son numéro, son type et le compte virtuel parent.
    //! \brief Constructeur à 4 arguments.
    CompteAbstrait(const QString& no,unsigned int n,Type t,CompteAbstrait* p=nullptr):nom(no),num(n),TypeCompte(t),pere(p){}
    //! Constructeur de recopie du compte abstrait.\n Il prend en argument un autre CompteAbstrait.
    //! \brief Constructeur de recopie.
    CompteAbstrait(const CompteAbstrait& c)=default;
    //!	Surcharge de l'opérateur '='.\n Permet d'assigner aux attributs d'un compte abstrait les attributs d'un autre CompteAbstrait défini en paramètres.
    //! \brief Affectation d'un CompteAbstrait.
    CompteAbstrait& operator=(const CompteAbstrait& c)=default;
public:
    //! Retourne le nom du CompteAbstrait.
    //! \brief Accesseur de nom.
    QString getNom() const { return nom; }
    //! Modifie la valeur de l'attribut nom.
    //! \brief Mutateur de nom.
    void setNom(const QString& n){nom=n;};
    //! Retourne le numéro du CompteAbstrait.
    //! \brief Accesseur de Numéro.
    unsigned int getNumero() const { return num;};
    //! Retourne un pointeur vers le CompteAbstrait parent.
    //! \brief Accesseur de pere.
    CompteAbstrait* getPere() const{ return pere;}
    //! Modifie le pointeur vers le CompteAbstrait parent.
    //! \brief Mutateur de pere.
    void setPere(CompteAbstrait* nouv) {pere=nouv;}
    //! Retourne le type de CompteAbstrait .
    //! \brief Accesseur de type.
    Type getType() const {return TypeCompte;};
    //! Retourne ce CompteAbstrait ou un CompteAbstrait fils si il possède le nom en paramètre.
    //! \brief Renvoie un CompteAbstrait correspondant au nom fourni en paramètre (Méthode virtuelle pure).
    virtual CompteAbstrait* getCompte(const QString& no)=0;
    //! Retourne ce CompteAbstrait ou un CompteAbstrait fils si il possède le numéro en paramètre.
    //! \brief Renvoie un CompteAbstrait correspondant au numéro fourni en paramètre (Méthode virtuelle pure).
    virtual CompteAbstrait* getCompte(unsigned int nu)=0;
    //! Retourne le solde de ce CompteAbstrait si c'est un Compte ou la somme des soldes des Compte fils si c'est un CompteVirtuel.
    //! \brief Renvoie le solde du compte (Méthode virtuelle).
    virtual double getSolde() const=0;
    //! Retourne le solde avant la date fournie en paramètre de ce CompteAbstrait si c'est un Compte ou la somme des soldes avant la date des Compte fils si c'est un CompteVirtuel.
    //! \brief Renvoie le solde du compte avant la date(Méthode virtuelle).
    virtual double getSoldeAvantDate(QDate date) const = 0;
    //! Retourne le solde sur la période définie en paramètres de ce CompteAbstrait si c'est un Compte ou la somme des soldes sur cette période des Compte fils si c'est un CompteVirtuel.
    //! \brief Renvoie le solde du compte sur la période donnée(Méthode virtuelle).
    virtual double getSoldeSurPeriode(QDate date_d,QDate date_f) const = 0;
    //! Retourne une QString contenant le solde sur la période et le nom du Compte et si c'est un compte abstrait fait de même pour l'ensemble de ses Compte fils.\n Cette méthode est utilisée pour l'affichage du resultat sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée(Méthode virtuelle).
    virtual QString res(QDate date_d,QDate date_f) const=0;
    //! Retourne une QString contenant le solde sur la période et le nom du Compte et si c'est un compte abstrait fait de même pour l'ensemble de ses Compte fils.\n Cette méthode est utilisée pour l'affichage du resultat du fichier web.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée(Méthode virtuelle).
    virtual QString resWeb(QDate date_d,QDate date_f) const=0;
    //! Retourne une QString contenant le solde avant la date donnée et le nom du Compte et si c'est un compte abstrait fait de même pour l'ensemble de ses Compte fils.\n Cette méthode est utilisée pour l'affichage du bilan sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde avant la date donnée(Méthode virtuelle).
    virtual QString bilan(QDate date)=0;
    //! Retourne une QString contenant le solde avant la date donnée et le nom du Compte et si c'est un compte abstrait fait de même pour l'ensemble de ses Compte fils.\n Cette méthode est utilisée pour l'affichage du bilan du fichier web.
    //! \brief Renvoie une QString avec le nom et le solde avant la date donnée(Méthode virtuelle).
    virtual QString BilanWeb(QDate date)=0;
    //! \brief Destructeur par défaut (Méthode virtuelle).
    virtual ~CompteAbstrait()=0;
    //! Effectue la sauvegarde d'un CompteAbstrait dans un fichier texte afin de conserver les données.
    //! \brief Permet de sauvegarder un CompteAbstrait.
    virtual void Save(QTextStream* flux) const{
        *flux<<nom<<";"<<converType(TypeCompte)<<";"<<num<<";";
        if(TypeCompte!=Type::M)
            *flux<<getPere()->num<<";";
    };
    //! Renvoie true si un CompteAbstrait a été impliqué dans une transaction.
    //! \brief Verifie la présence d'une transaction pour ce compte (Méthode virtuelle pure).
    virtual bool PresenceTrans() const=0;
    //! Renvoie true si un CompteAbstrait possède un solde, et est donc un Compte. Renvoie false si un CompteAbstrait est un CompteVirtuel.
    //! \brief Renseigne sur la virtualité ou non du compte (Méthode virtuelle pure).
    virtual bool concret() const=0;
    //! Prépare le vecteur de transaction Dépense permettant de solder les comptes dépenses ainsi que le vecteur de transaction Résultat permettant de solder les comptes résultats.
    //! \brief Fait partie du processus de cloture du livre (Méthode virtuelle pure).
    virtual void Cloture(vector<tuple<Compte*,double,double>>& dep, vector<tuple<Compte*,double,double>>& rec, double& d, double& r)=0;
    //! Remplie une QStringList avec les CompteAbstrait fils qui nous intéresse.
    //! \brief Utilisé pour récupérer des noms de CompteAbstrait et les afficher dans l'interface (Méthode virtuelle pure).
    virtual void liste_nom_type(QStringList& b,bool Concret)=0;
};

/**
   \class Compte
   \brief Classe de Compte Concret

   Le compte est un compte abstrait possédant un solde et pouvant apparaitre dans des Transactions\n
   Il est situé à l'intérieur des CompteVirtuel.

**/
class Compte: public CompteAbstrait {
    friend class FabriqueCompte;
    friend class CompteApplication;
    //! Solde du Compte.
    //! \brief Solde.
    double solde=0;
    //! Solde après le dernier rapprochement.
    //! \brief Solde rapproché.
    double derraproch=0;
    //!	Constructeur de recopie du Compte à partir d'une autre instance de Compte.
    //! \brief Constructeur de recopie de Compte.
    Compte(const Compte& cp)=default;
    //! Permet d'assigner aux attributs d'un Compte les attributs d'un autre Compte défini en paramètres.\n
    //! \brief Operateur =
    Compte& operator=(const Compte& cp)=default;
    //!	Constructeur de Compte à partir des attributs nom, numéro, solde, type et CompteAbstrait parent.
    //! \brief Constructeur de Compte.
    Compte(const QString& no,unsigned int nu,double s,double solderapp,Type t,CompteAbstrait* p=nullptr): CompteAbstrait(no,nu,t,p),solde(s),derraproch(solderapp){}
public:
    //! Renvoie la valeur du solde de ce Compte.
    //! \brief Accesseur du solde.
    double getSolde() const { return solde;};
    //! Renvoie la valeur du solde rapproché (derraproch) de ce Compte.
    //! \brief Accesseur du solde rapproché.
    double getSoldeRap() const{return derraproch;}
    //! Retourne le solde sur la période définie en paramètres de ce Compte.
    //! \brief Renvoie le solde du compte sur la période donnée.
    double getSoldeSurPeriode(QDate date_d,QDate date_f) const;
    //! Retourne le solde avant la date définie en paramètres de ce Compte.
    //! \brief Renvoie le solde du compte avant la date donnée.
    double getSoldeAvantDate(QDate date) const;
    //! Retourne une QString contenant le solde sur la période et le nom du Compte.\n Cette méthode est utilisée pour l'affichage du resultat sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée.
    QString res(QDate date_d,QDate date_f) const;
    //! Retourne une QString contenant le solde sur la période et le nom du Compte.\n Cette méthode est utilisée pour l'affichage du resultat au format web.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée.
    QString resWeb(QDate date_d,QDate date_f) const;
    //! Retourne une QString contenant le solde avant la date donnée et le nom du Compte.\n Cette méthode est utilisée pour l'affichage du bilan sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde avant la date donnée.
    QString bilan(QDate date);
    //! Retourne une QString contenant le solde avant la date donnée et le nom du Compte.\n Cette méthode est utilisée pour l'affichage du bilan au format web.
    //! \brief Renvoie une QString avec le nom et le solde avant la date donnée.
    QString BilanWeb(QDate date);
    //! Effectue la sauvegarde d'un Compte dans un fichier texte afin de conserver les données.
    //! \brief Permet de sauvegarder un Compte.
    void Save(QTextStream* flux) const;
    //! Retourne ce Compte si il possède le nom en paramètre ou un pointeur nul dans le cas contraire.
    //! \brief Renvoie un Compte correspondant au nom fourni en paramètre.
    CompteAbstrait * getCompte(const QString &no){if(no==nom) return this; return nullptr;}
    //! Retourne ce Compte si il possède le numéro en paramètre ou un pointeur nul dans le cas contraire.
    //! \brief Renvoie un Compte correspondant au numéro fourni en paramètre.
    CompteAbstrait* getCompte(unsigned int nu){if(num==nu) return this; return nullptr;}
    //! \brief Destructeur par default.
    ~Compte()=default;
    //! Permet de créditer un compte avec la valeur donnée en paramètre.
    //! \brief Crédite un compte.
    void crediter(double m);
    //! Permet de débiter un compte avec la valeur donnée en paramètre.
    //! \brief Débite un compte.
    void debiter(double m);
    //! Renvoie true si le Compte a été impliqué dans une transaction.
    //! \brief Verifie la présence d'une transaction pour ce compte.
    bool PresenceTrans() const;
    //! Renvoie true.
    //! \brief Renseigne sur la virtualité ou non du compte.
    bool concret()const {return true;};
    //! Prépare le vecteur de transaction Dépense permettant de solder les comptes dépenses ainsi que le vecteur de transaction Résultat permettant de solder les comptes résultats au niveau d'un Compte.
    //! \brief Fait partie du processus de cloture du livre.
    void Cloture(vector<tuple<Compte*,double,double>>& dep, vector<tuple<Compte*,double,double>>& rec, double& d, double& r);
    //! Place le nom du Compte dans la QStringList.
    //! \brief Utilisé pour récupérer des noms de CompteAbstrait et les afficher dans l'interface.
    void liste_nom_type(QStringList& b,bool Concret);
};

/**
   \class CompteVirtuel
   \brief Classe de CompteVirtuel/ Design Pattern Composite

   Le compte est un compte abstrait possédant une liste de compte fils.\n
   Il est situé à l'intérieur d'autres CompteVirtuel.

**/

class CompteVirtuel:public CompteAbstrait {
    friend class CompteApplication;
    friend class FabriqueCompte;
    //! Constructeur de recopie du compte abstrait.\n Il prend en argument un autre CompteAbstrait.
    //! \brief Constructeur de recopie de la classe CompteVirtuel.
    CompteVirtuel(const CompteVirtuel& cp)=default;
    //! \brief Opérateur d'affectation de la classe CompteVirtuel.
    //!  Initialise les attributs de ce CompteVirtuel avec ceux du CompteVirtuel fourni en argument.
    CompteVirtuel& operator=(const CompteVirtuel& cp)=default;
    //! Constructeur de compte abstrait.\n Il prend 4 arguments le nom du compte, son numéro, son type et le compte virtuel parent.
    //! \brief Constructeur à 4 arguments CompteVirtuel.
    CompteVirtuel(const QString& no,unsigned int nu,Type t,CompteAbstrait* p=nullptr):CompteAbstrait(no,nu,t,p){}
protected:
    //! \brief Liste des Compte fils de ce CompteAbstrait
    std::vector<CompteAbstrait*> listCompte;
public:
    //! \brief Destructeur virtuel de la classe CompteVirtuel.
    ~CompteVirtuel();
    //! \brief Sauvegarde du compte dans le fichier texte.
    //! Permet de sauvegarder les informations relatives à ce compte dans un document texte qui sera chargé lors des sessions suivantes.
    void Save(QTextStream* flux) const;
    //! \brief Renvoie un pointeur sur un CompteAbstrait du nom donné en argument.
    //! Renvoie un pointeur sur ce CompteVirtuel s'il possède le nom donné en argument, sur un des Compte fils s'il remplit cette condition ou un pointeur nul sinon.
    CompteAbstrait * getCompte(const QString &no);
    //! \brief Renvoie un pointeur sur un CompteAbstrait du numéro donné en argument.
    //!  Renvoie un pointeur sur ce CompteVirtuel s'il possède le numéro donné en argument, sur un des Compte fils s'il remplit cette condition ou un pointeur nul sinon.
    CompteAbstrait* getCompte(unsigned int nu);
    //! \brief Calcul la somme des soldes des Compte fils.
    //! Renvoie la somme des soldes des Compte fils de ce CompteVirtuel.
    double getSolde() const;
    //! \brief Renvoie la somme des soldes des Compte fils avant la date donnée.
    //! Retourne la somme des soldes des Compte fils avant la date définie en paramètres de ce Compte.
    double getSoldeAvantDate(QDate) const;
    //! \brief Renvoie la somme des soldes des Compte fils sur la période donnée.
    //! Retourne la somme des soldes des Compte fils sur la période définie en paramètres de ce Compte.
    double getSoldeSurPeriode(QDate,QDate) const;
    //! Retourne une QString contenant le solde sur la période et le nom du CompteVirtuel, ainsi que le solde sur la période de ses Compte fils.\n  Cette méthode est utilisée pour l'affichage du resultat sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée de ce CompteVirtuel et de ses fils.
    QString res(QDate date_d,QDate date_f) const;
    //! Retourne une QString contenant le solde sur la période et le nom du CompteVirtuel, ainsi que le solde sur la période de ses Compte fils.\n  Cette méthode est utilisée pour l'affichage du resultat au format web.
    //! \brief Renvoie une QString avec le nom et le solde sur la periode donnée de ce CompteVirtuel et de ses fils.
    QString resWeb(QDate date_d,QDate date_f) const;
    //! Retourne une QString contenant le solde avant la date définie en paramètre et le nom du CompteVirtuel, ainsi que le solde sur la période de ses Compte fils.\n  Cette méthode est utilisée pour l'affichage du bilan sur l'interface.
    //! \brief Renvoie une QString avec le nom et le solde savant la date définie de ce CompteVirtuel et de ses fils.
    QString bilan(QDate date);
    //! Retourne une QString contenant le solde avant la date définie en paramètre et le nom du CompteVirtuel, ainsi que le solde sur la période de ses Compte fils.\n  Cette méthode est utilisée pour l'affichage du bilan au format web.
    //! \brief Renvoie une QString avec le nom et le solde savant la date définie de ce CompteVirtuel et de ses fils.
    QString BilanWeb(QDate date);
    //! Place le nom des comptes fils direct de ce CompteVirtuel dans la QStringList.\nLes comptes choisis dépendent du type de compte (A,P,R,D) ainsi que du fait qu'il soit concret ou non.
    //! \brief Permet de renvoyer une QStringList contenant le nom des comptes fils direct de ce CompteVirtuel
    QStringList* getnomlistdirect(bool type=false,Type t=Type::A)const;
    //! \brief Ajout d'un CompteVirtuel fils.
    //! Ajoute un CompteVirtuel fils d'adresse fournie en paramètres à la liste des comptes fils de ce CompteVirtuel.
    CompteVirtuel& Add(CompteVirtuel* C);
    //! \brief Ajout d'un Compte fils.
    //! Ajoute un Compte fils d'adresse fournie en paramètres à la liste des comptes fils de ce CompteVirtuel.
    void Add(Compte* cp);
    //! Renvoie true si au moins un Compte de ce CompteVirtuel a été impliqué dans une transaction.
    //! \brief Verifie la présence de transactions pour ce compte.
    bool PresenceTrans() const;
    //! Renvoie false.
    //! \brief Renseigne sur la virtualité ou non du compte.
    bool concret() const {return false;};
    //! Prépare le vecteur de transaction Dépense permettant de solder les comptes dépenses ainsi que le vecteur de transaction Résultat permettant de solder les comptes résultats en parcourant ses fils.
    //! \brief Fait partie du processus de cloture du livre.
    void Cloture(vector<tuple<Compte*,double,double>>& dep, vector<tuple<Compte*,double,double>>& rec, double& d, double& r);
    //! Place le nom des comptes fils de ce CompteVirtuel dans la QStringList.\nLes comptes choisis dépendent du type de compte (A,P,R,D) ainsi que du fait qu'il soit concret ou non.
    //! \brief Utilisé pour récupérer des noms de CompteAbstrait et les afficher dans l'interface.
    void liste_nom_type(QStringList& b,bool Concret);
};

/**
   \class FabriqueCompte
   \brief Design Pattern Factory

   Permet la création de CompteVirtuel/ Compte selon la méthode factory.

**/

class TresorerieException;
class FabriqueCompte{
    friend class CompteApplication;
public:
     //! \brief Destructeur virtuel de la classe FabriqueCompte
    ~FabriqueCompte()=default;
    //! \brief Crée un CompteVirtuel/ Compte avec les arguments (nom,numero,Type,solde,père) passées à cette fonction.\n Le char id permet de savoir si le compte a crée est un compte Virtuel 'V'/Concret 'C'.
    //! \brief Permet la création de CompteVirtuel/ Compte
    virtual CompteAbstrait* Fabrication(char id,const QString& no, int nu, Type t, double s=0,double solde_rap=0,CompteAbstrait* p=nullptr){
        switch(id){
           case 'V': return new CompteVirtuel(no,nu,t,p);break;
           case 'C': return new Compte(no,nu,s,solde_rap,t,p);break;
        }
    }
};

class CompteApplication: public CompteVirtuel{
    //! \brief Numero du dernier CompteAbstrait crée.
    //! S'incrémente à la création d'un CompteAbstrait.
    unsigned int nb=1;
    //! \brief Nom du fichier de chargement.
    //! Reference du fichier de chargement de base des comptes.
    QString filename = "tmpBaseComptes.txt";
    //! \brief Constructeur de CompteApplication.
    //! Constructeur de la classe CompteApplication.\n Il prend en argument un numéro, un nom, un type, et un compte parent initialement nul.
    CompteApplication(const QString& no,unsigned int nu, Type t,CompteAbstrait* p=nullptr):CompteVirtuel(no,nu,t,p){};
    //! \brief Destructeur de CompteApplication.
    //! Destructeur de la classe CompteApplication.\n Il appelle la méthode save() pour sauvegarder l'état du fichier de compte avant de quitter.
    ~CompteApplication(){save();};
    //! \brief Constructeur de recopie de la classe CompteApplication (effacé).
    CompteApplication(CompteApplication& m)=delete;
    //! \brief Opérateur d'affectation de la classe CompteApplication (effacé).
    CompteApplication& operator=(CompteApplication& m)=delete;
    /**
       \struct Handler
       \brief Design Pattern Singleton

       Implémentation du design Pattern Singleton avec possibilité de supprimer/recréer l'instance.

    **/
    struct Handler{
     //! \brief Instance de CompteApplication.
     CompteApplication* instance;
     //! Constructeur de Handler initialisant l'instance de CompteApplication à nullptr.
     //! \brief Constructeur de Handler.
     Handler():instance(nullptr){}
     //! Appelle l'instance de CompteApplication.
     //! \brief Destructeur de Handler.
     ~Handler(){delete instance;}
    };
    //! \brief Singleton de CompteApplication
    static Handler handler;
public:
    //! Retourne l'instance courante de CompteApplication.\n Si celle-ci n'existe pas, la fonction en crée une et la retourne.
    //! \brief Création de CompteApplication.
    static CompteApplication& getCompteApplication();
    //! Supprime l'instance courante de CompteApplication.
    //! \brief Suppression de CompteApplication.
    static void libererCompteApplication();
    //! \brief Accesseur de l'attribut nb.
    //! Retourne un entier de la valeur de l'attribut nb.
    unsigned int getnb() const {return nb;};
    //! \brief Modificateur de l'attribut filename.
    //! Permet de modifier la valeur de l'attribut filename.
    void setFilename(const QString& f="tmpBaseComptes.txt") { filename = f; }
    //! \brief Accesseur de l'attribut filename.
    //! Retourne un QString de la valeur de l'attribut filename.
    QString getFilename(){ return filename;}
    //! \brief Charge un fichier contenant les comptes d'une session précédente.
    //! Charge un fichier texte contenant les comptes d'une session precedentes ainsi que tous leurs attributs.
    void load(const QString& f);
    //! \brief Sauvegarde l'état des comptes de la session actuelle.
    //! Sauvegarde dans un fichier texte les comptes et informations de la session actuelle.
    void save() const;
    //! \brief Crée un compte.
    //! Crée un compte avec les informations fournies en paramètres (nom, numéro, type, solde) en appelant une instance de FabriqueCompte.\n Le compte créé aura l'attribut CompteAbstrait* ca pour parent.\n Char c détermine si le compte est Virtuel/Concret.
    void creerCompte(FabriqueCompte* factory,CompteAbstrait* ca,char c,const QString& nom,Type t,double solde_init=0,double solde_rapp=0);
    //! \brief Supprime un compte.
    //! Supprime un compte de nom fourni en paramètres.
    void supp(const QString& nom);
    //! \brief Modifie le nom d'un compte.
    //! Change le nom du compte de nom fourni en paramètre en le deuxième nom fourni en paramètre.
    void modifcompte(const QString& nom1, const QString& nom2);
    //! \brief Effectue un rapprochement bancaire.
    //! Effectue le rapprochement bancaire du Compte de nom fourni en argument.
    void RapprochBanc(Compte* c);
    //! Créer le compte Résultat s'il n'existe pas puis les transactions Dépenses/Résultats qui crédite/débite le compte Résultat.\n Créer les Compte Déficit ou Excédent s'il n'existe pas, puis en fonction du solde de Résultat, il crédite Deficit ou Excedent tout en soldant Résultat.
    //! \brief Cloture le livre.
    void ClotureLivre();
    //! \brief Genere le bilan sur l'interface.
    //! Genere un QString du bilan qui est ensuite affiché sur l'interface.
    QString genererBilan(QDate date);
    //! \brief Genere le bilan au format web.
    //! Genere un fichier "bilan.html" du bilan qui contient le bilan au format web.
    void genererBilanWeb(QDate date);
    //! \brief Genere le compte de resultat sur l'interface.
    //! Genere un QString du compte de resultat qui est ensuite affiché sur l'interface.
    QString genererResultat(QDate date_d,QDate date_f);
    //! \brief Genere le compte de resultat au format web.
    //! Genere un fichier "compte_resultat.html" du bilan qui contient le compte de resultat au format web.
    void genererResultatWeb(QDate date_d,QDate date_f);
    //! Place le nom de tous les comptes fils de CompteApplication dans la QStringList.\nLes comptes choisis dépendent du type de compte (A,P,R,D) ainsi que du fait qu'il soit concret ou non.
    //! \brief Utilisé pour récupérer les noms de CompteAbstrait positionné sous la racine et les afficher dans l'interface.
    void liste_type(QStringList& b,bool Concret,bool Type,enum::Type a=Type::A);
    //! Deplace un CompteAbstrait.
    //! Deplace un CompteAbstrait d'un CompteAbstrait parent de départ vers un CompteAbstrait d'arrivée.
    void deplace(CompteAbstrait* dep,CompteAbstrait* arr);
};


#endif // TRESORERIE_H
