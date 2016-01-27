#include"formRedimensionnement.h"
#include<QFormLayout>
#include<QLineEdit>
#include<QVBoxLayout>
#include<QPushButton>
#include<QApplication>

formRedimensionnement::formRedimensionnement()
{
    largeur = new QSpinBox(this);
    largeur->setMinimum(0);
    largeur->setMaximum(1900);

    hauteur = new QSpinBox(this);
    hauteur->setMinimum(0);
    hauteur->setMaximum(1200);

    ok = new QPushButton("OK", this);
    annuler = new QPushButton("Annuler", this);

    QFormLayout*formLayout = new QFormLayout;
    formLayout->addRow("&Largeur", largeur);
    formLayout->addRow("&Hauteur", hauteur);

    bouttonLayout = new QHBoxLayout;
    bouttonLayout->addWidget(ok);
    bouttonLayout->addWidget(annuler);

    connect(annuler, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(this, SIGNAL(finished(int)), this, SLOT(quitterFormulaire(int)));

    //connect(this, SIGNAL(recupererValeur(valeurHauteur,valeurLargeur)), this, SLOT(quitterFormulaire()));
    layout = new QVBoxLayout;
    layout->addLayout(formLayout);
    layout->addLayout(bouttonLayout);

    resize(350,200);
    setLayout(layout);
    setWindowTitle("Fenêtre de Redimensionnement");
    this->show();
}

formRedimensionnement::~formRedimensionnement()
{
    delete layout;
    delete bouttonLayout;
    delete formulaire;
    delete largeur;
    delete hauteur;
    delete ok;
    delete annuler;

}

int formRedimensionnement::getHauteur()
{
     return this->valeurHauteur;

}

int formRedimensionnement::getLargeur()
{
    return this->valeurLargeur;
}



void formRedimensionnement::quitterFormulaire(int exec)
{
    if ( exec == QDialog::Accepted)
    {
        emit recupererValeur(hauteur->value(), largeur->value());
    }
}


QPushButton* formRedimensionnement::getButtonOK()
{
    return ok;
}
