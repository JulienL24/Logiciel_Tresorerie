#include "fenetre_compte.h"
#include "fenetre_transaction.h"

/* Constructeur de MaFenetreCompte*/
MaFenetreCompte::MaFenetreCompte(QWidget *parent) : QWidget(parent)
{

    this->setWindowTitle(QString("Affichage des Comptes"));

    t_creer = new QPushButton("Creer une transaction", this); // on connecte les boutons et on leur met des titres
    QObject::connect(t_creer, SIGNAL(clicked()), this, SLOT(creationT()));

    m_creer = new QPushButton("Creer un compte", this);
    QObject::connect(m_creer, SIGNAL(clicked()), this, SLOT(creation()));

    cloture=new QPushButton("Cloturer le livre",this);
    QObject::connect(cloture,SIGNAL(clicked()),this, SLOT(clotureLivre()));

    retour = new QPushButton("Retour", this);
    QObject::connect(retour, SIGNAL(clicked()), this, SLOT(retour_arriere()));

    charger= new QPushButton("Charger un fichier",this);
    QObject::connect(charger, SIGNAL(clicked()), this, SLOT(charge()));

    bilan=new QPushButton("Bilan",this);
    QObject::connect(bilan, SIGNAL(clicked()), this, SLOT(genereBilan()));

    compte_resultat=new QPushButton("Compte de résultat",this);
    QObject::connect(compte_resultat, SIGNAL(clicked()), this, SLOT(genereCompteResultat()));

    listetype=new QPushButton("Mode par type",this);
    QObject::connect(listetype,SIGNAL(clicked()),this,SLOT(affichage_ensemble()));

   compte_layout=new QHBoxLayout;
   compte_layout->addWidget(retour);
   compte_layout->addWidget(listetype);
   compte_layout->addWidget(charger);
   compte_layout->addStretch();


   couche=new QHBoxLayout;
   transfertsl=new QLabel("Ensemble de comptes :");



   vueComptes=new QTableWidget(0,4,this);
   vueComptes->verticalHeader()->setVisible(false);
   QStringList HHeaderList;
   HHeaderList<<"N° Compte"<<"Nom"<<"Solde"<<"Type";
   vueComptes->setHorizontalHeaderLabels(HHeaderList);
   vueComptes->setFixedWidth(453);
   vueComptes->setFixedHeight(300);
   vueComptes->setColumnWidth(0,100);
   vueComptes->setColumnWidth(1,150);
   vueComptes->setEditTriggers(QAbstractItemView::NoEditTriggers);
   couche->addWidget(vueComptes);
   QObject::connect(vueComptes, SIGNAL(cellClicked(int,int)), this, SLOT(AvancerHierarchie(int,int)));
   QObject::connect(vueComptes, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(menu(int,int)));
   final =new QVBoxLayout;
   final->addWidget(m_creer);
   final->addWidget(t_creer);
   final->addWidget(cloture);
   final->addWidget(compte_resultat);
   final->addWidget(bilan);
   couche->addLayout(final);
   fin=new QVBoxLayout;
   fin->addLayout(compte_layout);
   fin->addWidget(transfertsl);
   fin->addLayout(couche);
   this->setLayout(fin);
   dernierCompte.push_back(racine);
   CompteVirtuel* cv=dynamic_cast<CompteVirtuel*>(racine);
   QStringList* obj=cv->getnomlistdirect();
   foreach(QString element, *obj) // on initialise la QTableWidget avec les comptes directs fils du Compte Racine
    {
      vueComptes->insertRow(vueComptes->rowCount());
      QTableWidgetItem *item = new QTableWidgetItem(element);
      CompteAbstrait * cpt = cv->getCompte(element);
      QTableWidgetItem *item1 = new QTableWidgetItem(converType(cv->getType()));
      if(dynamic_cast<Compte*>(cpt)!=nullptr){
      Compte* j2 = dynamic_cast<Compte*>(cpt);
      QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));
      QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
      vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
      vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
    }
      vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
      vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
    }
}

