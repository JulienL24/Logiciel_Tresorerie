#include "compte.h"
#include<fstream>
#include<algorithm>
#include<QFile>
#include<QFileInfo>
#include <QDebug>
#include <cstdio>
#include <QString>

unsigned int nbite=0; // variable globale qui compte le nombre de tabulation à effectuer et permet l'affichage de la hierarchie des comptes dans certaines fonctions

CompteApplication::Handler CompteApplication::handler=CompteApplication::Handler();
TransactionApplication::Handler TransactionApplication::handler=TransactionApplication::Handler();

CompteAbstrait::~CompteAbstrait(){}

void Compte::crediter(double m) {
        if (m < 0) throw TresorerieException("montant a crediter negatif");
        solde += m;
}
void Compte::debiter(double m) {
        if (m < 0) throw TresorerieException("montant a debiter negatif");
        solde -= m;
}
QString Compte::res(QDate date_d,QDate date_f) const{
    QString flux;
    for(unsigned int i=0;i<nbite;i++)
        flux+="\t";
    flux+=this->nom;
    flux+="   ";
    flux+=QString::number(this->getSoldeSurPeriode(date_d,date_f));
    flux+="\n";
    return flux;

}
QString Compte::resWeb(QDate date_d,QDate date_f) const {
    QString flux;
    for(unsigned int i=0;i<nbite;i++)
        flux+="\t";
    flux+=getNom();
    flux+="\t";
    flux+="<b>";
    flux+=QString::number(getSoldeSurPeriode(date_d,date_f));
    flux+="</b>";
    return flux;
}

double Compte::getSoldeAvantDate(QDate date) const{
    double sad=getSolde();
    auto fit= TransactionApplication::getTransactionApplication().getFilterIterator((*this),true); // on parcours toutes les transactions associées à ce compte
    for(;!fit.isDone();fit.next()){
        if(fit.currentItem().getDate()>date){ // on s'occupe seulement des transactions après la date donnée en paramètre
            if(getType()==Type::A||getType()==Type::D){
                auto listComptes=fit.currentItem().getListCompte();
                for(unsigned int i=0;i<listComptes.size();i++){
                    if(std::get<0>(listComptes[i]) == this){
                        sad-=std::get<1>(listComptes[i]); // on soustrait le débit au solde avant date pour inverser la transaction
                        sad+=std::get<2>(listComptes[i]); // on additionne le crédit au solde avant date pour inverser la transaction
                    }
                }
            }
            else if(getType()==Type::R||getType()==Type::P){
                auto listComptes=fit.currentItem().getListCompte();
                for(unsigned int i=0;i<listComptes.size();i++){
                    if(std::get<0>(listComptes[i]) == this){
                        sad+=std::get<1>(listComptes[i]);// on additionne le débit au solde avant date pour inverser la transaction
                        sad-=std::get<2>(listComptes[i]); // on soustrait le crédit au solde avant date pour inverser la transaction
                    }
                }
            }
        }
    }

    return sad;
}

QString Compte::BilanWeb(QDate date){
    QString flux;
    for(unsigned int i=0;i<nbite;i++)
        flux+="\t";
    flux+=getNom();
    flux+="\t";
    flux+="<b>";
    flux+=QString::number(getSoldeAvantDate(date));
    flux+="</b>";
    return flux;
}
QString Compte::bilan(QDate date){
    QString flux;
    for(unsigned int i=0;i<nbite;i++)
        flux+="\t";
    flux+=this->nom;
    flux+="   ";
    flux+=QString::number(this->getSoldeAvantDate(date));
    flux+="\n";
    return flux;
}
double Compte::getSoldeSurPeriode(QDate date_d, QDate date_f) const{
    double ssp=getSoldeAvantDate(date_f)-getSoldeAvantDate(date_d); // on calcule le solde sur la période donnée en paramètre
    return ssp;
}

