//
// Created by ivelascog on 20/09/18.
//

#include "MeshVCG.h"
#include <boost/filesystem/path.hpp>
#include <vcg/complex/algorithms/geodesic.h>
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_off.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <iomanip>

MeshVCG::MeshVCG(const std::string &filename) {
    int loadMask = 0;
    boost::filesystem::path path(filename);
    auto extension = path.extension().string();
    this->name = path.stem().string();
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");
    if (extension == ".off") {
        if (vcg::tri::io::ImporterOFF<MyMesh>::Open(mesh, filename.c_str()) !=
            vcg::tri::io::ImporterOFF<MyMesh>::NoError) {
            printf("Error reading file  %s\n", filename);
        }
    } else {
        if (extension == ".obj") {
            vcg::tri::io::ImporterOBJ<MyMesh>::Open(mesh, filename.c_str(), loadMask);
        }
    }
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
}


void MeshVCG::calcGeodesicDistance(const std::vector<int> &vertexs, const std::string &exportPath) {
    for(int i=0; i< vertexs.size();i++){
        int numVexter = vertexs[i];
        auto vi = mesh.vert.begin();
        MyMesh::VertexPointer vp = &(*(vi+numVexter));
        std::vector<MyMesh::VertexPointer> seed;
        seed.push_back(vp);
        vcg::tri::Geodesic<MyMesh>::Compute(mesh,seed);

        std::ofstream file (exportPath+std::to_string(i)+".dat",std::ofstream::out);

        for (vi = mesh.vert.begin(); vi != mesh.vert.end(); ++vi) {
            auto vp = &(*vi);
            auto dist = vp->Q();
            file <<std::setprecision(10) << dist <<std::endl;
        }
    }
}