/* ce slot permet de sélectionner une option à réaliser sur un compte affiché par la QtableWidget */
void MaFenetreCompte::menu(int i, int j){
    QTableWidgetItem* a = vueComptes->item(i,1); // on selectionne le nom du compte sur lequel on a clique
    QString b = a->text();
    bool ok = false;
    QStringList d = {"Déplacer","Supprimer", "Modifier nom"};
    CompteAbstrait* cd=racine->getCompte(b);
    /* si le compte est un compte concret on ajoute deux possibilites : rapprocher et afficher transactions*/
    if(dynamic_cast<Compte*>(cd)!=nullptr){
            d.append("Afficher transactions");
            d.append("Rapprocher");
    }
    QString c = QInputDialog::getItem(this, "Options", "Choisir l'option",d,0,false, &ok); // on choisit ici l'option
    if (ok){
        // en fonction de l'option choisi on effectue l'action associée
        if(c=="Supprimer"){
            // on essaye de supprimer le compte, l'operation peut ne pas aboutir (pas de pere, ou encore le compte a encore des transactions associées) d'ou l'utilisation d'un bloc try
            try{CompteAbstrait* cp=racine->getCompte(b)->getPere();
            racine->supp(b);
            refresh(cp);}catch(TresorerieException t){QMessageBox::warning(this,"erreur",t.getInfo());};
        }
        if(c=="Afficher transactions"){
            CompteAbstrait* cp=CompteApplication::getCompteApplication().getCompte(b); // on recupere le compte en question
            AfficherTrans(cp); // permet d'afficher les transactions associées au compte
        }
        if(c=="Modifier nom"){
            // la modification du nom étant une source possible d'erreur, on utilise un bloc try.
            try{QString nom = QInputDialog::getText(this, "Nom", "Quel est le nouveau nom du compte?");
            CompteAbstrait* cp=CompteApplication::getCompteApplication().getCompte(b)->getPere(); // on récupère le père du compte pour lequel on veut modifier le nom
            racine->modifcompte(racine->getCompte(a->text())->getNom(),nom); // modif du nom
            refresh(cp);// on raffraichit la qtablewidget
            }catch(TresorerieException t){QMessageBox::warning(this,"erreur",t.getInfo());};
        }
        if(c=="Rapprocher"){
            CompteAbstrait* cp=CompteApplication::getCompteApplication().getCompte(b);
            AfficherRapp(cp);
        }
        if(c=="Déplacer"){
            CompteAbstrait* cp=CompteApplication::getCompteApplication().getCompte(b);
            MaFenetreDeplace* fenetre=new MaFenetreDeplace;
            fenetre->setWindowTitle("Déplacement de "+cp->getNom());
            fenetre->setConcerne(cp);
            fenetre->setPere(this);
            fenetre->setType(cp->getType());
            fenetre->init(cp->getType());
            fenetre->setModal(true);
            fenetre->exec();
        }

}
}

/* ce slot permet de charger de nouveaux fichiers de transactions/comptes */
void MaFenetreCompte::charge(){
    QString c=CompteApplication::getCompteApplication().getFilename();
    QString tr=TransactionApplication::getTransactionApplication().getFilename();
    try{
       int charg = QMessageBox::question(this, "Chargement fichier", "Etes-vous sûr de vouloir charger un autre fichier?", QMessageBox::Yes | QMessageBox::No);
       if(charg==QMessageBox::Yes){
           QMessageBox message1;
           message1.setText("Choisissez un fichier de compte à charger");
           message1.exec();
           QString filename1= QFileDialog::getOpenFileName(nullptr,"Choisir un fichier de compte",QDir::currentPath(),"Text files (*.txt)");
           QString filename2;
           int question;
           while(filename1.isNull()){ // permet d'annuler la modifications des fichiers
               question=QMessageBox::question(nullptr,"Confirmation","Etes vous sur de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);
               if (question==QMessageBox::Yes)
                   break;
               else
                   filename1= QFileDialog::getOpenFileName(nullptr,"Choisir un fichier de compte",QDir::currentPath(),"Text files (*.txt)");
           }
           if(!filename1.isNull()){
               QMessageBox message2;
               message2.setText("Choisissez le fichier de transaction correspondant à charger");
               message2.exec();
               filename2 = QFileDialog::getOpenFileName(nullptr,"Choisir un fichier de transaction",QDir::currentPath(),"Text files (*.txt)");
               while(filename2.isNull()){ //permet d'annuler la modifications des fichiers
                   question=QMessageBox::question(nullptr,"Confirmation","Etes vous sur de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);
                   if (question==QMessageBox::Yes)
                       break;
                   else
                       filename2= QFileDialog::getOpenFileName(nullptr,"Choisir un fichier de transaction",QDir::currentPath(),"Text files (*.txt)");
            }
           }
           if(!filename2.isNull()){
               CompteApplication* ca=&(CompteApplication::getCompteApplication());
               TransactionApplication* ta=&(TransactionApplication::getTransactionApplication());
               ca->save(); // on sauvegarde les comptes de l'ancien fichier de comptes
               ta->savetrans();// on sauvegarde les transactions de l'ancien fichier de transactions
               ta->libererTransactionApplication(); // on libere les fichiers associés
               ca->libererCompteApplication();
               ca=&(CompteApplication::getCompteApplication());
               ca->load(filename1); // on charge le nouveau fichier de compte
               ta=&(TransactionApplication::getTransactionApplication());
               ta->load(filename2); // on charge le nouveau fichier de transactions
               reset();
             }
   }
    }
    catch(TresorerieException t){try{CompteApplication::libererCompteApplication();CompteApplication::getCompteApplication().load(c);TransactionApplication::libererTransactionApplication();TransactionApplication::getTransactionApplication().load(tr);reset();QMessageBox::warning(this,"Erreur",t.getInfo());}catch(TresorerieException t){QMessageBox::warning(this,"Erreur",t.getInfo());}};
}

/* Ce slot permet la creation de comptes */
void MaFenetreCompte::creation(){
try{FabriqueCompte factory;
    QMessageBox msgBox;
    msgBox.setWindowTitle("Ensemble/Simple_compte");
    msgBox.setText("Quel catégorie de compte souhaitez-vous créer ?");
    // on demande d'abord si le compte que l'on veut créer est un compteVirtuel ou un compteconcret
    QAbstractButton *myYesButton=msgBox.addButton("Ensemble",QMessageBox::YesRole);
    QAbstractButton *myNoButton=msgBox.addButton("Concret",QMessageBox::NoRole);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.exec();
    QString nom = QInputDialog::getText(this, "Nom", "Quel est le nom du compte ?");
    QString nom_ss = dernierCompte[dernierCompte.size()-1]->getNom();  // on recupere le compte dans lequel on se trouvait.
    Type t = dernierCompte[dernierCompte.size()-1]->getType(); // on recupere le type du compte dans lequel on se trouvait.
    bool ok = true;
    if(dernier_type == Type::M){ // si on se trouve dans le compte racine alors on demande le type du compte à créer
       if(t==Type::M){
            ok = false;
            QString a = QInputDialog::getItem(this,"Type Compte", "Choisir le type du compte", {"Actif", "Passif", "Recettes", "Depenses"}, 0,false, &ok);
            if (ok){
                if (a =="Actif") t = Type::A;
                else if(a=="Passif") t= Type::P;
                else if(a=="Depenses") t= Type::D;
                else if(a=="Recettes") t= Type::R;
            }
        }
      }
       else // sinon on prend le type du compte dans lequel on se trouvait
           t=dernier_type;
    if (msgBox.clickedButton() == myYesButton)
        racine->creerCompte(&factory,CompteApplication::getCompteApplication().getCompte(nom_ss),'V',nom,t); // creaiton comptevituel si option sélectionnée
    else if(msgBox.clickedButton()== myNoButton){ // si l'option selectionnée est compte concret alors on regarde le type du compte et on effectue une eventuelle transaction (type P ou A)
        int cap;
        if(t==Type::P) // creation compte capitaux propre
            cap = QMessageBox::question(this, "Creation de Compte", "Souhaitez-vous créer ce compte passif en tant que capitaux propre?", QMessageBox::Yes | QMessageBox::No);
        if(t==Type::A || (t==Type::P && cap==QMessageBox::No)){
            QStringList compte;
            racine->liste_type(compte,false,true,Type::P);
            double solde = QInputDialog::getDouble(this, "Solde", "Quel est le solde initial du compte ?",0,-2147483647,2147483647,2);
            if (compte.size()==0){
                QMessageBox message1;
                message1.setText("Aucun compte de capitaux propre existant. Veuillez entrer un nom pour la création du premier");
                message1.exec();
                QString a = QInputDialog::getText(this,"Capitaux Propre", "Choisir le nom du compte de capitaux propre");
                racine->creerCompte(&factory,&CompteApplication::getCompteApplication(),'C',a,Type::P);
            }
            QStringList comptepr;
            racine->liste_type(comptepr,false,true,Type::P);
            QString nom_cptpropre = QInputDialog::getItem(this,"Creation de compte", "Choisir le compte de capitaux propre", comptepr,0,false);
            racine->creerCompte(&factory,CompteApplication::getCompteApplication().getCompte(nom_ss),'C',nom,t);
            std::vector<tuple<Compte*,double,double>> v;
            if(t==Type::A){// on crée une transaction pour le solde initial du compte.
                std::tuple<Compte*,double,double> t1(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte(nom)),solde,0);
                std::tuple<Compte*,double,double> t2(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte(nom_cptpropre)),0,solde);
                v.push_back(t1);
                v.push_back(t2);
            }
            else{
                 std::tuple<Compte*,double,double> t1(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte(nom)),0,solde);
                 std::tuple<Compte*,double,double> t2(dynamic_cast<Compte*>(CompteApplication::getCompteApplication().getCompte(nom_cptpropre)),solde,0);
                 v.push_back(t1);
                 v.push_back(t2);
            }
            sic += 1;
            TransactionApplication::getTransactionApplication().creerTransaction(v,QDate::currentDate(),"SI" + QString::number(sic),"Solde initial",TransactionApplication::getTransactionApplication().usenb());
            } // creation de la transaction
         else{
             racine->creerCompte(&factory,CompteApplication::getCompteApplication().getCompte(nom_ss),'C',nom,t);
             }

          }
    refresh(dernierCompte[dernierCompte.size()-1]);
}
catch(TresorerieException t){QMessageBox::warning(this,"erreur",t.getInfo());};
}

