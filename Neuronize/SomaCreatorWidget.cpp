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

#include <string>
#include <fstream>
#include <cstdio>

#include <QList>
#include <QFileDialog>
#include <QMessageBox>

#include <QDebug>

#include <libs/libNeuroUtils/AS2SWCV2.h>
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

  mDefaultVidsFileName = "GeoVertsIdsDendrite_";
  mDefaultGeoDistFileName = "GeoDistDendrite_";
  mFullPathSWCFileName = mSWCFileName = "";

  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera3Subdiv1Radio.off";
  mMehsFileName =    QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera4Subdiv1Radio.off";
  //mMehsFileName	= QCoreApplication::applicationDirPath() + "/Content/Meshes/IcoSphera5Subdiv1Radio.off";
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

    // Connections of load files
    connect(ui.traces, &QRadioButton::toggled, this, &SomaCreatorWidget::onRadioChanged);
    connect(ui.saveCheckBox, &QCheckBox::stateChanged, this, &SomaCreatorWidget::onSaveChanged);
    connect(ui.tracePathButton, &QPushButton::released, [=]() {
        openSelectFileDialog(ui.tracePath, "Select trace file",
                             "NeuroMorpho(*.swc *.SWC);;Neurolucida ASC(*.asc *.ASC)", false);
    });
    connect(ui.apiPathButton, &QPushButton::released, [=]() {
        openSelectFileDialog(ui.apiPath, "Select apical file", "Imaris VRML (*.vrml *.wrl)", false);
    });
    connect(ui.basalPathButton, &QPushButton::released, [=]() {
        openSelectFileDialog(ui.basalPath, "Select basal file", "Imaris VRML (*.vrml *.wrl)", true);
    });
    connect(ui.imarisPathButton, &QPushButton::released, [=]() {
        openSelectFileDialog(ui.imarisPath, "Select imaris spines volumes file", "Imaris VRML (*.vrml *.wrl)", false);
    });
    connect(ui.longsButton, &QPushButton::released, [=]() {
        openSelectFileDialog(ui.longsPath, "Select imaris spines longs file", "Imaris VRML (*.vrml *.wrl)", false);
    });

    connect(ui.pushButton_GoToSomaDeformer, &QPushButton::released, this, &SomaCreatorWidget::onOkPressed);

    connect(ui.ascButton, &QPushButton::released, [=]() {
        openSaveFileDialog(ui.ascPath, "Select ASC file", "Neurolucida (*.asc *.ASC)");
    });


    futureWatcher = new QFutureWatcher<void>();
    connect(futureWatcher, &QFutureWatcher<void>::finished, this, &SomaCreatorWidget::onProcessFinish);

    connect(ui.oneNeuronRadio,&QRadioButton::toggled,this,&SomaCreatorWidget::onRadioChanged2);
    connect(ui.adavencedOptionsButton,&QPushButton::released,this,[&](){ui.horizontalWidget->setVisible(!ui.horizontalWidget->isVisible());});
    connect(ui.inputDirectoryButton,&QPushButton::released,this,[&](){openDir(ui.inputDirectoryPath,"Select input folder");});
    connect(ui.outputDirectoryButton,&QPushButton::released,this,[&](){openDir(ui.outputDirectoryPath,"Select output directory");});
    connect(ui.pushButton_GenerateNeurons,&QPushButton::released,this,&SomaCreatorWidget::onGenerateNeurons);

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
    ui.tracePath->setText("");
    ui.apiPath->setText("");
    ui.basalPath->setText("");
    ui.imarisPath->setText("");
    ui.saveCheckBox->setChecked(false);
    ui.ascPath->setDisabled(true);
    ui.ascPath->setText("");
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
                  fileName = lLocalFilePath + "/" + info1.fileName() + ".swc";
                  auto result = AS2SWCV2::asc2swc(ascPath, fileName.toStdString(), useSoma);
                  somaMesh = std::get<0>(result);
                  this->spines = std::get<1>(result);                    // PREVIOUS VERSION
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

        int lNumDendrites = mSWCImporter->getNumDendritics();


        QFileInfo lMeshNameWIthoutExtension(mMehsFileName);
        //lMeshNameWIthoutExtension.baseName();

        //Consstruccion de ficheros de matlab
        QString lMatLAbScriptFile = "\n name =";
        lMatLAbScriptFile += "\'" + lMeshNameWIthoutExtension.baseName() + "_RadioReal" + "\'; \n";
        lMatLAbScriptFile += "idVertex = 1; \n";
        lMatLAbScriptFile += "nstart = 1; \n";

        mToolBoxDir = QCoreApplication::applicationDirPath() + "/Content/MatLab/Geodesic";
        //mToolBoxDir = "E:\\Devel\\MatLab\\Geodesic";
        QString lTmpPath = "\'" + mToolBoxDir + "/toolbox_fast_marching/'";
        lMatLAbScriptFile += "path(path, " + lTmpPath + ");\n";

        lTmpPath = "\'" + mToolBoxDir + "/toolbox_fast_marching/toolbox/\'";
        lMatLAbScriptFile += "path(path, " + lTmpPath + ");\n";

        lTmpPath = "\'" + mToolBoxDir + "/toolbox_graph/\'";
        lMatLAbScriptFile += "path(path, " + lTmpPath + ");\n";

        lTmpPath = "\'" + mToolBoxDir + "/toolbox_graph/off/\'";
        lMatLAbScriptFile += "path(path, " + lTmpPath + ");\n";

        lTmpPath = "\'" + mToolBoxDir + "/meshes/\'";
        lMatLAbScriptFile += "path(path, " + lTmpPath + ");\n";

        lMatLAbScriptFile += "path(path, \'" + mExitDirectory + "/\'" + ");\n";

        lMatLAbScriptFile += "[vertex,faces] = read_mesh([name '.off']);\n";

        lMatLAbScriptFile += "nverts = max(size(vertex)); start_points = idVertex; [D,S,Q] = perform_fast_marching_mesh(vertex, faces, start_points); \n";

        lMatLAbScriptFile += "cd(\'" + mExitDirectory + "\\\');\n";

        //Construir cada dendrita
        calcNearestVertex();
        std::vector<int> vertexs;

        for (int i = 0; i < lNumDendrites; ++i) {
            vertexs.push_back(mNearestVertex.at(i));
        }
        auto somaPath = somaContours ? mExitDirectory.toStdString() + "/" + "somaConvex.off" : mExitDirectory.toStdString() + "/IcoSphera4Subdiv1Radio_RadioReal.off";

        MeshVCG mesh(somaPath);
        std::string path = mExitDirectory.toStdString() + "/";
        path += mDefaultGeoDistFileName.toStdString();

        mesh.calcGeodesicDistance(vertexs, path);



        //Clean files
        //Exportaci�n de los vIds
        calcAndExportNearestVertexToSWCDendritics();

        //Clean files
        //std::string fileContent = pFileContent.toStdString();
        std::string matFileTXT = mExitDirectory.toStdString() + "/MatLabScript.m";

        std::ofstream(matFileTXT.c_str());

        //Open files to add data
        std::ofstream matOutputFileTXT;
        matOutputFileTXT.open(matFileTXT.c_str(), std::ios::app);

        matOutputFileTXT << lMatLAbScriptFile.toStdString() << endl;
        matOutputFileTXT.close();

        //Call MatLab to generate the files

        //engClose(m_pEngine);



        //Exportaci�n de los vIds
        //---calcAndExportNearestVertexToSWCDendritics ( );

        //Store current dir (Matlab will change this)
        fileName = QCoreApplication::applicationDirPath();


        //GeoCalcInitialize();





        //mwArray lPathName("E:\\WorkSpace\\VisualStudio\\CPP\\Produccion\\Puppeteer Engine\\tutorials\\Qt\\XNeuron");
        QString lPathDoubleBackSlah = QCoreApplication::applicationDirPath();
        //lPathDoubleBackSlah.replace("/","\\\\");

        //GeoCalcTerminate();

        //Restore current dir, Matlab change this
        QDir::setCurrent(fileName);

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

            //if (!fileName.isEmpty())
            {

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
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Export XMLFile
        //control de errores
        //QString fileName = QFileDialog::getSaveFileName(this);
        fileName = mExitDirectory + "/Definition";
        //if (!fileName.isEmpty())
        {
            if (mXMLSomaDefManager != NULL) delete mXMLSomaDefManager;
            mXMLSomaDefManager = new XMLSomaDefManager;

            mXMLSomaDefManager->generateXMLContent(mSWCFileName
                    //,mMehsFileName
                    , sourceInfoModel.baseName() + "_RadioReal.off", mDefaultGeoDistFileName, mDefaultVidsFileName, 0,
                                                   0, lNumDendrites, 1.0, 1.0);

            mXMLSomaDefManager->exportDomToFile(fileName.toStdString());
        }

        //ui.label_SomaGeneratedName->setText("Soma to build = "+ sourceInfo.fileName());

        emit somaCreated();
    }
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

