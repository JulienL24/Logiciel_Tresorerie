#include "fenetre_compte.h"
#include "fenetre_transaction.h"

/* fonction permettant l'affichage d'une fenetre MaFenetreAgTransR aggrandissant les transactions rapprochées*/
void AgrandirTransR(Transaction* t){
    MaFenetreAgTransR* fenetre=new MaFenetreAgTransR;
    fenetre->setWindowTitle("Transactions "+t->getTitre()+" "+t->getRef());
    fenetre->getLab()->setText("Transactions: "+t->getTitre()+"\tReference: "+t->getRef()+"\tDate: "+t->getDate().toString("dd/MM/yyyy")); // on met dans le label les infos de la transaction
    auto itb=t->getListCompte().begin();
    auto ite=t->getListCompte().end();
    for(;itb!=ite;itb++) // on affiche les informations par rapport aux comptes associés à la transaction
    {
     fenetre->getvue()->insertRow(fenetre->getvue()->rowCount());
     QTableWidgetItem *item = new QTableWidgetItem((std::get<0>(*itb))->getNom());
     QTableWidgetItem *item1 = new QTableWidgetItem(QString::number((std::get<0>(*itb))->getNumero()));
     QTableWidgetItem *item2=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
     QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
     QTableWidgetItem *item4=new QTableWidgetItem;
     if(t->getRapproche()==true) // on affiche si la transaction est rapprochée ou non, c'est un rappel pour l'utilisateur
         item4->setText("R");
     fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 0, item1);
     fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 1, item);
     fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 2, item2);
     fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 3, item3);
     fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 4, item4);
    }
    fenetre->setModal(true);
    fenetre->exec();
}

/* fonction permettant l'affichage d'une fenetre MaFenetreRapprochement affichant une interface pour rapprocher un compte*/
void AfficherRapp(CompteAbstrait* cp){
    MaFenetreRapprochement* fenetre=new MaFenetreRapprochement(); // création de fenetre
    fenetre->setWindowTitle("Rapprochement "+cp->getNom()); // affichage des infos sur un label
    fenetre->Transactions->setText("Transactions non rapprochés "+cp->getNom());
    auto it=TransactionApplication::getTransactionApplication().getFilterIterator(*(dynamic_cast<Compte*>(cp)),false);
    for (;!it.isDone();it.next()){ // on remplit la QTableWidget avec les participations, dans les différentes transactions, du compte que l'on rapproche
        auto itb=it.currentItem().getListCompte().begin();
        auto ite=it.currentItem().getListCompte().end();
        for(;itb!=ite;itb++)
        {
            if(std::get<0>(*itb)->getNumero()==cp->getNumero()) // lorsque l'on trouve le compte dans la transaction
            {
                fenetre->vueTransactions->insertRow(fenetre->vueTransactions->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem(it.currentItem().getTitre());
                QTableWidgetItem *item1 = new QTableWidgetItem(it.currentItem().getRef());
                QTableWidgetItem *item2= new QTableWidgetItem(it.currentItem().getDate().toString("dd/MM/yyyy"));
                QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
                QTableWidgetItem *item4=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
                QTableWidgetItem *item5=new QTableWidgetItem(QString::number(it.currentItem().getid()));
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1,0,item5);
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1, 1, item2);
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1, 2, item1);
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1, 3, item);
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1, 4, item3);
                fenetre->vueTransactions->setItem(fenetre->vueTransactions->rowCount()-1, 5, item4);
            }
        }
    }
    Compte* cc=dynamic_cast<Compte*>(cp);
    fenetre->concerne=cc;
    fenetre->vueSolde->insertRow(fenetre->vueSolde->rowCount());
    auto item1=new QTableWidgetItem(QString::number(cc->getSoldeRap())); // on affiche le solde avant dernier rapprochement
    auto item2=new QTableWidgetItem(QString::number(cc->getSolde())); // on affiche le solde courant
    fenetre->vueSolde->setItem(fenetre->vueSolde->rowCount()-1,0,item1);
    fenetre->vueSolde->setItem(fenetre->vueSolde->rowCount()-1,1,item2);
    QPushButton::connect(fenetre->buttonRapp, SIGNAL(clicked()),fenetre,SLOT(lancerRapprochement())); // on connecte les boutons
    QObject::connect(fenetre->retour,SIGNAL(clicked()),fenetre,SLOT(close()));
    if(fenetre->vueTransactions->rowCount()!=0)
        fenetre->show();
    else{
        QMessageBox message;
        message.setText("Ce compte a déjà été rapproché ou ne possède aucune transaction");
        message.exec();
    }
}