void Compte::Save(QTextStream* flux) const{
    CompteAbstrait::Save(flux);
    *flux<<solde<<";"<<derraproch<<";";
}

bool Compte::PresenceTrans() const{
    TransactionApplication::FilterIterator it=TransactionApplication::getTransactionApplication().getFilterIterator(*this,true);
    if(it.isDone()==true) // si l'itérateur pointe sur la fin du vecteur de transaction, il n'y a pas de transaction pour ce compte
        return false;
    return true;
}

void Compte::Cloture(vector<tuple<Compte *, double, double> > &dep, vector<tuple<Compte *, double, double> > &rec, double &d, double &r){
    if(getType()==Type::R){
        double tmpSolde=solde;
        std::tuple<Compte*,double,double> t1(this,tmpSolde,0);
        r+=tmpSolde;
        rec.push_back(t1); // on rajoute dans le vecteur Recette un tuple comportant le compte recette traité ainsi qu'un débit qui est égale au solde du compte, afin de mettre le compte à 0
     }
    else
        if(getType()==Type::D){
            double tmpSolde=solde;
            std::tuple<Compte*,double,double> t1(this,0,tmpSolde);
            d+=tmpSolde;
            dep.push_back(t1); // on rajoute dans le vecteur Depense un tuple comportant le compte dépense traité ainsi qu'un crédit qui est égale au solde du compte, afin de mettre le compte à 0
        }
}


void Compte::liste_nom_type(QStringList& b,bool Concret){
    b.push_back(this->getNom());
}

void CompteVirtuel::liste_nom_type(QStringList& b,bool Concret)
{   vector<CompteAbstrait*>::const_iterator itb= listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite= listCompte.end();
    if(Concret==true)
        b.push_back(this->getNom());
    for(;itb!=ite;itb++)
        (*itb)->liste_nom_type(b,Concret);
}

void CompteApplication::liste_type(QStringList& b,bool Concret,bool Type, enum::Type a)
{   vector<CompteAbstrait*>::const_iterator itb= listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite= listCompte.end();
    for(;itb!=ite;itb++)
        if(Type==false)
            (*itb)->liste_nom_type(b,Concret);
        else
                if((*itb)->getType()==a)
                    (*itb)->liste_nom_type(b,Concret);
}


void CompteVirtuel::Save(QTextStream* flux) const{
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    CompteAbstrait::Save(flux);
    for(;itb!=ite;itb++){
        *flux<<"\n";
        (*itb)->Save(flux); // on lance la fonction Save() sur les comptes fils, qui est polymorphique, cette fonction est récursive
    }
}

CompteAbstrait* CompteVirtuel::getCompte(const QString &no){
    if(nom==no) // le nom d'un compte étant unique, si on trouve le compte associé au nom on le renvoie
        return this;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
    {
        if((*itb)->getCompte(no)!=nullptr){ // on lance getCompte sur les comptes fils, qui est polymorphique et basé sur la récursivité
            return(*itb)->getCompte(no);
        }
    }
    return nullptr;
}

CompteAbstrait* CompteVirtuel::getCompte(unsigned int nu){
    if(num==nu) // le numéro d'un compte étant unique, si on trouve le compte associé au numéro on le renvoie
        return this;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
    {
        if((*itb)->getCompte(nu)!=nullptr){ // on lance getCompte sur les comptes fils, qui est polymorphique et basé sur la récursivité
            return(*itb)->getCompte(nu);
        }
    }
    return nullptr;
}

void CompteVirtuel::Cloture(vector<tuple<Compte*,double,double>>& dep,vector<tuple<Compte*,double,double>>& rec,double& d, double& r){
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::D || (*itb)->getType()==Type::R)
            (*itb)->Cloture(dep,rec,d,r); // on lance la cloture de compte sur les comptes Recettes et Dépenses fils, cette fonction est polymorphique. Les Compte remplieront les vecteurs Recette et Depense qui solderont les Compte recettes et dépenses.
    }
}


