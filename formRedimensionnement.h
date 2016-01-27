#ifndef FORMREDIMENSIONNEMENT_H
#define FORMREDIMENSIONNEMENT_H
#include<QDialog>
#include<QWidget>
#include<QFormLayout>
#include<QLineEdit>
#include<QVBoxLayout>
#include<QPushButton>
#include<QHBoxLayout>
#include<QSpinBox>
#include<QImage>

class formRedimensionnement : public QDialog
{
    Q_OBJECT

public:
    formRedimensionnement();
    ~formRedimensionnement();
    int getLargeur();
    int getHauteur();
    QPushButton* getButtonOK();

private slots:
     void quitterFormulaire(int exec);

signals:
    void recupererValeur(int valeurHauteur, int valeurLargeur);

private:
    QVBoxLayout* layout;
    QHBoxLayout* bouttonLayout;
    QFormLayout* formulaire;
    QSpinBox* largeur;
    QSpinBox* hauteur;
    QPushButton* ok;
    QPushButton* annuler;

    int valeurLargeur, valeurHauteur;//Valeurs du formulaire
};

#endif // FORMREDIMENSIONNEMENT_H
