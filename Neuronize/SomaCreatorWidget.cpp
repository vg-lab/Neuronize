/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "SomaCreatorWidget.h"
#include "CompareMeshesWidget.h"
#include "RepairWidget.h"
#include "neuronize.h"
#include "WizzardInputOne.h"
#include "NeuronSetDialog.h"

#include <string>
#include <fstream>
#include <cstdio>

#include <QList>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

#include <libs/libNeuroUtils/ASC2SWCV2.h>
#include <QtWidgets/QInputDialog>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QToolTip>

using namespace std;

SomaCreatorWidget::SomaCreatorWidget (const QString &tempDir, QWidget *parent )
//: QMainWindow(parent, flags)
  : QWidget ( parent )
{
  ui.setupUi ( this );

  somaContours = false;

  //---mXMLSomaDefTree		=	NULL;
  mXMLSomaDefManager = NULL;

  mSWCImporter = NULL;
  mBaseMesh = NULL;
  neuron = nullptr;
    ASCparser = nullptr;

  mDefaultVidsFileName = "GeoVertsIdsDendrite_";
  mDefaultGeoDistFileName = "GeoDistDendrite_";
  mFullPathSWCFileName = mSWCFileName = "";

  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera3Subdiv1Radio.off";
  //mMehsFileName =    QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera4Subdiv1Radio.off";
  mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera5Subdiv1Radio.off";
  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera6Subdiv1Radio.off";

  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/TETIcoSphera4Subdiv1Radio.off";
  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/Icosphere.1.off";

  mExitDirectory = tempDir;

  QObject::connect ( ui.pushButton_generateFile, SIGNAL( clicked ( )), this, SLOT( generateXMLContent ( )) );
  QObject::connect ( ui.pushButton_LoadXML, SIGNAL( clicked ( )), this, SLOT( loadXMLContent ( )) );

  QObject::connect ( ui.pushButton_LoadSWCFileName, SIGNAL( clicked ( )), this, SLOT( setSWCFile ( )) );
  QObject::connect ( ui.pushButton_LoadModelFileName, SIGNAL( clicked ( )), this, SLOT( loadModelName ( )) );

  QObject::connect ( ui.pushButton_generateNearestVertexDendritic,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( calcAndExportNearestVertexToSWCDendritics ( )) );
  QObject::connect ( ui.pushButton_generateVertexIdToDendDeform,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( calcAndExportIdVertexToDendriticsDeformation ( )) );

  QObject::connect ( ui.checkBox_loadSWC, SIGNAL( clicked ( )), this, SLOT( loadSWCFile ( )) );
  QObject::connect ( ui.checkBox_loadModel, SIGNAL( clicked ( )), this, SLOT( loadModel ( )) );

  QObject::connect ( ui.pushButton_VertexIdsHumbraFile,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( loadGeodesicDendriticDistanceFile ( )) );

  QObject::connect ( ui.spinBox_VertexIdsHumbraDendriticSelector,
                     SIGNAL( valueChanged ( int )),
                     this,
                     SLOT( setGeoDistanceFromSWCImporter ( )) );

  QObject::connect ( ui.pushButton_LoadMorphologyFile, SIGNAL( clicked ( )), this, SLOT( loadSWCFile ( )) );
  QObject::connect ( ui.pushButton_LoadInitialMesh, SIGNAL( clicked ( )), this, SLOT( loadModel ( )) );
  QObject::connect ( ui.pushButton_ExitDirectory, SIGNAL( clicked ( )), this, SLOT( setExitDirectory ( )) );
    //QObject::connect ( ui.pushButton_LoadSWCFile, SIGNAL( clicked ( )), this, SLOT( generateXMLSoma () ));
  //connect(ui.pushButton_LoadSWCFile, &QPushButton::clicked,this, [=]() {generateXMLSoma ( QString("if6 cing porta 1 capa3 cel11 bis.ASC")); });
    //QObject::connect ( ui.pushButton_GoToSomaDeformer, SIGNAL( clicked ( )), this, SIGNAL( somaCreated ( )) );


    connect(ui.generateSetPushButton,&QPushButton::released,this,&SomaCreatorWidget::onGenerateNeurons);
    connect(ui.generateOnePushButton,&QPushButton::released,this,&SomaCreatorWidget::onGenerateOneNeuron);

  ui.tabWidget_Main->setVisible ( false );

  resetInterface ( );
}

