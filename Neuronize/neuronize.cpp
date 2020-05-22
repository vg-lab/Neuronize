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


#include <QMessageBox>
#include <QtCore/QDirIterator>
#include <QSettings>

#include "neuronize.h"
#include "ExportDialog.h"
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <clocale>
#include <MeshReconstructWrapper/MeshReconstruct.h>

#include <QtWidgets/QDialog>


QString Neuronize::configPath;
QString Neuronize::envPath;
QString Neuronize::outPath;
BBDD::BBDD Neuronize::bbdd;
QString Neuronize::tmpPath;
bool Neuronize::hasPython;

Neuronize::Neuronize ( QWidget *parent )
        : QMainWindow(parent) {
    ui.setupUi(this);
    // Para evitar problemas con las tildes en español.
    std::setlocale(LC_ALL, "es_ES.UTF-8");


    //Init glut
    int argc = 0;
    char **argv = NULL;
    glutInit(&argc, argv);
    if (!tempDir.isValid()) {
        throw "Cant create temporary dir";
    }
    std::cout << "TmpDir: " << tempDir.path().toStdString() << std::endl;
    Neuronize::tmpPath = tempDir.path();;

    //QObject::connect(ui.actionGenerateNewNeuron	,SIGNAL(triggered())	,this,SLOT(generateNewNeuron()));
    QObject::connect(ui.actionGenerateNewNeuron, SIGNAL(triggered()), this, SLOT(actionNewNeuron()));
    connect(ui.actionRepair_Meshes, &QAction::triggered, [&]() {
        resetNeuronnizeInterface();
        ui.tabWidget_MainContainer->setCurrentIndex(1);
    });
    connect(ui.actionCompare_Meshes, &QAction::triggered, [&]() {
        resetNeuronnizeInterface();
        ui.tabWidget_MainContainer->setCurrentIndex(2);
    });
    connect(ui.actionExport_Neuron_info,&QAction::triggered,this,&Neuronize::showExportDialog);
    connect(ui.actionReset_Python_Enviorement,&QAction::triggered,this,&Neuronize::resetPythonEnv);
    connect(ui.actionDelete_Local_Database,&QAction::triggered,this,&Neuronize::deleteDatabase);
    connect(ui.actionExport_Database_File,&QAction::triggered,this,&Neuronize::exportDatabase);
    QObject::connect(ui.actionTake_a_snapshot, SIGNAL(triggered()), this, SLOT(takeASnapshot()));
    QObject::connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    QObject::connect(ui.actionQuit, SIGNAL(triggered()), this, SLOT(actionQuit()));
    QObject::connect(ui.actionAbout_Neuronize, SIGNAL(triggered()), this, SLOT(actionAbout()));
    QObject::connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(actionBack()));


    //QObject::connect(ui.actionBatchBuilder, SIGNAL(triggered()), this, SLOT(showBatchBuilder()));

    mSomaCreatorWidget = nullptr;
    mSomaDeformerWidget = nullptr;
    mNeuroGeneratorWidget = nullptr;
    mRepairWidget = nullptr;
    mCompareMeshesWidget = nullptr;
    hasPython = true; //Se pone a true para que se muestre en la interfaz, e sobrescribira despues con el valor real

    resetNeuronnizeInterface();
    mActiveTab = 0;

#ifdef _WIN32
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope,"Neuronize","preferences");
#else
    QSettings settings("Neuronize", "preferences");
#endif

    configPath = QFileInfo(settings.fileName()).absoluteDir().absolutePath();
    QDir dir(configPath);
    if (!dir.exists()) {
        dir.mkpath(configPath);
    }
    std::cout << configPath.toStdString() << std::endl;

    createDatabase();
    this->showNormal();
    resize(1200, 800);
    initPythonEnv();


    initrand();
}

Neuronize::~Neuronize ( )
{
    delete mSomaCreatorWidget;
    delete mSomaDeformerWidget;
    delete mNeuroGeneratorWidget;
}

