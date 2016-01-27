#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"formRedimensionnement.h"
#include<math.h>
#include<QtGui>
#include<QMessageBox>
#include<QFileDialog>
#include<QFile>
#include<QFileInfo>
#include<QSize>
#include<QImage>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<opencv2/video/background_segm.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<iostream>

using namespace cv;
using namespace std;

//Constructeur pour la fenetre
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    creer_shortcut();
    creer_connexions();
}

//Destructeur de la fenêtre
MainWindow::~MainWindow()
{
    delete ui;
}

//Creation des fonctions de raccourci clavier
void MainWindow::creer_shortcut()
{
    ui->actionOuvrir->setShortcut(tr("Ctrl+O"));
    ui->actionEnregistrer->setShortcut(tr("Ctrl+S"));
    ui->actionFermer->setShortcut(tr("Ctrl+Q"));
    ui->actionDecoupage->setShortcut(tr("Ctrl+D"));
    ui->actionRedimension->setShortcut(tr("Ctrl+R"));
}

//Connection des trigger avec des fonctions
void MainWindow::creer_connexions()
{
    QObject::connect(ui->actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
    QObject::connect(ui->actionFermer, SIGNAL(triggered()), this, SLOT(fermer()));
    QObject::connect(ui->actionEnregistrer, SIGNAL(triggered()), this, SLOT(enregistrer()));
    QObject::connect(ui->actionNoir_et_blanc, SIGNAL(triggered()), this, SLOT(noir_et_blanc()));
    QObject::connect(ui->actionInversion, SIGNAL(triggered()), this, SLOT(inversion()));
    QObject::connect(ui->actionFlou, SIGNAL(triggered()), this, SLOT(flou()));
    QObject::connect(ui->actionContraste, SIGNAL(triggered()), this, SLOT(contraste()));
    //QObject::connect(ui->bouttonCrop, SIGNAL(clicked()), this, SLOT(cropper()));
    QObject::connect(ui->actionContour_Prewitt,SIGNAL(triggered()),this,SLOT(contourprewitt()));
    QObject::connect(ui->actionContour_Sobel,SIGNAL(triggered()),this,SLOT(contoursobel()));
    QObject::connect(ui->actionRedimension, SIGNAL(triggered()), this, SLOT(creer_fenetre_redimension()));
    QObjectCleanupHandler::connect(ui->actionDecoupage, SIGNAL(triggered()), this, SLOT(cropper()));
    QObject::connect(ui->actionInitialiser, SIGNAL(triggered()), this, SLOT(initialiser()));
    QObject::connect(ui->action_Propos, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionHistogramme, SIGNAL(triggered()), this, SLOT(afficher_histogramme()));
    QObject::connect(ui->actionFusion_milieu, SIGNAL(triggered()), this, SLOT(fusionmilieu()));
    QObject::connect(ui->actionContour, SIGNAL(triggered()), this, SLOT(contourPerso()));
    QObject::connect(ui->actionRepoussage, SIGNAL(triggered()), this, SLOT(repoussage()));

}

//Chargement d'une image en fonction de sa taille
void MainWindow::loadImage()
{
    QSize size(ui->label_image->width(), ui->label_image->height());
    QImage image2 = image1.scaled(size, Qt::KeepAspectRatio);
    ui->label_image->setPixmap(QPixmap::fromImage(image2));
}

//Ouverture d'une image
void MainWindow::ouvrir()
{
    fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.png *.gif *.jpg *.jpeg)");
     if(fileName!=NULL)
     {
         QImage image(fileName);
         image1 = image;

         if (image1.isNull())
         {
             QMessageBox::information(this, "MainWindow", tr("Ne peut pas être chargé").arg(fileName));
             return;
         }

         //int fact = image1.depth()/8;
         //traiterImage = new TraiterImage(image1.height(), fact*image1.width());

         this->loadImage();
    }
    else
    {
        QMessageBox::information(this, "Fichier", "Vous n'avez rien sélectionné\n");
    }

}

//Enregistrement d'une image
void MainWindow::enregistrer()
{
    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer le fichier", fileName, "Images (*.png *.gif *.jpg *.jpeg)");
    QFile file(fichier);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }


    QImage imageSave = ui->label_image->pixmap()->toImage();
    QString extension = QFileInfo(fichier).suffix();
    imageSave.save(fichier);
}