SomaCreatorWidget::~SomaCreatorWidget ( )
{
  if ( mXMLSomaDefManager != NULL )
    delete mXMLSomaDefManager;

  if ( mSWCImporter != NULL )
    delete mSWCImporter;
  if ( mBaseMesh != NULL )
    delete mBaseMesh;
}

void SomaCreatorWidget::setSWCFile ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "SWC File(*.swc)" ));

  if ( !fileName.isNull ( ))
  {
    QFileInfo file ( fileName );

    ui.lineEdit_SWCFile->setText ( file.fileName ( ));
  }
}

QString SomaCreatorWidget::getSWCFile ( )
{
  return mSWCFileName;
}

QString SomaCreatorWidget::getFullPathToSWCFile ( )
{
  return mFullPathSWCFileName;
}

void SomaCreatorWidget::loadModelName ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Model(*.obj)" ));

  if ( !fileName.isNull ( ))
  {
    QFileInfo file ( fileName );

    ui.lineEdit_ModelFile->setText ( file.fileName ( ));
  }
}

void SomaCreatorWidget::generateXMLContent ( )
{
  if ( ui.lineEdit_SWCFile->text ( ).length ( ) == 0 )
  {
    QMessageBox::critical ( this, "�Problem in statments!",
                            "Need SWC file",
                            QMessageBox::Ok );
    return;
  }

  if ( ui.lineEdit_GeoDistFileName->text ( ).length ( ) == 0 )
  {
    QMessageBox::critical ( this, "�Problem in statments!",
                            "Need Geodesic file root name",
                            QMessageBox::Ok );
    return;
  }

  if ( ui.lineEdit_GeoVertsIdsFileName->text ( ).length ( ) == 0 )
  {
    QMessageBox::critical ( this, "�Problem in statments!",
                            "Need Geodesic verts ids file root name",
                            QMessageBox::Ok );
    return;
  }

  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( !fileName.isEmpty ( ))
  {
    if ( mXMLSomaDefManager != NULL )
      delete mXMLSomaDefManager;
    mXMLSomaDefManager = new XMLSomaDefManager;

    mXMLSomaDefManager->generateXMLContent ( ui.lineEdit_SWCFile->text ( ),
                                             ui.lineEdit_ModelFile->text ( ),
                                             ui.lineEdit_GeoDistFileName->text ( ),
                                             ui.lineEdit_GeoVertsIdsFileName->text ( ),
                                             ui.spinBox_SomaHorResol->value ( ),
                                             ui.spinBox_SomaVerResol->value ( ),
                                             ui.spinBox_NumDendritics->value ( ),
                                             ui.doubleSpinBox_DefaultaDendriticeValue1->value ( ),
                                             ui.doubleSpinBox_DefaultaDendriticeValue2->value ( )
    );

    mXMLSomaDefManager->exportDomToFile ( fileName.toStdString ( ));
  }
}

void SomaCreatorWidget::loadXMLContent ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Soma Def. File(*.xml)" ));

  if ( !fileName.isNull ( ))
  {
    if ( mXMLSomaDefManager != NULL )
      delete mXMLSomaDefManager;
    mXMLSomaDefManager = new XMLSomaDefManager;

    mXMLSomaDefManager->loadXMLContent ( fileName );
  }
}

//GeodesicMethods
void SomaCreatorWidget::loadSWCFile ( )
{
  if ( ui.checkBox_loadSWC->isChecked ( ))
  {
    QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "NeuroMorpho(*.swc)" ));

    if ( !fileName.isNull ( ))
    {
      if ( mSWCImporter != NULL )
        delete mSWCImporter;
      mSWCImporter = new SWCImporter ( fileName.toStdString ( ));
      ui.lineEdit_MorphologyPathFile->setText ( fileName );

      QFileInfo file ( fileName );
      mSWCFileName = file.fileName ( );
    }
  }
}

