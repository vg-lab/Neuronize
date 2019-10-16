//
// Created by ivelascog on 20/05/19.
//

#ifndef NEURONIZE_BBDD_H
#define NEURONIZE_BBDD_H

#include <neuroutils/api.h>

#include <string>
#include <vector>
#include <map>
#include <libs/libNeuroUtils/MeshVCG.h>
#include <libs/libNeuroUtils/sqlite3.h>
#include <libs/libNeuroUtils/SWCImporter.h>
#include <SkelGeneratorUtil/Spine.h>

namespace  BBDD {
    enum FileType {
        Obj, Stl
    };
    enum SpineOrigin {
        Imaris,FilamentTracer,Neurolucida,Neuronize
    };

    enum ReconstructionMethod {
        Spring_Mass,FEM
    };

    struct Spine {
        int id;
        float area, volume;
        std::string file, name;
        FileType ext;
        SpineOrigin origin;
        OpenMesh::Vec3f massCenter, displacement;
        vcg::Quaternionf q;
    };

    struct Soma {
        float area, area2D, volume;
        OpenMesh::Vec3f massCenter;
    };


    class NEUROUTILS_API BBDD {
        sqlite3 *_db;
        char *_err = NULL;
    public:
        BBDD () = default;
        explicit BBDD(const std::string &filename);

        void addSpine(const std::string &name, float area, float volume, const std::string &neuronName, int origin,
                      const std::string &file);
        bool addNeuron(const std::string& name, const std::string& swcFile);

        void addSoma(const std::string& neuronName, MeshVCG& model,ReconstructionMethod reconstructionMethod,std::vector<std::vector<OpenMesh::Vec3d>> contours);

        void addDendrite(const std::string& neuronName,int initCounter,int lastCounter,NSSWCImporter::DendriticType type);

        void addSpineVRML(const skelgenerator::Spine* const spine,const std::string& meshPath,const std::string& neuronName, const std::string& tmpDir, const OpenMesh::Vec3f& displacement);

        void addSpineImaris(const std::string& originalSpine, const std::string& repairedSpine, const std::string& ext, const std::string& name);

        void addSpine(const std::string& neuronName, int spineModel, const OpenMesh::Vec3f& displacement,const boost::numeric::ublas::matrix<float>& transform);

        std::vector<std::tuple<int,std::string>> getRandomSpines(int n,const std::string& tmpPath);

        void openTransaction();

        void closeTransaction();

        bool haveSpinesNeuron(const std::string& neuronName);

        void exportNeuron(const std::string& id,const std::string& path);

        std::vector<std::string> getNeuronsNames();

        static const std::vector<std::string> neuriteTypeDesc;
        static const std::vector<std::string> fileTypeDesc;
        static const std::vector<std::string> spineOriginDesc;
        static const std::vector<std::string> reconstructionMethodDesc;


    private:
        void createSchema();

        void addDefaultSpinesModels();

        vcg::Matrix44d orientSpine(MeshVCG& mesh,const skelgenerator::Spine* const spine);

        static std::vector<char> readBytes(const std::string &filename);

    };
}


#endif //NEURONIZE_BBDD_H