/* Constructeur de MaFenetreTrans*/
MaFenetreTrans::MaFenetreTrans(QDialog *parent) : QDialog(parent)
{
    vueTransactions=new QTableWidget(0,7,this);
    QStringList HHeaderList;
    HHeaderList<<"ID"<<"Date"<<"Référence"<<"Intitulé"<<"Débit"<<"Crédit"<<"Etat";
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    vueTransactions->setFixedWidth(721);
    vueTransactions->setFixedHeight(400);
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers); // ne pas modifier la QTableWidget
    QObject::connect(vueTransactions, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(menu(int,int))); // on connecte la QTableWidget a un menu permettant d'aggrandir la transaction, d'afficher plus de détails par rapport à la transaction, comme par exemple la participation des autres comptes
    transactions_layout=new QVBoxLayout;
    Transactions=new QLabel("Oui");
    transactions_layout->addWidget(Transactions);
    transactions_layout->addWidget(vueTransactions);
    setLayout(transactions_layout);
}

/* Cette méthode permet de mettre à jour l'affichage des transactions après une modification dans une autre fenetre */
void MaFenetreTrans::refresh()
{
    int j = vueTransactions->rowCount();
    for(int x(0);x!=j;x++){ // on supprime le contenu de la qtablewidget
        vueTransactions->removeRow(0);
    }
    TransactionApplication::FilterIterator it=TransactionApplication::getTransactionApplication().getFilterIterator(*(dynamic_cast<Compte*>(cp)),true);
    for(;!it.isDone();it.next()) // on parcourt la transaction à l'aide de l'iterateur
    {
        vector<tuple<Compte*,double,double>>::const_iterator itb=it.currentItem().getListCompte().begin();
        vector<tuple<Compte*,double,double>>::const_iterator ite=it.currentItem().getListCompte().end();
        for(;itb!=ite;itb++) // on parcourt les listes de comtpe qui se trouvent dans les differentes transactions
        {
            if(std::get<0>(*itb)->getNumero()==cp->getNumero())
            { // on met a jour la table
                vueTransactions->insertRow(vueTransactions->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem(it.currentItem().getTitre());
                QTableWidgetItem *item1 = new QTableWidgetItem(it.currentItem().getRef());
                QTableWidgetItem *item2= new QTableWidgetItem(it.currentItem().getDate().toString("dd/MM/yyyy"));
                QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
                QTableWidgetItem *item4=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
                QTableWidgetItem *item5=new QTableWidgetItem(QString::number(it.currentItem().getid()));
                QTableWidgetItem *item6=new QTableWidgetItem;
                if(it.currentItem().getRapproche()==true)
                    item6->setText("R");
                vueTransactions->setItem(vueTransactions->rowCount()-1, 0, item5);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 1, item2);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 2, item1);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 3, item);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 4, item3);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 5, item4);
                vueTransactions->setItem(vueTransactions->rowCount()-1, 6, item6);
            }
        }

    }

}

