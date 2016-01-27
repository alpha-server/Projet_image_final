#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Mini Photoshop");
    w.setWindowIcon(QIcon("/home/b/baillema/Desktop/alpha-server-Projet_image_final/logo.png"));
    w.show();

    return a.exec();
}
