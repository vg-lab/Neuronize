/**	Autor: �ngel Luis L�pez Monterroso
* Clase cabecera ASC2SWC.h
* Lee un fichero ASC y lo convierte a un SWC compatible y v�lido
* Uso singletone, s�lo a trav�s de el m�todo p�blico convierteASWC pasando la ruta donde esta el fichero y una coincidencia del fichero ASC
* Crea un fichero llamado RECONVERTIDO.swc en la misma ruta que el ASC
*/
#include "LecturaFichero.h"
#include "NodoParseado.h"
#include <stack>
#ifndef ASC2SWC_H
#define ASC2SWC_H
#define APICAL 4
#define BASAL 3
#define AXON 2
using namespace std;
class ASC2SWC
{
  private:
    int padre;
    int nodo;
    ofstream ficheroEscritura;
    LecturaFichero *leerFichero;
    ASC2SWC ( string ruta, string fileName );
    ~ASC2SWC ( );
    NodoParseado iniciaParseo ( );
    void rellenaSoma ( NodoParseado nodo );
    void comienzaAConvertir ( NodoParseado miNodoParseado );
    void rellenaDendritaApical ( NodoParseado nodo );
    void rellenaDendritaBasal ( NodoParseado nodo );
    void rellenaAxon ( NodoParseado nodo );
    void rellenaDendrita ( NodoParseado nodo, int comienzo = 2, int tipo = BASAL );
    void rellenaRama ( std::vector < NodoParseado * > nodo, int comienzo = 0, int padre = -1 );
    float dameX ( NodoParseado miNodo );
    float dameY ( NodoParseado miNodo );
    float dameZ ( NodoParseado miNodo );
    float dameR ( NodoParseado miNodo );
  public:
    static void convierteASWC ( string ruta, string ficheroSalida );
};
#endif