/* slot utilisé pour ouvrir une fenetre permettant d'agrandir la Transaction sur laquelle on double clique*/
void MaFenetreTrans::menu(int i,int j){
    QTableWidgetItem* a = vueTransactions->item(i,0);
    QString b = a->text();
    Transaction* t=TransactionApplication::getTransactionApplication().getTransaction(b.toInt());
    if(t->getRapproche()==true){ // si la transaction est rapprochée
        QMessageBox message;
        message.setText("Cette transaction étant rapprochée, elle ne peut subir aucune modification");
        message.exec();
        AgrandirTransR(t); // on lance la fonction créant une fenetre MaFenetreAgTransR avec les détails de la transaction rapprochée
        }
    else
        AgrandirTransNR(t); // on lance la fonction créant une fenetre MaFenetreAgTransR avec les détails de la transaction non rapprochée et une possibilité de la modifier/supprimer
}

/* méthode permettant de créer une fenetre MaFenetreAgTransNR qui agrandit la transaction dont on s'intéresse*/
void MaFenetreTrans::AgrandirTransNR(Transaction* t){
    MaFenetreAgTransNR* fenetre=new MaFenetreAgTransNR;// on crée une fenetre de type MaFenetreAgTransNR destinée à afficher des transactions non rapprochées que l'on peut modifier
    fenetre->refresh(t);
    fenetre->setTrans(t); // on initialise l'attribut concerne avec la transaction que l'on souhaite afficher dans la fenetre, cette fenetre est donc destinée à traiter cette transaction
    fenetre->setPere(this);
    fenetre->setModal(true);
    fenetre->exec();
}


/* Constructeur de MaFenetreRapprochement*/
MaFenetreRapprochement::MaFenetreRapprochement(QDialog *parent) : QDialog(parent)
{
    vueTransactions=new QTableWidget(0,6,this);
    QStringList HHeaderList;
    HHeaderList<<"ID"<<"Date"<<"Référence"<<"Intitulé"<<"Débit"<<"Crédit";
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    vueTransactions->setFixedWidth(620);
    vueTransactions->setFixedHeight(400);
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QObject::connect(vueTransactions, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(menu(int,int)));// on connecte la QTableWidget pour voir un menu apparaitre quand on double clique sur une ligne

    vueSolde=new QTableWidget(0,2,this);
    vueSolde->setFixedWidth(318);
    QStringList SoldeHeaderList;
    SoldeHeaderList<<"Solde au dernier rapprochement"<<"Solde actuel";
    vueSolde->setHorizontalHeaderLabels(SoldeHeaderList);
    vueSolde->setFixedHeight(50);
    vueSolde->setColumnWidth(0,200);
    vueSolde->setEditTriggers(QAbstractItemView::NoEditTriggers);

    transactions_layout=new QVBoxLayout;
    Transactions=new QLabel("Oui");
    buttonRapp=new QPushButton("Rapprocher",this);
    retour=new QPushButton("Refuser",this);
    transactions_layout->addWidget(Transactions);
    transactions_layout->addWidget(vueTransactions);
    transactions_layout->addWidget(vueSolde);
    transactions_layout->addWidget(buttonRapp);
    transactions_layout->addWidget(retour);
    setLayout(transactions_layout);
}

/*slot permettant de lancer le rapprochement lors de la validation de celui-ci*/
void MaFenetreRapprochement::lancerRapprochement(){
    CompteApplication::getCompteApplication().RapprochBanc(concerne); // lorsque l'on valide le rapprochement sur la fentre MaFenetreRapprochement, on lance le rapprochement du compte concerné (concerne pointe sur le compte concerné)
    this->close();
}

/* slot agrandissant la transaction sur laquelle on double clique*/
void MaFenetreRapprochement::menu(int i,int j){
    QTableWidgetItem* a = vueTransactions->item(i,0);
    QString b = a->text();
    Transaction* t=TransactionApplication::getTransactionApplication().getTransaction(b.toInt());
    AgrandirTransR(t); // on lance la fonction créant une fenetre MaFenetreAgTransR avec les détails de la transaction
}