/* cette méthode permet de rafraichir la qtablewidget lorsqu'on la modifie, on utilise souvent la rafraichissement de la table lorsque l'on modifie un element
  d'un compte ou que l'on retourne en arrière dans la hiérarchie, elle permet de factoriser du code ! */
void MaFenetreCompte::refresh(CompteAbstrait* cp)
{
    int j = vueComptes->rowCount();
    for(int x(0);x!=j;x++){ // on met la table à 0
        vueComptes->removeRow(0);
    }
    CompteVirtuel* cv=dynamic_cast<CompteVirtuel*>(cp);
    QStringList* obj=cv->getnomlistdirect(); // on accede aux descendants directs
    foreach(QString element, *obj) // parcourt des descendants directs et actualisation de la qtablewidget
     {
       vueComptes->insertRow(vueComptes->rowCount());
       QTableWidgetItem *item = new QTableWidgetItem(element);
       CompteAbstrait * cpt = cv->getCompte(element);
       QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
       vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
       if(dynamic_cast<Compte*>(cpt)!=nullptr){ // verification du fait que le compte est concret, auquel cas on ajoute le solde et le numero
       Compte* j2 = dynamic_cast<Compte*>(cpt);
       QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
       QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
       vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
       vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);

     }
       vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
     }
}

