/**	Autor: �ngel Luis L�pez Monterroso
* Implementaci�n clase LecturaFichero
* Intenta abrir un fichero y se asocia a �l
* Lleva la idea de Iterador de Java, pudiendo tokenizar el fichero en lineas, y las lineas en tokens.
   Cuando Acaba una linea pone una bandera indicando el fin de la linea, igual con el fin de fichero,�til para ser comprobado en bucles.
* Se destruye s�lo
*/
#include "LecturaFichero.h"
//###LecturaFichero::LecturaFichero(string directorioActual = ""){
LecturaFichero::LecturaFichero ( string directorioActual )
{
  existeDirectorio = false;
  ficheroFinalizado = false;
  directorioInicial = ".";
  if ( !directorioActual.empty ( ))
  {
    directorioInicial = directorioActual;
  }
  dp = opendir ( directorioInicial.c_str ( ));
  if ( dp != NULL )
  {
    existeDirectorio = true;
  }
}

LecturaFichero::~LecturaFichero ( )
{
  cerrarFichero ( );
}

bool LecturaFichero::buscaFichero ( string coincidencia, bool buscaDirectorios, string rutaActual )
{
  DIR *dirTemp;
  bool retornado = false;
  ficheroAbierto = false;
  string miCoincidencia = coincidencia;
  transform ( miCoincidencia.begin ( ), miCoincidencia.end ( ), miCoincidencia.begin ( ), ::tolower );
  if ( existeDirectorioInicial ( ))
  {
    while (( dirp = readdir ( dp )) && !ficheroAbierto )
    {
      string rutaOriginal = directorioInicial + "/" + dirp->d_name;
      ruta = rutaOriginal;
      transform ( ruta.begin ( ), ruta.end ( ), ruta.begin ( ), ::tolower );
      size_t encontrado;
      encontrado = ruta.find ( coincidencia );
      if ( encontrado != string::npos )
      {
        fichero.open ( rutaOriginal.c_str ( ));
        nombreFichero = ruta; //TODO - TO-DO ESTO ESTA MAL
        ficheroAbierto = retornado = true;
      }
      else
      {
        dirTemp = opendir ( rutaOriginal.c_str ( ));
        if ( dirTemp != NULL && buscaDirectorios )
        {
          string miRuta = dirp->d_name;
          retornado = buscaFichero ( miCoincidencia, buscaDirectorios, miRuta + "/" );
        }
      }
    }
  }
  return retornado;
}

string LecturaFichero::siguienteLinea ( )
{
  if ( ficheroAbierto && !ficheroFinalizado )
  {
    std::getline ( fichero, lineaActual );
    if ( fichero.good ( ))
    {
      if ( fichero.eof ( ))
      {
        ficheroFinalizado = true;
      }
      return lineaActual;
    }
    else
    {
      ficheroFinalizado = true;
      lineaActual.clear ( );
      return "";
    }
  }
}

bool LecturaFichero::existeDirectorioInicial ( )
{
  return existeDirectorio;
}

bool LecturaFichero::cerrarFichero ( )
{
  if ( ficheroAbierto )
  {
    fichero.close ( );
    ficheroAbierto = false;
    existeDirectorio = false;
  }
  return !ficheroAbierto;
}

bool LecturaFichero::hayMasLineas ( )
{
  return !ficheroFinalizado;
}

bool LecturaFichero::hayMasTokens ( )
{
  return !lineaFinalizada;
}

bool LecturaFichero::preparaLineaTokenizer ( char token )
{
  lineaPreparada = false;
  lineaFinalizada = false;
  if ( hayMasLineas ( ))
  {
    iss = new istringstream ( lineaActual );
    this->token = token;
    lineaPreparada = true;
  }
  return lineaPreparada;
}

string LecturaFichero::tokenLinea ( )
{
  tokenActual = "";
  if ( hayMasTokens ( ) && lineaPreparada )
  {
    getline ( *iss, tokenActual, this->token );
    if ( iss->eof ( ))
    {
      lineaFinalizada = true;
    }
    return tokenActual;
  }
  return tokenActual;
}