/* Constructeur de MaFenetreAgTransR*/
MaFenetreAgTransR::MaFenetreAgTransR(QDialog *parent) : QDialog(parent)
{
    vueTransactions=new QTableWidget(0,5,this);
    QStringList HHeaderList;
    HHeaderList<<"Numéro"<<"Nom"<<"Débit"<<"Crédit"<<"Etat"; // etat signifie rapproché ou non (infos pour l'utilisateur)
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    vueTransactions->setFixedWidth(520);
    vueTransactions->setFixedHeight(400);
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers); // QTableWidget non modifiable
    transactions_layout=new QVBoxLayout;
    Transactions=new QLabel("Oui");
    transactions_layout->addWidget(Transactions);
    transactions_layout->addWidget(vueTransactions);
    setLayout(transactions_layout);
}

/* Constructeur de MaFenetreAgTransNR*/
MaFenetreAgTransNR::MaFenetreAgTransNR(QDialog *parent) : QDialog(parent)
{
    vueTransactions=new QTableWidget(0,5,this);
    QStringList HHeaderList;
    HHeaderList<<"Numéro"<<"Nom"<<"Débit"<<"Crédit"<<"Etat";
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    vueTransactions->setFixedWidth(520);
    vueTransactions->setFixedHeight(400);
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Modif=new QPushButton("Modifier",this);
    Supp=new QPushButton("Supprimer",this);
    QObject::connect(Supp,SIGNAL(clicked()),this,SLOT(SuppTrans()));
    QObject::connect(Modif,SIGNAL(clicked()),this,SLOT(modifTrans()));
    transactions_layout=new QVBoxLayout;
    Transactions=new QLabel("Oui");
    transactions_layout->addWidget(Transactions);
    transactions_layout->addWidget(vueTransactions);
    transactions_layout->addWidget(Modif);
    transactions_layout->addWidget(Supp);
    setLayout(transactions_layout);
}

/*slot permettant la suppression d'une transaction dans MaFenetreAgTransNR*/

void MaFenetreAgTransNR::SuppTrans(){ // cette fonction est appelée lors de l'appuie sur le bouton supprimer d'une fenetre MaFenetreAgTransNR
    TransactionApplication::getTransactionApplication().suppTrans(concerne); //on suppprime la transaction affichée dans la fênetre, celle qui est concernée
    pere->getpere()->refresh(pere->getpere()->dernierCompte[pere->getpere()->dernierCompte.size()-1]); // on refresh la QTableWidget de MaFenetreCompte dans l'emplacement de la hierarchie ou l'on est actuellement suite à la suppression de la transaction, pour la mise à jour du solde
    pere->refresh(); // on refresh le père de cette fenetre, c'est à dire MaFenetreTrans car celle-ci ne contient plus les mêmes infos dans sa QTableWidget suite à la suppression de la transaction
    this->close();
}

/*slot permettant la modification d'une transaction dans MaFenetreAgTransNR*/