/* permet de se deplacer en arriere dans la qtablewidget qui affiche les comptes ! */
void MaFenetreCompte::retour_arriere(){
    if (dernier_type==Type::M){
        if(dernierCompte.size() == 1){ // si le type est M et que la pile est de taille1 -> on raffraichit au niveau de la racine
            refresh(racine);
        }
        else{
            dernierCompte.pop_back();
            refresh(dernierCompte[dernierCompte.size()-1]);
        }
    }
    else{ // sinon on raffraichit avec les descendahts du derniercompte de la pile
        int j = vueComptes->rowCount();
        for(int x(0);x!=j;x++){ // on supprime le contenu de la qtablewidget
            vueComptes->removeRow(0);
        }
         QStringList aff_ensemble;
         racine->liste_type(aff_ensemble,true,true,dernier_type); // on recupere l'ensemble des comptes fils
         foreach(QString element, aff_ensemble) // on affiche l'ensemble des comptes fils
         {
                vueComptes->insertRow(vueComptes->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem(element);
                CompteAbstrait * cpt = racine->getCompte(element);
                QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
                vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
                if(dynamic_cast<Compte*>(cpt)!=nullptr){
                Compte* j2 = dynamic_cast<Compte*>(cpt);
                QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
                QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
                vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
                vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
                 //penser à faire un if si
                }
                vueComptes->setItem(vueComptes->rowCount()-1, 1, item);


            }
            dernier_type = Type::M;
        }
}

/* ce slot permet la creation de transactions ! */
void MaFenetreCompte::creationT(){ // rajouter une exception si il n'y a pas de compte
    QStringList comptea;
    racine->liste_type(comptea,false,false);
    if(!comptea.empty()){
        Cont=true;
        Cre=true;
        chgt = false;
        v.clear();
        tuple<Compte*,double,double> t;
        double credit,debit;
        QString titre,ref;
        titre=QInputDialog::getText(this, "Titre", "Quel est le titre de la transaction ?"); // on recupere le titre de la transaction
        ref=QInputDialog::getText(this, "Reference", "Quel est la référence de la transaction ?"); // on recupere la reference de la transaction
        FenDate* nouvelledate = new FenDate;
        nouvelledate->setModal(true);
        nouvelledate->exec();
        QDate Date = nouvelledate->selectedDate();
        QString d = QString::number(Date.year()) +"/" + QString::number(Date.month()) + "/" + QString::number(Date.day());
        QString nom_cptpropre;
        while(Cont == true){ // tant que l'on a pas modifie le booléen CONT (à l'aide des boutons créer ou annuler) on continue de modifier la transaction
            Cont=false;
            Cre=true;
            MaFenetreEnCours* fenetre=new MaFenetreEnCours;
            fenetre->setPere(this);
            fenetre->setWindowTitle("Transactions "+titre+" "+ref);
            fenetre->getLab()->setText("Transactions: "+titre+"\tReference: "+ref+"\tDate: "+Date.toString("dd/MM/yyyy"));
            auto itb=v.begin();
            auto ite=v.end();
            for(;itb!=ite;itb++) // on parcourt le vecteur de la transaction et on affiche les comptes associés avec leurs debits/credits
            {
                fenetre->getvue()->insertRow(fenetre->getvue()->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem((std::get<0>(*itb))->getNom());
                QTableWidgetItem *item1 = new QTableWidgetItem(QString::number((std::get<0>(*itb))->getNumero()));
                QTableWidgetItem *item2=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
                QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 0, item1);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 1, item);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 2, item2);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 3, item3);
            }
            QObject::connect(fenetre->getCont(),SIGNAL(clicked()),fenetre,SLOT(m_continuer()));
            QObject::connect(fenetre->getArr(),SIGNAL(clicked()),fenetre,SLOT(cancel()));
            QObject::connect(fenetre->getCre(),SIGNAL(clicked()),fenetre,SLOT(creer()));
            fenetre->getmod()->hide();
            fenetre->setModal(true);
            fenetre->exec();
            if(chgt==true){ //permet d'ajouter un compte à l a transaction
                nom_cptpropre = QInputDialog::getItem(this,"Choix du Compte", "Choisir un compte", comptea,0,  false);
                debit = QInputDialog::getDouble(this, "Debit", "Quel est le montant à débiter ?",0,-2147483647,2147483647,2);
                credit = QInputDialog::getDouble(this, "Credit", "Quel est le montant à créditer ?",0,-2147483647,2147483647,2);
                try{CompteAbstrait* cp=racine->getCompte(nom_cptpropre);
                    if(cp!=nullptr){
                        t=make_tuple(dynamic_cast<Compte*>(racine->getCompte(nom_cptpropre)),debit,credit);
                        v.push_back(t);
                     }
                    else{
                        QMessageBox message;
                        message.setText("Compte inexistant");
                        message.exec();
                    }
                }catch(TresorerieException te){QMessageBox::warning(this, "Erreur", te.getInfo());}
                chgt=false;
            }
        if(Cre==false){ // cette partie s'active si on decide de créer la transaction
            try{ // on essaye de créer la transaction mais on est pas à l'abri d'une erreur de saisie d'ou l'utilisation d'un bloc try
            TransactionApplication::getTransactionApplication().creerTransaction(v,Date,ref,titre,TransactionApplication::getTransactionApplication().usenb());
            refresh(dernierCompte[dernierCompte.size()-1]); // on raffraichit la qtable widget
            }
        catch(TresorerieException t){ // si une erreur est interceptée on va retourner dans la boucle avec CONT, mais on modifie d'abord la transaction
            QMessageBox::warning(this, "Erreur", t.getInfo());
            QMessageBox::warning(this, "Info", "Cliquez sur un des compte pour le supprimer, editer ses montants ou ajouter un nouveau compte puis appuuyez sur modifier pour confirmer");
            MaFenetreEnCours* fenetre=new MaFenetreEnCours; // on crée une nouvelle fenetre qui va permettre de modifier la transaction
            fenetre->setPere(this);
            fenetre->setWindowTitle("Transactions "+titre+" "+ref);
            fenetre->getLab()->setText("Transactions: "+titre+"\tReference: "+ref+"\tDate: "+Date.toString("dd/MM/yyyy"));
            auto itb=v.begin();
            auto ite=v.end();
            for(;itb!=ite;itb++) // on parcourt la transaction
            {
                fenetre->getvue()->insertRow(fenetre->getvue()->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem((std::get<0>(*itb))->getNom());
                QTableWidgetItem *item1 = new QTableWidgetItem(QString::number((std::get<0>(*itb))->getNumero()));
                QTableWidgetItem *item2=new QTableWidgetItem(QString::number(std::get<1>(*itb)));
                QTableWidgetItem *item3=new QTableWidgetItem(QString::number(std::get<2>(*itb)));
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 0, item1);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 1, item);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 2, item2);
                fenetre->getvue()->setItem(fenetre->getvue()->rowCount()-1, 3, item3);
            }
            fenetre->getCont()->hide(); // on invisibilise les boutons qui n'ont pas d'interet ici
            fenetre->getArr()->hide();// on invisibilise les boutons qui n'ont pas d'interet ici
            fenetre->getCre()->hide();// on invisibilise les boutons qui n'ont pas d'interet ici
            QObject::connect(fenetre->getmod(), SIGNAL(clicked()), fenetre, SLOT(modifier())); // on connecte le bouton au slot qui permet de modifier la transaction
            QObject::connect(fenetre->getvue(), SIGNAL(cellClicked(int,int)), fenetre, SLOT(modif_cpt(int,int)));
            fenetre->setModal(true);
            fenetre->exec();
            }
        }
      }
     }
    else{
        QMessageBox message;
        message.setText("Votre fichier de compte est vide, vous ne pouvez pas créer de transaction");
        message.exec();
    }
}

