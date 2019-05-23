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
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/convex_hull.h>
#include <iomanip>
#include <libs/libNeuroUtils/AS2SWCV2.h>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/voronoi_remesher.h>
#include <vcg/complex/algorithms/stat.h>
#include <QtCore/QFile>
#include <clocale>


MeshVCG::MeshVCG(const std::string &filename) {
    int loadMask = 0;
    boost::filesystem::path path(filename);
    auto extension = path.extension().string();
    this->name = path.stem().string();
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");
    if (extension == ".off") {
        if (vcg::tri::io::ImporterOFF<MyMesh>::Open(mesh, filename.c_str()) !=
            vcg::tri::io::ImporterOFF<MyMesh>::NoError) {
          printf("Error reading file  %s\n", filename.c_str());
        }
    } else {
        if (extension == ".obj") {
            vcg::tri::io::ImporterOBJ<MyMesh>::Open(mesh, filename.c_str(), loadMask);
        }
    }
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);
}


void MeshVCG::calcGeodesicDistance(const std::vector<int> &vertexs, const std::string &exportPath) {
    for(int i=0; i< vertexs.size();i++){
        int numVexter = vertexs[i];
        auto vi = mesh.vert.begin();
        MyMesh::VertexPointer vp = &(*(vi+numVexter));
        std::vector<MyMesh::VertexPointer> seed;
        seed.push_back(vp);
        vcg::tri::Geodesic<MyMesh>::Compute(mesh,seed);

        QFile file1 (QString::fromStdString(exportPath+std::to_string(i)+".dat"));
        file1.remove();
        std::ofstream file (exportPath+std::to_string(i)+".dat",std::ofstream::out);

        for (vi = mesh.vert.begin(); vi != mesh.vert.end(); ++vi) {
            auto vp = &(*vi);
            auto dist = vp->Q();
            file <<std::setprecision(10) << dist <<std::endl;
        }
    }
}