QString converType(Type t){
    if(t==Type::A)
        return "A";
    else
        if(t==Type::D)
            return "D";
    else
        if(t==Type::M)
            return "M";
    else
        if(t==Type::P)
            return "P";
    else
            return "R";
}

Type converChar(QString c){
    if(c=='A')
        return Type::A;
    else
        if(c=='P')
            return Type::P;
    else
        if(c=='R')
            return Type::R;
    else
        if(c=='D')
            return Type::D;
    else
            return Type::M;
}

double CompteVirtuel::getSolde() const{
    double somsolde=0;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++){
        somsolde+=(*itb)->getSolde(); // on lance la fonction getSolde sur les comptes fils, cette fonction est polymorphique.
    }
    return somsolde; // retourne le solde du CompteVirtuel, c'est-à-dire la somme des soldes des comptes fils.
}

QString CompteVirtuel::BilanWeb(QDate date){
    QString flux;
    for(unsigned int j=0;j<nbite;j++)
        flux+="\t";
    flux+="<i>";
    flux+=getNom();
    flux+="</i>";
    flux+="(Virtuel)";
    flux+="\t";
    flux+="<b>";
    flux+=QString::number(getSoldeAvantDate(date));
    flux+="</b>";
    flux+="<ul>";
    nbite++;
    for(unsigned int i=0;i<listCompte.size();i++){
        flux+="<li>";
        flux+=listCompte[i]->BilanWeb(date);
        flux+="</li>";
    }
    nbite--;
    flux+="</ul>";
    return flux;
}
QString CompteVirtuel::bilan(QDate date){
    QString flux;
    for(unsigned int j=0;j<nbite;j++)
        flux+="\t";
    flux+=getNom();
    flux+="(Virtuel)";
    flux+="   ";
    flux+=QString::number(getSoldeAvantDate(date));
    flux+="\n";
    nbite++;
    for(unsigned int i=0;i<listCompte.size();i++){
        flux+=listCompte[i]->bilan(date);
    }
    nbite--;
    return flux;
}
QString CompteVirtuel::res(QDate date_d,QDate date_f) const{
    QString flux;
    for(unsigned int j=0;j<nbite;j++)
        flux+="\t";
    flux+=getNom();
    flux+="(Virtuel)";
    flux+="   ";
    flux+=QString::number(getSoldeSurPeriode(date_d,date_f));
    flux+="\n";
    nbite++;
    for(unsigned int i=0;i<listCompte.size();i++){
        flux+=listCompte[i]->res(date_d,date_f);
    }
    nbite--;
    return flux;
}
QString CompteVirtuel::resWeb(QDate date_d,QDate date_f) const {
    QString flux;
    for(unsigned int j=0;j<nbite;j++)
        flux+="\t";
    flux+="<i>";
    flux+=getNom();
    flux+="</i>";
    flux+="(Virtuel)";
    flux+="\t";
    flux+="<b>";
    flux+=QString::number(getSoldeSurPeriode(date_d,date_f));
    flux+="</b>";
    flux+="<ul>";
    nbite++;
    for(unsigned int i=0;i<listCompte.size();i++){
        flux+="<li>";
        flux+=listCompte[i]->resWeb(date_d,date_f);
        flux+="</li>";
    }
    nbite--;
    flux+="</ul>";
    return flux;
}
double CompteVirtuel::getSoldeAvantDate(QDate date)const {
    double somme_sad=0;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++){
        somme_sad+=(*itb)->getSoldeAvantDate(date); // on lance la fonction polymorphique et récursive sur les comptes fils
    }
    return somme_sad;
}
double CompteVirtuel::getSoldeSurPeriode(QDate d, QDate f) const{
    double somme_ssp=0;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++){
        somme_ssp+=(*itb)->getSoldeSurPeriode(d,f); // on lance la fonction polymorphique et récursive sur les comptes fils
    }
    return somme_ssp;
}