/* cette méthode permet de raffraichir la qtablewidget de la fenetre principale de l'application lorsque l'on se déplace dans l'arborescence des comptes  */
void MaFenetreCompte::AvancerHierarchie(int i,int j){
    dernier_type = Type::M;
    QTableWidgetItem* a = vueComptes->item(i,1); // on sélectionne le compte sur lequel on a cliqué
    QString b = a->text();
    CompteAbstrait* c=racine->getCompte(b); // on récupère le compte
    int y = vueComptes->rowCount();
    if (dynamic_cast<CompteVirtuel*>(c)!=nullptr){
        dernierCompte.push_back(racine->getCompte(b)); // si le compte est bien virtuel, alors on le rajoute à la pile de compte qui permet de savoir ou l'on se situe dans l'arborescence
        for(int z(0);z!=y;z++){
        vueComptes->removeRow(0); // suppression de l'entierete de la table
        }
        CompteVirtuel* j = dynamic_cast<CompteVirtuel*>(c); // conversion effective du compteAbstrait* en compteVirtuel*
        QStringList* d = j->getnomlistdirect(); // obtention des noms se situant directement
        QString s;
        foreach(QString element, *d) // on parcourt la liste de nom et on raffraichit la qtablewidget
        {
          vueComptes->insertRow(vueComptes->rowCount());
          QTableWidgetItem *item = new QTableWidgetItem(element);
          CompteAbstrait * cpt = j->getCompte(element);
          QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
          vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
          if(dynamic_cast<Compte*>(cpt)!=nullptr){ // si le compte est concret on affiche son numero et son solde
          Compte* j2 = dynamic_cast<Compte*>(cpt);
          QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
          QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
          vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
          vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
          //penser à faire un if si
          }
          vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
        }
          delete(d); // suppresion du pointeur

          }
}

/* ce slot est utilise pour l'affichage de transaction, il permet l'affichage de toutes les transactions associés à un compte */
void MaFenetreCompte::AfficherTrans(CompteAbstrait* cp){
    MaFenetreTrans* fenetre=new MaFenetreTrans;
    fenetre->setWindowTitle("Transactions "+cp->getNom());
    fenetre->getlab()->setText("Transactions "+cp->getNom());
    fenetre->setPere(this);
    fenetre->setCompte(cp);
    fenetre->refresh();
    fenetre->setModal(true);
    fenetre->exec();
}

/* ce slot est utilise pour l'affichage des comptes par rapport à leur type lors de l'appuie sur le bouton Mode par Type*/
void MaFenetreCompte::affichage_ensemble(){
    MaFenetreType* nouv=new MaFenetreType;
    nouv->setWindowTitle("Affichage par type de Compte");
    nouv->setModal(true);
    nouv->exec();
}

/* ce slot est utilise pour la procédure de cloture d'un compte*/
void MaFenetreCompte::clotureLivre(){
    try{QStringList depenses;
    racine->liste_type(depenses,false,true,Type::D);
    QStringList recettes;
    racine->liste_type(recettes,false,true,Type::R);
    if (depenses.size()!=0 || recettes.size()!=0){ // si on a aucun compte recette et depense on met un message car la cloture ne sert à rien, on aurait pu la permettre mais on aurait eu des soldes à 0
        int question=QMessageBox::question(this,"Confirmation","Etes vous sur de vouloir cloturer le livre ?", QMessageBox::Yes | QMessageBox::No);
        if (question==QMessageBox::Yes)
            CompteApplication::getCompteApplication().ClotureLivre(); // si l'utilisateur confirme on lance la cloture
        refresh(dynamic_cast<CompteAbstrait*>(racine)); // on raffraichit la fenetre principale  pour l'utilisateur
    }
    else{
        QMessageBox message;
        message.setText("Vous ne pouvez pas cloturer le livre si vous n'avez pas de comptes recettes ou dépenses");
        message.exec();
    }}catch(TresorerieException t){QMessageBox::warning(this, "Erreur", t.getInfo());}
}

