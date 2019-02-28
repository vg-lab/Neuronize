//
// Created by ivelascog on 20/09/18.
//

#ifndef NEURONIZE_MESHVCG_H
#define NEURONIZE_MESHVCG_H



#include <string>
#include <vector>
#include <vcg/complex/complex.h>
#include <vcg/complex/all_types.h>
#include <OpenMesh/Core/Mesh/Traits.hh>


class MyVertex; class MyEdge; class MyFace;
    struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
            vcg::Use<MyEdge>     ::AsEdgeType,
            vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes,vcg::vertex::Coord3d, vcg::vertex::Normal3d,vcg::vertex::Qualityf, vcg::vertex::VFAdj ,vcg::vertex::BitFlags>{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::Mark ,vcg::face::FFAdj,vcg::face::VFAdj,  vcg::face::VertexRef,vcg::face::Normal3d , vcg::face::BitFlags > {};
    class MyEdge    : public vcg::Edge<   MyUsedTypes, vcg::edge::EFAdj,vcg::edge::VertexRef> {};
    class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};

    class MeshVCG {
        MyMesh mesh;
        std::string name;

    public:
        explicit MeshVCG(const std::string &filename);
        explicit MeshVCG(const std::vector<std::vector<OpenMesh::Vec3d>> &contours);
        MeshVCG() = default;

        void toObj(std::string filename);

        void calcGeodesicDistance(const std::vector<int> &vertex,
                                  const std::string &exportPath);

        void convexHull(MeshVCG& destination);

        void toOff(std::string filename);

        void subdivide();

        bool isRegular();

        void remesh(MeshVCG &outMesh);

        OpenMesh::Vec3d getCenter();

        OpenMesh::Vec3d center();

        bool RayIntersects(OpenMesh::Vec3d rayOrigin,
                            OpenMesh::Vec3d rayVector,
                           std::vector<OpenMesh::Vec3d> &outIntersectionPoint,
                           std::vector<MyMesh::FacePointer> &intersectTriangles);

        bool RayIntersectsTriangle(OpenMesh::Vec3d rayOrigin,
                                   OpenMesh::Vec3d rayVector,
                                   MyMesh::FacePointer face,
                                   OpenMesh::Vec3d& outIntersectionPoint);

        double getVolume();

        double getArea();




    private:
    };



#endif //NEURONIZE_MESHVCG_H
