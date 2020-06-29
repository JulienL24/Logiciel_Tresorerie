#include "transaction.h"
#include<fstream>
#include<algorithm>
#include<QFile>
#include<QFileInfo>
#include <QDebug>
#include <cstdio>
#include <QString>


bool Transaction::comptePresent(const Compte *c) const{
    vector<std::tuple<Compte*,double,double>>::const_iterator itb=listCompte.begin();
    const vector<std::tuple<Compte*,double,double>>::const_iterator ite=listCompte.end();
    for(;itb!=ite;itb++)
        if(std::get<0>((*itb))==c) // si le compte est présent dans la transaction on renvoie vrai
            return true;
    return false;
}

TransactionApplication& TransactionApplication::getTransactionApplication(){
    if(handler.instance==nullptr) handler.instance=new TransactionApplication;
    return *handler.instance;
}

void TransactionApplication::libererTransactionApplication(){
    delete handler.instance;
    handler.instance=nullptr;
}

void TransactionApplication::savetrans() const{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) throw TresorerieException("Erreur sauvegarde fichier transaction");
    QTextStream flux(&file); // on ouvre un flux sur le fichier afin d'écrire dedans directement
    vector<Transaction*>::const_iterator itb=listTransaction.begin();
    const vector<Transaction*>::const_iterator ite=listTransaction.end();
    for(;itb!=ite;itb++)
    {
            flux<<(*itb)->getid()<<";"<<(*itb)->getRef()<<";"<<(*itb)->getDate().day()<<"."<<(*itb)->getDate().month()<<"."<<(*itb)->getDate().year()<<";"<<(*itb)->getTitre()<<";"<<(*itb)->getRapproche()<<";";
            vector<std::tuple<Compte*,double,double>>::const_iterator itcb=(*itb)->listCompte.begin();
            vector<std::tuple<Compte*,double,double>>::const_iterator itce=(*itb)->listCompte.end();
            for(;itcb!=itce;itcb++)
                flux<<(std::get<0>((*itcb)))->getNumero()<<";"<<std::get<1>((*itcb))<<";"<<std::get<2>((*itcb))<<";";
            flux<<"\n";
    }
    file.close();
}

void TransactionApplication::load(const QString &f){
    QFile file(f);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
            throw TresorerieException("Erreur chargemement fichier transaction");
    QTextStream flux(&file);
    int max=-1;
    CompteApplication& ca=CompteApplication::getCompteApplication();
    CompteAbstrait* c;
    while(!flux.atEnd()){
        QString ligne = flux.readLine();
        QStringList infos = ligne.split(";",QString::SkipEmptyParts);
        if(infos.length()<=6 && infos.length()!=0) // ici le fichier n'est pas un fichier de transaction car il y a moins de 6 informations, donc on envoie un message d'erreur. Si il y a 0 information c'est seulement que le fichier est vide et donc aucun message d'erreur est envoyé.
            throw TresorerieException("Le fichier "+f+" n'est pas un fichier de transaction, impossible de le charger");
        QStringList date = infos[2].split(".",QString::SkipEmptyParts);
        nb=infos[0].toInt();
        if((int)nb>max)
            max=nb; // ici on cherche l'ID max parmis toutes les transactions
        vector<tuple<Compte*,double,double>> v;
        tuple<Compte*,double,double> t;
        for(int i=5;i<infos.length();i=i+3)
        {
            c=ca.getCompte(infos[i].toInt());
            if(c==nullptr) // étant donné qu'un compte ne peut être supprimé que si il n'a participé à aucune transaction, si une transaction est associé à un compte inexistant alors le fichier de transaction ne correspond pas au fichier de compte chargé précedemment
                throw TresorerieException("Le fichier de transaction que vous avez chargé ne correspond pas au fichier de compte");
            t=make_tuple(dynamic_cast<Compte*>(c),infos[i+1].toDouble(),infos[i+2].toDouble());
            v.push_back(t);
        }
        if(infos[4].toInt()==0) // si la transaction n'est pas rapproché
            creerTransaction(v,QDate(date[2].toInt(),date[1].toInt(),date[0].toInt()),infos[1],infos[3],nb,false);
        else // si la transaction est rapproché
            creerTransaction(v,QDate(date[2].toInt(),date[1].toInt(),date[0].toInt()),infos[1],infos[3],nb,false)->setRapproch();
    }
    nb=max+1; // les transactions ayant un ID unique qui est nb, on prend nb=max(max ID des transactions existantes) +1 afin d'avoir un ID unique pour chaque transaction, si on ne fait pas ça on aurait des doublons car nb recommencerait à 0
    filename=f;
    file.close();
}