/* ce slot est utilise pour générer le bilan*/
void MaFenetreCompte::genereBilan(){
    QMessageBox message; // on demande la date limite de la création du bilan, on génère le bilan depuis la création du fichier jusqu'à la date rentrée
    message.setText("Veuillez choisir une date limite pour la génération du bilan");
    message.exec();
    FenDate* nouvelledate = new FenDate;
    nouvelledate->setWindowTitle("Choisir une date pour générer le bilan");
    nouvelledate->setModal(true);
    nouvelledate->exec();
    QDate date = nouvelledate->selectedDate();
    QString reponse;
    QWidget *window = new QWidget;
    window->setWindowTitle("Fenetre bilan");
    reponse=racine->genererBilan(date); // on récupère la génération du bilan
    QLabel *label = new QLabel(reponse); // on place la génération du bilan dans un label que l'on va afficher par la suite grâce à window
    QHBoxLayout* layout_bilan=new QHBoxLayout(window);
    layout_bilan->addWidget(label);
    window->setLayout(layout_bilan);
    racine->genererBilanWeb(date); // on génère le bilan web
    window->show();

}

/* ce slot est utilise pour générer le résultat*/
void MaFenetreCompte::genereCompteResultat(){
    QMessageBox message1; // on choisit la période de génération du compte résultat
    message1.setText("Veuillez choisir une date à partir de laquelle vous voulez faire la balance des recettes et des dépenses");
    message1.exec();
    FenDate* nouvelledate1 = new FenDate;
    nouvelledate1->setWindowTitle("Choisir une date");
    nouvelledate1->setModal(true);
    nouvelledate1->exec();
    QDate date_d = nouvelledate1->selectedDate();
    QMessageBox message2;
    message2.setText("Veuillez choisir une date jusqu'à laquelle vous voulez faire la balance des recettes et des dépenses");
    message2.exec();
    FenDate* nouvelledate2 = new FenDate;
    nouvelledate2->setWindowTitle("Choisir une date");
    nouvelledate2->setModal(true);
    nouvelledate2->exec();
    QDate date_f = nouvelledate2->selectedDate();
    QString reponse;
    reponse=racine->genererResultat(date_d,date_f); // on génère le résultat et on le met dans un label
    QWidget *window = new QWidget;
    window->setWindowTitle("Fenetre compte de résultat");
    QLabel *label = new QLabel(reponse);// on place le résultat dans un label et on l'affiche à l'aide de la fenetre window
    QHBoxLayout* layout_cr=new QHBoxLayout(window);
    layout_cr->addWidget(label);
    window->setLayout(layout_cr);
    racine->genererResultatWeb(date_d,date_f); // on génère le résultat web
    window->show();
}

/* Constructeur de MaFenetreDeplace */
MaFenetreDeplace::MaFenetreDeplace(QDialog *parent) : QDialog(parent)
{
    valider = new QPushButton("Déplacer", this);
    QObject::connect(valider, SIGNAL(clicked()), this, SLOT(Deplacement()));

    refuser = new QPushButton("Refuser", this);
    QObject::connect(refuser, SIGNAL(clicked()), this, SLOT(close()));

    retour = new QPushButton("Retour", this);
    QObject::connect(retour, SIGNAL(clicked()), this, SLOT(retour_arriere()));

   compte_layout=new QHBoxLayout;
   compte_layout->addWidget(retour);
   compte_layout->addStretch();


   couche=new QVBoxLayout ;
   couche->addLayout(compte_layout);
   compte =new QLabel("Veuillez choisir un nouvel emplacement :");
   couche->addWidget(compte);


   vueComptes=new QTableWidget(0,4,this);
   QStringList HHeaderList;
   HHeaderList<<"Numero"<<"Nom"<<"Solde"<<"Type";
   vueComptes->setHorizontalHeaderLabels(HHeaderList);
   vueComptes->setFixedWidth(420);
   vueComptes->setFixedHeight(300);
   vueComptes->setColumnWidth(0,100);
   vueComptes->setEditTriggers(QAbstractItemView::NoEditTriggers);
   couche->addWidget(vueComptes);
   QObject::connect(vueComptes, SIGNAL(cellClicked(int,int)), this, SLOT(AvancerHierarchie(int,int)));
   couche->addWidget(valider);
   couche->addWidget(refuser);
   setLayout(couche);
}

/* ce slot est utilisé pour réagir à l'appuie du bouton déplacer, afin de déplacer un compte dans la hierarchie*/
void MaFenetreDeplace::Deplacement()
{
    CompteApplication::getCompteApplication().deplace(concerne,dernierCompte[dernierCompte.size()-1]); // ici on lance le déplacement du compte, à l'intérieur du compte virtuel ou l'on est situé dans la QTableWidget
    pere->refresh(pere->getDernierCompte()[pere->getDernierCompte().size()-1]); // on refresh MaFenetreCompte pour mettre à jour la table
    close();
}

