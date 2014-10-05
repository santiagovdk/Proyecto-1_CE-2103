#include "Facade.h"
#include "listafigura.h"

#include <QDebug>

Facade::Facade()
{
    _memoria=new listaFigura();
}

Facade::~Facade()
{
    delete _memoria;
}

bool Facade::recordar(int vertices)
{
    if(_memoria->buscarIndice(vertices)!=NULL){
        return true;
    }
    else{
        return false;
    }
}

void Facade::aprender(int vertices, string nombre)
{
   qDebug() << QString::fromStdString (nombre) ;
    figura *forma=new figura(vertices,nombre);
    _memoria->insertarFinal(forma);
}

void Facade::reaprender(int vertices, string nombre)
{
    _memoria->buscarIndice(vertices)->getFigura()->setNombre(nombre);
}

string Facade::nombreRecuerdo(int posicion)
{
    return _memoria->getPos(posicion)->getFigura()->getNombre();
}

int Facade::verticesRecuerdo(int posicion)
{
    return _memoria->getPos(posicion)->getFigura()->getVertices();
}

int Facade::cantidadRecuerdos()
{
    return _memoria->getTamanio();
}

void Facade::nuevaCombinacion()
{
    _combinaciones->insertarFinal(new combinaciones());
}

void Facade::agregarEnUltimaCombinacion(CvSeq *pResult, CvPoint *pPuntos)
{
    _combinaciones->getTail()->getCombinaciones()->insertarFigura(new figura(pResult->total,""));
    _combinaciones->getTail()->getCombinaciones()->getListaElementos().getTail()->getFigura()->setConfig(pResult);
    _combinaciones->getTail()->getCombinaciones()->getListaElementos().getTail()->getFigura()->setPuntos(pPuntos);
}

bool Facade::CombinacionCorrecta(int pPos)
{
    int posiciones[20];
    int cantidad=_combinaciones->getPos(pPos)->getCombinaciones()->getCantidad();
    int i=0,j=0;
    while(i<cantidad){
        int a=i;
        while(j<_listaTemporal->getTamanio()){
            if (_combinaciones->getPos(pPos)->getCombinaciones()->getListaElementos().getPos(i)->getFigura()->getVertices()==_listaTemporal->getPos(j)->getFigura()->getVertices()){
                i++;
                j=_listaTemporal->getTamanio();
                posiciones[i]=j;
            }
            else{
                j++;
            }

        }
        if(a==i){
            return false;
        }

    }

    for(int i=0; i<cantidad;i++){
        _listaTemporal->borrarPos(posiciones[i]);
    }
    return true;
}

int Facade::getTamanioCombinaciones()
{
    return _combinaciones->getTamanio();
}

listaFigura Facade::getListaTemporal()
{
    return *_listaTemporal;
}

void Facade::insertarListaTemporal(CvSeq *pResult)
{
    _listaTemporal->insertarFinal(new figura(pResult->total,""));
    _listaTemporal->getTail()->getFigura()->setConfig(pResult);

}