void SomaCreatorWidget::loadSWCFile ( QString pFileToLoad )
{
  if ( !pFileToLoad.isNull ( ))
  {
    if ( mSWCImporter != NULL )
      delete mSWCImporter;
    mSWCImporter = new SWCImporter ( pFileToLoad.toStdString ( ));
    ui.lineEdit_MorphologyPathFile->setText ( pFileToLoad );

    QFileInfo file ( pFileToLoad );
    mSWCFileName = file.fileName ( );
  }
}

void SomaCreatorWidget::loadModel ( )
{

  if ( ui.checkBox_loadModel->isChecked ( ))
  {
    QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Mesh(*.off)" ));

    if ( !fileName.isNull ( ))
    {

      if ( mBaseMesh == NULL )
      {
        mBaseMesh = new BaseMesh ( );
      }
      else
      {
        delete mBaseMesh;
        mBaseMesh = new BaseMesh ( );
      }

      ui.lineEdit_InitialMeshPathFile->setText ( fileName );
      mBaseMesh->loadModel ( fileName.toStdString ( ));

      QFileInfo file ( fileName );
      mMehsFileName = file.fileName ( );
    }
  }
}

void SomaCreatorWidget::loadModel ( QString fileName )
{

  if ( !fileName.isNull ( ))
  {

    if ( mBaseMesh == NULL )
    {
      mBaseMesh = new BaseMesh ( );
    }
    else
    {
      delete mBaseMesh;
      mBaseMesh = new BaseMesh ( );
    }

    ui.lineEdit_InitialMeshPathFile->setText ( fileName );

    mBaseMesh->loadModel ( fileName.toStdString ( ));
  }
}

void SomaCreatorWidget::calcNearestVertex ( )
{
  mNearestVertex.clear ( );

  if (( mSWCImporter != NULL )
    && ( mBaseMesh != NULL )
    )
  {
    unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
    SWCNode nodeAux;
    unsigned int idAux = 0;
    OpenMesh::Vec3f lVecPos ( 0, 0, 0 );

    float lActLenght = 0;
    float lLenght = 0;
    unsigned int lCandId = 0;

    //!!!Realmente es mejor hacerlo con las dendritas
    for ( int i = 0; i < ( lNumDendritics ); ++i )
    {
      //Seleccionamos el nodo inicial
      idAux = mSWCImporter->getDendritics ( )[i].initialNode;
      nodeAux = mSWCImporter->getElementAt ( idAux );

      //Cogemos como referencia el primer vertice
      MeshDef::ConstVertexIter v_it = mBaseMesh->getMesh ( )->vertices_begin ( );
      lVecPos = mBaseMesh->getMesh ( )->point ( *v_it );
      lLenght = ( lVecPos - nodeAux.position ).length ( );

      for ( ;
        v_it != mBaseMesh->getMesh ( )->vertices_end ( );
        ++v_it )
      {
        //Coger possci�n del vertice
        lVecPos = mBaseMesh->getMesh ( )->point ( *v_it );

        //Calcular longitud con respecto a la dendrita
        lActLenght = ( lVecPos - nodeAux.position ).length ( );

        //Si es menor y no lo hemos seleccionado ya, se anyade al contenecdor
        if (( lActLenght < lLenght )
          && ( !isIdInContainer ( v_it->idx ( ), mNearestVertex ))
          )
        {
          lCandId = v_it->idx ( );
          lLenght = lActLenght;
        }
      }

      mNearestVertex.push_back ( lCandId );
    }
  }
  else
    throw "";
}

void SomaCreatorWidget::calcAndExportNearestVertexToSWCDendritics ( )
{
  QString fileName = mExitDirectory + "/vIds.txt";

  if ( !fileName.isEmpty ( ))
  {
    calcNearestVertex ( );

    //Construcci�n de la string a exportar a fichero
    QString lQString ( "" );

    lQString = "Vertex identifiers:";
    lQString += "\nNote:You must remember add 1 to each vertex for MatLab";
    lQString += "\n----------------------------";

    for ( unsigned int i = 0; i < mNearestVertex.size ( ); ++i )
    {
      lQString += "\nDendrite_" + QString::number ( i ) + " => Vertex:" + QString::number ( mNearestVertex.at ( i ));
    }

    stringToFile ( lQString, fileName );
  }
}