void MainWindow::initialiser()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }
    this->loadImage();
    ui->label_histo1->show();
    ui->label_histo2->show();
    ui->label_histo3->show();
    afficher_histogramme_rgb(QImage2Mat(image1));

}

//Detection des contours via matrice de convolution
void MainWindow::contourPerso()
{
    QImage imgCpy = image1;

    for(int i = 1; i<imgCpy.width()-1;i++)
    {
        for(int j = 1; j<imgCpy.height()-1;j++)
        {
            //Case centrale
            QRgb rgb = image1.pixel(i,j);
            int rouge = qRed(rgb);
            int vert = qGreen(rgb);
            int bleu =  qBlue(rgb);

            //Case droite
            QRgb rgbD = image1.pixel(i+1,j);
            int rougeD = qRed(rgbD);
            int vertD = qGreen(rgbD);
            int bleuD =  qBlue(rgbD);

            //Case gauche
            QRgb rgbG = image1.pixel(i-1,j);
            int rougeG = qRed(rgbG);
            int vertG = qGreen(rgbG);
            int bleuG =  qBlue(rgbG);

            //Case haut
            QRgb rgbH = image1.pixel(i,j+1);
            int rougeH = qRed(rgbH);
            int vertH = qGreen(rgbH);
            int bleuH =  qBlue(rgbH);

            //Case bas
            QRgb rgbB = image1.pixel(i,j-1);
            int rougeB = qRed(rgbB);
            int vertB = qGreen(rgbB);
            int bleuB =  qBlue(rgbB);

            //Calcul du résultat
            int resR = ((-4*rouge+(rougeD+rougeG+rougeH+rougeB)<0) ? 0 : (-4*rouge+(rougeD+rougeG+rougeH+rougeB)));
            int resG = ((-4*vert+(vertD+vertG+vertH+vertB)<0) ? 0 : (-4*vert+(vertD+vertG+vertH+vertB)));
            int resB = ((-4*bleu+(bleuD+bleuG+bleuH+bleuB)<0) ? 0 : (-4*bleu+(bleuD+bleuG+bleuH+bleuB)));

            imgCpy.setPixel(i,j,qRgb(resR,resG,resB));
        }
    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
}

//Detection de contour par Prewitt
void MainWindow::contourprewitt()
{
    int result;
    QImage vertical = image1 ;
    QImage horizontal = image1 ;

    QImage composante = ui->label_image->pixmap()->toImage();

    printf("dans contour\n");

    //calcul de la composante horizontale
    for(int i = 1; i<horizontal.width()-1;i++)
    {
        for(int j = 1; j<horizontal.height()-1;j++)
        {
            //Case droite
            QRgb rgbD = ui->label_image->pixmap()->toImage().pixel(i+1,j);
            int greyD = (qRed(rgbD)+qBlue(rgbD)+qGreen(rgbD))/3;


            //Case gauche
            QRgb rgbG = ui->label_image->pixmap()->toImage().pixel(i-1,j);
            int greyG = (qRed(rgbG)+qBlue(rgbG)+qGreen(rgbG))/3;


            //case haut droit
            QRgb rgbHD = ui->label_image->pixmap()->toImage().pixel(i+1,j+1);
            int greyHD = (qRed(rgbHD)+qBlue(rgbHD)+qGreen(rgbHD))/3;

            //case haut gauche
            QRgb rgbHG = ui->label_image->pixmap()->toImage().pixel(i-1,j+1);
            int greyHG = (qRed(rgbHG)+qBlue(rgbHG)+qGreen(rgbHG))/3;

            //case bas droit
            QRgb rgbBD = ui->label_image->pixmap()->toImage().pixel(i+1,j-1);
            int greyBD = (qRed(rgbBD)+qBlue(rgbBD)+qGreen(rgbBD))/3;

            //case bas gauche
            QRgb rgbBG = ui->label_image->pixmap()->toImage().pixel(i-1,j-1);
            int greyBG = (qRed(rgbBG)+qBlue(rgbBG)+qGreen(rgbBG))/3;

            result = ( greyD + greyHD + greyBD )-( greyG + greyHG + greyBG);

            horizontal.setPixel(i,j,qRgb(result,result,result));
        }
    }

    //calcul de la composante vertical
    for(int i = 1; i<vertical.width()-1;i++)
    {
        for(int j = 1; j<vertical.height()-1;j++)
        {
            //Case haut
            QRgb rgbH = ui->label_image->pixmap()->toImage().pixel(i,j+1);
            int greyH = (qRed(rgbH)+qBlue(rgbH)+qGreen(rgbH))/3;


            //Case bas
            QRgb rgbB = ui->label_image->pixmap()->toImage().pixel(i,j-1);
            int greyB = (qRed(rgbB)+qBlue(rgbB)+qGreen(rgbB))/3;


            //case haut droit
            QRgb rgbHD = ui->label_image->pixmap()->toImage().pixel(i+1,j+1);
            int greyHD = (qRed(rgbHD)+qBlue(rgbHD)+qGreen(rgbHD))/3;

            //case haut gauche
            QRgb rgbHG = ui->label_image->pixmap()->toImage().pixel(i-1,j+1);
            int greyHG = (qRed(rgbHG)+qBlue(rgbHG)+qGreen(rgbHG))/3;

            //case bas droit
            QRgb rgbBD = ui->label_image->pixmap()->toImage().pixel(i+1,j-1);
            int greyBD = (qRed(rgbBD)+qBlue(rgbBD)+qGreen(rgbBD))/3;

            //case bas gauche
            QRgb rgbBG = ui->label_image->pixmap()->toImage().pixel(i-1,j-1);
            int greyBG = (qRed(rgbBG)+qBlue(rgbBG)+qGreen(rgbBG))/3;

            result = ( greyB  + greyBG + greyBD )-( greyH + greyHG + greyHD);

            vertical.setPixel(i,j,qRgb(result,result,result));
        }
    }

    //addition des deux composantes
    for(int i = 1; i<composante.width()-1;i++)
    {
        for(int j = 1; j<composante.height()-1;j++)
        {
            QRgb verti = vertical.pixel(i,j);
            QRgb hori = horizontal.pixel(i,j);
            result = ( sqrt(qRed(verti)*qRed(verti))+(qRed(hori)*qRed(hori)));
            composante.setPixel(i,j,qRgb(result,result,result));
        }

    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    composante = composante.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(composante));

}

//fusion des images par le milieu
void MainWindow::fusionmilieu()
{
    //copie de l'image de base
    QImage resultat = ui->label_image->pixmap()->toImage();
    QImage toto;
    //ouverture d'une seconde image
    fileName = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.png *.gif *.jpg *.jpeg)");
    if(fileName!=NULL)
    {
        QImage image2(fileName);
        toto=image2;
        if (image2.isNull())
        {
            QMessageBox::information(this, "MainWindow", tr("ne peut pas être chargé").arg(fileName));
            return;
        }
        // int fact = image1.depth()/8;
        //traiterImage = new TraiterImage(image1.height(), fact*image1.width());
        this->loadImage();

    }

    else{
        QMessageBox::information(this, "Fichier", "Vous n'avez rien sélectiooné\n");


    }

    //on verifie la taille des images pour savoir laquelle va dans l'autre
    if(resultat.width() < toto.width()) //on met image 1 dans image 2
    {
        for(int i = 1; i<resultat.width()-1;i++)
        {
            for(int j = 1; j<resultat.height()-1;j++)
            {
                QRgb rgb2 = resultat.pixel(i,j);
                QRgb rgb1 = toto.pixel(i,j);
                int red=(((1)*qRed(rgb1))+((1)*qRed(rgb2)))/2;
                int green=(((1)*qGreen(rgb1))+((1)*qGreen(rgb2)))/2;
                int blue=(((1)*qBlue(rgb1))+((1)*qBlue(rgb2)))/2;

                resultat.setPixel(i,j,qRgb(red,green,blue));

            }
        }
    }
    else//on met image2 dans image1
    {
        for(int i = 1; i<toto.width()-1;i++)
        {
            for(int j = 1; j<toto.height()-1;j++)
            {
                QRgb rgb1 = resultat.pixel(i,j);
                QRgb rgb2 = toto.pixel(i,j);

                int red=(((1)*qRed(rgb1))+((1)*qRed(rgb2)))/2;
                int green=(((1)*qGreen(rgb1))+((1)*qGreen(rgb2)))/2;
                int blue=(((1)*qBlue(rgb1))+((1)*qBlue(rgb2)))/2;

                resultat.setPixel(i,j,qRgb(red,green,blue));

            }
        }
    }


    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    resultat = resultat.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(resultat));
}

