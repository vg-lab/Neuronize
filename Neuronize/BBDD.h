//
// Created by ivelascog on 20/05/19.
//

#ifndef NEURONIZE_BBDD_H
#define NEURONIZE_BBDD_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <map>
#include <libs/libNeuroUtils/MeshVCG.h>
#include <libs/libNeuroUtils/SWCImporter.h>
#include <SkelGeneratorUtil/Spine.h>

namespace BBDD {

    enum FileType {
        Obj, Stl
    };
    enum SpineOrigin {
        Imaris,FilamentTracer,Neurolucida,Neuronize
    };

    enum ReconstructionMethod {
        Spring_Mass,FEM
    };

    class BBDD {
        sqlite3 *_db;
        char *_err = NULL;
    public:
        BBDD () = default;
        explicit BBDD(const std::string &filename);

        void addSpine(const std::string &name, float area, float volume, const std::string &neuronName, int origin,
                      const std::string &file);
        void addNeuron(const std::string& name, const std::string& swcFile);

        void addSoma(const std::string& neuronName, MeshVCG& model,ReconstructionMethod reconstructionMethod);

        void addDendrite(const std::string& neuronName,int initCounter,int lastCounter,NSSWCImporter::DendriticType type);

        void addSpineVRML(const skelgenerator::Spine* const spine,const std::string& meshPath,const std::string& neuronName, const std::string& tmpDir, const OpenMesh::Vec3f& displacement);

        void addSpine(const std::string& neuronName, int spineModel, MeshVCG& orientedMesh);

        void openTransaction();

        void closeTransaction();

        void test();

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