bool CompteVirtuel::PresenceTrans() const{
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
        if((*itb)->PresenceTrans()==true) // si les comptes fils sont impliqués dans une transaction, on renvoie true
            return true;
    return false;
}

QStringList* CompteVirtuel::getnomlistdirect(bool type,Type t)const {
    QStringList* a = new QStringList;
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
    {
       if(type==false)
            a->push_back((*itb)->getNom());
       else
           if((*itb)->getType()==t)
                a->push_back((*itb)->getNom());
    }
    return a;
}

CompteVirtuel::~CompteVirtuel()
{
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++) // on parcourt tous les comptes fils et on les supprime, fonction récursive
    {
        delete (*itb);
    }
}

CompteVirtuel& CompteVirtuel::Add(CompteVirtuel *C){
    listCompte.push_back(C);
    return *C;
}

void CompteVirtuel::Add(Compte *cp){
    listCompte.push_back(cp);
}

CompteApplication& CompteApplication::getCompteApplication()
{
    if(handler.instance==nullptr) handler.instance=new CompteApplication("Compte_Racine",0,Type::M);
    return *handler.instance;
}

void CompteApplication::libererCompteApplication(){
    delete handler.instance;
    handler.instance=nullptr;
}

void CompteApplication::creerCompte(FabriqueCompte *factory, CompteAbstrait* ca,char c, const QString &nom,Type t,double solde_init,double solde_rapp){
    if(getCompte(nom)==nullptr){// si le aucun compte possède le même nom, on peut continuer la création
        if(ca->concret()==false){ //ca est le compte père qui contiendra le compte que l'on va créer, ca doit donc être virtuel
            if(ca->getType()==Type::M || ca->getType()==t){// soit le père est le compte manager, soit le père a le même type que le compte que l'on va crée
                CompteAbstrait* cp=factory->Fabrication(c,nom,nb++,t,solde_init,solde_rapp,ca);
                if(cp->concret()==false)
                    dynamic_cast<CompteVirtuel*>(ca)->Add(dynamic_cast<CompteVirtuel*>(cp));
                else{dynamic_cast<CompteVirtuel*>(ca)->Add(dynamic_cast<Compte*>(cp));}
            }
            else
                throw TresorerieException(" On ajoute un compte qui n'a pas le même type que son père");
        }
        else
            throw TresorerieException("Le père n'est pas un compte virtuel \n");
      }
    else
        throw TresorerieException("Un compte possede déjà le même nom \""+nom+"\"");
}

void CompteApplication::deplace(CompteAbstrait *dep, CompteAbstrait *arr)
{   CompteVirtuel* v;
    if((v=dynamic_cast<CompteVirtuel*>(arr))!=nullptr) // ici le compte d'arrivée doit être un CompteVirtuel, pouvant contenir des autres comptes
    {
        if(dep->concret()==true) // si le compte que l'on veut déplacer est non virtuel
            v->Add(dynamic_cast<Compte*>(dep));
        else
            v->Add(dynamic_cast<CompteVirtuel*>(dep)); // si il est virtuel
        CompteVirtuel* pere=dynamic_cast<CompteVirtuel*>(dep->getPere());
        auto itb=pere->listCompte.begin();
        auto ite=pere->listCompte.end();
        auto it=find(itb,ite,dep);
        pere->listCompte.erase(it); // on supprime le compte que l'on vient de déplacer, de l'ancien compte père
        dep->setPere(arr);
    }
    else
        throw TresorerieException("Erreur, on ne peut rien ajouter dans un compte concret \n");
}