void SomaCreatorWidget::calcAndExportIdVertexToDendriticsDeformation ( )
{
  //Lectura del fichero
  //Abrir el fichero
  std::ifstream inputFileTXT;
  QString fileTXT = ui.lineEdit_VertexIdsHumbraFile->text ( );
  inputFileTXT.open ( fileTXT.toStdString ( ).c_str ( ), std::ios::in );

  if ( !inputFileTXT )
  {

    QMessageBox::critical ( this, "�Problem in file!", "Cant open file: "
                              + fileTXT,
                            QMessageBox::Ok );

    inputFileTXT.close ( );

    return;
  }

  QString fileName = QFileDialog::getSaveFileName ( this );

  if ( !fileName.isEmpty ( ))
  {

    DenModifInfo lDenModifInfo;
    unsigned int i = 0;
    unsigned int lNumNodes = 0;

    std::vector < DenModifInfo > lGeoDistanceContainer;
    lGeoDistanceContainer.clear ( );

    int pos = 0;
    while ( inputFileTXT >> lDenModifInfo.distance )
    {
      lDenModifInfo.id = i;

      lGeoDistanceContainer.push_back ( lDenModifInfo );

      ++i;
    }

    inputFileTXT.close ( );

    float lMinDistance = ui.doubleSpinBox_humbraValue->value ( );
    QString lFileExport ( "" );
    lFileExport += "# Contour distance applied:" + QString::number ( ui.doubleSpinBox_humbraValue->value ( ));

    for ( unsigned int j = 0; j < lGeoDistanceContainer.size ( ); ++j )
    {
      if ( lGeoDistanceContainer.at ( j ).distance < lMinDistance )
      {
        lFileExport += "\n";
        lFileExport += QString::number ( lGeoDistanceContainer.at ( j ).id );

      }
    }
    lFileExport += "\n";

    stringToFile ( lFileExport, fileName );
  }
}

bool SomaCreatorWidget::isIdInContainer ( unsigned int pId, std::vector < unsigned int > pVector )
{

  for ( int i = 0; i < pVector.size ( ); ++i )
  {
    if ( pVector.at ( i ) == pId )
      return true;
  }
  return false;
}

void SomaCreatorWidget::stringToFile ( QString pFileContent, QString pFilePath )
{
  std::string fileContent = pFileContent.toStdString ( );
  std::string fileTXT = pFilePath.toStdString ( );

  //Clean files
    std::remove(fileTXT.c_str());
    std::ofstream ( fileTXT.c_str ( ));

  //Open files to add data
  std::ofstream outputFileTXT;
  outputFileTXT.open ( fileTXT.c_str ( ), std::ios::app );

  outputFileTXT << fileContent.c_str ( ) << endl;
  outputFileTXT.close ( );
}

void SomaCreatorWidget::loadGeodesicDendriticDistanceFile ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "Geo. Distance File(*.dat)" ));

  if ( !fileName.isNull ( ))
  {
    ui.lineEdit_VertexIdsHumbraFile->setText ( fileName );
  }
}

void SomaCreatorWidget::setGeoDistanceFromSWCImporter ( )
{
  if ( mSWCImporter == NULL )
  {
    QMessageBox::critical ( this, "�Problem with SWC file!",
                            "You need to load SWC file for use this",
                            QMessageBox::Ok );

    return;
  }

  unsigned int lNumDendritics = mSWCImporter->getNumDendritics ( );
  unsigned int lSelectionDendritics = ui.spinBox_VertexIdsHumbraDendriticSelector->value ( );

  if ( lSelectionDendritics < lNumDendritics )
  {
    unsigned int idAux = mSWCImporter->getDendritics ( )[lSelectionDendritics].initialNode;
    ui.doubleSpinBox_humbraValue->setValue ( mSWCImporter->getElementAt ( idAux ).radius );
  }
  else
  {
    QMessageBox::critical ( this, "�Problem with the dendritic selection!",
                            "This SWC only have "
                              + QString::number ( lNumDendritics )
                              + " dendritics (0.."
                              + QString::number ( lNumDendritics - 1 )
                              + ").",
                            QMessageBox::Ok );

    ui.doubleSpinBox_humbraValue->setValue ( 0 );
  }

}

