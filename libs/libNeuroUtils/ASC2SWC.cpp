/**	Autor: �ngel Luis L�pez Monterroso
* Implementaci�n clase ASC2SWC
* Lee un fichero ASC y lo convierte a un SWC compatible y v�lido
* Uso singletone, s�lo a trav�s de el m�todo p�blico convierteASWC pasando la ruta donde esta el fichero y una coincidencia del fichero ASC
* Crea un fichero llamado RECONVERTIDO.swc en la misma ruta que el ASC
*/
#include "ASC2SWC.h"
ASC2SWC::ASC2SWC ( string ruta, string fileName )
{
  leerFichero = new LecturaFichero ( ruta );
  leerFichero->buscaFichero ( fileName, true, "" );
}
void ASC2SWC::convierteASWC ( string ruta, string ficheroSalida )
{
  ASC2SWC miParser ( ruta, ficheroSalida );
  NodoParseado miNodoParseado = miParser.iniciaParseo ( );
  //miParser.ficheroEscritura = ofstream(ruta+"/"+"RECONVERTIDO.swc");
  miParser.ficheroEscritura = ofstream ( ruta + "/" + ficheroSalida + ".swc" );
  miParser.ficheroEscritura << "#" << endl;
  miParser.comienzaAConvertir ( miNodoParseado );
  miParser.ficheroEscritura.close ( );
}
ASC2SWC::~ASC2SWC ( )
{
  leerFichero->~LecturaFichero ( );
}