/* cette méthode initialise MaFenetreDeplace permettant le déplacement d'un compte, avec les comptes fils du compte racine ayant le même type que le compte que l'on déplace*/
void MaFenetreDeplace::init(Type t)
{
    dernierCompte.push_back(racine);
    CompteVirtuel* cv=dynamic_cast<CompteVirtuel*>(racine);
    QStringList* obj=cv->getnomlistdirect(true,t);// on met dans la QStringList seulement les comptes fils du compte racine du même type que le compte que l'on veut déplacer
    obj->removeOne(concerne->getNom()); // on enlève le compte que l'on veut déplacer de la liste
    foreach(QString element, *obj) // on initialise la QTableWidget avec les comptes fils du compte racine contenu dans la QStringList
     {
       vueComptes->insertRow(vueComptes->rowCount());
       QTableWidgetItem *item = new QTableWidgetItem(element);
       CompteAbstrait * cpt = cv->getCompte(element);
       QTableWidgetItem *item1 = new QTableWidgetItem(converType(concerne->getType()));
       vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
       if(dynamic_cast<Compte*>(cpt)!=nullptr){
       Compte* j2 = dynamic_cast<Compte*>(cpt);
       QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));
       QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
       vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
       vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
             //penser à faire un if si
     }
       vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
     }
}

/* Ce slot permet de raffraichir l'affichage de la qtablewidget, c'est elle qui permet le parcours de l'arborescence des comptes
 à travers le parcours de la qtablewidget */

void MaFenetreDeplace::AvancerHierarchie(int i,int j){
    QTableWidgetItem* a = vueComptes->item(i,1); // selection du nom du compte
    QString b = a->text();
    CompteAbstrait* c=racine->getCompte(b);
    int y = vueComptes->rowCount();
    if (dynamic_cast<CompteVirtuel*>(c)!=nullptr){ // on vérifie que le compte sélectionné est concret
        dernierCompte.push_back(racine->getCompte(b)); // si il est concret on l'ajoute sur le haut de la pile (la fin de la liste de comptes effectivement visitée)
        for(int z(0);z!=y;z++){
        vueComptes->removeRow(0); //on supprime l'entiereté de la qtablewidget
        }
        CompteVirtuel* j = dynamic_cast<CompteVirtuel*>(c); // on convertit le compteAbstrait* en comptevirtuel
        QStringList* d = j->getnomlistdirect(true,actuel); // on récupère les comptes qui se trouvent directement apres j dans l'arborescence.
        d->removeOne(concerne->getNom());
        QString s;
        foreach(QString element, *d) // on parcourt la liste des noms de compte
        {
          vueComptes->insertRow(vueComptes->rowCount());
          QTableWidgetItem *item = new QTableWidgetItem(element);
          CompteAbstrait * cpt = j->getCompte(element);
          QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
          vueComptes->setItem(vueComptes->rowCount()-1, 3, item1); // affichage du type du compte
          if(dynamic_cast<Compte*>(cpt)!=nullptr){ // si le compte est un compte concret alors on va afficher son solde et son numero de compte
          Compte* j2 = dynamic_cast<Compte*>(cpt); // on convertit le compteAbstrait* en Compte*
          QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
          QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
          vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
          vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
          //penser à faire un if si
          }
          vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
        }
          delete(d);

          }
}

/* ce slot permet de se deplacer en arriere dans la qtablewidget, de remonter l'arborescence de comptes, lorsque l'on deplace un compte*/
void MaFenetreDeplace::retour_arriere(){
    if (dernier_type==Type::M){
        if(dernierCompte.size() == 1){
            refresh(racine); // si la pile de compte est égale à 1 et que le type est M, c'est forcement la racine.
        }
        else{
            /*si le type du dernier compte est M mais que la taille de la pile n'est pas égale à 1, on supprime le dernier compte de la pile (qui est la racine) et on raffraichit sur l'avant denrier compte  */
            dernierCompte.pop_back();
            refresh(dernierCompte[dernierCompte.size()-1]);
        }
    }
    else{
        int j = vueComptes->rowCount();
        for(int x(0);x!=j;x++){
            vueComptes->removeRow(0);
        } // on supprime le contenu de la QTablewidget
         QStringList aff_ensemble;
         // on recupere les noms de compte du dernier type qui se situent sous la racine et on réactualise.
         racine->liste_type(aff_ensemble,true,true,dernier_type);
         aff_ensemble.removeOne(concerne->getNom());
         foreach(QString element, aff_ensemble)
         {
                vueComptes->insertRow(vueComptes->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem(element);
                CompteAbstrait * cpt = racine->getCompte(element);
                QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
                vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
                if(dynamic_cast<Compte*>(cpt)!=nullptr){
                Compte* j2 = dynamic_cast<Compte*>(cpt);
                QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
                QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
                vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
                vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
                 //penser à faire un if si
                }
                vueComptes->setItem(vueComptes->rowCount()-1, 1, item);


            }
            dernier_type = Type::M;
        }
}

/* cette méthode permet de raffraichir la qtablewidget lorsque l'on se déplace en arrière dans la hierarchie */
void MaFenetreDeplace::refresh(CompteAbstrait* cp) //
{
    int j = vueComptes->rowCount();
    for(int x(0);x!=j;x++){
        vueComptes->removeRow(0); // on supprime l'entiereté de la qtablewidget
    }
    CompteVirtuel* cv=dynamic_cast<CompteVirtuel*>(cp);
    QStringList* obj=cv->getnomlistdirect(true,actuel); // on recupere les comptes qui se situent juste sous le compte considéré
    obj->removeOne(concerne->getNom()); // on n'affiche pas le comtpe lui meme mais seulement ses descendants directs
    foreach(QString element, *obj)
     {
       vueComptes->insertRow(vueComptes->rowCount());
       QTableWidgetItem *item = new QTableWidgetItem(element);
       CompteAbstrait * cpt = cv->getCompte(element);
       QTableWidgetItem *item1 = new QTableWidgetItem(converType(cpt->getType()));
       vueComptes->setItem(vueComptes->rowCount()-1, 3, item1);
       if(dynamic_cast<Compte*>(cpt)!=nullptr){ // si le compte est concret alors on affiche son solde et son numero
       Compte* j2 = dynamic_cast<Compte*>(cpt);
       QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));//penser à faire un if pour verifier si c'est pas un ensemble
       QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
       vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
       vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
             //penser à faire un if si
     }
       vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
     }
}