//Detection de contour par Sobel
void MainWindow::contoursobel()
{
    int result;
    QImage vertical = image1 ;
    QImage horizontal = image1 ;

    QImage composante = ui->label_image->pixmap()->toImage();

    printf("dans contour\n");

    //calcul de la composante horizontale
    for(int i = 1; i<horizontal.width()-1;i++)
    {
        for(int j = 1; j<horizontal.height()-1;j++)
        {
            //Case droite
            QRgb rgbD = ui->label_image->pixmap()->toImage().pixel(i+1,j);
            int greyD = (qRed(rgbD)+qBlue(rgbD)+qGreen(rgbD))/3;


            //Case gauche
            QRgb rgbG = ui->label_image->pixmap()->toImage().pixel(i-1,j);
            int greyG = (qRed(rgbG)+qBlue(rgbG)+qGreen(rgbG))/3;


            //case haut droit
            QRgb rgbHD = ui->label_image->pixmap()->toImage().pixel(i+1,j+1);
            int greyHD = (qRed(rgbHD)+qBlue(rgbHD)+qGreen(rgbHD))/3;

            //case haut gauche
            QRgb rgbHG = ui->label_image->pixmap()->toImage().pixel(i-1,j+1);
            int greyHG = (qRed(rgbHG)+qBlue(rgbHG)+qGreen(rgbHG))/3;

            //case bas droit
            QRgb rgbBD = ui->label_image->pixmap()->toImage().pixel(i+1,j-1);
            int greyBD = (qRed(rgbBD)+qBlue(rgbBD)+qGreen(rgbBD))/3;

            //case bas gauche
            QRgb rgbBG = ui->label_image->pixmap()->toImage().pixel(i-1,j-1);
            int greyBG = (qRed(rgbBG)+qBlue(rgbBG)+qGreen(rgbBG))/3;

            result = ( ( 2 * greyD ) + greyHD + greyBD )-( ( 2 * greyG ) + greyHG + greyBG);

            horizontal.setPixel(i,j,qRgb(result,result,result));
        }
    }

    //calcul de la composante vertical
    for(int i = 1; i<vertical.width()-1;i++)
    {
        for(int j = 1; j<vertical.height()-1;j++)
        {
            //Case haut
            QRgb rgbH = ui->label_image->pixmap()->toImage().pixel(i,j+1);
            int greyH = (qRed(rgbH)+qBlue(rgbH)+qGreen(rgbH))/3;


            //Case bas
            QRgb rgbB = ui->label_image->pixmap()->toImage().pixel(i,j-1);
            int greyB = (qRed(rgbB)+qBlue(rgbB)+qGreen(rgbB))/3;


            //case haut droit
            QRgb rgbHD = ui->label_image->pixmap()->toImage().pixel(i+1,j+1);
            int greyHD = (qRed(rgbHD)+qBlue(rgbHD)+qGreen(rgbHD))/3;

            //case haut gauche
            QRgb rgbHG = ui->label_image->pixmap()->toImage().pixel(i-1,j+1);
            int greyHG = (qRed(rgbHG)+qBlue(rgbHG)+qGreen(rgbHG))/3;

            //case bas droit
            QRgb rgbBD = ui->label_image->pixmap()->toImage().pixel(i+1,j-1);
            int greyBD = (qRed(rgbBD)+qBlue(rgbBD)+qGreen(rgbBD))/3;

            //case bas gauche
            QRgb rgbBG = ui->label_image->pixmap()->toImage().pixel(i-1,j-1);
            int greyBG = (qRed(rgbBG)+qBlue(rgbBG)+qGreen(rgbBG))/3;

            result = ( ( 2 * greyB ) + greyBG + greyBD )-( ( 2 * greyH ) + greyHG + greyHD);

            vertical.setPixel(i,j,qRgb(result,result,result));
        }
    }

    //addition des deux composantes
    for(int i = 1; i<composante.width()-1;i++)
    {
        for(int j = 1; j<composante.height()-1;j++)
        {

           QRgb verti = vertical.pixel(i,j);
           QRgb hori = horizontal.pixel(i,j);
           result = ( sqrt(qRed(verti)*qRed(verti)+(qRed(hori)*qRed(hori))));
           composante.setPixel(i,j,qRgb(result,result,result));
        }

    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    composante = composante.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(composante));

}

