//
// Created by ivelascog on 20/09/18.
//

#ifndef NEURONIZE_MESHVCG_H
#define NEURONIZE_MESHVCG_H

#include <neuroutils/api.h>

#include <string>
#include <vector>
#include <vcg/complex/complex.h>
#include <vcg/complex/all_types.h>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <vcg/simplex/face/component_ep.h>
#include <QColor>

using Edge = std::tuple<int, int, float>;

struct HausdorffRet {
    double max1;
    double max2;
    double mean1;
    double mean2;
    double min1;
    double min2;
};

class MyVertex; class MyEdge; class MyFace;
    struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
            vcg::Use<MyEdge>     ::AsEdgeType,
            vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes,vcg::vertex::Coord3d, vcg::vertex::Normal3d, vcg::vertex::Color4b,vcg::vertex::Qualityf, vcg::vertex::VFAdj ,vcg::vertex::BitFlags>{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::Mark ,vcg::face::FFAdj,vcg::face::VFAdj,  vcg::face::VertexRef,vcg::face::Normal3d , vcg::face::BitFlags,vcg::face::EdgePlane > {};
    class MyEdge    : public vcg::Edge<   MyUsedTypes, vcg::edge::EFAdj,vcg::edge::VertexRef, vcg::edge::BitFlags,vcg::edge::EEAdj> {};
    class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};

    class NEUROUTILS_API MeshVCG {
        MyMesh mesh;
        std::string name;

    public:
        explicit MeshVCG(const std::string &filename);

        explicit MeshVCG(const std::vector<std::vector<OpenMesh::Vec3d>> &contours);

        MeshVCG() = default;

        void toObj(std::string filename);

        void calcGeodesicDistance(const std::vector<int> &vertex,
                                  const std::string &exportPath);

        void convexHull(MeshVCG &destination);

        void toOff(std::string filename);

        void subdivide();

        bool isRegular();

        void remesh(MeshVCG &outMesh);

        OpenMesh::Vec3d getCenter();

        OpenMesh::Vec3d center();

        bool rayIntersects(OpenMesh::Vec3d rayOrigin,
                           OpenMesh::Vec3d rayVector,
                           std::vector<OpenMesh::Vec3d> &outIntersectionPoint);


        double getVolume();

        double getArea();

        std::vector<MeshVCG*> slice(float zStep);

        static QColor getColor(float value);

        HausdorffRet hausdorffDistance(MeshVCG &otherMesh, const std::string &path = "");



        float getMax2DArea(float zStep = 0.1f);
        static float getMax2DArea(const std::vector<std::vector<OpenMesh::Vec3d>>& contours);

    private:

         MeshVCG*  sliceAux(float z);

    };



#endif //NEURONIZE_MESHVCG_H