NodoParseado ASC2SWC::iniciaParseo ( )
{
  NodoParseado raiz;                      //Primer elemento, es el fichero en s�.
  string lineaActual;
  raiz.entraEnDerecha = false;
  std::stack < NodoParseado * > listaActual;            //Pila donde se almacenar�n las listas.
  listaActual.push ( &raiz );                  //Se introduce la raiz en la lista.
  int inicioPalabraActual = -1;                //Si es -1, no hay palabra, sino, hay palabra
  bool heEntradoEnLista =
    false;                //Comprueba si sale de lista, y si hay algo m�s antes de acabar linea, lo guarda en la lista anterior
  NodoParseado *lista;                    //Lista de la que se ha salido anteriormente
  while ( leerFichero->hayMasLineas ( ))
  {              //Parsea el fichero l�nea por l�nea
    int i;
    lineaActual = leerFichero->siguienteLinea ( );      //La l�nea actual
    for ( i = 0; i < lineaActual.length ( ); i++ )
    {          //Va car�cter por car�cter
      char caracterActual = lineaActual[i];        //El car�cter actual
      if ( caracterActual == '(' )
      {              //Si abre par�ntesis, entra un nivel
        if ( inicioPalabraActual != -1 )
        {          //Hab�a algo antes del par�ntesis, se agrega como palabra
          NodoParseado *miElemento = ( listaActual.top ( ));
          miElemento->elementosLista.push_back ( lineaActual.substr ( inicioPalabraActual, i - inicioPalabraActual ));
          inicioPalabraActual = -1;                //Se busca una nueva palabra
        }
        NodoParseado *nuevalista = new NodoParseado ( );      //Se insertar� en la lista que hay dentro del elemento
        nuevalista->entraEnDerecha = false;
        NodoParseado *listaSuperior = listaActual.top ( );    //Se coge la lista que hay en la pila
        if ( !listaSuperior->entraEnDerecha )
        {            //Cuando hay un | se detecta una bifurcaci�n, pasa a la rama derecha
          listaSuperior->ramaIzquierda
                       .push_back ( nuevalista );  //Se introduce la lista en la lista de listas del elemento nodo
        }
        else
        {
          listaSuperior->ramaDerecha
                       .push_back ( nuevalista );  //Se introduce la lista en la lista de listas del elemento nodo
        }

        listaActual.push ( nuevalista );              //Se introduce esta lista en la pila para crear el �mbito
      }
      else if ( caracterActual == ')' )
      {                //Se sale de la lista actual
        if ( inicioPalabraActual != -1 )
        {          //Hab�a algo antes del par�ntesis, se agrega como palabra
          NodoParseado *miElemento = ( listaActual.top ( ));
          miElemento->elementosLista.push_back ( lineaActual.substr ( inicioPalabraActual, i - inicioPalabraActual ));
          inicioPalabraActual = -1;              //Se busca una nueva palabra
        }
        heEntradoEnLista = true;              //Se sale de lista, por lo que vemos si hay algo m�s
        lista = listaActual.top ( );              //Se coge la lista
        listaActual.pop ( );                  //Se borra la lista de la pila
      }
      else if ( caracterActual == ';' )
      {              //Comentario
        if ( inicioPalabraActual != -1 )
        {            //Hab�a algo antes del par�ntesis, se agrega como palabra
          NodoParseado *miElemento = ( listaActual.top ( ));
          miElemento->elementosLista.push_back ( lineaActual.substr ( inicioPalabraActual, i - inicioPalabraActual ));
          inicioPalabraActual = -1;                //Se busca una nueva palabra
        }
        if ( heEntradoEnLista )
        {                  //Si acaba de salir de la lista anterior, el comentario era de esta.
          lista->comentario = lineaActual.substr ( i );      //Se almacena el comentario en la lista anterior
          heEntradoEnLista = false;              //Se indica que ya no hay que a�adir m�s en la lista anterior.
        }
        else
        {
          listaActual.top ( )->comentario = lineaActual.substr ( i );  //El comentario va a la linea anterior.
        }
        i = lineaActual.length ( );                //Salimos del bucle
        inicioPalabraActual = -1;                //Si hubiera palabras se indica que no hay m�s.
      }
      else if ( caracterActual == '|' )
      {                //Se encuentra con una ramificaci�n
        NodoParseado *listaSuperior = listaActual.top ( );
        if ( listaSuperior->elementosLista.size ( ) > 0 )
        {
          std::string tipo = listaSuperior->elementosLista.at ( listaSuperior->elementosLista.size ( ) - 1 );
          listaSuperior->tipoIzquierda = tipo;
          listaSuperior->elementosLista.pop_back ( );
        }
        listaSuperior->entraEnDerecha = true;
      }
      else if ( caracterActual != '"' && caracterActual != ',' )
      {    //Se ignoran comillas y comas, el resto si no estaba en comprobaciones anteriores entra aqu� como palabra.
        if ( caracterActual == ' ' )
        {                //Vemos si es una palabra nueva
          if ( inicioPalabraActual != -1 )
          {          //Hab�a algo antes del par�ntesis, se agrega como palabra
            NodoParseado *miElemento = ( listaActual.top ( ));
            miElemento->elementosLista.push_back ( lineaActual.substr ( inicioPalabraActual, i - inicioPalabraActual ));
            inicioPalabraActual = -1;                //Se busca una nueva palabra
          }
        }
        else
        {                          //Es una palabra
          if ( inicioPalabraActual == -1 )
          {            //Si a�n no se ha detectado palabra...
            inicioPalabraActual = i;            //Se coge el elemento en la l�nea.
          }
        }
      }

    }
    if ( inicioPalabraActual != -1 )
    {          //Hab�a algo antes del par�ntesis, se agrega como palabra
      NodoParseado *miElemento = ( listaActual.top ( ));
      miElemento->elementosLista.push_back ( lineaActual.substr ( inicioPalabraActual, i - inicioPalabraActual ));
    }
    inicioPalabraActual = -1;
    heEntradoEnLista = false;
  }
  return raiz;
}