//Methods for magazine.
void SomaCreatorWidget::setExitDirectory ( )
{
  QString dir = QFileDialog::getExistingDirectory ( this, tr ( "Open Directory" ), "./", QFileDialog::ShowDirsOnly
    | QFileDialog::DontResolveSymlinks
  );
  if ( !dir.isNull ( ))
  {

    mExitDirectory = dir;
    ui.lineEdit_ExitDirectoryPathFile->setText ( mExitDirectory );
  }
}


void SomaCreatorWidget::resetInterface ( )
{
    ui.horizontalWidget->hide();
    //ui.pushButton_GoToSomaDeformer->setEnabled ( false );
}

void SomaCreatorWidget::generateXMLSoma ( QString fileName, bool useSoma ) {
    MeshVCG *somaMesh = nullptr;
    QFileInfo info1;
    if (ui.checkBox_loadSWC->isChecked()) {
        /*fileName =
          QFileDialog::getOpenFileName ( this, tr ( "Open File" ), "./", tr ( "NeuroMorpho(*.swc);;Neurolucida(*.asc)" )); */


            this->mInputFile = fileName;

            if (!fileName.isNull()) {
                if (mSWCImporter != NULL)
                    delete mSWCImporter;

                info1 = QFileInfo(fileName);
                QString ext = info1.suffix();

                //QString lFullTmpFile = mExitDirectory  + "/" + QFileInfo(fileName).fileName() + ".swc";
                QString lLocalFilePath = info1.absolutePath();

                if ((ext == "asc") || (ext == "ASC")) {
                  auto ascPath = lLocalFilePath.toStdString() + "/" + info1.fileName().toStdString();
                  fileName = Neuronize::tmpPath + "/" + info1.fileName() + ".swc";
                  delete ASCparser;
                    ASCparser = new ASC2SWCV2(ascPath, useSoma);
                  ASCparser->toSWC(fileName.toStdString());
                  somaMesh = ASCparser->getSomaMesh();
                  this->spines = ASCparser->getSpines();
                  this->contours = ASCparser->getContours();
                  // PREVIOUS VERSION
                  //ASC2SWC::convierteASWC(lLocalFilePath.toStdString(), info1.fileName().toStdString());
                  //fileName = lLocalFilePath + "/" + info1.fileName() + ".swc";
                }

                mSWCImporter = new SWCImporter(fileName.toStdString());

                QFileInfo file(fileName);
                mFullPathSWCFileName = fileName;
                mSWCFileName = file.fileName();
            } else
                return;

        //Copiar el swc y copiar el .off
        QFileInfo sourceInfo(mFullPathSWCFileName);
        QString destination = mExitDirectory + "/" + sourceInfo.fileName();

        //QFile::copy(mFullPathSWCFileName, destination)
        if (mFullPathSWCFileName != destination) {
            if (!QFile::copy(mFullPathSWCFileName, destination)) {
                QFile::remove(destination);
                QFile::copy(mFullPathSWCFileName, destination);
            }
        }

        QFileInfo sourceInfoModel(mMehsFileName);
        destination = mExitDirectory + "/" + sourceInfoModel.fileName();
        //QFile::copy(mMehsFileName, destination);
        if (!QFile::copy(mMehsFileName, destination)) {
            QFile::remove(destination);
            QFile::copy(mMehsFileName, destination);
        }

        //Transformar el .off base a las dimensiones del soma real (necesitamos crear uno nuevo)
        if (somaMesh == nullptr) {
            loadModel(destination);
            this->somaContours = false;
            BaseMesh *lBaseMesh = new BaseMesh();
            lBaseMesh->JoinBaseMesh(mBaseMesh);
            //Escalamos al radio del soma
            lBaseMesh->scaleBaseMesh(mSWCImporter->getElementAt(1).radius);
            lBaseMesh->exportMesh(
                    mExitDirectory.toStdString() + "/" + sourceInfoModel.baseName().toStdString() + "_RadioReal.off");
            delete lBaseMesh;
        } else {
            somaContours = true;
            QFile::remove(destination);
            QFile::remove(mExitDirectory + "/" "somaConvex.off");
            somaMesh->toOff(mExitDirectory.toStdString() + "/" + "somaConvex.off");
            somaMesh->toOff(mExitDirectory.toStdString() + "/" + sourceInfoModel.baseName().toStdString() +
                            "_RadioReal.off"); //Necesario para deformar
            loadModel(mExitDirectory + "/" + "somaConvex.off");

        }

        //ui.label_SomaGeneratedName->setText("Soma to build = "+ sourceInfo.fileName());

        emit somaCreated();
    }
}