/* Constructeur de MaFenetreType*/
MaFenetreType::MaFenetreType(QDialog *parent): QDialog(parent)
{
   type =new QLabel("Type du compte");

   ligne=new QHBoxLayout;

   listetype=new QComboBox(this);
   listetype->setFixedWidth(200);

   listetype->addItem("Actif");
   listetype->addItem("Passif");
   listetype->addItem("Depenses");
   listetype->addItem("Recettes"); // on affiche les types de compte

   QObject::connect(listetype,SIGNAL(currentIndexChanged(int)),this,SLOT(affichage(int)));

   ligne->addWidget(type);
   ligne->addWidget(listetype);

   col=new QVBoxLayout;
   col->addLayout(ligne);
   vueComptes=new QTableWidget(0,3,this);
   vueComptes->verticalHeader()->setVisible(false);
   QStringList HHeaderList;
   HHeaderList<<"N° Compte"<<"Nom"<<"Solde";
   vueComptes->setHorizontalHeaderLabels(HHeaderList);
   vueComptes->setFixedWidth(304);
   vueComptes->setFixedHeight(300);
   vueComptes->setEditTriggers(QAbstractItemView::NoEditTriggers);

   col->addWidget(vueComptes);
   QObject::connect(vueComptes, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(menu(int,int)));
   this->setLayout(col);
}

/* definition de la fenetre permettant l'affichage par type de compte */
void MaFenetreType::affichage(int i){
    int j = vueComptes->rowCount();
    for(int x(0);x!=j;x++){
        vueComptes->removeRow(0);
    } // suppression du contenu de la qtablewidget
    QString ttype=listetype->itemText(i); // on recupere le type de compte selectionne dans la qcombobox
    Type a ;
    if (i ==0) a = Type::A;
    else if(i==1) a= Type::P;
    else if(i==2) a= Type::D;
    else a= Type::R;

        QStringList aff_ensemble;
        CompteApplication::getCompteApplication().liste_type(aff_ensemble,false,true,a); // on recupere la liste de compte qui correspond au type selectionné
        foreach(QString element, aff_ensemble) // affichage des comptes filtrés
        {
            vueComptes->insertRow(vueComptes->rowCount());
            QTableWidgetItem *item = new QTableWidgetItem(element);
            CompteAbstrait * cpt = CompteApplication::getCompteApplication().getCompte(element);
            if(dynamic_cast<Compte*>(cpt)!=nullptr){ // si le compte est concret affichage du numero et du solde
            Compte* j2 = dynamic_cast<Compte*>(cpt);
            QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(j2->getNumero()));
            QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(j2->getSolde()));
            vueComptes->setItem(vueComptes->rowCount()-1, 0, item2);
            vueComptes->setItem(vueComptes->rowCount()-1, 2, item3);
            }
            vueComptes->setItem(vueComptes->rowCount()-1, 1, item);
        }
}

/* slot permettant la creation d'une fenetre lors du double click sur la QTableWidget de MaFenetreType afin d'afficher le chemin absolu du compte ! */

void MaFenetreType::menu(int i,int j){
    QTableWidgetItem* a = vueComptes->item(i,1);
    QString b = a->text();
    QList<QString> liste;
    CompteAbstrait* cp=CompteApplication::getCompteApplication().getCompte(b); // on get le compte sur lequel on double clique
    while(cp!=nullptr) // permet de rajouter le nom des comptes parents dans la QList
    {
        liste.append(cp->getNom());
        cp=cp->getPere();
    }
    QString reponse;
    for(QList<QString>::reverse_iterator it=liste.rbegin();it!=liste.rend();it++){ // on parcourt la liste à l'envers afin de reconstituer le chemin hierarchique du compte sur lequel on a double cliqué, ceci revient à afficher le chemin absolu d'un fichier sur un ordinateur par exemple
        if(it!=liste.rbegin())
            reponse+="/";
        reponse+=*it;
    }
    QDialog *window = new QDialog(); // on crée une fenetre avec un lable dessus, affichant le chemin absolu du compte dans la hierarchie
    window->setWindowTitle("Chemin du fichier");
    window->resize(300,50);
    QLabel *label = new QLabel(reponse);
    QHBoxLayout* layout_cr=new QHBoxLayout(window);
    layout_cr->addWidget(label);
    window->setLayout(layout_cr);
    window->setModal(true);
    window->exec();
}