void MaFenetreAgTransNR::modifTrans(){
    Transaction* nouvelle=concerne->dedouble(); // on dédouble la transaction concerné, et on effectue les modifications sur celle ci
    int modiftr=QMessageBox::Yes;
    while(modiftr!=QMessageBox::No){
        modiftr = QMessageBox::question(this, "Changement", "Souhaitez vous apporter des modifications à la transaction ? ", QMessageBox::Yes | QMessageBox::No);
        if(modiftr==QMessageBox::Yes ){
            QStringList b = {"Date","Ajouter compte","Supprimer compte","Ref","Titre","Modifier les montants"}; // on crée une liste avec différentes actions possibles
            QString c = QInputDialog::getItem(this, "Options", "Choisir l'option",b,0,false);
            if (c=="Date"){
                FenDate* nouvelledate = new FenDate; // on crée une fenetre date pour la modification de la date de la transaction
                nouvelledate->setModal(true);
                nouvelledate->exec();
                QDate Date = nouvelledate->selectedDate();
                QString d = QString::number(Date.year()) +"/" + QString::number(Date.month()) + "/" + QString::number(Date.day());
                nouvelle->setDate(Date); // on change la date dans la transaction
                refresh(nouvelle); //on refresh la fenetre pour que l'utilisateur voit les modifications
            }
            else if(c=="Ref"){
                QString d = QInputDialog::getText(this, "Ref", "Quel est la nouvelle reference ?");
                nouvelle->setRef(d); // on modifie la référence
                refresh(nouvelle);//on refresh la fenetre pour que l'utilisateur voit les modifications
            }
            else if(c=="Titre"){
                QString d = QInputDialog::getText(this, "Titre", "Quel est le nouveau Titre de la reference?");
                nouvelle->setTitre(d); // on modifie le titre
                refresh(nouvelle); //on refresh la fenetre pour que l'utilisateur voit les modifications
            }
            else if(c=="Modifier les montants"){
                QStringList nomliste = nouvelle->nomlistecompte(); // on obtient le nom de tous les comptes associés à la transaction
                QString nom = QInputDialog::getItem(this, "Options", "Choisir le compte dont les montants doivent etre modifié",nomliste,0,false);
                QString c2 = QInputDialog::getItem(this, "Options", "Choisir l'option",{"Debit","Credit"},0,false);
                auto itb=nouvelle->getListCompte().begin();
                auto ite=nouvelle->getListCompte().end();
                for(;itb!=ite;itb++){
                    if(std::get<0>(*itb)->getNom()==nom)
                        break;
                }
                if (c2 == "Debit"){
                    double deb =  QInputDialog::getDouble(this, "Titre", "Quel est le nouveau debit ?",0,-2147483647,2147483647,2);
                    std::get<1>(*itb)=deb;
                }
                else if (c2 == "Credit"){
                    double cred =  QInputDialog::getDouble(this, "Titre", "Quel est le nouveau credit ?",0,-2147483647,2147483647,2);
                    std::get<2>(*itb)=cred;
                }
                refresh(nouvelle);//on refresh la fenetre pour que l'utilisateur voit les modifications
           }
            // ------ ajouter compte
            else if(c=="Ajouter compte"){
                int modif=QMessageBox::Yes;
                while(modif==QMessageBox::Yes){
                    modif = QMessageBox::question(this, "Changement credit/debit", "Souhaitez vous ajouter un nouveau compte à la transaction ?", QMessageBox::Yes | QMessageBox::No);
                    if (modif == QMessageBox::Yes){
                        QStringList nomliste;
                        CompteApplication::getCompteApplication().liste_type(nomliste,false,false);// on récupère tous les comptes concrets
                        QString nom = QInputDialog::getItem(this, "Options", "Choisir l'option",nomliste,0,false);
                        Compte* cpt = dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte(nom));
                        bool ok = true;
                        if(ok){
                            double deb =  QInputDialog::getDouble(this, "Titre", "Quel est le nouveau debit ?",0,-2147483647,2147483647,2);
                            double cred =  QInputDialog::getDouble(this, "Titre", "Quel est le nouveau credit ?",0,-2147483647,2147483647,2);
                            std::tuple<Compte*,double,double> t = make_tuple(cpt,deb,cred);
                            nouvelle->getListCompte().push_back(t);
                            refresh(nouvelle);}
                        }
                        }
                }
            else if(c=="Supprimer compte"){
                QStringList nomliste = nouvelle->nomlistecompte();
                QString nom = QInputDialog::getItem(this, "Options", "Choisir le compte à supprimer",nomliste,0,false);
                int itb = 0;
                int ite = nouvelle->getListCompte().size();
                for(;itb!=ite;itb++){
                    if(std::get<0>(nouvelle->getListCompte()[itb])->getNom()==nom){
                        nouvelle->getListCompte().erase(nouvelle->getListCompte().begin()+itb);
                     }
                 }
                refresh(nouvelle);
             }
           }
        }
    try{
        TransactionApplication::getTransactionApplication().creerTransaction(nouvelle->getListCompte(),nouvelle->getDate(),nouvelle->getRef(),nouvelle->getTitre(),concerne->getid());// on essaye de recreer la transaction
        TransactionApplication::getTransactionApplication().suppTrans(concerne); // on supprime la transaction d'avant
        concerne = TransactionApplication::getTransactionApplication().getTransaction(TransactionApplication::getTransactionApplication().getnb()-1);
        pere->refresh();// on refresh la fenetre MaFenetreTrans car on a modifié la transaction
    }
   catch(TresorerieException t){
        QMessageBox::critical(this,"erreur",t.getInfo()+".\nLa transaction ne pourra donc pas être modifiée.");
          }

}

