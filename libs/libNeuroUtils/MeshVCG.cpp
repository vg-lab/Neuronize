//
// Created by ivelascog on 20/09/18.
//
#define NOMINMAX

#include "MeshVCG.h"
#include <boost/filesystem/path.hpp>
#include <vcg/complex/algorithms/geodesic.h>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/convex_hull.h>
#include <iomanip>
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/intersection.h>
#include <QtCore/QFile>
#include <clocale>
#include <vcg/complex/complex.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/simplex/face/component_ep.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <queue>
#include <unordered_set>
#include <QtConcurrent/QtConcurrent>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#include <wrap/gl/glu_tessellator_cap.h>


MeshVCG::MeshVCG(const std::string &filename) {
    int loadMask = 0;
    boost::filesystem::path path(filename);
    auto extension = path.extension().string();
    this->name = path.stem().string();
    this->path = filename;
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");
    if (vcg::tri::io::Importer<MyMesh>::Open(mesh, filename.c_str()) != 0, loadMask) {
        throw "Error to read Mesh";
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
    vcg::tri::io::Exporter<MyMesh>::Save(mesh, filename.c_str(), 0);

}

void MeshVCG::toOff(std::string filename) {
    vcg::tri::io::Exporter<MyMesh>::Save(mesh, filename.c_str(), 0);
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

void MeshVCG::applyMatrix(const vcg::Matrix44d& matrix) {
    vcg::tri::UpdatePosition<MyMesh>::Matrix(mesh,matrix);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);
}

bool
MeshVCG::rayIntersects(OpenMesh::Vec3d rayOrigin, OpenMesh::Vec3d rayVector, std::vector<OpenMesh::Vec3d> &outIntersectionPoint) {

    vcg::Point3d origin (rayOrigin[0],rayOrigin[1],rayOrigin[2]);
    vcg::Point3d dir (rayVector[0],rayVector[1],rayVector[2]);
    vcg::Line3d ray(origin,dir);

    bool hit = false;
    double bar1,bar2,dist;
    vcg::Point3d p1;
    vcg::Point3d p2;
    vcg::Point3d p3;
    for ( auto fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
        p1 = vcg::Point3d( (*fi).P(0).X() ,(*fi).P(0).Y(),(*fi).P(0).Z() );
        p2 = vcg::Point3d( (*fi).P(1).X() ,(*fi).P(1).Y(),(*fi).P(1).Z() );
        p3 = vcg::Point3d( (*fi).P(2).X() ,(*fi).P(2).Y(),(*fi).P(2).Z() );
        if(vcg::IntersectionLineTriangle<double>(ray,p1,p2,p3,dist,bar1,bar2)) {
            if (dist > 0) { // Direccion del rayo.
                auto hitPoint = p1 * (1 - bar1 - bar2) + p2 * bar1 + p3 * bar2;
                OpenMesh::Vec3d aux(hitPoint.X(), hitPoint.Y(), hitPoint.Z());
                outIntersectionPoint.push_back(aux);
                hit = true;
            }
        }
    }

    return hit;

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

const string &MeshVCG::getPath() const {
    return path;
}

MyMesh::VertContainer MeshVCG::getVertex() {
    return this->mesh.vert;

}

int MeshVCG::getNumVertex() {
    return this->mesh.VN();
}

MeshVCG* MeshVCG::sliceAux(float z) {
   vcg::Point3d planeCenter(0,0,z);
   vcg::Point3d planeDir(0,0,1);
   vcg::Plane3d plane;
   plane.Init(planeCenter,planeDir);

   MeshVCG* sliceContour = new MeshVCG();
   vcg::IntersectionPlaneMesh<MyMesh,MyMesh,double>(this->mesh,plane,sliceContour->mesh);
   vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(sliceContour->mesh);
    return sliceContour;
}


std::vector<MeshVCG*> MeshVCG::slice(float zStep) {
    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
    auto min = mesh.bbox.P(0);
    float minZ = min[2];
    auto max = mesh.bbox.P(7);
    float maxZ = max[2];
    std::vector<MeshVCG* > contours;
    for (float currentZ  = minZ + zStep; currentZ <= maxZ ; currentZ+=zStep) {
        contours.push_back(sliceAux(currentZ));
    }

    return contours;
}

static int cont =0;
float MeshVCG::getMax2DArea(float zStep) {

    auto contours = this->slice(zStep);
    double minArea = -1;
    std::cout << contours.size() << std::endl;
    for (auto& contour: contours) {
        MeshVCG meshAux;
        vcg::tri::CapEdgeMesh(contour->mesh,meshAux.mesh);
        minArea = std::max(minArea,meshAux.getArea());
        cont++;
        delete contour;
    }

    return minArea;
}

float MeshVCG::getMax2DArea(const std::vector<std::vector<OpenMesh::Vec3d>>& contours) {
    double minArea = -1;
    for (const auto &contour: contours) {
        MeshVCG mesh;
        auto vi = vcg::tri::Allocator<MyMesh>::AddVertices(mesh.mesh, contour.size());
        auto ei = vcg::tri::Allocator<MyMesh>::AddEdges(mesh.mesh, contour.size());
        auto firstPoint = &*vi;
        vi->P() = MyMesh::CoordType(contour[0][0], contour[0][1], contour[0][2]);
        ++vi;
        auto previusPoint = firstPoint;
        for (int i = 1; i < contour.size(); i++) {
            auto actualPoint = &*vi;
            vi->P() = MyMesh::CoordType(contour[i][0], contour[i][1], contour[i][2]);
            ++vi;
            ei->V(0) = previusPoint;
            ei->V(1) = actualPoint;
            ++ei;

            previusPoint = actualPoint;

        }
        ei->V(0) = previusPoint;
        ei->V(1) = firstPoint;

        MeshVCG meshAux;
        vcg::tri::CapEdgeMesh(mesh.mesh,meshAux.mesh);
        minArea = std::max(minArea,meshAux.getArea());
    }


    return minArea;

}


HausdorffRet MeshVCG::hausdorffDistance(MeshVCG &otherMesh, const std::string &colorMeshPath) {
    QThreadPool pool;
    QSemaphore sem1(1);
    QSemaphore sem2(1);
    double meanDist1 = 0;
    for (auto &vp : mesh.vert) {
        QtConcurrent::run(&pool, [&]() {
            double minDist = 1000.0f;
            for (auto &fp : otherMesh.mesh.face) {
                vcg::Point3d q(0, 0, 0);
                MyFace::ScalarType dist = 1000.0f;
                vcg::face::PointDistanceBase(fp, vp.P(), minDist, q);
                minDist = minDist < dist ? minDist : dist;
            }
            vp.Q() = minDist;

            sem1.acquire();
            meanDist1 += minDist;
            sem1.release();
        });
    }

    double meanDist2 = 0;
    for (auto &vp :otherMesh.mesh.vert) {
        QtConcurrent::run(&pool, [&]() {
            double minDist = 1000.0f;
            for (auto &fp : mesh.face) {
                vcg::Point3d q(0, 0, 0);
                MyFace::ScalarType dist = 1000.0f;
                vcg::face::PointDistanceBase(fp, vp.P(), minDist, q);
                minDist = minDist < dist ? minDist : dist;
            }
            vp.Q() = minDist;

            sem2.acquire();
            meanDist2 += minDist;
            sem2.release();
        });
    }

    pool.waitForDone();

    meanDist1 /= mesh.VN();
    meanDist2 /= otherMesh.mesh.VN();


    std::pair<MyMesh::ScalarType, MyMesh::ScalarType> minmaxS1 = Stat<MyMesh>::ComputePerVertexQualityMinMax(mesh);
    std::pair<MyMesh::ScalarType, MyMesh::ScalarType> minmaxS2 = Stat<MyMesh>::ComputePerVertexQualityMinMax(
            otherMesh.mesh);

    vcg::tri::UpdateColor<MyMesh>::PerVertexQualityRamp(mesh, minmaxS1.second, minmaxS1.first);
    vcg::tri::UpdateColor<MyMesh>::PerVertexQualityRamp(otherMesh.mesh, minmaxS2.second, minmaxS2.first);
    if (!colorMeshPath.empty()) {
        vcg::tri::Inertia<MyMesh> Ib(mesh);
        auto cc = Ib.CenterOfMass();
        vcg::Matrix44d trans;
        trans.SetTranslate(-cc[0], -cc[1], -cc[2]);
        vcg::tri::UpdatePosition<MyMesh>::Matrix(mesh, trans);
        vcg::tri::UpdatePosition<MyMesh>::Matrix(otherMesh.mesh, trans);

        std::string s1Name = colorMeshPath + "/" + this->name + ".obj";
        std::string s2Name = colorMeshPath + "/" + otherMesh.name + ".obj";

        int saveMask = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTQUALITY;
        vcg::tri::io::ExporterOBJ<MyMesh>::Save(mesh, s1Name.c_str(), saveMask);
        vcg::tri::io::ExporterOBJ<MyMesh>::Save(otherMesh.mesh, s2Name.c_str(), saveMask);
    }

    HausdorffRet hausdorffRet{minmaxS1.second, minmaxS2.second, meanDist1, meanDist2, minmaxS1.first, minmaxS2.first};

    return hausdorffRet;
}
 QColor MeshVCG::getColor(float value) {
    vcg::Color4f color;
    color.SetColorRamp(0.0f,1.0f,value);
    return QColor::fromRgbF(color[0],color[1],color[2],color[3]);

}









