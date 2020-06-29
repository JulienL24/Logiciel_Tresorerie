#include <QApplication>
#include "compte.h"
#include "fenetre_compte.h"
#include <QDate>
#include <QtDebug>
#include <QtWidgets>
#include <QDialog>

/**
\mainpage Documentation projet Tresorerie

\section Introduction
Le but de ce projet est de réaliser un logiciel de gestion de trésorerie pour une association, en utilisant\n
les principes de comptabilité en partie double. L’utilisateur doit pouvoir définir différents comptes en\n
fonction de ses besoins, saisir et corriger des transactions, et générer différents documents relatifs à sa\n
comptabilité (bilan et comptes de résultat).\n

\section Documentation

Dans ce projet, vous trouverez plusieurs fichiers:\n
- Le main\n
- Les fichiers de compte\n
- Les fichiers de transactions\n
- Les fichiers d'interface\n
**/

int main (int argc, char *argv[])
{   QApplication app(argc, argv);
    MaFenetreCompte fenetre;
    fenetre.show();
    try{
        int question;
        question=QMessageBox::question(&fenetre,"Paramètre de base","Voulez-vous utilisez les fichiers par défaut?", QMessageBox::Yes | QMessageBox::No);
        if(question==QMessageBox::No){
            QMessageBox message1;
                    message1.setText("Choisissez un fichier de compte à charger");
                    message1.exec();
                    QString filename1= QFileDialog::getOpenFileName(&fenetre,"Choisir un fichier de compte",QDir::currentPath(),"Text files (*.txt)");
                    QString filename2;
                    while(filename1.isNull()){
                        question=QMessageBox::question(&fenetre,"Confirmation","Si vous ne choisissez pas de fichier, l'application va se quitter! \nEtes vous sur de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);
                        if (question==QMessageBox::Yes){
                            CompteApplication::getCompteApplication().load(CompteApplication::getCompteApplication().getFilename());
                            TransactionApplication::getTransactionApplication().load(TransactionApplication::getTransactionApplication().getFilename());
                            return 0;
                         }
                        else
                            filename1= QFileDialog::getOpenFileName(&fenetre,"Choisir un fichier de compte",QDir::currentPath(),"Text files (*.txt)");
                    }
                    QMessageBox message2;
                    message2.setText("Choisissez le fichier de transaction correspondant à charger");
                    message2.exec();
                    filename2 = QFileDialog::getOpenFileName(&fenetre,"Choisir un fichier de transaction",QDir::currentPath(),"Text files (*.txt)");
                    while(filename2.isNull()){
                        question=QMessageBox::question(&fenetre,"Confirmation"," Si vous ne choisissez pas de fichier, l'application va se quitter! \nEtes vous sur de vouloir quitter ?", QMessageBox::Yes | QMessageBox::No);
                        if (question==QMessageBox::Yes){
                            CompteApplication::getCompteApplication().load(CompteApplication::getCompteApplication().getFilename());
                            TransactionApplication::getTransactionApplication().load(TransactionApplication::getTransactionApplication().getFilename());
                            return 0;
                           }
                        else
                            filename2= QFileDialog::getOpenFileName(&fenetre,"Choisir un fichier de transaction",QDir::currentPath(),"Text files (*.txt)");
                    }
                    CompteApplication& ca=CompteApplication::getCompteApplication();
                    TransactionApplication& ta=TransactionApplication::getTransactionApplication();
                    ca.load(filename1);
                    ta.load(filename2);
                    fenetre.setRacine(ca);
                    fenetre.refresh(&fenetre.getRacine());
                }
            else{
                CompteApplication& ca=CompteApplication::getCompteApplication();
                TransactionApplication& ta=TransactionApplication::getTransactionApplication();
                ca.load(ca.getFilename());
                ta.load(ta.getFilename());
                fenetre.reset();
            }
            }
            catch(TresorerieException t){
                                        CompteApplication& ca1=CompteApplication::getCompteApplication();
                                        TransactionApplication& ta1=TransactionApplication::getTransactionApplication();
                                        QMessageBox::warning(&fenetre,"Erreur",t.getInfo());
                                         try{
                                         ca1.setFilename();ta1.setFilename();ca1.load(ca1.getFilename());
                                          }catch(TresorerieException t){QMessageBox::warning(&fenetre,"Erreur",t.getInfo());}
                                          try{
                                         ta1.load(ta1.getFilename());
                                           }catch(TresorerieException t){QMessageBox::warning(&fenetre,"Erreur",t.getInfo());}
                                         fenetre.setRacine(ca1);
                                         QMessageBox message;
                                         message.setText("Les fichiers de base "+ca1.getFilename()+" et "+ta1.getFilename()+" ont été chargés");
                                         message.exec();
                                         fenetre.reset();};

        return app.exec();
}