Transaction* TransactionApplication::getTransaction(unsigned int id){
    auto itb=listTransaction.begin();
    auto ite=listTransaction.end();
    for(;itb!=ite;itb++){
        if((*itb)->getid()==id) // on considère combinaison (nom,ref) non unique pour les transactions
            return (*itb);
    }
    return nullptr;
}

bool TransactionApplication::verifTransac(vector<std::tuple<Compte*,double,double>>& liste){
    // on vérifie que la transaction contient un compte
    if (liste.empty())
        throw TresorerieException("Il n'y a aucun compte dans la transaction");
    // on verifie que tous les comptes associés à la transaction existent
    auto itb=liste.begin();
    auto ite=liste.end();
    for(;itb!=ite;itb++)
        if(CompteApplication::getCompteApplication().getCompte(std::get<0>(*itb)->getNumero())==nullptr)
            throw TresorerieException("Un des comptes n'existent pas");
    // ici si on a plusieurs fois le même compte impliqué dans la même transaction , on va factoriser les apparitions en une
    itb=liste.begin();
    vector <tuple<Compte*,double,double>> nouv;
    bool trouve;
    for(;itb!=ite;itb++)
    {   trouve=false;
        for(auto itnouv=nouv.begin();itnouv!=nouv.end();itnouv++)
            if(std::get<0>(*itnouv)==std::get<0>(*itb)){
                trouve=true;
                break;
            }
        if(trouve==false) // si c'est la première apparition du compte dans le vecteur de base, on le rajoute dans le nouveau vecteur, on aura dans nouv l'apparition de chaque compte une fois
            nouv.push_back(make_tuple(std::get<0>(*itb),0,0));
    }
    itb=liste.begin();
    for(;itb!=ite;itb++){ // ici on rajoute le débit et crédit de chaque compte qui apparait maintenant une fois
        auto itnouv=nouv.begin();
        for(;itnouv!=nouv.end() && std::get<0>(*itnouv)!=std::get<0>(*itb);itnouv++);
        std::get<1>(*itnouv)+=std::get<1>(*itb);
        std::get<2>(*itnouv)+=std::get<2>(*itb);
    }
    liste=nouv;
    // au moins deux comptes doivent prendre part à la transaction
    itb=liste.begin();
    itb++;
    for(;itb!=ite;itb++)
        if(std::get<0>((*itb))!=std::get<0>((*liste.begin())))
            break;
    if (itb==ite)
        throw TresorerieException("Seul un compte participe à la transaction");
    itb=liste.begin();
    // Traitement des montants négatifs/ une seule valeur positive entre débit et crédit/ Somme des crédits= somme des débits
    itb = liste.begin();
    ite=liste.end();
    double cred=0,deb=0;
    for(;itb!=ite; itb++){
        if(std::get<2>((*itb))<0 && std::get<1>((*itb))<0){ // si crédit et débit négatif
            double t;
            t=std::get<1>((*itb));
            std::get<1>((*itb))=-std::get<2>((*itb));
            std::get<2>((*itb))=-t; // le crédit devient un débit positif et le débit devient un crédit positif
        }
        else
            if(std::get<2>((*itb))<0){// si crédit negatif
                std::get<1>((*itb))=std::get<1>((*itb)) -std::get<2>((*itb));// on soustrait le crédit au débit mais le crédit est négatif donc ceci revient à une addition du crédit et débit
                std::get<2>((*itb))=0;// on met le crédit à 0
            }
        else
            if(std::get<1>((*itb))<0){// si débit négatif
                std::get<2>((*itb))=std::get<2>((*itb)) -std::get<1>((*itb));// on soustrait le débit au crédit mais le débit est négatif donc ceci revient à une addition du crédit et débit
                std::get<1>((*itb))=0;// on met le débit à 0
            }
        if((std::get<1>((*itb))!=0 && std::get<2>((*itb))!=0) && (std::get<2>((*itb))>std::get<1>((*itb)))){// si crédit et débit non null et crédit>débit
            std::get<2>((*itb))=std::get<2>((*itb)) -std::get<1>((*itb)); // on retranche le débit au crédit
            std::get<1>((*itb))=0; // on met à 0 le débit

        }
        else if((std::get<1>((*itb))!=0 && std::get<2>((*itb))!=0) && (std::get<2>((*itb))>std::get<1>((*itb)))){ // si crédit et débit non null et débit>crédit
            std::get<1>((*itb))=std::get<1>((*itb)) -std::get<2>((*itb));// on retranche le crédit au débit
            std::get<2>((*itb))=0;// on met le crédit à 0

         }
        cred+=std::get<2>((*itb)); // on prépare comparaison entre somme crédit et somme débit
        deb+=std::get<1>((*itb));
   }
   if(cred!=deb) // somme crédit!=somme débit
       throw TresorerieException("La somme des crédits n'est pas égale à la somme des débits");
   return true;
}