void ASC2SWC::comienzaAConvertir ( NodoParseado miNodoParseado )
{
  this->padre = -1;
  this->nodo = 1;
  for ( int i = 0; i < miNodoParseado.ramaIzquierda.size ( ); i++ )
  {
    NodoParseado nodoActual = *miNodoParseado.ramaIzquierda.at ( i );
    string elementoEnRaiz = "";
    string elementoEnSegundoHijo = "";
    string elementoEnTercerHijo = "";
    string comentarioEnRaiz = "";
    NodoParseado segundoHijoRamaPrincipal =
      nodoActual.ramaIzquierda.size ( ) > 0 ? *nodoActual.ramaIzquierda.at ( 1 ) : *new NodoParseado ( );
    NodoParseado tercerHijoRamaPrincipal =
      nodoActual.ramaIzquierda.size ( ) > 0 ? *nodoActual.ramaIzquierda.at ( 2 ) : *new NodoParseado ( );
    elementoEnRaiz = nodoActual.elementosLista.size ( ) > 0 ? nodoActual.elementosLista.at ( 0 ) : "";
    elementoEnSegundoHijo =
      segundoHijoRamaPrincipal.elementosLista.size ( ) > 0 ? segundoHijoRamaPrincipal.elementosLista.at ( 0 ) : "";
    elementoEnTercerHijo =
      tercerHijoRamaPrincipal.elementosLista.size ( ) > 0 ? tercerHijoRamaPrincipal.elementosLista.at ( 0 ) : "";
    comentarioEnRaiz = nodoActual.comentario;
    if ( nodoActual.ramaIzquierda.size ( ) > 0 && !nodoActual.entraEnDerecha
      && ( elementoEnRaiz == "CellBody" || elementoEnSegundoHijo == "CellBody" )
      && comentarioEnRaiz == ";  End of contour" )
    {//Es el soma
      this->rellenaSoma ( nodoActual );
    }
    else if ( nodoActual.ramaIzquierda.size ( ) > 0
      && ( elementoEnRaiz == "Dendrite" || elementoEnSegundoHijo == "Dendrite" || elementoEnTercerHijo == "Dendrite" )
      && comentarioEnRaiz == ";  End of tree" )
    {//Es una dendrita basal
      this->rellenaDendritaBasal ( nodoActual );
    }
    else if ( nodoActual.ramaIzquierda.size ( ) > 0
      && ( elementoEnRaiz == "Apical" || elementoEnSegundoHijo == "Apical" || elementoEnTercerHijo == "Apical" )
      && comentarioEnRaiz == ";  End of tree" )
    {//Es una dendrita apical
      this->rellenaDendritaApical ( nodoActual );
    }
    else if ( nodoActual.ramaIzquierda.size ( ) > 0
      && ( elementoEnRaiz == "Axon" || elementoEnSegundoHijo == "Axon" || elementoEnTercerHijo == "Axon" ))
    {  //Es un ax�n
      this->rellenaAxon ( nodoActual );
    }
    else
    {//Se mete en la neurona en otrosDatos, es probable que se metan axones, espinas...
    }
  }
}

void ASC2SWC::rellenaSoma ( NodoParseado nodo )
{
  NodoParseado miNodo = *nodo.ramaIzquierda.at ( 0 );
  int i = miNodo.elementosLista.at ( 0 ) == "Color" ? 2 : 1;//S�lo tenemos el tipo, que es un cellbody, pasamos de �l.
  for ( int j = i; j < nodo.ramaIzquierda.size ( ); j++ )
  {
    NodoParseado miNodo = *nodo.ramaIzquierda.at ( j );
    float x = atof ( miNodo.elementosLista.at ( 0 ).c_str ( ));
    float y = atof ( miNodo.elementosLista.at ( 1 ).c_str ( ));
    float z = atof ( miNodo.elementosLista.at ( 2 ).c_str ( ));
    float d = atof ( miNodo.elementosLista.at ( 3 ).c_str ( ));
    this->ficheroEscritura << " " << this->nodo << " 1 " << x << " " << y << " " << z << " " << d/2 << " "
                           << this->padre << endl;
    this->padre = this->nodo;
    this->nodo++;
  }
}

void ASC2SWC::rellenaDendritaApical ( NodoParseado nodo )
{
  int i = 2;
  if ( nodo.ramaIzquierda.size ( ) > 0 && ( nodo.ramaIzquierda.at ( 0 )->elementosLista.size ( ) > 0
    && nodo.ramaIzquierda.at ( 0 )->elementosLista.at ( 0 ) == "Color" ))
  {
    i = 3;
  }
  return rellenaDendrita ( nodo, i, APICAL );
}