void Neuronize::resetNeuronnizeInterface ( )
{
    delete mSomaCreatorWidget;
    delete mSomaDeformerWidget;
    delete mNeuroGeneratorWidget;
    delete mRepairWidget;
    delete mCompareMeshesWidget;

  mSomaCreatorWidget = new SomaCreatorWidget (tempDir.path());

  ui.verticalLayout_SomaCreator->addWidget ( mSomaCreatorWidget );

    mRepairWidget = new RepairWidget();
    ui.verticalLayout_RepairMeshes->addWidget(mRepairWidget);

    mCompareMeshesWidget = new CompareMeshesWidget(tempDir.path().toStdString());
    ui.verticalLayout_CompareMeshes->addWidget(mCompareMeshesWidget);


    QObject::connect(mSomaCreatorWidget, SIGNAL(somaCreated()), this, SLOT(onSomaBuildFinish()));
    connect(mSomaCreatorWidget,&SomaCreatorWidget::generateNeurons,this,&Neuronize::genetareNeuronsInBatch);

  mSomaDeformerWidget = new SomaDeformerWidget ( tempDir.path() );
  ui.verticalLayout_SomaDeformer->addWidget ( mSomaDeformerWidget );
  QObject::connect ( mSomaDeformerWidget, SIGNAL( finishSoma ( )), this, SLOT( showDendriteGenerator ( )) );

  mNeuroGeneratorWidget = new NeuroGeneratorWidget ( tempDir.path() );
  ui.verticalLayout_DendritesGenerator->addWidget ( mNeuroGeneratorWidget );
  mNeuroGeneratorWidget->loadSpinesModelFromPath ( QCoreApplication::applicationDirPath() + "/Content/Spines/Low/" );
  connect(mNeuroGeneratorWidget,&NeuroGeneratorWidget::finish,this,[&](){
     resetNeuronnizeInterface();
  });

  //mNeuroGeneratorWidget ->getUI().tabWidget_RenderControl->removeTab(1);

  QObject::connect ( mNeuroGeneratorWidget,
                     SIGNAL( finishNeuronSurfaceAndGoToSpines ( )),
                     this,
                     SLOT( showSpinesGenerator ( )) );
  QObject::connect ( mNeuroGeneratorWidget,
                     SIGNAL( finishNeuronSurfaceAndRestart ( )),
                     this,
                     SLOT( showSomaCreator ( )) );
  //QObject::connect(mNeuroGeneratorWidget,SIGNAL(finishSpinesAndRestart())	,this		,SLOT(showSomaCreator()));
  QObject::connect ( mNeuroGeneratorWidget,
                     SIGNAL( finishSpinesAndRestart ( )),
                     this,
                     SLOT( NewNeuronQuestionAndRestart ( )) );

  QObject::connect ( mNeuroGeneratorWidget, SIGNAL( resetToSomaState ( )), this, SLOT( showDendriteGenerator ( )) );

  //mNeuroGeneratorWidget->getUI().tabWidget_RenderControl->removeTab(1);

  //Noise options
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSoma->setVisible ( true );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseDendritic->setVisible ( true );

  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseNeuron->setVisible ( true );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSpines->setChecked ( false );

  //Smooth
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setVisible ( false );

  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setChecked ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setChecked ( false );

  //Close all tabs except the loader
    for (int i = 0; i < 6; ++i) {
        ui.tabWidget_MainContainer->removeTab(0);
    }
    ui.tabWidget_MainContainer->setTabEnabled(1,hasPython);


  showSomaCreator ( );
}

void Neuronize::showSomaCreator ( )
{
  mActiveTab = 0;

    ui.tabWidget_MainContainer->removeTab(0);
    ui.tabWidget_MainContainer->insertTab(1, ui.tab_SomaCreator, "Generate Neuron");
    ui.tabWidget_MainContainer->insertTab(2, ui.tab_RepairMeshes, "Repair and/or Correct Meshes");
    ui.tabWidget_MainContainer->insertTab(3, ui.tab_CompareMeshes, "Compare Meshes");

    ui.tabWidget_MainContainer->setTabEnabled(1, true);
    ui.tabWidget_MainContainer->setTabEnabled(2, true);

    if (!hasPython) {
       ui.tabWidget_MainContainer->setTabEnabled(1,false);
    }

  mSomaCreatorWidget->deleteTreeViewer ( );
  mSomaCreatorWidget->resetInterface ( );
}

void Neuronize::showSomaDeformer ( )
{
    mActiveTab = 1;
  mSomaDeformerWidget->setSomaCreator(mSomaCreatorWidget);

  mFullSWCFilePath = mSomaCreatorWidget->getFullPathToSWCFile ( );

  ui.tabWidget_MainContainer->removeTab ( 0 );
  ui.tabWidget_MainContainer->insertTab ( 0, ui.tab_SomaGenerator, "Soma builder" );
  ui.tabWidget_MainContainer->setTabEnabled(1,false);
  ui.tabWidget_MainContainer->setTabEnabled(2,false);

  mSomaDeformerWidget->loadPredefinedXMLSomaDef();
  //mSomaDeformerWidget->startDeformation();
}