const vector<Spine> &SomaCreatorWidget::getSpines() const {
  return spines;
}

void SomaCreatorWidget::onRadioChanged(bool b) {
    ui.tracePath->setDisabled(!b);
    ui.tracePathButton->setDisabled(!b);
    ui.apiPath->setDisabled(b);
    ui.apiPathButton->setDisabled(b);
    ui.basalPath->setDisabled(b);
    ui.basalPathButton->setDisabled(b);
    ui.longsButton->setDisabled(b);
    ui.longsPath->setDisabled(b);
    ui.imarisPath->setDisabled(b);
    ui.imarisPathButton->setDisabled(b);
    ui.saveCheckBox->setDisabled(b);
    if (b) {
        ui.ascButton->setDisabled(b);
        ui.ascPath->setDisabled(b);
    } else {
        bool active = ui.saveCheckBox->checkState() == Qt::Checked;
        ui.ascPath->setDisabled(!active);
        ui.ascButton->setDisabled(!active);
    }
}

void SomaCreatorWidget::onSaveChanged(int state) {
    bool active = state == Qt::Checked;
    ui.ascPath->setDisabled(!active);
    ui.ascButton->setDisabled(!active);
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

void SomaCreatorWidget::onOkPressed() {
    if (ui.traces->isChecked()) {
        if (ui.tracePath->text().isEmpty()) {
            QToolTip::showText(ui.tracePath->mapToGlobal(QPoint(0, 0)), "Need a Input File");
        } else {
            generateXMLSoma(ui.tracePath->text(), true);
        }
    } else {
        if (ui.basalPath->text().isEmpty()) {
            QToolTip::showText(ui.tracePath->mapToGlobal(QPoint(0, 0)), "Need a Input File");
        } else if (ui.saveCheckBox->checkState() == Qt::Checked && ui.ascPath->text().isEmpty()) {
            QToolTip::showText(ui.ascPath->mapToGlobal(QPoint(0, 0)), "Need a Output File");
        } else {
            QFileInfo fi (ui.basalPath->text());
            auto name = fi.dir().dirName();

            this->mInputFile = this->mExitDirectory + "/" + name + ".asc";

            QFuture<void> future = QtConcurrent::run(
                    [=]() { processSkel(this->mInputFile.toStdString());});

            futureWatcher->setFuture(future);
            progresDialog = new QProgressDialog("Generating tracing", "Cancel", 0, 0, this);
            progresDialog->setValue(0);
            progresDialog->setCancelButton(0);
            progresDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
            progresDialog->exec();
        }
    }
}


void SomaCreatorWidget::processSkel(const std::string &fileName) {
    auto basalFiles = ui.basalPath->text().split(";");
    auto apiFile = ui.apiPath->text().toStdString();
    auto imarisFile = ui.imarisPath->text().toStdString();
    auto longsFile = ui.longsPath->text().toStdString();
    std::vector<std::string> basalFilesStd;
    for (const auto &string: basalFiles) {
        basalFilesStd.push_back(string.toStdString());
    }

    float newThreshold = 0.5f;
    auto neuron = new skelgenerator::Neuron(apiFile, basalFilesStd, imarisFile, longsFile, newThreshold);
    bool ignore = false;
    while (neuron->isIncorrectConecctions() || neuron->getReamingSegments() > 0 && !ignore) {
        if (neuron->isIncorrectConecctions()) {
            QMetaObject::invokeMethod(this, "showWarningDialogIncorrectConnections", Qt::BlockingQueuedConnection,
                                      Q_ARG(float &, newThreshold));

        } else {
            if (neuron->getReamingSegments() > 0) {
                QMetaObject::invokeMethod(this, "showWarningDialogReaminingSegments", Qt::BlockingQueuedConnection,
                                          Q_ARG(int, neuron->getReamingSegments()),
                                          Q_ARG(float &, newThreshold));
            }
        }

        ignore = newThreshold < 0;
        if (!ignore) {
            delete (neuron);
            neuron = new skelgenerator::Neuron(apiFile, basalFilesStd, imarisFile,longsFile, newThreshold);
        }

    }
    this->neuron = neuron;

}

void SomaCreatorWidget::onProcessFinish() {
    progresDialog->setMaximum(1);
    progresDialog->setValue(1);
    QMessageBox msgBox(this);
    QString outPath;
    if (ui.saveCheckBox->checkState() == Qt::Checked && ui.ascPath->text() != nullptr && ui.ascPath->text() != "") {
        outPath = ui.ascPath->text();
    } else {
        outPath = this->mInputFile;
    }

    std::ofstream file;
    file.open(outPath.toStdString());
    file << this->neuron->to_asc();
    file.close();

    msgBox.setText("Task Finished");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
    generateXMLSoma(outPath, true);
}

void SomaCreatorWidget::showWarningDialogIncorrectConnections(float &newThreshold) {
    auto *msgBox = new QMessageBox(this);;
    std::string msg = "The neuron maybe has incorrect connections.\t";
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    msgBox->setInformativeText(
            "Do you want to process the neuron again changing the \"Connection Threshold\" or ignore the maybe incorrect conections?");
    msgBox->setText(QString::fromStdString(msg));
    QPushButton *changeButton = msgBox->addButton(tr(" Change Threshold "), QMessageBox::NoRole);
    QPushButton *ignoreButton = msgBox->addButton(tr("Ignore"), QMessageBox::NoRole);
    msgBox->setDefaultButton(changeButton);
    msgBox->exec();

    if (msgBox->clickedButton() == changeButton) {
        QInputDialog inputDialog(this);
        inputDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        inputDialog.setLabelText("New Connection Threshold");
        inputDialog.setTextValue("Insert new value");
        inputDialog.setInputMode(QInputDialog::DoubleInput);
        inputDialog.setDoubleRange(0.0,40.0);
        inputDialog.setDoubleStep(0.1f);
        inputDialog.setDoubleDecimals(2);
        inputDialog.setDoubleValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.doubleValue();
    } else {
        newThreshold = -1;
    }
}

void SomaCreatorWidget::showWarningDialogReaminingSegments(int sobrantes, float &newThreshold) {
    QMessageBox *msgBox = new QMessageBox(this);;
    std::string msg = "This neuron has " + std::to_string(sobrantes) +
                      " segments that have not been connected and therefore will be ignored.";
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    msgBox->setInformativeText(
            "Do you want to process the neuron again changing the \"Connection Threshold\" or ignore the missing segments?");
    msgBox->setText(QString::fromStdString(msg));
    QPushButton *changeButton = msgBox->addButton(tr(" Change Threshold "), QMessageBox::NoRole);
    QPushButton *ignoreButton = msgBox->addButton(tr("Ignore"), QMessageBox::NoRole);
    msgBox->setDefaultButton(changeButton);
    msgBox->exec();

    if (msgBox->clickedButton() == changeButton) {
        QInputDialog inputDialog;
        inputDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        inputDialog.setLabelText("New Connection Threshold");
        inputDialog.setTextValue("Insert new value");
        inputDialog.setInputMode(QInputDialog::DoubleInput);
        inputDialog.setDoubleRange(0.0,40.0);
        inputDialog.setDoubleStep(0.1f);
        inputDialog.setDoubleDecimals(2);
        inputDialog.setDoubleValue(newThreshold);
        inputDialog.exec();
        newThreshold = inputDialog.doubleValue();


    } else {
        newThreshold = -1;
    }
}



void SomaCreatorWidget::deleteTreeViewer() {

}

void SomaCreatorWidget::onRadioChanged2(bool b) {

    if (b) {
        ui.traces->setEnabled(true);
        ui.vrmls->setEnabled(true);
        ui.pushButton_GoToSomaDeformer->setEnabled(true);

        bool trace = ui.traces->isChecked();
        ui.tracePathButton->setEnabled(trace);
        ui.tracePath->setEnabled(trace);

        ui.basalPath->setEnabled(!trace);
        ui.basalPathButton->setEnabled(!trace);
        ui.apiPath->setEnabled(!trace);
        ui.apiPathButton->setEnabled(!trace);
        ui.saveCheckBox->setEnabled(!trace);
        if (ui.saveCheckBox->isChecked()) {
            ui.ascPath->setEnabled(!trace);
            ui.ascButton->setEnabled(!trace);
        }

        ui.inputDirectoryButton->setEnabled(false);
        ui.inputDirectoryPath->setEnabled(false);
        ui.outputDirectoryButton->setEnabled(false);
        ui.outputDirectoryPath->setEnabled(false);
        ui.adavencedOptionsButton->setEnabled(false);
        ui.subdivisionsLabel->setEnabled(false);
        ui.subdivisionsSpinBox->setEnabled(false);
        ui.baseNameLabel->setEnabled(false);
        ui.baseNameLineEdit->setEnabled(false);
        ui.pushButton_GenerateNeurons->setEnabled(false);


    } else {
        ui.traces->setEnabled(false);
        ui.vrmls->setEnabled(false);

        ui.tracePathButton->setEnabled(false);
        ui.basalPathButton->setEnabled(false);
        ui.apiPathButton->setEnabled(false);
        ui.saveCheckBox->setEnabled(false);
        ui.ascButton->setEnabled(false);

        ui.tracePath->setEnabled(false);
        ui.basalPath->setEnabled(false);
        ui.apiPath->setEnabled(false);
        ui.saveCheckBox->setEnabled(false);
        ui.ascPath->setEnabled(false);

        ui.pushButton_GoToSomaDeformer->setEnabled(false);

        ui.inputDirectoryButton->setEnabled(true);
        ui.inputDirectoryPath->setEnabled(true);
        ui.outputDirectoryButton->setEnabled(true);
        ui.outputDirectoryPath->setEnabled(true);
        ui.adavencedOptionsButton->setEnabled(true);
        ui.subdivisionsLabel->setEnabled(true);
        ui.subdivisionsSpinBox->setEnabled(true);
        ui.baseNameLabel->setEnabled(true);
        ui.baseNameLineEdit->setEnabled(true);
        ui.pushButton_GenerateNeurons->setEnabled(true);

    }
}

const vector<vector<OpenMesh::Vec3d>> &SomaCreatorWidget::getContours() const {
    return contours;
}


void SomaCreatorWidget::openDir(QLineEdit* dest,QString message) {
    auto folder = QFileDialog::getExistingDirectory(this, message, QString());
    dest->setText(folder);
}

void SomaCreatorWidget::onGenerateNeurons() {
    auto inputDir = ui.inputDirectoryPath->text();
    auto outputDir = ui.outputDirectoryPath->text();
    if (inputDir.isEmpty()) {
        QToolTip::showText(ui.inputDirectoryPath->mapToGlobal(QPoint(0, 0)), "Need a Input Directory");
        return;
    }

    if (outputDir.isEmpty()) {
        QToolTip::showText(ui.outputDirectoryPath->mapToGlobal(QPoint(0, 0)), "Need a Output Directory");
        return;
    }

    int subdivisions = ui.subdivisionsSpinBox->value();
    auto baseName = ui.baseNameLineEdit->text();
    emit generateNeurons(inputDir,outputDir,subdivisions,baseName);
}

SWCImporter *SomaCreatorWidget::getMswcImporter() const {
    return mSWCImporter;
}