//Filtre noir et blanc
void MainWindow::noir_et_blanc()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }
    QImage imgCpy = ui->label_image->pixmap()->toImage();

    for(int i = 0; i<imgCpy.width(); i++)
    {
        for(int j = 0; j<imgCpy.height(); j++)
        {
            QRgb rgb = imgCpy.pixel(i,j);

            //Calcul par moyenne
            int res = (qGreen(rgb)+qRed(rgb)+qBlue(rgb))/3;

            //Calcul par maximun
            //int resM = max(max(qGreen(rgb),qRed(rgb)),qBlue(rgb));

            imgCpy.setPixel(i,j,qRgb(res,res,res));
        }
    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
    ui->label_histo1->hide();
    ui->label_histo2->hide();
    ui->label_histo3->hide();
}

//Filtre d'inversion
void MainWindow::inversion()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }
    QImage imgCpy = ui->label_image->pixmap()->toImage();

    imgCpy.invertPixels();

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_histo1->hide();
    ui->label_histo2->hide();
    ui->label_histo3->hide();
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
}

//Filtre de flou
void MainWindow::flou()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }
    QImage imgCpy = ui->label_image->pixmap()->toImage();

    for(int i = 1; i<imgCpy.width()-1;i++)
    {
        for(int j = 1; j<imgCpy.height()-1;j++)
        {
            QRgb gauche = imgCpy.pixel(i-1,j);
            QRgb droite = imgCpy.pixel(i+1,j);
            QRgb haut = imgCpy.pixel(i,j+1);
            QRgb bas = imgCpy.pixel(i,j-1);

            int resR = (qRed(haut)+qRed(bas)+qRed(gauche)+qRed(droite))/4;
            int resG = (qGreen(haut)+qGreen(bas)+qGreen(gauche)+qGreen(droite))/4;
            int resB = (qBlue(haut)+qBlue(bas)+qBlue(gauche)+qBlue(droite))/4;

            imgCpy.setPixel(i,j, qRgb(resR, resG, resB));
        }
    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_histo1->hide();
    ui->label_histo2->hide();
    ui->label_histo3->hide();
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
}