void CompteApplication::supp(const QString& nom){
    CompteAbstrait* fils=getCompte(nom); // on recupère le compte que l'on veut supprimer
    if(fils!=nullptr && fils->getType()!=Type::M){ // on ne peut supprimer le compte Manager
        if(fils->concret()==false){ // traitement si le compte est virtuel, on ne peut le supprimer que si il ne contient aucun compte fils
            if((dynamic_cast<CompteVirtuel*>(fils)->listCompte).empty()==true){// si il ne contient aucun compte fils
                CompteVirtuel* cp=dynamic_cast<CompteVirtuel *>(fils->getPere());
                vector<CompteAbstrait*>::const_iterator itb=cp->listCompte.begin();
                const vector<CompteAbstrait*>::const_iterator ite=cp->listCompte.end();
                for(;itb!=ite;itb++)
                    if((*itb)==fils)
                        break;
                cp->listCompte.erase(itb); // on enlève le compte que l'on vient de supprimer de son compte père
                fils->~CompteAbstrait();
               }
            else
                throw TresorerieException(" Ce compte possède des sous comptes \n");
        }
        else{// si le compte est concret, dans ce cas on ne peut le supprimer que si il n'a participé à aucune transaction
            if(fils->PresenceTrans()==false){// si il n'a participé à aucune transaction
                CompteVirtuel* cp=dynamic_cast<CompteVirtuel *>(fils->getPere());
                vector<CompteAbstrait*>::const_iterator itb=cp->listCompte.begin();
                const vector<CompteAbstrait*>::const_iterator ite=cp->listCompte.end();
                for(;itb!=ite;itb++)
                    if((*itb)==fils)
                        break;
                cp->listCompte.erase(itb); // on enlève le compte que l'on vient de supprimer de son compte père
                fils->~CompteAbstrait();
             }
            else
                throw TresorerieException("Ce compte a participé à des transactions \n");
        }
    }
    else
        throw TresorerieException("Le compte n'existe pas ou c'est le compte racine \n");
}

void CompteApplication::modifcompte(const QString &nom1,const QString &nom2){
    CompteAbstrait* modif;
    if((modif=getCompte(nom1))!=nullptr)//si le compte que l'on veut supprimer existe
        if(getCompte(nom2)==nullptr)// si le nouveau nom est disponible (nom unique)
            modif->setNom(nom2);
        else
            throw TresorerieException("Le nouveau nom existe déjà pour un autre compte \n");
    else
        throw TresorerieException("Le compte à modifier n'existe pas \n");
}

void CompteApplication::save() const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier compte");
    QTextStream flux(&file); // on ouvre un flux afin d'écrire directement dans un fichier texte
    Save(&flux);
    file.close();
}


void CompteApplication::load(const QString &f){
    FabriqueCompte factory;
    QFile file(f);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
            throw TresorerieException("Erreur chargement fichier compte");
    QTextStream flux(&file); // on ouvre un flux afin d'écrire directement dans un fichier texte
    unsigned int max=0;
    getCompteApplication();
    while(!flux.atEnd()){
        QString ligne = flux.readLine();
        QStringList infos = ligne.split(";",QString::SkipEmptyParts);
        if(infos.length()>6) // si le fichier contient trop d'informations, on sait que ce n'est pas un fichier de compte
            throw TresorerieException("Le fichier "+f+" n'est pas un fichier de compte, impossible de le charger");
        if(infos[1]!='M'){
            nb=infos[2].toInt();
            if(nb>max)
                max=nb; // ici on veut connaitre le plus grand numéro d'un compte existant dans le fichier, afin de recommencer, après le chargement du fichier, a donner des numéros unique au compte à partir du dernier compte crée et existant qui aura le numéro le plus grand
            if(infos.length()==6){ // si le compte a un solde on crée un compte concret
                getCompteApplication().creerCompte(&factory,getCompteApplication().getCompte(infos[3].toInt()),'C',infos[0],converChar(infos[1]),infos[4].toDouble(),infos[5].toDouble());
                }
            else{ // on crée un compte virtuel
                getCompteApplication().creerCompte(&factory,getCompteApplication().getCompte(infos[3].toInt()),'V',infos[0],converChar(infos[1]));
                }
            }
        }
    if(max!=0)
        nb=max+1; // ici nb prend max+1 pour que le prochain compte crée est un numéro unique
    filename=f;
    file.close();
}


