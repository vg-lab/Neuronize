//
// Created by ivelascog on 20/09/18.
//

#ifndef NEURONIZE_MESHVCG_H
#define NEURONIZE_MESHVCG_H


#include <string>
#include <vector>
#include <vcg/complex/complex.h>
#include <vcg/complex/all_types.h>

    class MyVertex; class MyEdge; class MyFace;
    struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
            vcg::Use<MyEdge>     ::AsEdgeType,
            vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes,vcg::vertex::Coord3d, vcg::vertex::Normal3d,vcg::vertex::Qualityf, vcg::vertex::VFAdj ,vcg::vertex::BitFlags>{};
    class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj,vcg::face::VFAdj,  vcg::face::VertexRef, vcg::face::BitFlags > {};
    class MyEdge    : public vcg::Edge<   MyUsedTypes, vcg::edge::EFAdj> {};
    class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};

    class MeshVCG {
        MyMesh mesh;
        std::string name;

    public:
        explicit MeshVCG(const std::string &filename);

        void calcGeodesicDistance(const std::vector<int> &vertex,
                                  const std::string &exportPath);

    private:
    };



#endif //NEURONIZE_MESHVCG_H