void Neuronize::showDendriteGenerator ( )
{
  mActiveTab = 2;


  ////Esto solohay que hacerse al terminar la deformaci�n (no en cada rebuild
  mSomaDeformerWidget->finalSomaOptimization();
  mSomaDeformerWidget->exportModelWithSTDName();
  mSomaDeformerWidget->stopDeformation();

  ui.tabWidget_MainContainer->removeTab ( 0 );
  ui.tabWidget_MainContainer->insertTab ( 0, ui.tab_DendritesGenerator, "Neurites/Spines builder" );
  ui.tabWidget_MainContainer->setCurrentIndex(0);

  mNeuroGeneratorWidget->setSpines(mSomaCreatorWidget->getSpines());
  mNeuroGeneratorWidget->setContours(mSomaCreatorWidget->getContours());

  //Noise options
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSoma->setVisible ( true );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseDendritic->setVisible ( true );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseNeuron->setVisible ( true );

  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSpines->setChecked ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSpines->setVisible ( false );

  //Smooth
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setChecked ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setVisible ( false );

  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setChecked ( true );

  mNeuroGeneratorWidget->showDendirtesTab ( );

  //Cargar el nuevo soma y generar la neurona
  mNeuroGeneratorWidget->getUI ( ).comboBox_SomaModels->clear ( );

  mNeuroGeneratorWidget->ReLoadSomaModelsFromPath ( tempDir.path() + "/SomaGenerated/" );

  //El metod anterior resetea el mSWCFIle ...
  mNeuroGeneratorWidget->setMorphologyFile ( mFullSWCFilePath );

  //Load the deformed soma
  mNeuroGeneratorWidget->loadSTDSoma(!mSomaCreatorWidget->isSomaContours());


  //mNeuroGeneratorWidget->getUI().checkBox_SmoothNeuron->setChecked(true);
  //mNeuroGeneratorWidget->getUI().checkBox_SmoothSpines->setChecked(false);

  mNeuroGeneratorWidget->getUI ( ).pushButton_GoToGenerateSpines->setEnabled ( false );
  mNeuroGeneratorWidget->getUI ( ).pushButton_FinishNeuronAndRestart->setEnabled ( false );
  mNeuroGeneratorWidget->getUI ( ).pushButton_SaveNeuronSurface->setEnabled ( false );
  mNeuroGeneratorWidget->getUI ( ).pushButton_SmoothDendrites->setEnabled ( false );

  mNeuroGeneratorWidget->getUI ( ).tabWidget_RenderControl->setCurrentIndex ( 0 );

  //mNeuroGeneratorWidget->getUI().pushButton_GoToGenerateSpines->setEnabled(false);
  mNeuroGeneratorWidget->getUI ( ).pushButton_NextStep->setEnabled ( false );
  mNeuroGeneratorWidget->destroyAllGroupsSpines ( );

  //Adapt the viewers
  mNeuroGeneratorWidget->getViewer ( )->camera ( )
                       ->setPosition ( mSomaDeformerWidget->getViewer ( )->camera ( )->position ( ));
  mNeuroGeneratorWidget->getViewer ( )->camera ( )
                       ->setOrientation ( mSomaDeformerWidget->getViewer ( )->camera ( )->orientation ( ));
  mNeuroGeneratorWidget->generateDendrites();
}

void Neuronize::showSpinesGenerator ( )
{
  mActiveTab = 3;

  ////Pasar la malla y la definicion al spines generator
  //ui.tabWidget_MainContainer->removeTab(0);
  //ui.tabWidget_MainContainer->insertTab(1,ui.tab_SpinesGenerator,"Spines generator");
          mNeuroGeneratorWidget->setNeuron(mSomaCreatorWidget->getNeuron());

  //Noise options
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSoma->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSoma->setChecked ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseDendritic->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseDendritic->setChecked ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseNeuron->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseNeuron->setChecked ( false );

  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSpines->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_NoiseSpines->setChecked ( true );

  //Smooth
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setChecked ( true );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothSpines->setVisible ( false );

  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setVisible ( false );
  mNeuroGeneratorWidget->getUI ( ).checkBox_SmoothNeuron->setChecked ( false );

  mNeuroGeneratorWidget->showSpinesTab ( );

  //mNeuroGeneratorWidget->getUI().checkBox_SmoothNeuron->setChecked(false);
  //mNeuroGeneratorWidget->getUI().checkBox_SmoothSpines->setChecked(true);

  mNeuroGeneratorWidget->getUI ( ).tabWidget_RenderControl->setCurrentIndex ( 0 );
}