Transaction* TransactionApplication::creerTransaction(vector<std::tuple<Compte*,double,double>> liste,QDate date, const QString& ref, const QString& titre, unsigned int n,bool changeCompte){
    if(verifTransac(liste)==true){ // on vérifie la transaction
            Transaction* t;
            t=new Transaction(date,ref,titre,liste,n);

            if(listTransaction.empty())
                listTransaction.push_back(t);
            else{// cette boucle permet de trier les transactions par date
                auto it=listTransaction.begin();
                for (;it!=listTransaction.end();it++){
                    if ((*it)->getDate()>=t->getDate()){
                        listTransaction.insert(it,t);
                        break;
                    }
                }
                if (it==listTransaction.end()) // ici on le met à la fin car la nouvelle transaction à la date la plus grande
                    listTransaction.push_back(t);
            }

            //mis_jour solde des comptes:
            auto itb=liste.begin();
            auto ite=liste.end();
            if(changeCompte==true){
            for(;itb!=ite;itb++){
                if(std::get<0>(*itb)->getType() == Type::A || std::get<0>(*itb)->getType() == Type::D){// cas d'un compte actif ou dépense
                    double m=std::get<1>(*itb); // 1 er double  = debit
                    if (m !=0){ // si débit non nulle
                        std::get<0>(*itb)->crediter(m); // ici le débit correspond au fait de créditer un compte
                    }
                    else{
                        std::get<0>(*itb)->debiter(std::get<2>(*itb)); //ici le crédit correspond au fait de débiter un compte
                    }
                }
                else if(std::get<0>(*itb)->getType() == Type::P || std::get<0>(*itb)->getType() == Type::R){// cas d'un compte recette ou passif
                    double m = std::get<1>(*itb);// 1 er double  = debit
                    if (m !=0){// débit non nulle
                        std::get<0>(*itb)->debiter(m); // ici le débit correspond au fait de débiter un compte
                    }
                    else{
                        std::get<0>(*itb)->crediter(std::get<2>(*itb)); //ici le crédit correspond au fait de créditer un compte
                    }

                }
            }
            }
            return t;
    }
    return nullptr;
}

void TransactionApplication::suppTrans(Transaction* t){
    if(t!=nullptr && t->getRapproche()==false){ // si la transaction n'est pas rapprochée car sinon on ne peut pas la modifier ou supprimer
        //Reversion de l'action de la transaction
        auto itcompteb=t->listCompte.begin();
        auto itcomptee=t->listCompte.end();
        for(;itcompteb!=itcomptee;itcompteb++){
            if(std::get<0>(*itcompteb)->getType() == Type::A || std::get<0>(*itcompteb)->getType() == Type::D){
                double m=std::get<1>(*itcompteb); // 1 er double  = debit
                if (m !=0){
                    std::get<0>(*itcompteb)->debiter(m); // debit
                }
                else{
                    std::get<0>(*itcompteb)->crediter(std::get<2>(*itcompteb)); //credit
                    }
                }
            else if(std::get<0>(*itcompteb)->getType() == Type::P || std::get<0>(*itcompteb)->getType() == Type::R){
                double m = std::get<1>(*itcompteb);// 1 er double  = debit
                if (m !=0){
                    std::get<0>(*itcompteb)->crediter(m); // debit
                 }
                else{
                    std::get<0>(*itcompteb)->debiter(std::get<2>(*itcompteb)); //credit
                }

            }
       }
            auto itb=std::find(listTransaction.begin(),listTransaction.end(),t);
            getTransactionApplication().listTransaction.erase(itb); // on supprime la transaction du vecteur de transaction
            delete t;
        }
        else{
            throw TresorerieException("La transaction est deja rapprochee, impossible de la supprimer.\n");
        }
}

QStringList Transaction::nomlistecompte() const{
    QStringList a;
    auto itb = listCompte.begin();
    auto ite = listCompte.end();
    for(;itb!=ite;itb++){
        a.push_back(std::get<0>(*itb)->getNom()); // on ajoute les noms des comptes associées à la transaction dans une QStringList
    }
    return a;
}

Transaction* Transaction::dedouble()
{
    return new Transaction(*this);
}
