 #include"traiterImage.h"
#include<iostream>
using namespace std;

//Constructeur par défaut
TraiterImage::TraiterImage() : m_Image(NULL)
{
    cout<<"+Constructeur par défaut"<<endl;
    m_width = m_heigth = 0;
}

//Constructeur prennant la taille de l'image en argument
TraiterImage::TraiterImage(int w, int h) : m_Image(NULL)
{
    m_width = w;
    m_heigth = h;
    Allouer();
}

//Destructeur
TraiterImage::~TraiterImage()
{
    Liberer();
}

//Allocation d'une image
void TraiterImage::Allouer()
{
    if (m_Image!=NULL)
    {
        return;
    }

    m_Image = new byte*[m_heigth];
    m_normex = new PIXEL*[m_heigth];
    m_normey = new PIXEL*[m_heigth];
    m_norme = new PIXEL*[m_heigth];
    m_buffer = new byte*[m_heigth];

    for(int i = 0; i<m_heigth ; i++)
    {
        m_Image[i] = new byte[m_width];
        m_normex[i] = new PIXEL[m_width];
        m_normey[i] = new PIXEL[m_width];
        m_norme[i] = new PIXEL[m_width];
        m_buffer[i] = new byte[m_width];
    }

    return;
}

//Desallocation d'une image
void TraiterImage::Liberer()
{
    for(int i=0; i<m_heigth; i++)
    {
        delete[]m_Image;
        delete[]m_normex[i];
        delete[]m_normey[i];
        delete[]m_norme[i];
        delete[]m_buffer[i];
    }

    delete[]m_Image;
    delete[]m_normex;
    delete[]m_normey;
    delete[]m_norme;
    delete[]m_buffer;

    return;
}