QString CompteApplication::genererBilan(QDate date){
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    nbite=0;
    QString bilan="";
    double soldeA=0,soldeP=0;
    for(;itb!=ite;itb++){
        if((*itb)->getType()==Type::A){
            bilan+=(*itb)->bilan(date);
            soldeA+=(*itb)->getSoldeAvantDate(date);
        }
    }
    bilan+="-----------------------------------\n";
    bilan+="Total Actif\t";
    bilan+=QString::number(soldeA);
    bilan+="\n";
    bilan+="-----------------------------------\n";
    bilan+="\n";
    itb=listCompte.begin();
    nbite=0;
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::P){
            bilan+=(*itb)->bilan(date);
            soldeP+=(*itb)->getSoldeAvantDate(date);
        }
    }
    bilan+="-----------------------------------\n";
    bilan+="Total Passif\t";
    bilan+=QString::number(soldeP);
    bilan+="\n";
    bilan+="-----------------------------------\n";
    nbite=0;
    double bil=soldeA-soldeP;
    bilan+="-----------------------------------\n";
    bilan+="Bilan : ";
    bilan+=QString::number(bil);
    bilan+="\n Un fichier \"bilan.html\" a également été généré si vous souhaitez exporter ce bilan au format web.\n";
    return bilan;
}
QString CompteApplication::genererResultat(QDate date_d,QDate date_f){
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    QString resultat="";
    double soldeDep=0;double soldeRec=0;

    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::R){
            resultat+=(*itb)->res(date_d,date_f);
            soldeRec+=(*itb)->getSoldeSurPeriode(date_d,date_f);
        }
    }
    resultat+="-----------------------------------\n";
    resultat+="\nTotal Recettes\t";
    resultat+=QString::number(soldeRec);
    resultat+="\n";
    resultat+="-----------------------------------\n";
    itb=listCompte.begin();
    nbite=0;
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::D){
            resultat+=(*itb)->res(date_d,date_f);
            soldeDep+=(*itb)->getSoldeSurPeriode(date_d,date_f);
        }
    }
    nbite=0;
    resultat+="-----------------------------------\n";
    resultat+="\nTotal Depenses\t";
    resultat+=QString::number(soldeDep);
    resultat+="\n";
    resultat+="-----------------------------------\n";
    double Res=soldeRec-soldeDep;
    resultat+="-----------------------------------\n";
    if(Res>=0)
        resultat+="Benefice de\t";
    else
        resultat+="Deficit de\t";

    resultat+=QString::number(Res);
    resultat+="\n\n Un fichier \"compte_resultat.html\" a également été généré si vous souhaitez exporter ce compte de résultat au format web.\n";
    return resultat;
}
void CompteApplication:: genererBilanWeb(QDate date){
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    QFile file("bilan.html");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier compte");
    QTextStream flux(&file);

    ///Head du fichier html

    flux<<"<head>\n\t";
    flux<<"<title>Bilan</title>\n\t";
    flux<<"<meta charset=\"utf-8\">\n\t";
    flux<<"<style>td {border: 0.5px solid black;}</style>\n\t";
    flux<<"<style>ul {list-style: none;}</style>\n\t";
    flux<<"</head>"<<endl;


    ///Corps du fichier html

    //Titre de section
    flux<<"<body>\n<table><tr><td><h3>Actifs</h3></td><td><h3>Passif</h3></td></tr>\n";
    double soldeA=0;double soldeP=0;

    //Comptes Actifs
    flux<<"<tr><td><table>\n";
    nbite=0;
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::A){
            flux<<"<tr><p>"<<(*itb)->BilanWeb(date)<<"</p></tr>"<<endl;
            soldeA+=(*itb)->getSoldeAvantDate(date);
        }
    }
    flux<<"</table></td>\n";
    itb=listCompte.begin();
    nbite=0;

    //Comptes Passifs
    flux<<"<td><table>\n";
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::P){
            flux<<"<tr><p>"<<(*itb)->BilanWeb(date)<<"</p></tr>"<<endl;
            soldeP+=(*itb)->getSoldeAvantDate(date);
        }
    }
    flux<<"</table></td></tr>\n<tr><td><table>\n";



    //Totaux par catégorie
    flux<<"\t<tr>"<<"<td style=\"border: none;\">Total de l'actif</td><td style=\"border: none;\">"<<soldeA<<"</td>"<<"</tr>\n";
    flux<<"</table></td>\n<td><table>\n";
    flux<<"\t<tr>"<<"<td style=\"border: none;\">Total du passif</td><td style=\"border: none;\">"<<soldeP<<"</td>"<<"</tr>\n";
    flux<<"</table></td></tr>\n";
    flux<<"<tr>";
    flux<<"</tr>\n</table>";
    flux<<"</body>\n";
    nbite=0;
}
void CompteApplication::genererResultatWeb(QDate date_d,QDate date_f){
    vector<CompteAbstrait*>::const_iterator itb=listCompte.begin();
    const vector<CompteAbstrait*>::const_iterator ite=listCompte.end();
    QFile file("compte_resultat.html");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier compte");
    QTextStream flux(&file);

    ///Head du fichier html

    flux<<"<head>\n\t";
    flux<<"<title>Compte de resultat</title>\n\t";
    flux<<"<meta charset=\"utf-8\">\n\t";
    flux<<"<style>td {border: 0.5px solid black;}</style>\n\t";
    flux<<"<style>ul {list-style: none;}</style>\n\t";
    flux<<"</head>"<<endl;

    ///Corps du fichier html

    //Titre de section
    flux<<"<body>\n<table><tr><td><h3>Recettes</h3></td><td><h3>Dépenses</h3></td></tr>\n";
    double soldeR=0;double soldeD=0;

    //Comptes de recettes
    flux<<"<tr><td><table>\n";
    nbite=0;
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::R){
            flux<<"<tr><p>"<<(*itb)->resWeb(date_d,date_f)<<"</p></tr>"<<endl;
            soldeR+=(*itb)->getSoldeSurPeriode(date_d,date_f);
        }
    }
    flux<<"</table></td>\n";
    itb=listCompte.begin();
    nbite=0;

    //Comptes de dépenses
    flux<<"<td><table>\n";
    for(;itb!=ite;itb++)
    {
        if((*itb)->getType()==Type::D){
            flux<<"<tr><p>"<<(*itb)->resWeb(date_d,date_f)<<"</p></tr>"<<endl;
            soldeD+=(*itb)->getSoldeSurPeriode(date_d,date_f);
        }
    }
    flux<<"</table></td></tr>\n<tr><td><table>\n";

    //Totaux par catégorie
    flux<<"\t<tr>"<<"<td style=\"border: none;\">Total des recettes</td><td style=\"border: none;\">"<<soldeR<<"</td>"<<"</tr>\n";
    flux<<"</table></td>\n<td><table>\n";
    flux<<"\t<tr>"<<"<td style=\"border: none;\">Total des dépenses</td><td style=\"border: none;\">"<<soldeD<<"</td>"<<"</tr>\n";
    flux<<"</table></td></tr>\n";
    double Res=soldeR-soldeD;
    flux<<"<tr>";
    if(Res>=0)
        flux<<"<td>Benefice</td><td>"<<Res<<"</td>\n";
    else
        flux<<"<td>Deficit</td><td>"<<Res<<"</td>\n";
    flux<<"</tr>\n</table>";
    flux<<"</body>\n";
    nbite=0;


}


