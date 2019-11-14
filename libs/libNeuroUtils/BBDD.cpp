//
// Created by ivelascog on 20/05/19.
//

#include "BBDD.h"
#include <boost/format.hpp>
#include <fstream>
#include <boost/filesystem.hpp>
#include <libs/libNeuroUtils/MeshVCG.h>
#include <QCoreApplication>
#include <QDirIterator>
#include <clocale>


#define ERRCHECK {if (_err!=NULL) {std::cerr << "BBDD Error : " << _err << "\n" << std::endl; sqlite3_free(_err);}}

static int getSpineCallback(void *spines, int columns, char **data, char **columnNames) {
    auto spinesCast = (std::vector<BBDD::Spine> *) spines;

    BBDD::Spine spine;
    spine.id = std::stoi(data[0]);
    spine.ext = (BBDD::FileType) std::stoi(data[1]);
    spine.file = std::string(data[2]);
    spinesCast->push_back(spine);
    return 0;
}

static int countCallback(void *count, int columns, char **data, char **columnNames) {
   int* countCast = (int *) count;
   *countCast = std::stoi(data[0]);
    return 0;
}

static int getSomaInfoCallback(void *soma, int columns, char **data, char **columnNames) {
    auto somaCast = (BBDD::Soma*) soma;
    somaCast->area = std::stof(data[0]);
    somaCast->area2D = std::stof(data[1]);
    somaCast->volume = std::stof(data[2]);
    somaCast->massCenter[0] = std::stof(data[3]);
    somaCast->massCenter[1] = std::stof(data[4]);
    somaCast->massCenter[2] = std::stof(data[5]);
    return 0;
}

static int getSpineInfoCallBack(void* spines, int columns,char **data,char **comlumnNames) {
    auto spinesCast = (std::vector<BBDD::Spine> *) spines;
    BBDD::Spine spine;
    spine.area = std::stof(data[0]);
    spine.volume = std::stof(data[1]);
    spine.origin = (BBDD::SpineOrigin) std::stoi(data[2]);
    spine.massCenter[0] = std::stof(data[3]);
    spine.massCenter[1] = std::stof(data[4]);
    spine.massCenter[2] = std::stof(data[5]);
    spine.name = data[6];
    if (data[7] != nullptr) {
        spine.displacement[0] = std::stof(data[7]);
        spine.displacement[1] = std::stof(data[8]);
        spine.displacement[2] = std::stof(data[9]);
    } else {
        spine.displacement = {0,0,0};
    }

    if (data[10] != nullptr) {
        spine.q[0] = std::stof(data[10]);
        spine.q[1] = std::stof(data[11]);
        spine.q[2] = std::stof(data[12]);
        spine.q[3] = std::stof(data[13]);
    } else {
        spine.q.SetIdentity();
    }
    spinesCast->push_back(spine);
    return 0;
}

static int getNeuronNamesCallBack(void* names,int columns,char** data,char** columnNames) {
    auto namesCast = (std::vector<std::string>*) names;
    namesCast->push_back(data[0]);
    return 0;
}


namespace BBDD {

    const std::vector<std::string> BBDD::neuriteTypeDesc = {"Basal","Apical","Axon"};

    const std::vector<std::string> BBDD::fileTypeDesc = {"OBJ", "STL"};

    const std::vector<std::string> BBDD::spineOriginDesc = {"Imaris","FilamentTracer","Neurolucida","Neuronize"};

    const std::vector<std::string> BBDD::reconstructionMethodDesc = {"Spring-Mass","FEM"};




    BBDD::BBDD(const std::string &filename) {
        this->file = filename;
        if (!boost::filesystem::exists(filename)) {
            sqlite3_open(filename.c_str(), &_db);
            createSchema();
        } else {
            sqlite3_open(filename.c_str(), &_db);
        }


    }

    std::vector<char> BBDD::readBytes(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
        std::ifstream::pos_type pos = ifs.tellg();

        std::vector<char> result(pos);

        ifs.seekg(0, std::ios::beg);
        ifs.read(result.data(), pos);

        return result;
    }