void MeshVCG::convexHull(MeshVCG& destination) {
    vcg::tri::ConvexHull<MyMesh,MyMesh>::ComputeConvexHull(this->mesh,destination.mesh);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(destination.mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(destination.mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerFaceNormalized(destination.mesh);
}

MeshVCG::MeshVCG(const std::vector<std::vector<OpenMesh::Vec3d>> &contours) {
    for (const auto & contour: contours) {
       auto vi = vcg::tri::Allocator<MyMesh>::AddVertices(mesh,contour.size());
       auto ei = vcg::tri::Allocator<MyMesh>::AddEdges(mesh,contour.size());
       auto firstPoint = &*vi;
       vi->P() = MyMesh::CoordType (contour[0][0],contour[0][1],contour[0][2]);
       ++vi;
       auto previusPoint = firstPoint;
        for (int i=1;i < contour.size() ; i++) {
            auto actualPoint = &*vi;
            vi->P() = MyMesh::CoordType (contour[i][0],contour[i][1],contour[i][2]);
            ++vi;
            ei->V(0) = previusPoint;
            ei->V(1) = actualPoint;
            ++ei;

            previusPoint = actualPoint;

        }
        ei->V(0) = previusPoint;
        ei->V(1) = firstPoint;
        toObj("test.obj");
    }

}

void MeshVCG::toObj(std::string filename) {
    vcg::tri::io::ExporterOBJ<MyMesh>::Save(mesh, filename.c_str(), 0);

}

void MeshVCG::toOff(std::string filename) {
    vcg::tri::io::ExporterOFF<MyMesh>::Save(mesh, filename.c_str(), 0);
}

void MeshVCG::subdivide() {
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::Refine<MyMesh, vcg::tri::MidPoint<MyMesh> >(mesh,vcg::tri::MidPoint<MyMesh>(&mesh),0.0);

}

bool MeshVCG::isRegular() {
    bool isRegular;
    bool isSemiRegular;

    vcg::tri::Clean< MyMesh >::IsRegularMesh(mesh,isRegular,isSemiRegular);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh);
    vcg::tri::UpdateTopology<MyMesh>::VertexFace(mesh);
    return isRegular || isSemiRegular;
}

OpenMesh::Vec3d MeshVCG::getCenter() {
    vcg::tri::Inertia<MyMesh> Ib(mesh);
    auto cc = Ib.CenterOfMass();
    return  {cc[0],cc[1],cc[2]};
}

OpenMesh::Vec3d MeshVCG::center() {
    auto center = this->getCenter();
    vcg::Matrix44d trans;
    trans.SetTranslate(-center[0],-center[1],-center[2]);
    vcg::tri::UpdatePosition<MyMesh>::Matrix(mesh,trans);
    return center;
}

bool MeshVCG::RayIntersectsTriangle(OpenMesh::Vec3d rayOrigin,
                           OpenMesh::Vec3d rayVector,
                           MyMesh::FacePointer face,
                           OpenMesh::Vec3d& outIntersectionPoint) {
    OpenMesh::Vec3d v0 = {face->V(0)->P()[0],face->V(0)->P()[1],face->V(0)->P()[2]};
    OpenMesh::Vec3d v1 = {face->V(1)->P()[0],face->V(1)->P()[1],face->V(1)->P()[2]};
    OpenMesh::Vec3d v2 = {face->V(2)->P()[0],face->V(2)->P()[1],face->V(2)->P()[2]};

    double kEpsilon = 0.00001;

    OpenMesh::Vec3d v0v1 = v1 - v0;
    OpenMesh::Vec3d v0v2 = v2 - v0;
    OpenMesh::Vec3d pvec = rayVector % v0v2;
    float det = OpenMesh::dot(v0v1,pvec);
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;
    float invDet = 1 / det;

    OpenMesh::Vec3d tvec = rayOrigin - v0;
    auto u = OpenMesh::dot(tvec,pvec) * invDet;
    if (u < 0 || u > 1) return false;

    OpenMesh::Vec3d qvec = tvec % v0v1;
    auto v = OpenMesh::dot(rayVector,qvec) * invDet;
    if (v < 0 || u + v > 1) return false;

    auto t = OpenMesh::dot(v0v2,qvec) * invDet;
    outIntersectionPoint = rayOrigin + rayVector * t;

    return true;

}

bool
MeshVCG::RayIntersects(OpenMesh::Vec3d rayOrigin, OpenMesh::Vec3d rayVector, std::vector<OpenMesh::Vec3d> &outIntersectionPoint, std::vector<MyMesh::FacePointer> &intersectTriangles) {
    OpenMesh::Vec3d intersectPointAux;
    auto center = getCenter();
    bool intersect = false;
    for (auto fi = mesh.face.begin(); fi != mesh.face.end(); fi++) {
        auto fp = &(*fi);
        intersect = RayIntersectsTriangle(center, rayVector, fp, intersectPointAux);
        if (intersect) {
           outIntersectionPoint.push_back(intersectPointAux);
           intersectTriangles.push_back(fp);
        }
    }
    return intersect;
}


void MeshVCG::remesh(MeshVCG &outMesh) {
       float targetLenPerc=.6f;
       int iterNum=5;

       // Mesh cleaning
       vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
       vcg::tri::Clean<MyMesh>::RemoveUnreferencedVertex(mesh);
       vcg::tri::Allocator<MyMesh>::CompactEveryVector(mesh);
       vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(mesh);
       vcg::tri::UpdateBounding<MyMesh>::Box(mesh);

       vcg::tri::Append<MyMesh,MyMesh>::MeshCopy(outMesh.mesh,mesh);
       vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(outMesh.mesh);
       vcg::tri::UpdateBounding<MyMesh>::Box(outMesh.mesh);

       vcg::tri::UpdateTopology<MyMesh>::FaceFace(outMesh.mesh);
       vcg::tri::MeshAssert<MyMesh>::FFTwoManifoldEdge(outMesh.mesh);
       float lengthThr = targetLenPerc*(mesh.bbox.Diag()/100.f);


       vcg::tri::IsotropicRemeshing<MyMesh>::Params params;
       params.SetTargetLen(lengthThr);
       params.SetFeatureAngleDeg(10);
       params.iter=iterNum;
       vcg::tri::IsotropicRemeshing<MyMesh>::Do(outMesh.mesh, mesh, params);

}

double MeshVCG::getVolume() {
    return vcg::tri::Stat<MyMesh>::ComputeMeshVolume(mesh);
}

double MeshVCG::getArea() {
    return vcg::tri::Stat<MyMesh>::ComputeMeshArea(mesh);
}






