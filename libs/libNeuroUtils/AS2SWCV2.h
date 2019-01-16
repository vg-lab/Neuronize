//
// Created by ivelascog on 10/10/18.
//

#ifndef NEURONIZE_AS2SWCV2_H
#define NEURONIZE_AS2SWCV2_H


#include <vector>
#include <string>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include "MeshVCG.h"

class SimplePoint {
public:
    OpenMesh::Vec3d point;
    double d;

    SimplePoint (double x, double y, double z,double d) {
        point[0] = x;
        point[1] = y;
        point[2] = z;
        this->d = d;
    }
};

class SWCPoint: public SimplePoint {
public:
    int parent;
    int type;

    SWCPoint(double x, double y, double z , double d , int parent,int type): SimplePoint(x,y,z,d) {
        this->parent = parent;
        this->type = type;
    }
};

using Dendrite = std::vector<SWCPoint>;

class AS2SWCV2 {
public:
    static MeshVCG* asc2swc(const std::string& inputFile, const std::string& outFile);

private:
    static void procesSomaPart(std::ifstream &file,std::vector<std::vector<OpenMesh::Vec3d>> &countours);

    static Dendrite processDendrite(std::ifstream &inputStream, int &counter, int type, std::vector<int>& parentsCount);

    static void toSWC(const std::string &filename,std::vector<Dendrite> &dendrites, SimplePoint* &soma);

    static SimplePoint* calcSoma(std::vector<Dendrite> &vector1);

    static double calcSommaRadius(OpenMesh::Vec3d center,std::vector<Dendrite> &dentrites);
};




#endif //NEURONIZE_AS2SWCV2_H
