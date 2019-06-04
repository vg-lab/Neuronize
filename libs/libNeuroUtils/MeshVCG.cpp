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
#include <vcg/complex/algorithms/refine_loop.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include <vcg/complex/algorithms/voronoi_remesher.h>
#include <vcg/complex/algorithms/stat.h>
#include <QtCore/QFile>
#include <clocale>
#include <queue>
#include <unordered_set>


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

using triangle =tuple<std::vector<OpenMesh::Vec3d>,std::vector<OpenMesh::Vec3d>>;
std::vector<OpenMesh::Vec3d> MeshVCG::sliceAux(float z) {
    std::set<triangle> triangles;
    std::vector<OpenMesh::Vec3d> slice;
    for (auto fi = mesh.face.begin(); fi != mesh.face.end(); fi++) {
        std::vector<OpenMesh::Vec3d> underPoints;
        std::vector<OpenMesh::Vec3d> upperPoints;
        auto fp = &*fi;
        OpenMesh::Vec3d v0(fp->V(0)->P()[0], fp->V(0)->P()[1], fp->V(0)->P()[2]);
        OpenMesh::Vec3d v1(fp->V(1)->P()[0], fp->V(1)->P()[1], fp->V(1)->P()[2]);
        OpenMesh::Vec3d v2(fp->V(2)->P()[0], fp->V(2)->P()[1], fp->V(2)->P()[2]);
        if (!(v0[2] > z && v1[2] > z && v2[2] > z) && !(v0[2] < z && v1[2] < z && v2[2] < z)) {
            if (v0[2] == z) {
                slice.push_back(v0);
            } else if (v0[2] > z) {
                upperPoints.push_back(v0);
            } else {
                underPoints.push_back(v0);
            }

            if (v1[2] == z) {
                slice.push_back(v1);
            } else if (v1[2] > z) {
                upperPoints.push_back(v1);
            } else {
                underPoints.push_back(v1);
            }

            if (v2[2] == z) {
                slice.push_back(v2);
            } else if (v2[2] > z) {
                upperPoints.push_back(v2);
            } else {
                underPoints.push_back(v2);
            }

            triangles.emplace(upperPoints, underPoints);
        }
    }
    for (const auto &triangle:triangles) {
        const auto &upperPoints = std::get<0>(triangle);
        const auto &underPoints = std::get<1>(triangle);
        std::vector<OpenMesh::Vec3f> oneP;
        std::vector<OpenMesh::Vec3f> twoP;

        for (int i = 0; i < upperPoints.size(); i++) {
            for (int j = 0; j < underPoints.size();j++) {
                auto v = underPoints[j] - upperPoints[i];
                float alpha = (z - upperPoints[i][2]) / v[2];
                float x = upperPoints[i][0] + alpha * v[0];
                float y = upperPoints[i][1] + alpha * v[1];
                slice.emplace_back(x, y, z);
            }
        }
    }

    return slice;
}

std::vector<std::vector<OpenMesh::Vec3d>> MeshVCG::slice(float zStep) {
    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
    auto min = mesh.bbox.P(0);
    float minZ = min[2];
    auto max = mesh.bbox.P(7);
    float maxZ = max[2];

    std::vector<std::vector<OpenMesh::Vec3d>> contours;

    for (float currentZ  = minZ; currentZ <= maxZ ; currentZ+=zStep) {
        contours.push_back(sliceAux(currentZ));
    }
    return contours;
}
static int cont = 0;
float MeshVCG::getContourArea(const std::vector<OpenMesh::Vec3d>& contour) {
    MeshVCG* meshAux = triangulateContour(contour);
    float area = meshAux->getArea();
    meshAux->toObj("contour" + std::to_string(cont) +".obj");
    cont++;
    delete(meshAux);
    return area;
}

float MeshVCG::getMax2DArea() {
    auto contours = this->slice(0.1f);
    return getMax2DArea(contours);
}

float MeshVCG::getMax2DArea(const std::vector<std::vector<OpenMesh::Vec3d>>& contours){
    float maxArea = -1;
    for(const auto& contour:contours) {
        maxArea = std::max(maxArea,getContourArea(contour));
    }
    return maxArea;
}


using Edge = tuple<int,int,float>;

float MeshVCG::edgeEQ (OpenMesh::Vec3d point, Edge edge,const std::vector<OpenMesh::Vec3d>& points) {
    OpenMesh::Vec3d edgeInit = points[std::get<0>(edge)];
    OpenMesh::Vec3d edgeFin = points[std::get<1>(edge)];
    return ( point[0] - edgeInit[0]) * (edgeFin[1] - edgeInit[1]) - (point[1] - edgeInit[1]) * ( edgeFin[0] - edgeInit[0]);
}