void Neuronize::takeASnapshot ( )
{
  if (( mActiveTab == 1 )
    || ( mActiveTab == 2 )
    || ( mActiveTab == 3 ))
  {
    QString fileName = QFileDialog::getSaveFileName ( this, tr ( "Save snapshot" ), "./"
    );

    //QKeyEvent *lEvent = QKeyEvent(Qt::Key_Up);
    switch ( mActiveTab )
    {
      case 1:
        //mSomaDeformerWidget->getViewer()->takeSnapShot();
        mSomaDeformerWidget->getViewer ( )->saveSnapshot ( fileName + ".png" );
        break;

      case 2:
      case 3: mNeuroGeneratorWidget->getViewer ( )->saveSnapshot ( fileName + ".png" );
        break;
    }
  }
}

void Neuronize::showHelp ( )
{
  //if ( (mActiveTab==1)
  //|| (mActiveTab==2)
  //|| (mActiveTab==3) )
  //{
  //	//QKeyEvent *lEvent = QKeyEvent(Qt::Key_Up);
  //	switch (mActiveTab)
  //	{
  //		case 1:
  //		//	//mSomaDeformerWidget->getViewer()->takeSnapShot();
  //		//	mSomaDeformerWidget->getViewer()->help();
  //		//break;

  //		case 2:
  //		case 3:
  //		break;
  //	}
  //}

  mNeuroGeneratorWidget->getViewer ( )->help ( );
}