void SomaCreatorWidget::generateGeodesicFiles(float lscale) {
    QFileInfo sourceInfoModel(mMehsFileName);
    auto destination = mExitDirectory + "/" + sourceInfoModel.fileName();
    if (!somaContours) {
        loadModel(destination);
        this->somaContours = false;
        BaseMesh *lBaseMesh = new BaseMesh();
        lBaseMesh->JoinBaseMesh(mBaseMesh);
        //Escalamos al radio del soma
        lBaseMesh->scaleBaseMesh(mSWCImporter->getElementAt(1).radius * lscale);
        lBaseMesh->exportMesh(
                mExitDirectory.toStdString() + "/" + sourceInfoModel.baseName().toStdString() + "_RadioReal.off");
        delete lBaseMesh;
    }

    int lNumDendrites = mSWCImporter->getNumDendritics();

    //Construir cada dendrita
    calcNearestVertex();
    std::vector<int> vertexs;

    for (int i = 0; i < lNumDendrites; ++i) {
        vertexs.push_back(mNearestVertex.at(i));
    }
    auto somaPath = somaContours ? mExitDirectory.toStdString() + "/" + "somaConvex.off" : mExitDirectory.toStdString() + "/" + sourceInfoModel.baseName().toStdString() + "_RadioReal.off";

    MeshVCG mesh(somaPath);
    std::string path = mExitDirectory.toStdString() + "/";
    path += mDefaultGeoDistFileName.toStdString();

    mesh.calcGeodesicDistance(vertexs, path);

    //Generacion de los ficheros con los identificadores de los vertices que pertenecen a cada dendrita
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int k = 0; k < lNumDendrites; ++k) {
        //Lectura del fichero
        //Abrir el fichero
        std::ifstream vIdsInputFileTXT;
        QString vIdsFileTXT = mExitDirectory + "/" + mDefaultGeoDistFileName + QString::number(k) + ".dat";
        vIdsInputFileTXT.open(vIdsFileTXT.toStdString().c_str(), std::ios::in);

        if (!vIdsInputFileTXT) {
            QMessageBox::critical(this, "�Problem in file!", "Cant open file: "
                                                             + vIdsFileTXT, QMessageBox::Ok);

            vIdsInputFileTXT.close();
            return;
        }

        //QString fileName = QFileDialog::getSaveFileName(this);
        QString fileName = mExitDirectory + "/" + mDefaultVidsFileName + QString::number(k) + ".vid";
        QFile file (fileName);
        file.remove();

        DenModifInfo lDenModifInfo;
        int t = 0;
        int lNumNodes = 0;

        std::vector<DenModifInfo> lGeoDistanceContainer;
        lGeoDistanceContainer.clear();

        //Node readed
        //Esto ya no esta (el save de matlab no lo saca)
        int pos = 0;
        while (vIdsInputFileTXT >> lDenModifInfo.distance) {
            lDenModifInfo.id = t;
            lGeoDistanceContainer.push_back(lDenModifInfo);
            ++t;
        }

        vIdsInputFileTXT.close();

        //Construcci�n de la string a exportar
        //float lMinDistance = ui.doubleSpinBox_humbraValue->value();

        //### Esto hay que revisarlo, no se deber�a tener que sumar uno a este indice ...
        int lNodeSomaId = mSWCImporter->getDendriticSomaConnection().at(k + 1);

        float lMinDistance = mSWCImporter->getElementAt(lNodeSomaId).radius;

        QString lFileExport("");
        lFileExport += "# Contour distance applied:" + QString::number(lMinDistance);

        for (unsigned int j = 0; j < lGeoDistanceContainer.size(); ++j) {
            if (lGeoDistanceContainer.at(j).distance < lMinDistance) {
                lFileExport += "\n";
                lFileExport += QString::number(lGeoDistanceContainer.at(j).id);
            }
        }
        lFileExport += "\n";

        stringToFile(lFileExport, fileName);

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Export XMLFile
    //control de errores
    //QString fileName = QFileDialog::getSaveFileName(this);
    QString fileName = mExitDirectory + "/Definition";
    if (mXMLSomaDefManager != NULL) delete mXMLSomaDefManager;
    mXMLSomaDefManager = new XMLSomaDefManager;

    mXMLSomaDefManager->generateXMLContent(mSWCFileName
            //,mMehsFileName
            , sourceInfoModel.baseName() + "_RadioReal.off", mDefaultGeoDistFileName, mDefaultVidsFileName, 0,
                                           0, lNumDendrites, 1.0, 1.0);

    mXMLSomaDefManager->exportDomToFile(fileName.toStdString());
}