/*Methode raffraichissant MaFenetreAgTransNR lors de la modification de la transaction */
void MaFenetreAgTransNR::refresh(Transaction* t){

    setWindowTitle("Transactions "+t->getTitre()+" "+t->getRef());
    Transactions->setText("Transactions: "+t->getTitre()+"\tReference: "+t->getRef()+"\tDate: "+t->getDate().toString("dd/MM/yyyy"));// charge le label avec les informations de la transaction
    int j = vueTransactions->rowCount();
    for(int x(0);x!=j;x++){
        vueTransactions->removeRow(0);
    }
    auto itb=t->getListCompte().begin();
    auto ite=t->getListCompte().end();
    for(;itb!=ite;itb++)
    {
        vueTransactions->insertRow(vueTransactions->rowCount());
        QTableWidgetItem *item = new QTableWidgetItem((std::get<0>(*itb))->getNom());
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number((std::get<0>(*itb))->getNumero()));
        QTableWidgetItem *item2=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
        QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
        QTableWidgetItem *item4=new QTableWidgetItem;
        if(t->getRapproche()==true)
            item4->setText("R");
        vueTransactions->setItem(vueTransactions->rowCount()-1, 0, item1);
        vueTransactions->setItem(vueTransactions->rowCount()-1, 1, item);
        vueTransactions->setItem(vueTransactions->rowCount()-1, 2, item2);
        vueTransactions->setItem(vueTransactions->rowCount()-1, 3, item3);
        vueTransactions->setItem(vueTransactions->rowCount()-1, 4, item4);
    }
}

/* Constructeur de MaFenetreEnCours*/

MaFenetreEnCours::MaFenetreEnCours(QDialog *parent) : QDialog(parent)
{
    vueTransactions=new QTableWidget(0,4,this);
    QStringList HHeaderList;
    HHeaderList<<"Numéro"<<"Nom"<<"Débit"<<"Crédit";
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    vueTransactions->setFixedWidth(405);
    vueTransactions->setFixedHeight(400);
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    continuer=new QPushButton("RajouterCompte",this);
    arreter=new QPushButton("Cancel",this);
    cre=new QPushButton("Créer",this);
    modif = new QPushButton("modifier",this);
    transactions_layout=new QVBoxLayout;
    Transactions=new QLabel("Oui");
    transactions_layout->addWidget(Transactions);
    transactions_layout->addWidget(vueTransactions);
    transactions_layout->addWidget(cre);
    transactions_layout->addWidget(continuer);
    transactions_layout->addWidget(arreter);
    transactions_layout->addWidget(modif);
    setLayout(transactions_layout);
}

/* ce slot est utilise pour la creation de transaction, il permet de continuer a rajouter des comtpes */
void MaFenetreEnCours::m_continuer(){
    getpere()->setFalseC(true);
    getpere()->setFalseCr(true);
    getpere()->setfchgt(true);
    this->close();
}

/* ce slot est utilise pour la creation de transaction, il permet de modifier une transaction en creation */
void MaFenetreEnCours::modifier(){
    getpere()->setFalseC(true);
    getpere()->setfchgt(false);
    this->close();
}