void Neuronize::genetareNeuronsInBatch (QString inputFilePath,QString outputFilePath,int subdivisions,QString baseName)
{

    int ret = QMessageBox::warning(this,"Neuronize","The process will take some time, please, do not close the interface (which could turn black). A notification will indicate the ending of the process.",QMessageBox::Ok | QMessageBox::Cancel);

    if (ret == QMessageBox::Cancel) {
        return;
    }

  mInputFilePath = inputFilePath;
  mOuputFilePath = outputFilePath;

  QString lBaseName = baseName;

  //Cargar todos los ficheros del directorio de entrada
  /*QDir directory;
  if ( mInputFilePath.isNull ( ) == false )
  {
    directory.setPath ( mInputFilePath );

    //viewer->loadSomaContainer(directory);

    QStringList filters;
    filters << "*.swc" << "*.SWC" << "*.asc" << "*.ASC";

    QStringList list = directory.entryList ( filters, QDir::Files );

    for ( int i = 0; i < list.size ( ); ++i )
    {
      QString mFileName = directory.absolutePath ( ) + "/" + list.at ( i );
      mFilesContainer.push_back ( mFileName );
    }
  }*/

  if ( !mInputFilePath.isNull( ) ) {
    QDirIterator it(mInputFilePath, QDir::AllEntries | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
      QFileInfo current = it.next();
      if (current.isFile()) {
        auto ext = current.suffix();
        if (ext == "asc" || ext == "ASC" || ext == "swc" || ext == "SWC") {
          mFilesContainer.emplace_back(current.absoluteFilePath(), nullptr);
        }

      } else if (current.isDir()) {
          QDir dir (current.absoluteFilePath());
          QDirIterator itFiles(dir.absolutePath(), QDir::AllEntries | QDir::NoDotAndDotDot);
          std::string apiFile;
          std::string longsFile;
          std::string volsFile;
          std::string somaFile;
          std::vector <std::string> basalFiles;
          while (itFiles.hasNext()) {
            QFileInfo info = itFiles.next();
            QString path = info.absoluteFilePath();
            if (info.isFile()) {
              auto name = info.baseName();
              if (name.contains("api", Qt::CaseInsensitive)) {
                apiFile = path.toStdString();
              } else if (name.contains("bas", Qt::CaseInsensitive)) {
                basalFiles.emplace_back(path.toStdString());
              } else if (name.contains("longs", Qt::CaseInsensitive)) {
                  longsFile = path.toStdString();
              } else if (name.contains("vols", Qt::CaseInsensitive)) {
                  volsFile = path.toStdString();
              } else if (name.contains("soma", Qt::CaseInsensitive)){
                somaFile = path.toStdString();
              }
            } else {
              QDirIterator itBasalFiles(path, QDir::AllEntries | QDir::NoDotAndDotDot);
              while (itBasalFiles.hasNext()) {
                basalFiles.emplace_back(itBasalFiles.next().toStdString());
              }
            }
          }
          std::string resultFile = Neuronize::tmpPath.toStdString() ;
          std::string name = dir.dirName().toStdString();
          resultFile.append("/").append(name).append(".asc");

          float threshold = 0.1f;
          auto neuron = new skelgenerator::Neuron( apiFile, basalFiles,
                                                   somaFile, volsFile,
                                                   longsFile, threshold );
          while (neuron->getReamingSegments() > 0) {
              threshold += 0.1f;
              neuron->reComputeSkel(threshold);
          }
          std::ofstream file;
          file.open(resultFile);
          file << neuron->to_asc();
          file.close();
          mFilesContainer.emplace_back(QString::fromStdString(resultFile),neuron);
        }
      }
    }


  for ( unsigned int i = 0; i < mFilesContainer.size ( ); ++i )
  {
    showSomaCreator ( );
    auto filePath = std::get<0>(mFilesContainer[i]);
    auto neuron = std::get<1>(mFilesContainer[i]);
    std::cout << "Init: " << filePath.toStdString() << std::endl;

      QFileInfo f ( filePath );
      QString lFileName = f.fileName ( );

      QString lTmpId = "";
      if ( i < 10 )
          lTmpId = "00" + QString::number ( i );
      else if ( i < 100 )
          lTmpId = "0" + QString::number ( i );
      else
          lTmpId = QString::number ( i );

      QString lTmpPath = mOuputFilePath + "/" + lBaseName + lTmpId;
      QDir ( ).mkdir ( lTmpPath );
      Neuronize::outPath = lTmpPath;

    mSomaCreatorWidget->generateXMLSoma ( filePath, true );
    auto spines = mSomaCreatorWidget->getSpines();

    //Luego pasar al somaDeformer y hacer el build
    showSomaDeformer ( );
    mSomaDeformerWidget->startDeformation ( );
    mSomaDeformerWidget->deformDuringNSteps ( 1000 );
    mSomaDeformerWidget->finalizeSoma ( );

    std::cout << "Deformer" << std::endl << std::flush;

    //Luego pasar al NeuriteGenerator y hacer el build
    mNeuroGeneratorWidget->setNeuron(mSomaCreatorWidget->getNeuron());
    showDendriteGenerator ( );
    mNeuroGeneratorWidget->loadNeuronDefinitionAndGenerateMeshBatch ( );

    std::cout << "Dendrites" << std::endl << std::flush;

    //Suavizado
    mNeuroGeneratorWidget->applySmooth ( subdivisions, 0, 0, 0 );



    mNeuroGeneratorWidget->exportNeuron ( lTmpPath + "/" + f.baseName() );

    //Copiar el .SWC
    QFile::copy ( filePath, lTmpPath + "/" + lFileName );
    QFile::copy ( lTmpPath + "/" + lFileName + ".obj.xml", lTmpPath + "/" + lBaseName + lTmpId + ".xml" );

    //Spines
    std::cout << "Spines" << std::endl << std::flush;

    mNeuroGeneratorWidget->batchSpinesGeneration(neuron, spines);
    lTmpId = "";
    if ( i < 10 )
      lTmpId = "00" + QString::number ( i );
    else if ( i < 100 )
      lTmpId = "0" + QString::number ( i );
    else
      lTmpId = QString::number ( i );

    mNeuroGeneratorWidget->exportSpinesInmediatly ( lTmpPath + "/" + f.baseName() + "_Spines.obj" );

    //Luego volver al inicio
  }
  QMessageBox::information(this,"Neuronize","Task Finished",QMessageBox::Ok);
}

void Neuronize::actionNewNeuron ( )
{
  QString text = QObject::tr ( "Are you sure you build a new neuron?" );
  QMessageBox msgBox ( QObject::tr ( "Restart" ),
                       text,
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton );
  if ( msgBox.exec ( ) == QMessageBox::Yes )
  {
    resetNeuronnizeInterface ( );
  }
}


void Neuronize::actionQuit ( )
{
  QString text = QObject::tr ( "Are you sure you want to quit?" );
  QMessageBox msgBox ( QObject::tr ( "Quit" ),
                       text,
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton );
  if ( msgBox.exec ( ) == QMessageBox::Yes )
  {
    close ( );
  }
}