void SomaCreatorWidget::setNeuron(skelgenerator::Neuron *neuron) {
  SomaCreatorWidget::neuron = neuron;
}

skelgenerator::Neuron *SomaCreatorWidget::getNeuron() const {
  return neuron;
}

bool SomaCreatorWidget::isSomaContours() {
  return somaContours;
}

const QString &SomaCreatorWidget::getInputFile() const {
    return mInputFile;
}

const vector<Spine*> &SomaCreatorWidget::getSpines() const {
  return spines;
}


void
SomaCreatorWidget::openSelectFileDialog(QLineEdit *target, const QString &title, const QString &types, bool multiFile) {
    if (multiFile) {
        auto files = QFileDialog::getOpenFileNames(this, title, QString(), types);
        if (!files.isEmpty()) {
            QString filesString;
            filesString.push_back(files[0]);
            for (int i = 1; i < files.size(); i++) {
                filesString.push_back(";" + files[i]);
            }
            target->setText(filesString);
        }
    } else {
        auto file = QFileDialog::getOpenFileName(this, title, QString(), types);
        target->setText(file);
    }

}

void SomaCreatorWidget::openSaveFileDialog(QLineEdit *target, const QString &title, const QString &types) {
    auto file = QFileDialog::getSaveFileName(this, title, QString(), types);
    target->setText(file);

}

void SomaCreatorWidget::openDirectory(QLineEdit* target, const QString& title){
    auto dir = QFileDialog::getExistingDirectory(this,title,QString());
    target->setText(dir);
}

void SomaCreatorWidget::deleteTreeViewer() {

}


const vector<vector<OpenMesh::Vec3d>> &SomaCreatorWidget::getContours() const {
    return contours;
}


void SomaCreatorWidget::openDir(QLineEdit* dest,QString message) {
    auto folder = QFileDialog::getExistingDirectory(this, message, QString());
    dest->setText(folder);
}

void SomaCreatorWidget::onGenerateNeurons() {
    NeuronSetDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit generateNeurons(dialog.getInputDir(),dialog.getOutputDir(),dialog.getSubdivisions(),dialog.getBaseName());
    }
}

SWCImporter *SomaCreatorWidget::getMswcImporter() const {
    return mSWCImporter;
}

void SomaCreatorWidget::onGenerateOneNeuron() {
    WizzardInputOne wizzard(this);
    wizzard.exec();
    if (wizzard.result() == QDialog::Accepted)
    {
      Neuronize::outPath = wizzard.getOutputPath( );
      if( wizzard.isFilament( ) )
      {
        this->neuron = wizzard.getNeuron( );
        this->mInputFile = this->mExitDirectory + "/temp.asc";
        std::ofstream file;
        file.open( this->mInputFile.toStdString( ) );
        file << this->neuron->to_asc( );
        file.close( );

        generateXMLSoma( this->mInputFile, true );
      }
      else
      {
        generateXMLSoma( wizzard.getTraceFile( ), true );
      }
    }

}

ASC2SWCV2 *SomaCreatorWidget::getASCparser() const {
    return ASCparser;
}