bool MeshVCG::intersectEdges(const Edge &edge1, const Edge &edge2,const std::vector<OpenMesh::Vec3d>& points){

    float p1E1 = edgeEQ(points[std::get<0>(edge1)],edge2,points);
    float p2E1 = edgeEQ(points[std::get<1>(edge1)],edge2,points);
    float p1E2 = edgeEQ(points[std::get<0>(edge2)],edge1,points);
    float p2E2 = edgeEQ(points[std::get<1>(edge2)],edge1,points);
    if (p1E1 == 0 && p2E1 == 0 && p1E2 == 0 && p2E2 == 0) {
        return true;
    }

    bool diferent_sides1 = (p1E1 != 0 && p2E1 != 0) && ((p1E1 < 0) != (p2E1 < 0));
    bool difrente_sides2 = (p1E2 != 0 && p2E2 != 0) && ((p1E2 < 0) != (p2E2 < 0));
    return diferent_sides1 && difrente_sides2;
}

bool MeshVCG::findEdge(int v, const std::vector<Edge>& edges) {
    for (const auto & edge : edges) {
        int v0 = std::get<0>(edge);
        int v1 = std::get<1>(edge);
        if (v == v0 || v == v1) {
            return true;
        }
    }
    return false;
}


MeshVCG* MeshVCG::triangulateContour(const std::vector<OpenMesh::Vec3d>& contour) {
    auto compare = [](Edge e1, Edge e2) {
        float lenght1 = std::get<2>(e1);
        float lenght2 = std::get<2>(e2);
        return lenght1 >= lenght2;
    };

    std::priority_queue<Edge, std::vector<Edge>, decltype(compare)> edgeCandidates(compare);
    std::set<Edge> finalEdges;
    for (int i = 0; i < contour.size(); ++i) {
        for (int j = i + 1; j < contour.size(); ++j) {
            edgeCandidates.emplace(i, j, ((contour[i] - contour[j]).norm()));
        }
    }

    std::vector<std::vector<Edge >> edgeListAdajacency( contour.size() );

    while (!edgeCandidates.empty()) {
        Edge currentEdge = edgeCandidates.top();
        edgeCandidates.pop();
        bool cross = false;
        for (const auto &finalEdge: finalEdges) {
            if (intersectEdges(currentEdge, finalEdge, contour)) {
                cross = true;
                break;
            }
        }

        if (!cross) {
            finalEdges.insert(currentEdge);
            edgeListAdajacency[std::get<0>(currentEdge)].push_back(currentEdge);
            edgeListAdajacency[std::get<1>(currentEdge)].push_back(currentEdge);
        }
    }

    std::set<OpenMesh::Vec3i> triangles;

    for (unsigned int i= 0; i < contour.size(); i++) {
        for (unsigned int j = 0; j < edgeListAdajacency[i].size();j++) {
            for (unsigned int k = j + 1; k < edgeListAdajacency[i].size() ; k++) {
                auto edge1 = edgeListAdajacency[i][j];
                auto edge2 = edgeListAdajacency[i][k];
                int edge1v0 = std::get<0>(edge1);
                int edge1v1 = std::get<1>(edge1);
                int edge2v0 = std::get<0>(edge2);
                int edge2v1 = std::get<1>(edge2);

                int extv0 = edge1v0 == i ? edge1v1 : edge1v0;
                int extv1 = edge2v0 == i ? edge2v1 : edge2v0;

                if (findEdge(extv1,edgeListAdajacency[extv0])) {
                    OpenMesh::Vec3i triangle (i,extv0,extv1);
                    std::sort(triangle.begin(),triangle.end());
                    triangles.insert(triangle);
                }
            }

        }
    }


    MeshVCG *meshAux = new MeshVCG();
    auto vi = vcg::tri::Allocator<MyMesh>::AddVertices(meshAux->mesh, contour.size());
    for (const auto &point: contour) {
        vi->P() = MyMesh::CoordType(point[0], point[1], point[2]);
        ++vi;
    }
    auto fi = vcg::tri::Allocator<MyMesh>::AddFaces(meshAux->mesh,triangles.size());
    for (const auto& triangle: triangles) {
        MyMesh::VertexPointer	 v0 = &(meshAux->mesh.vert[triangle[0]]);
        MyMesh::VertexPointer	 v1 = &(meshAux->mesh.vert[triangle[1]]);
        MyMesh::VertexPointer	 v2 = &(meshAux->mesh.vert[triangle[2]]);
        fi->V(0) = v0;
        fi->V(1) = v1;
        fi->V(2) = v2;
        ++fi;
    }
    return meshAux;


}