void Neuronize::actionAbout ( )
{
  QMessageBox Msgbox;
  QString lMsge = "";

  lMsge =
    "Neuronize is a C++ standalone tool to build neurons and spines from computer aided tracing files (.swc and .asc).\n\n";

  lMsge += "Version: 0.3\n\n";

  lMsge += "Developed in:\n";
  lMsge += "	GMRV:Grupo de Modelado y Realida Virtual (Spain).\n";
  lMsge += "	URJC:Universidad Rey Juan Carlos (Spain).\n";
  lMsge += "	UPM:Universidad Polit�cnica de Madrid (Spain).\n";
  lMsge += "	CSIC:Centro Superior de Investigaciones Cient�ficas.\n";
  lMsge += "	Cajal Blue Brain Project, Spanish partner of the Blue Brain Project initiative from EPFL\n\n";

  lMsge += "Neuronize uses Qt, libQGLViwer, Boost, OpenMesh\n";

  Msgbox.setWindowTitle ( "Neuronize" );
  Msgbox.setText ( lMsge );
  Msgbox.exec ( );
}

void Neuronize::actionBack ( )
{
  switch ( mActiveTab )
  {
    case 0:
    case 1: showSomaCreator ( );
      break;
    case 2: showSomaDeformer ( );
      break;
    case 3: showDendriteGenerator ( );
      break;
  }
}

void Neuronize::NewNeuronQuestionAndRestart ( )
{
  QString text = QObject::tr ( "Would you like to build a new neuron?" );
  QMessageBox msgBox ( QObject::tr ( "New neuron?" ),
                       text,
                       QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton );
  if ( msgBox.exec ( ) == QMessageBox::Yes )
  {
    showSomaCreator ( );
  }
}

void Neuronize::onSomaBuildFinish() {
    this->showMaximized();
    showSomaDeformer();
}

void Neuronize::showExportDialog(){
    ExportDialog dialog;
    dialog.exec();
}

void Neuronize::deleteDatabase() {
    int ret = QMessageBox::question(this,tr("Neuronize"),tr("Are you sure you want to delete the database?"));

    if (ret == QMessageBox::Yes) {
        QFile::remove(QString::fromStdString(Neuronize::bbdd.getFile()));
        createDatabase();
    }
}

void Neuronize::createDatabase() {
    QString path = Neuronize::configPath + "/neuronize.sqlite";
    Neuronize::bbdd = BBDD::BBDD(path.toStdString());
}

void Neuronize::resetPythonEnv() {
    QFuture<void> future = QtConcurrent::run([&]() {meshreconstruct::MeshReconstruct::getInstance()->resetPythonEnv();});
    QFutureWatcher<void> watcher;
    QProgressDialog progress(this);
    connect(&watcher, SIGNAL(finished()), &progress, SLOT(close()));
    watcher.setFuture(future);
    progress.setLabelText("Installing python dependencies");
    progress.setCancelButton(0);
    progress.setMaximum(0);
    progress.setMinimum(0);
    progress.exec();

    hasPython = meshreconstruct::MeshReconstruct::getInstance()->isInit();
    if (!hasPython)
    {
        QString message("Python 3 not found. Mesh repair is disabled");
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setText(message);
        msgBox.exec();
    }
    resetNeuronnizeInterface();
}


void Neuronize::initPythonEnv() {

  QFuture<void> future = QtConcurrent::run([&]() {meshreconstruct::MeshReconstruct::getInstance();});
  QFutureWatcher<void> watcher;
  QProgressDialog progress(this);
  connect(&watcher, SIGNAL(finished()), &progress, SLOT(close()));
  watcher.setFuture(future);
  progress.setLabelText("Installing python dependencies");
  progress.setCancelButton(0);
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.exec();
  hasPython = meshreconstruct::MeshReconstruct::getInstance()->isInit();
  if (!hasPython)
  {
    QString message("Python 3 not found. Mesh repair is disabled");
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setText(message);
    msgBox.exec();

    ui.tabWidget_MainContainer->setTabEnabled(1,false);
  }

}

void Neuronize::exportDatabase() {
    QString fileName = QFileDialog::getSaveFileName ( this, tr ( "Save Database" ), "./");
    if (!fileName.isEmpty()) {
        QFile::copy(QString::fromStdString(Neuronize::bbdd.getFile()),fileName);
    }


}