//Filtre de contraste (correspond à l'accentuation des contours)
void MainWindow::contraste()
{
    QImage imgCpy = ui->label_image->pixmap()->toImage();

    for(int i = 1; i<imgCpy.width()-1;i++)
    {
        for(int j = 1; j<imgCpy.height()-1;j++)
        {
            //Case centrale
            QRgb rgb = ui->label_image->pixmap()->toImage().pixel(i,j);

            int rouge = qRed(rgb);
            int vert = qGreen(rgb);
            int bleu =  qBlue(rgb);

            //Case droite
            QRgb rgbD = ui->label_image->pixmap()->toImage().pixel(i+1,j);
            int rougeD = qRed(rgbD);
            int vertD = qGreen(rgbD);
            int bleuD =  qBlue(rgbD);

            //Case gauche
            QRgb rgbG = ui->label_image->pixmap()->toImage().pixel(i-1,j);
            int rougeG = qRed(rgbG);
            int vertG = qGreen(rgbG);
            int bleuG =  qBlue(rgbG);

            //Case haut
            QRgb rgbH = ui->label_image->pixmap()->toImage().pixel(i,j+1);
            int rougeH = qRed(rgbH);
            int vertH = qGreen(rgbH);
            int bleuH =  qBlue(rgbH);

            //Case bas
            QRgb rgbB = ui->label_image->pixmap()->toImage().pixel(i,j-1);
            int rougeB = qRed(rgbB);
            int vertB = qGreen(rgbB);
            int bleuB =  qBlue(rgbB);

            //Calcul du résultat
            int resR = ((5*rouge-(rougeD+rougeG+rougeH+rougeB)>255) ? 255 : (5*rouge-(rougeD+rougeG+rougeH+rougeB)));
            int resG = ((5*vert-(vertD+vertG+vertH+vertB)>255) ? 255 : (5*vert-(vertD+vertG+vertH+vertB)));
            int resB = ((5*bleu-(bleuD+bleuG+bleuH+bleuB)>255) ? 255 : (5*bleu-(bleuD+bleuG+bleuH+bleuB)));

            imgCpy.setPixel(i,j,qRgb(resR,resG,resB));
        }
    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
}

void MainWindow::repoussage()
{
    QImage imgCpy = image1;

    for(int i = 1; i<imgCpy.width()-1;i++)
    {
        for(int j = 1; j<imgCpy.height()-1;j++)
        {
            //Case centrale
            QRgb rgb = image1.pixel(i,j);
            int rouge = qRed(rgb);
            int vert = qGreen(rgb);
            int bleu =  qBlue(rgb);

            //Case droite
            QRgb rgbD = image1.pixel(i+1,j);
            int rougeD = qRed(rgbD);
            int vertD = qGreen(rgbD);
            int bleuD =  qBlue(rgbD);

            //Case droite bas
            QRgb rgbDB = image1.pixel(i+1,j-1);
            int rougeDB = qRed(rgbDB);
            int vertDB = qGreen(rgbDB);
            int bleuDB =  qBlue(rgbDB);

            //Case gauche
            QRgb rgbG = image1.pixel(i-1,j);
            int rougeG = qRed(rgbG);
            int vertG = qGreen(rgbG);
            int bleuG =  qBlue(rgbG);

            //Case gauche haut
            QRgb rgbGH = image1.pixel(i-1,j+1);
            int rougeGH = qRed(rgbGH);
            int vertGH = qGreen(rgbGH);
            int bleuGH =  qBlue(rgbGH);

            //Case haut
            QRgb rgbH = image1.pixel(i,j+1);
            int rougeH = qRed(rgbH);
            int vertH = qGreen(rgbH);
            int bleuH =  qBlue(rgbH);

            //Case bas
            QRgb rgbB = image1.pixel(i,j-1);
            int rougeB = qRed(rgbB);
            int vertB = qGreen(rgbB);
            int bleuB =  qBlue(rgbB);

            //Calcul du résultat
            int resR = -2*rougeGH-rougeH-rougeG+rouge+rougeD+rougeB+rougeDB;
            int resG = -2*vertGH-vertH-vertG+vert+vertD+vertB+vertDB;
            int resB = -2*bleuGH-bleuH-bleuG+bleu+bleuD+bleuB+bleuDB;

            imgCpy.setPixel(i,j,qRgb(resR,resG,resB));
        }
    }

    //Adaptation à la taille de la fenetre
    QSize size(ui->label_image->width(), ui->label_image->height());
    imgCpy = imgCpy.scaled(size, Qt::KeepAspectRatio);

    //Affichage
    ui->label_image->setPixmap(QPixmap::fromImage(imgCpy));
}

void MainWindow::about()

{
    QMessageBox::about(this, "A propos de l'application",
                       tr("<p>Cette <b> application </b> a pour objectif de s'initier au traitement d'image en C++ avec Qt"  ));

}

void MainWindow::fermer()
{
    qApp->exit();
}

void MainWindow::afficher_histogramme_rgb(Mat src)
{
    vector<Mat>tab = histogramme(src);
    for(int i = 0; i<3; i++){
        if(i == 0)
        {
            QPixmap image_histo = Mat2QPixmap(tab[i]);
            QSize size(ui->label_histo1->width(), ui->label_histo1->height());
            QImage image2 = image_histo.toImage();
            image2.scaled(size, Qt::KeepAspectRatio);
            ui->label_histo1->setPixmap(QPixmap::fromImage(image2));
        }

        else
        {
            if(i == 1)
            {
                QPixmap image_histo = Mat2QPixmap(tab[i]);
                QSize size(ui->label_histo2->width(), ui->label_histo2->height());
                QImage image2 = image_histo.toImage();
                image2.scaled(size, Qt::KeepAspectRatio);
                ui->label_histo2->setPixmap(QPixmap::fromImage(image2));
            }

            else
            {
                QPixmap image_histo = Mat2QPixmap(tab[i]);
                QSize size(ui->label_histo2->width(), ui->label_histo2->height());
                QImage image2 = image_histo.toImage();
                image2.scaled(size, Qt::KeepAspectRatio);
                ui->label_histo3->setPixmap(QPixmap::fromImage(image2));
            }
        }
    }
}

cv::Mat MainWindow::QImage2Mat(QImage& img)
{
    cv::Mat tmp(img.height(),img.width(),CV_8UC3,(uchar*)img.bits(),img.bytesPerLine());
    cv::Mat result;
    cvtColor(tmp, result,CV_BGR2RGB);
    return result;

}

void MainWindow::afficher_histogramme()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return ;


    }

    afficher_histogramme_rgb(QImage2Mat(image1));//afficher les histogrammes en couleur
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    rubberBand->setGeometry(QRect(myPoint, event->pos()).normalized());
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{

    if(ui->label_image->underMouse())
    {
        myPoint = event->pos();
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(myPoint, QSize()));
        rubberBand->show();

    }


}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint p1 = mapToGlobal(myPoint);
    QPoint p2 = event->globalPos();
    //p1 = ui->label_image->mapFromGlobal(p1);
    //p2 = ui->label_image->mapFromGlobal(p2);
    double X = image1.width();
    double Y = image1.height();
    QPixmap originalQpix(*ui->label_image->pixmap());
    X = originalQpix.width() / X;
    Y = originalQpix.height() / Y;
    /*p1.setX(int(p1.x() * X));
   p1.setY(int(p1.y()* Y));

   p2.setX(int(p2.x() * X));
   p2.setY(int(p2.y() * Y));*/
    if(X-1 < p1.x()){
        p1.setX(int(X));
    }
    if(Y-1 < p1.y()){
        p2.setX(int(Y));
    }
    if(X-1 < p2.x()){
        p2.setX(int(X));
    }

    if(Y-1 < p2.y()){
        p2.setY(int(Y));
    }
    QRect rect(p1, p2);

    if(ui->actionDecoupage->isChecked())
    {
        rubberBand->hide();
        QImage image = cropImage(rect);



    }

}