void ASC2SWC::rellenaDendritaBasal ( NodoParseado nodo )
{
  int i = 1;
  if ( nodo.ramaIzquierda.size ( ) > 0 && ( nodo.ramaIzquierda.at ( 0 )->elementosLista.size ( ) > 0
    && nodo.ramaIzquierda.at ( 0 )->elementosLista.at ( 0 ) == "Color" ))
  {
    i = 2;
  }
  return rellenaDendrita ( nodo, i, BASAL );
}

void ASC2SWC::rellenaAxon ( NodoParseado nodo )
{
  int i = 1;
  if ( nodo.ramaIzquierda.size ( ) > 0 && ( nodo.ramaIzquierda.at ( 0 )->elementosLista.size ( ) > 0
    && nodo.ramaIzquierda.at ( 0 )->elementosLista.at ( 0 ) == "Color" ))
  {
    i = 2;
  }
  return rellenaDendrita ( nodo, i, AXON );
}

void ASC2SWC::rellenaDendrita ( NodoParseado nodo, int i, int tipo )
{
  int padre = -1;
  padre = this->padre;
  this->ficheroEscritura << " " << this->nodo << " " << tipo << " " << this->dameX ( *nodo.ramaIzquierda.at ( i ))
                         << " " << this->dameY ( *nodo.ramaIzquierda.at ( i )) << " "
                         << this->dameZ ( *nodo.ramaIzquierda.at ( i )) << " "
                         << this->dameR ( *nodo.ramaIzquierda.at ( i ))/2 << " " << this->padre << endl;
  this->padre = this->nodo;
  this->nodo++;
  this->rellenaRama ( nodo.ramaIzquierda, i + 1, tipo );
  if ( nodo.entraEnDerecha )
  {
    this->ficheroEscritura << " " << this->nodo << " " << tipo << " " << this->dameX ( *nodo.ramaIzquierda.at ( i ))
                           << " " << this->dameY ( *nodo.ramaIzquierda.at ( i )) << " "
                           << this->dameZ ( *nodo.ramaIzquierda.at ( i )) << " "
                           << this->dameR ( *nodo.ramaIzquierda.at ( i ))/2 << " " << this->padre << endl;
    this->padre = this->nodo;
    this->nodo++;
    this->rellenaRama ( nodo.ramaDerecha, i + 1, tipo );
  }
  this->padre = padre;
}

void ASC2SWC::rellenaRama ( std::vector < NodoParseado * > nodo, int comienzo, int tipo )
{
  if ( nodo.size ( ) > comienzo )
  {
    int padre = -1;
    NodoParseado miNodo = *nodo.at ( comienzo );
    if ( nodo.at ( comienzo )->entraEnDerecha )
    {
      padre = this->padre;
      this->rellenaRama ( miNodo.ramaIzquierda, 0, tipo );
      this->padre = padre;
      this->rellenaRama ( miNodo.ramaDerecha, 0, tipo );
      this->padre = padre;
    }
    else
    {
      if ( miNodo.elementosLista.size ( ) < 3 )
      {
        rellenaRama ( miNodo.ramaIzquierda, 0, tipo );
      }
      else
      {
        this->ficheroEscritura << " " << this->nodo << " " << tipo << " " << this->dameX ( miNodo ) << " "
                               << this->dameY ( miNodo ) << " " << this->dameZ ( miNodo ) << " "
                               << this->dameR ( miNodo )/2 << " " << this->padre << endl;
        this->padre = this->nodo;
        this->nodo++;
      }
    }
    rellenaRama ( nodo, comienzo + 1, tipo );
  }
}
float ASC2SWC::dameX ( NodoParseado miNodo )
{
  return atof ( miNodo.elementosLista.at ( 0 ).c_str ( ));
}
float ASC2SWC::dameY ( NodoParseado miNodo )
{
  return atof ( miNodo.elementosLista.at ( 1 ).c_str ( ));
}
float ASC2SWC::dameZ ( NodoParseado miNodo )
{
  return atof ( miNodo.elementosLista.at ( 2 ).c_str ( ));
}
float ASC2SWC::dameR ( NodoParseado miNodo )
{
  return atof ( miNodo.elementosLista.at ( 3 ).c_str ( ));
}

