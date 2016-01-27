#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QImage>
#include<QLabel>
#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<opencv2/video/background_segm.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<QMouseEvent>
#include<QRubberBand>
using namespace cv;
using namespace std;


namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


protected slots:
    //Les slots
    void ouvrir();  //ouvrir l'image
    void enregistrer();  //enregistrer l'image
    void about();
    void fermer();  //fermer l'image
    void noir_et_blanc(); //mettre une image en noir et blanc
    void inversion();
    void flou();
    void contraste();
    void cropper();
    void contoursobel();
    void contourprewitt();
    void creer_fenetre_redimension();
    void initialiser();//revenir à l'image de départ
    void redimensionner(int largeur, int hauteur);
    void afficher_histogramme();

private:
    QImage  histogramme_yuv(QImage image);
    QPixmap Mat2QPixmap(const Mat&);  //convertir un Mat en QPixmap
    QImage  cropImage(QRect);//découpaer l'image
     void afficher_histogramme_rgb(Mat); //afficher l'histogramme en couleur
  //  void afficher_histogramme();      //Afficher les histogrammes de ccouleurs de l'image
    void afficher_histogramme_yuv(Mat);
    vector<Mat> histogramme(Mat&);  //Créer l'histogramme de couleurs de l'image
    void creer_connexions();       //créer les connexions
    void loadImage();             //charger l'image
    void creer_shortcut();        //creer les raccourcis
    QPixmap IPlImage2QImage(const IplImage*);  //Convertir une IplImage en QImage
    cv::Mat QImage2Mat(QImage&);  //convertir QImage en Mat
    QString fileName; //fichier pour ouverture de l'image
    QImage image1;  //image a ouvrir
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    QMouseEvent*event;
    QRubberBand*rubberBand;  //découper l'image
    QPoint myPoint;         //Point pour connaitre la position de la souris


    Ui::MainWindow *ui;//fenêtre principale
};

#endif // MAINWINDOW_H
