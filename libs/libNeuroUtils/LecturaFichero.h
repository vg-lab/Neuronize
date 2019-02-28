/**	Autor: �ngel Luis L�pez Monterroso
* Clase cabecera LecturaFichero.h
* Intenta abrir un fichero y se asocia a �l
* Lleva la idea de Iterador de Java, pudiendo tokenizar el fichero en lineas, y las lineas en tokens.
   Cuando Acaba una linea pone una bandera indicando el fin de la linea, igual con el fin de fichero,�til para ser comprobado en bucles.
* Se destruye s�lo
*/
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <sstream>
#ifndef LECTURA_FICHERO_H
#define LECTURA_FICHERO_H
using namespace std;
class LecturaFichero
{
  private:
    string directorioInicial;
    boost::filesystem::path dp;
    struct dirent *dirp;
    bool existeDirectorio;
    string ruta;
    string nombreFichero;
    ifstream fichero;
    bool ficheroAbierto;
    bool ficheroFinalizado;
    string tokenSeparacion;
    string lineaActual;
    istringstream *iss;
    char token;
    string tokenActual;
    bool lineaPreparada;
    bool lineaFinalizada;
  public:
    LecturaFichero ( string directorioActual );
    ~LecturaFichero ( );
    bool buscaFichero ( string coincidencia, bool buscaDirectorios = true, string rutaActual = "" );
    string siguienteLinea ( );
    bool existeDirectorioInicial ( );
    bool cerrarFichero ( );
    bool hayMasLineas ( );
    string tokenLinea ( );
    bool hayMasTokens ( );
    bool preparaLineaTokenizer ( char token = ' ' );
};
#endif