    void BBDD::createSchema() {

        //Creacion del esquema
        std::string create_neuron = "CREATE TABLE NEURON (ID TEXT PRIMARY KEY NOT NULL,\n"
                                    "                     SWC_FILE BLOB NOT NULL,\n"
                                    "                     AREA REAL,\n"
                                    "                     VOLUME REAL);";

        std::string create_reconstructionMethod = "CREATE TABLE RECONSTRUCTION_METHOD(ID INTEGER PRIMARY KEY NOT NULL,\n"
                                                  "                                   DESCRIPTION TEXT);";

        std::string create_soma = "CREATE TABLE SOMA (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                  "                   AREA REAL,\n"
                                  "                   AREA_2D REAL,\n"
                                  "                   VOLUME REAL,\n"
                                  "                   MASS_CENTER_X REAL,\n"
                                  "                   MASS_CENTER_Y REAL,\n"
                                  "                   MASS_CENTER_Z REAL,\n"
                                  "                   NEURON TEXT NOT NULL,\n"
                                  "                   MODEL BLOB,\n"
                                  "                   RECONSTRUCTION_METHOD INTEGER,\n"
                                  "                   RECONSTRUCTION_PARAMETES BLOB,\n"
                                  "                   FOREIGN KEY (RECONSTRUCTION_METHOD) REFERENCES RECONSTRUCTION_METHOD(ID),\n"
                                  "                   FOREIGN KEY (NEURON) REFERENCES NEURON(ID) );";

        std::string create_contours = "CREATE TABLE CONTOURS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                      "                      SOMA INTEGER,\n"
                                      "                      FOREIGN KEY (SOMA) REFERENCES SOMA(ID));";

        std::string create_point = "CREATE TABLE POINT (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                   "                    X REAL,\n"
                                   "                    Y REAL,\n"
                                   "                    Z REAL,\n"
                                   "                    CONTOUR INTEGER,\n"
                                   "                    FOREIGN KEY(CONTOUR) REFERENCES CONTOURS(ID));";

        std::string create_spinesOrigin = "CREATE TABLE SPINE_ORIGIN(ID INTEGER PRIMARY KEY NOT NULL,\n"
                                          "                           DESCRIPTION TEXT NOT NULL);";

        std::string create_FileType = "CREATE TABLE FILE_TYPE(ID INTEGER PRIMARY KEY NOT NULL,\n"
                                      "                       EXTENSION VARCHAR(7));";

        std::string create_SpineModel = "CREATE TABLE SPINE_MODEL(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                        "            AREA REAL,\n"
                                        "            VOLUME REAL,\n"
                                        "            NAME TEXT,\n"
                                        "            MASS_CENTER_X REAL,\n"
                                        "            MASS_CENTER_Y REAL,\n"
                                        "            MASS_CENTER_Z REAL,\n"
                                        "            ORIGIN INT,\n"
                                        "            MODEL BLOB NOT NULL,\n"
                                        "            MODEL_NR BLOB,\n"
                                        "            FILE_TYPE INTEGER NOT NULL,\n"
                                        "            FOREIGN KEY (ORIGIN) REFERENCES SPINES_ORIGIN(ID),\n"
                                        "            FOREIGN KEY (FILE_TYPE) REFERENCES FILE_TYPE(ID));";

        std::string create_spines = "CREATE TABLE SPINES (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                    "            NEURON TEXT NOT NULL,\n"
                                    "            SPINE_MODEL INTEGER,\n"
                                    "            TRANSLATION_X REAL,\n"
                                    "            TRANSLATION_Y REAL,\n"
                                    "            TRANSLATION_Z REAL,\n"
                                    "            QUATERNION_1 REAL,\n"
                                    "            QUATERNION_2 REAL,\n"
                                    "            QUATERNION_3 REAL,\n"
                                    "            QUATERNION_4 REAL,\n"
                                    "            FOREIGN KEY (NEURON) REFERENCES NEURON(ID),\n"
                                    "            FOREIGN KEY (SPINE_MODEL) REFERENCES SPINE_MODEL(ID));";

        std::string create_neuriteTypes = "CREATE TABLE NEURITE_TYPES(ID INTEGER PRIMARY KEY NOT NULL,\n"
                                          "                           DESCRIPTION TEXT NOT NULL);";

        std::string create_neurites = "CREATE TABLE NEURITES (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                                      "            AREA REAL,\n"
                                      "            VOLUME REAL,\n"
                                      "            TYPE INTEGER,\n"
                                      "            FIRST_COUNTER INTEGER NOT NULL,\n"
                                      "            LAST_COUNTER INTEGER NOT NULL,\n"
                                      "            NEURON TEXT NOT NULL,\n"
                                      "            CONSTRAINT neurites_info UNIQUE (NEURON,FIRST_COUNTER,LAST_COUNTER),\n"
                                      "            FOREIGN KEY(TYPE) REFERENCES NEURITE_TYPES(ID),\n"
                                      "            FOREIGN KEY (NEURON) REFERENCES NEURON(ID));";

        sqlite3_exec(_db, create_neuron.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_reconstructionMethod.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_soma.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_contours.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_point.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_spinesOrigin.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_FileType.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_SpineModel.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_spines.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_neuriteTypes.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        sqlite3_exec(_db, create_neurites.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        // Añadir los intendificadores predeterminados
        std::string query = "INSERT INTO FILE_TYPE VALUES (%i,'%s');";
        for (int i = 0; i < fileTypeDesc.size(); ++i) {
            std::string formatedQuery = str( boost::format(query) % i % fileTypeDesc[i]);
            sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
            ERRCHECK
        }

        query = "INSERT INTO NEURITE_TYPES VALUES (%i,'%s');";

        for(int i = 0; i < neuriteTypeDesc.size(); i++) {
            std::string formatedQuery = str( boost::format(query) % i % neuriteTypeDesc[i]);
            sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
            ERRCHECK
        }

        query = "INSERT INTO SPINE_ORIGIN VALUES (%i,'%s');";

        for (int i = 0; i < spineOriginDesc.size(); ++i) {
            std::string formatedQuery = str (boost::format(query) % i % spineOriginDesc[i]);
            sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
            ERRCHECK
        }

        query = "INSERT INTO RECONSTRUCTION_METHOD VALUES (%i,'%s');";

        for (int i = 0; i < reconstructionMethodDesc.size(); ++i) {
            std::string formatedQuery = str(boost::format(query) % i % reconstructionMethodDesc[i]);
            sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
            ERRCHECK
        }

        addDefaultSpinesModels();


    }

    void BBDD::addSpine(const std::string &name, float area, float volume, const std::string &neuronName, int origin,
                        const std::string &file) {
        std::string query = "INSERT INTO SPINES (NAME, AREA, VOLUME, NEURON, ORIGIN, MODEL) VALUES ('%s',%f,%f,'%s',%i,'%x')";
        std::string file2 = readBytes(file).data();
        std::string formatedQuery = str(
                boost::format(query) % name % area % volume % neuronName % origin % readBytes(file).data());
        sqlite3_exec(_db, formatedQuery.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

    }

    bool BBDD::addNeuron(const std::string &name, const std::string& swcFile) {
        std::string query = "INSERT INTO NEURON (ID,SWC_FILE) VALUES ('%s','%x');";
        std::string file = readBytes(swcFile).data();
        std::string formatedQuery = str( boost::format(query) % name % file);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        if (_err!=NULL) {
            std::string err (_err);
            if (err.find("UNIQUE constraint failed: NEURON.ID") != std::string::npos)  {
                sqlite3_free(_err);
                return true;
            } else {
                std::cerr << "BBDD Error : " << _err << "\n" << std::endl;
                sqlite3_free(_err);
                return false;
            }
        }

        return false;
    }

    void BBDD::addSoma(const std::string& neuronName, MeshVCG& model,ReconstructionMethod reconstructionMethod,std::vector<std::vector<OpenMesh::Vec3d>> contours) {
        std::string query = "INSERT INTO SOMA (AREA, AREA_2D, VOLUME, NEURON, MODEL, RECONSTRUCTION_METHOD, MASS_CENTER_X, MASS_CENTER_Y,MASS_CENTER_Z) "
                            "VALUES (%f,%f,%f,'%s','%x',%i,%f,%f,%f)";
        float area = model.getArea();
        float volume = model.getVolume();
        float area2D = model.getMax2DArea(0.1f);
        auto massCenter = model.getCenter();
        std::string file = readBytes(model.getPath()).data();
        std::string formatedQuery = str(
                boost::format(query) % area % area2D % volume % neuronName % file % reconstructionMethod % massCenter[0] % massCenter[1] % massCenter[2]);
        sqlite3_exec(_db, formatedQuery.c_str(), nullptr, nullptr, &_err);
        ERRCHECK

        if (!contours.empty()) {
            openTransaction();

            query = "INSERT INTO CONTOURS (SOMA) VALUES (" + sqlite3_last_insert_rowid(_db) + std::string(");");
            std::string queryPoint = "INSERT INTO POINT (X, Y, Z, CONTOUR) VALUES (%f,%f,%f,%i);";
            for (const auto &contour: contours) {
                sqlite3_exec(_db, query.c_str(), nullptr, nullptr, &_err);
                ERRCHECK
                int contourId = sqlite3_last_insert_rowid(_db);
                for (const auto &point: contour) {
                    std::string formatedQuery = str(boost::format(queryPoint) % point[0] % point[1] % point[2] % contourId);
                    sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
                    ERRCHECK
                }
            }

            closeTransaction();
        }
    }

    void BBDD::addDendrite(const std::string &neuronName, int initCounter, int lastCounter, NSSWCImporter::DendriticType type) {
        std::string query = "INSERT INTO NEURITES (TYPE, FIRST_COUNTER, LAST_COUNTER, NEURON) VALUES (%i,%i,%i,'%s');";
        std::string formatedQuery = str(boost::format(query) % type % initCounter % lastCounter % neuronName);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK

    }

    vcg::Matrix44d BBDD::orientSpine(MeshVCG& mesh,const skelgenerator::Spine* const spine) {
        std::vector<skelgenerator::SamplePoint> medialAxis = spine->getMedialAxisWithoutBase();
        auto const& firstPoint = medialAxis[0].getPoint();
        
        auto normalm = spine->getSpineOrientation();

        Eigen::Vector3d normal (normalm[0],normalm[1],normalm[2]);
        normal.normalize();

        float angleX = std::atan2(normal[2],normal[1]);
        auto rotX = Eigen::AngleAxisd (-angleX,Eigen::Vector3d(1,0,0));
        normal = rotX * normal;

        float angleZ = std::atan2(normal[0],normal[1]);
        auto rotZ = Eigen::AngleAxisd (angleZ,Eigen::Vector3d(0,0,1));
        normal = rotZ * normal;

        vcg::Matrix44d translation;
        translation.SetTranslate(-firstPoint[0], -firstPoint[1], -firstPoint[2]);

        vcg::Matrix44d rot1;
        rot1.SetRotateRad(-angleX,{1,0,0});

        vcg::Matrix44d rot2;
        rot2.SetRotateRad(angleZ,{0,0,1});

        vcg::Matrix44d transform = rot2 * rot1 * translation;

        mesh.applyMatrix(transform);
        return transform;
    }

    void BBDD::addSpineImaris(const std::string& originalSpine, const std::string& repairedSpine, const std::string& ext,const std::string& name) {
        std::string query = "INSERT INTO SPINE_MODEL (AREA, VOLUME, ORIGIN, MODEL, MODEL_NR, FILE_TYPE, MASS_CENTER_X, MASS_CENTER_Y, MASS_CENTER_Z,NAME) "
                            "VALUES (%f,%f,%i,'%x','%x',%i,%f,%f,%f,'%x');";
        MeshVCG mesh (repairedSpine);
        float area = mesh.getArea();
        float volume = mesh.getVolume();
        auto massCenter = mesh.getCenter();

        auto bytes = readBytes(repairedSpine);
        std::string repairedFile (bytes.begin(), bytes.end());
        bytes = readBytes(originalSpine);
        std::string originalFile (bytes.begin(), bytes.end());

        FileType fileType;

        if (ext == "Obj") {
            fileType = Obj;
        } else if (ext == "Stl") {
            fileType = Stl;
        }

        std::string formatedQuery = str(boost::format(query) % area % volume % Imaris % repairedFile % originalFile % fileType % massCenter[0] % massCenter[1] % massCenter[2] % name);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK
    }

    void BBDD::addSpineImarisNeuron(const std::string& originalSpine, const std::string& repairedSpine,const std::string& name, const std::string& neuronName) {
        addSpineImaris(originalSpine,repairedSpine,"Obj",name);
        std::string query = "INSERT INTO SPINES (NEURON,SPINE_MODEL) VALUES ('%x',%i)";
        std::string formatedQuery = str(boost::format(query) % neuronName % sqlite3_last_insert_rowid(_db));
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK

    }

    void BBDD::addSpineImarisNeuron(const std::string& originalSpine,const std::string& name, const std::string& neuronName){
        std::string query = "INSERT INTO SPINE_MODEL (AREA, VOLUME, ORIGIN, MODEL_NR, FILE_TYPE, MASS_CENTER_X, MASS_CENTER_Y, MASS_CENTER_Z,NAME) "
                            "VALUES (%f,%f,%i,'%x',%i,%f,%f,%f,'%x');";
        MeshVCG mesh (originalSpine);
        float area = mesh.getArea();
        float volume = mesh.getVolume();
        auto massCenter = mesh.getCenter();

        auto bytes = readBytes(originalSpine);
        std::string originalFile (bytes.begin(), bytes.end());

        std::string formatedQuery = str(boost::format(query) % area % volume % Imaris  % originalFile % "Obj" % massCenter[0] % massCenter[1] % massCenter[2] % name);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK
    }


    void BBDD::addSpineFilament(const skelgenerator::Spine* const spine, const std::string& meshPath, const std::string& neuronName, const std::string& tmpDir, const OpenMesh::Vec3f& displacement) {
        std::string query = "INSERT INTO SPINE_MODEL (AREA, VOLUME, ORIGIN, MODEL, File_TYPE, MASS_CENTER_X, MASS_CENTER_Y, MASS_CENTER_Z)"
                            " VALUES (%f,%f,%i,'%x',%i,%f,%f,%f);";
        MeshVCG mesh (meshPath);
        float area = mesh.getArea();
        float volume = mesh.getVolume();
        auto massCenter = mesh.getCenter();

        auto transform = orientSpine(mesh,spine);
        mesh.toObj(tmpDir + "spine.obj");
        auto bytes = readBytes(tmpDir + "spine.obj");
        std::string file (bytes.begin(),bytes.end());

        std::string formatedQuery = str(boost::format(query) % area % volume % FilamentTracer % file % Obj % massCenter[0] % massCenter[1] % massCenter[2] );
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK

        OpenMesh::Vec3f tranlation (transform.ElementAt(0,3),transform.ElementAt(1,3),transform.ElementAt(2,3));
        tranlation -= displacement;
        transform.SetColumn(3,{0,0,0}); //Eliminar la traslacion
        vcg::Quaterniond q;
        q.FromMatrix(transform);
        query = "INSERT INTO SPINES (NEURON, SPINE_MODEL, TRANSLATION_X, TRANSLATION_Y, TRANSLATION_Z, QUATERNION_1, QUATERNION_2, QUATERNION_3, QUATERNION_4) VALUES('%s',%i,%f,%f,%f,%f,%f,%f,%f);";
        formatedQuery = str(boost::format(query) % neuronName % sqlite3_last_insert_rowid(_db) % tranlation[0] % tranlation[1] % tranlation[2] % q[0] % q[1] %q[2] %q[3]);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        ERRCHECK
    }


    void BBDD::addSpine(const std::string& neuronName, int spineModel, const OpenMesh::Vec3f& displacement,const boost::numeric::ublas::matrix<float>& transform) {

        vcg::Matrix44f rot = vcg::Matrix44f::Identity();
        for (int i =0; i < 3; i++) {
            for (int j = 0 ; j < 3 ; j++) {
                rot.ElementAt(i,j) = transform(i,j);
            }
        }

        vcg::Quaternionf q;
        q.FromMatrix(rot);

        std::string query = "INSERT INTO SPINES (NEURON, SPINE_MODEL, TRANSLATION_X, TRANSLATION_Y, TRANSLATION_Z, QUATERNION_1, QUATERNION_2, QUATERNION_3, QUATERNION_4) VALUES('%s',%i,%f,%f,%f,%f,%f,%f,%f);";
        std::string formatedQuery = str(boost::format(query) % neuronName % spineModel % displacement[0] % displacement[1] % displacement[2] % q[0] % q[1] %q[2] %q[3]);
        sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
        std::cout << "Spine Added" << std::endl;
        ERRCHECK
    }

    void BBDD::addDefaultSpinesModels() {
        QString spinesPath = QCoreApplication::applicationDirPath() + "/Content/Spines/Low";
        QDir spinesDir (spinesPath);
        spinesDir.setNameFilters(QStringList() << "*.obj");

        std::string query = "INSERT INTO SPINE_MODEL(ID, AREA, VOLUME, ORIGIN, MODEL, FILE_TYPE,MASS_CENTER_X,MASS_CENTER_Y,MASS_CENTER_Z,NAME) VALUES (%i,%f,%f,%i,'%x',%i,%f,%f,%f,'%x')";

        this->openTransaction();
        for (const auto& spine:spinesDir.entryInfoList()) {
            MeshVCG mesh (spine.filePath().toStdString());
            float area = mesh.getArea();
            float volume = mesh.getVolume();
            auto center = mesh.getCenter();
            QString name = spine.baseName();
            int id = name.mid(1,name.size() - 2).toInt();
            std::string spineName = std::string("Default-") + std::to_string(id);
            std::string file = readBytes(spine.filePath().toStdString()).data();
            std::string formatedQuery = str(boost::format(query) % id % area % volume % Neuronize % file % Obj % center[0] % center[1] % center[2] % spineName);
            sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
            ERRCHECK
        }
        this->closeTransaction();
    }

    void BBDD::openTransaction() {
        sqlite3_exec(_db,"BEGIN TRANSACTION", nullptr, nullptr,&_err);
        ERRCHECK

    }

    void BBDD::closeTransaction() {
        sqlite3_exec(_db,"COMMIT", nullptr, nullptr,&_err);
        ERRCHECK
    }

    std::vector<std::tuple<int,std::string>> BBDD::getRandomSpines(int n,const std::string& tmpPath) {
        std::string query = "SELECT ID,FILE_TYPE,MODEL FROM SPINE_MODEL WHERE ORIGIN == 1 OR ORIGIN == 3 ORDER BY RANDOM() LIMIT " + std::to_string(n);
        std::vector<Spine> spines;
        spines.reserve(n);
        sqlite3_exec(_db,query.c_str() ,getSpineCallback,&spines,&_err);
        ERRCHECK

        std::vector<std::tuple<int,std::string>> spinesOut;
        for (int i =0 ; i < spines.size(); i++) {
            const auto& spine = spines[i];
            std::string filePath = tmpPath + "/spine" + std::to_string(i) +  "." + fileTypeDesc[spine.ext];
            std::ofstream file (filePath);
            file << spine.file;
            file.close();
            spinesOut.emplace_back(spine.id,filePath);
        }

        return spinesOut;

    }

    bool BBDD::haveSpinesNeuron(const std::string &neuronName) {
        std::string query = "SELECT COUNT(ID) from SPINES WHERE NEURON == %s';";
        std::string formatedQuery = str(boost::format(query) % neuronName);
        int count;
        sqlite3_exec(_db,formatedQuery.c_str(),countCallback,&count,&_err);
        return count != 0;
    }

    void BBDD::exportNeuron(const std::string& id,const std::string& path) {
        std::setlocale(LC_NUMERIC, "en_US.UTF-8");
        std::string querySoma = "SELECT\n"
                                "    SOMA.AREA,SOMA.AREA_2D,SOMA.VOLUME,SOMA.MASS_CENTER_X,SOMA.MASS_CENTER_Y,SOMA.MASS_CENTER_Z\n"
                                "FROM\n"
                                "     NEURON\n"
                                "INNER JOIN SOMA on NEURON.ID = SOMA.NEURON\n"
                                "where NEURON.ID = '%x'";

        std::string formatedQuerySoma = str(boost::format(querySoma) % id);
        Soma soma{};
        sqlite3_exec(_db,formatedQuerySoma.c_str(),getSomaInfoCallback,&soma,&_err);
        ERRCHECK

        std::string querySpines = "SELECT\n"
                                  "    SM.AREA,SM.VOLUME,SM.ORIGIN,SM.MASS_CENTER_X,SM.MASS_CENTER_Y,SM.MASS_CENTER_Z,SM.NAME,\n"
                                  "    SPINES.TRANSLATION_X,TRANSLATION_Y,TRANSLATION_Z,QUATERNION_1,QUATERNION_2,QUATERNION_3,QUATERNION_4\n"
                                  "FROM\n"
                                  "     NEURON\n"
                                  "     INNER JOIN SPINES on NEURON.ID = SPINES.NEURON\n"
                                  "     INNER JOIN SPINE_MODEL SM on SPINES.SPINE_MODEL = SM.ID\n"
                                  "where NEURON.ID = '%x'";

        std::string formatedQuerySpines = str(boost::format(querySpines) % id);
        std::vector<Spine> spines;
        sqlite3_exec(_db,formatedQuerySpines.c_str(),getSpineInfoCallBack,&spines,&_err);

        std::string outputDir = path + "/" + id;
        boost::filesystem::create_directory(outputDir);

        std::ofstream spinesStream;
        spinesStream.open (outputDir + "/spines.csv",std::ofstream::out);
        spinesStream << "Name;Area;Volume;Mass Center-x;Mass Center-y;Mass Center-z;Origin" << std::endl;
        for (const auto& spine:spines) {
            vcg::Point3f massCenter(spine.massCenter[0], spine.massCenter[1], spine.massCenter[2]);
            massCenter = spine.q.Rotate(massCenter);
            massCenter[0] += spine.displacement[0];
            massCenter[1] += spine.displacement[1];
            massCenter[2] += spine.displacement[2];


            spinesStream << spine.name << ";" << spine.area << ";" << spine.volume << ";" << massCenter[0] << ";" <<
                            massCenter[1] << ";" << massCenter[2] << ";" << spineOriginDesc[spine.origin]
                            << std::endl;
        }
        spinesStream.close();

        std::ofstream somaStream;
        somaStream.open (outputDir + "/soma.csv",std::ofstream::out);
        somaStream << "Area;Area2D;Volume;Mass Center-x;Mass Center-y;Mass Center-z" << std::endl;
        somaStream << soma.area << ";" << soma.area2D << ";" << soma.volume << ";" << soma.massCenter[0] << ";" <<
                      soma.massCenter[1] << ";" << soma.massCenter[2] << std::endl;
        somaStream.close();
    }

    std::vector<std::string> BBDD::getNeuronsNames(){
        std::string query = "SELECT ID FROM NEURON;";
        std::vector<std::string> names;
        sqlite3_exec(_db,query.c_str(),getNeuronNamesCallBack,&names,&_err);
        ERRCHECK
        return names;
    }

    const string &BBDD::getFile() const {
        return file;
    }

}


