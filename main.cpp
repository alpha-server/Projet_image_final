#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int largeur = QApplication::desktop()->width();
    int hauteur = QApplication::desktop()->height();

    MainWindow w;
    w.setWindowTitle("Mini Photoshop");
    w.setWindowIcon(QIcon("/home/b/baillema/Desktop/alpha-server-Projet_image_final/logo.png"));
    w.setFixedSize(largeur, hauteur);
    w.show();

    return a.exec();
}