/* ce slot est utilise pour la creation de transaction, il permet de modifier les valeurs associées à un comtpe dans une transaction en creation qui est erronnée */
void MaFenetreEnCours::modif_cpt(int i,int j){
    QTableWidgetItem* a = vueTransactions->item(i,1);
    QString b = a->text();
    QStringList d = {"Supprimer", "Modifier credit/debit"};
    QString c = QInputDialog::getItem(this, "Options", "Choisir l'option",d,0,false);
    double debit = 0;
    double credit =0;
    if(c=="Supprimer"){
        vueTransactions->removeRow(i);
        auto itb=getpere()->getv().begin();
        auto ite=getpere()->getv().end();
        for(;itb!=ite;itb++) // on parcourt la liste et on supprime le compte où il se trouve
        {
            if(b == (std::get<0>(*itb))->getNom()){
                getpere()->getv().erase(std::remove(getpere()->getv().begin(), getpere()->getv().end(), (*itb)), getpere()->getv().end());
            }
        }

    }
    else if(c=="Modifier credit/debit"){ // on modifie les montants
        QString a = QInputDialog::getItem(this, "Options", "Choisir l'option",{"credit","debit"},0,false);
        if(a=="credit"){
            credit = QInputDialog::getDouble(this, "Credit", "Quel est le montant à créditer ?",0,-2147483647,2147483647,2);
        }
        else if(a=="debit"){
            debit = QInputDialog::getDouble(this, "Debit", "Quel est le montant à débiter ?",0,-2147483647,2147483647,2);
        }
        QTableWidgetItem *item2;
        QTableWidgetItem *item3;
        auto itb=getpere()->getv().begin();
        auto ite=getpere()->getv().end();
        for(;itb!=ite;itb++) // on parcourt la liste de compte associé et on modifie les montants
        {
            if(b == (std::get<0>(*itb))->getNom()){
                if(a=="debit"){
                    item3=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
                    (std::get<1>(*itb)) = debit;
                    item2=new QTableWidgetItem(QString::number(debit));
                 }
                else{
                    item2=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
                    (std::get<2>(*itb)) = credit;
                    item3=new QTableWidgetItem(QString::number(credit));
                   }
            }
        }
        vueTransactions->setItem(i,2,item2);
        vueTransactions->setItem(i,3,item3);
        }
   }

/* ce slot est utilise pour la creation de transaction, il permet d'annuler la creation de la transaction*/
void MaFenetreEnCours::cancel()
{
    // on change les booléens utiliser dans la boucle utilisée dans la creation de transactions
    pere->setFalseC(false);
    pere->setFalseCr(true);
    getpere()->setfchgt(false);
    this->close();
}

/* ce slot est utilise pour la creation de transaction, il permet la creation de la transaction*/
void MaFenetreEnCours::creer()
{
    pere->setFalseC(false);
    pere->setFalseCr(false);
    getpere()->setfchgt(false);
    this->close();
}

/* Constructeur de FenDate*/
FenDate::FenDate(QDialog *parent) : QDialog(parent)
{
    this->setWindowTitle("Choisir une date");
    this->setSizeGripEnabled(false);
    resize(260,230);
    widget= new QWidget(this);
    widget->setGeometry(QRect(0,10,258,215));

    aff = new QVBoxLayout(widget);
    aff->setContentsMargins(0,0,0,0);

    calendarWidget= new QCalendarWidget(widget); // on crée un calendrier permettant de choisir la date

    aff->addWidget(calendarWidget);

    valider = new QPushButton("Valider");
    aff->addWidget(valider);
    this->setLayout(aff);
    QObject::connect(valider,SIGNAL(clicked()),this,SLOT(close()));

}

/* slot permettant de renvoyer la date selectionnée*/

QDate FenDate::selectedDate() const{
    return calendarWidget->selectedDate();
}