void CompteApplication::RapprochBanc(Compte* c){
    TransactionApplication::FilterIterator it=TransactionApplication::getTransactionApplication().getFilterIterator(*c,false); // on parcourt les transactions du compte c afin de rapprocher les transactions
    for(;!it.isDone();it.next())
        it.currentItem().setRapproch(); // on rapproche la transaction
    c->derraproch=c->solde; // le solde au dernier rapprochement devient égale au solde courant
}

void CompteApplication::ClotureLivre(){
    if (getCompte("Résultat")==nullptr){
        FabriqueCompte factory;
        creerCompte(&factory,this,'C',"Résultat",Type::P); // si le compte résultat n'existe pas on le crée
    }
    vector<tuple<Compte*,double,double>> dep;
    vector<tuple<Compte*,double,double>> rec;
    double d=0;double r=0;
    Cloture(dep,rec,d,r); // ici on remplit le vecteur depense et recette avec le solde des comptes recettes et dépenses afin de les solder
    if(!dep.empty()){// on ne crée la transaction relative au dépense que si il y a des comptes dépenses
        std::tuple<Compte*,double,double> t1(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte("Résultat")),d,0);
        dep.push_back(t1); // on rajoute au vecteur le tuple permettant de débiter à Résultat le solde des dépenses
        TransactionApplication::getTransactionApplication().creerTransaction(dep,QDate::currentDate(),"CLD"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),"ClotureD"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),TransactionApplication::getTransactionApplication().usenb());
    }
    if(!rec.empty()){ // on ne crée la transaction relative au recette que si il y a des comptes recettes
        std::tuple<Compte*,double,double> t2(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte("Résultat")),0,r);
        rec.push_back(t2); // on rajoute au vecteur le tuple permettant de créditer à Résultat le solde des recettes, puis on crée les deux transactions
        TransactionApplication::getTransactionApplication().creerTransaction(rec,QDate::currentDate(),"CLR"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),"ClotureR"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),TransactionApplication::getTransactionApplication().usenb());
    }
    double solde=getCompte("Résultat")->getSolde(); // on récupère le solde du compte résultat
    if (solde>=0){ // si il est positif ou nulle on crée un compte excédent
        if (getCompte("Excédent")==nullptr){
            FabriqueCompte factory;
            creerCompte(&factory,this,'C',"Excédent",Type::P);
        }
        std::vector<tuple<Compte*,double,double>> v;
        std::tuple<Compte*,double,double> t1(dynamic_cast<Compte*>(getCompte("Résultat")),solde,0);
        std::tuple<Compte*,double,double> t2(dynamic_cast<Compte*>(getCompte("Excédent")),0,solde);
        v.push_back(t1);
        v.push_back(t2);// on crée la transaction soldant Résultat et créditant excédent
        TransactionApplication::getTransactionApplication().creerTransaction(v,QDate::currentDate(),"CONS-"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),"Constatation"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),TransactionApplication::getTransactionApplication().usenb());
    }
    else if (solde<0){// sinon on crée un compte déficit
        if (getCompte("Déficit")==nullptr){
            FabriqueCompte factory;
            creerCompte(&factory,this,'C',"Déficit",Type::P);
        }
        std::vector<tuple<Compte*,double,double>> v;
        std::tuple<Compte*,double,double> t1(dynamic_cast<Compte*>(getCompte("Résultat")),solde,0);
        std::tuple<Compte*,double,double> t2(dynamic_cast<Compte*>(getCompte("Déficit")),0,solde);
        v.push_back(t1);
        v.push_back(t2); // on crée la transaction soldant Résultat et débitant Déficit (solde négatif)
        TransactionApplication::getTransactionApplication().creerTransaction(v,QDate::currentDate(),"CONS-"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),"Constatation"+QString::number(QDate::currentDate().year())+"-"+QDateTime::currentDateTime().toString("ss"),TransactionApplication::getTransactionApplication().usenb());
    }
}