void MainWindow::cropper()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }

    if(rubberBand != NULL && ui->label_image->pixmap() !=NULL)
    {
        rubberBand->hide();
        QRect myRect= rubberBand->rect();
        QImage myImage =  cropImage(myRect);
        Mat imageCrop = QImage2Mat(myImage);
        afficher_histogramme_rgb(imageCrop);
    }

}

QImage MainWindow::cropImage(QRect rect)
{
    if(ui->label_image->pixmap() != NULL){
        QPixmap oldPix(*ui->label_image->pixmap());
        QImage image = oldPix.toImage();
        QImage imageCopy = image.copy(rect);
        ui->label_image->setPixmap(QPixmap::fromImage(imageCopy));
        return imageCopy;
    }
    else{

        QMessageBox::warning(this, "Image", "Aucune image à cropper\n");

    }

}

QPixmap MainWindow::IPlImage2QImage(const IplImage *newImage)
{
    QPixmap monPixmap;
    QImage qtemp;
    if (newImage && cvGetSize(newImage).width>0)
    {
        int x;
        int y;
        char*data = newImage->imageData;
        qtemp = QImage(newImage->width, newImage->height, QImage::Format_RGB32);
        for(y = 0 ; y<newImage->height ; y++, data+=newImage->widthStep)
        {
            for(x = 0; x<newImage->width; x++)
            {
                uint*p = (uint*)qtemp.scanLine(y)+x;
                *p = qRgb(data[x*newImage->nChannels+2],data[x*newImage->nChannels+1],data[x*newImage->nChannels]);
            }
        }
    }

    monPixmap = QPixmap::fromImage(qtemp);

    return monPixmap;
}

vector<Mat> MainWindow::histogramme(Mat &img)
{
    int bins = 256;  //nombre de bacs
    int nc = img.channels();//nombre de channels

    vector<Mat>hist(nc);  //tableau d'histogrammes

    //initialiser l'histogramme
    for(int i = 0; i<(int)hist.size();i++)
    {
        hist[i] = Mat::zeros(1, bins, CV_32SC1);

    }

    for(int i = 0; i<img.rows; i++)
    {
        for(int j=0;j<img.cols;j++)
        {
            for(int k = 0; k<nc; k++)
            {
                uchar val;

                if(nc == 1)
                {
                    val = img.at<uchar>(i,j);
                }

                else
                {
                    val = img.at<Vec3b>(i,j)[k];
                }

                hist[k].at<int>(val) += 1;
            }
        }
    }

    int hmax[3] = {0,0,0};
    for(int i=0;i<nc;i++)
    {
        for(int j=0;j<bins-1;j++)
        {
            if( hist[i].at<int>(j) > hmax[i] )
            {

                hmax[i] = hist[i].at<int>(j);
            }
            //Wut ?
            else
            {

            }
        }
    }

    //Nouvel Ajout a modifier aprés
    //QLabel *myimage = new QLabel();
    // myimage->setBackgroundRole(QPalette::Dark);
    // myimage->setScaledContents(true);

    //const char*wname[3] = {"blue", "green", "red"};
    Scalar colors[3] = {Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
    vector<Mat> canavas(nc);
    for(int i =0; i<nc; i++)
    {
        canavas[i] = Mat::ones(125, bins, CV_8UC3);
        for(int j=0, rows = canavas[i].rows; j<bins-1; j++ )
        {
            Scalar s;
            if(nc == 1)
                s = Scalar(200,200,200);
            else
                s = colors[i];

            line(canavas[i], Point(j,rows), Point(j,rows-(hist[i].at<int>(j)*rows/hmax[i])), s, 1, 8, 0); //nc = 1  ? Scalar(200,200,200): colors[i],
        }
        // imshow(nc ==1 ? "value" : wname[i], canavas[i]);}
        //  QLabel label(ui->Tab1);
        //label.setPixmap(Mat2QPixmap(canavas[i]));

        //QPixmap pix = Mat2QPixmap(canavas[i]);
        //  int fact1 = pix.depth()/8;
        //  traiterImage = new TraiterImage(pix.height(), fact1*pix.width() );
        //      myimage->setPixmap(pix);
        //    ui->verticalLayout->addWidget(myimage);
        //load image histogramme
    }

    return canavas;
}

QPixmap MainWindow::Mat2QPixmap(const Mat &mat)
{
    Mat rgb;
    QPixmap p;
    cvtColor(mat, rgb, CV_BGR2RGB);
    p.convertFromImage(QImage((const unsigned char*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888));
    return p;
}

void MainWindow::creer_fenetre_redimension()
{
    if(image1.isNull())
    {
        QMessageBox::information(this, "MainWindow", "Veillez chargez une image,"
                                 "pour charger une image, aller dans le menu fichier->ouvrir,"
                                 "sélectionner une image ou faites Ctrl+O");
        return;
    }
    formRedimensionnement *formulaire = new formRedimensionnement;

    connect(formulaire, SIGNAL(recupererValeur(int,int)), this, SLOT(redimensionner(int,int)));



}

void MainWindow::redimensionner(int largeur, int hauteur)
{
    QImage image =  image1.scaled(largeur, hauteur, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

QImage MainWindow::histogramme_yuv(QImage image)
{
    QImage image_yuv;
    for(int i = 0; i<image.width(); i++)
    {
        for(int j = 0; j<image.height(); j++)
        {
            QRgb rgb = image.pixel(i,j);
            int y = (0.3*qRed(rgb) +0.6*qGreen(rgb) + 0.1*qBlue(rgb));
            y = (y>255 ? 255 : y);
            int u =(-0.3*qRed(rgb) -0.6*qGreen(rgb) + 0.9*qBlue(rgb));
            u = (u>255 ? 255 : u);
            int v =(0.7*qRed(rgb) - 0.6*qGreen(rgb) - 0.1*qBlue(rgb));
            y = (v>255 ? 255 : u);

            image_yuv.setPixel(i,j,qRgb(y,u,v));

        }
    }

    return image_yuv;
}

void MainWindow::afficher_histogramme_yuv(Mat src)
{
    vector<Mat>tab = histogramme(src);
    for(int i = 0; i<3; i++){
        if(i == 0)
        {
            QPixmap image_histo = Mat2QPixmap(tab[i]);


            QSize size(ui->label_histoY->width(), ui->label_histoY->height());
            QImage image2 = image_histo.toImage();
            image2.scaled(size, Qt::KeepAspectRatio);
            ui->label_histoY->setPixmap(QPixmap::fromImage(image2));

        }
        else{
            if(i == 1)
            {
                QPixmap image_histo = Mat2QPixmap(tab[i]);
                QSize size(ui->label_histoU->width(), ui->label_histoU->height());
                QImage image2 = image_histo.toImage();
                image2.scaled(size, Qt::KeepAspectRatio);
                ui->label_histoU->setPixmap(QPixmap::fromImage(image2));

            }
            else
            {
                QPixmap image_histo = Mat2QPixmap(tab[i]);
                QSize size(ui->label_histoV->width(), ui->label_histoV->height());
                QImage image2 = image_histo.toImage();
                image2.scaled(size, Qt::KeepAspectRatio);
                ui->label_histoV->setPixmap(QPixmap::fromImage(image2));

            }
        }





    }

}
