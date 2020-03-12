//
// Created by ivelascog on 10/10/18.
//

#ifndef NEURONIZE_ASC2SWCV2_H
#define NEURONIZE_ASC2SWCV2_H

#include <neuroutils/api.h>

#include <vector>
#include <string>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include "MeshVCG.h"


#define MAXPOINT {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}

class NEUROUTILS_API SimplePoint {
public:
    OpenMesh::Vec3d point;
    double r;

    SimplePoint (double x, double y, double z,double r) {
        point[0] = x;
        point[1] = y;
        point[2] = z;
        this->r = r;
    }
    SimplePoint (double x, double y, double z){
        point[0] = x;
        point[1] = y;
        point[2] = z;
        this->r = -1;
    }

    virtual void toASC(const std::string &tab, std::ofstream &file) const;
    virtual void toSWC(int counter, int parent, int type, std::ofstream& file) const;
    virtual bool isSpine(){ return false;}
};

class NEUROUTILS_API Spine: public SimplePoint {
public:
    SimplePoint finalPoint;
    Spine(SimplePoint initPoint, SimplePoint finalPoint) : SimplePoint(initPoint) ,finalPoint(finalPoint){}

    void toASC(const std::string &tab, std::ofstream &file) const override;

    void toSWC(int counter, int parent, int type, std::ofstream &file) const override {};

    bool isSpine() override {return true;}
};

class NEUROUTILS_API SubDendrite {
public:
    std::vector<SimplePoint*> section;
    std::vector<SubDendrite> subDendrites;

    void toASC(std::string tab, std::ofstream &file) const;

    void toSWC(int &counter, int parent, int type, std::set<int> &usedParents, std::ofstream &file) const;

    bool removeEmptySections(SubDendrite *parent, int index);

    void removeOnly1SubDend();

    void moveFirstSpines(SubDendrite *parent);
};

class NEUROUTILS_API Dendrite {
public:
    enum DendType {Apical,Basal};
    SubDendrite dendrite;
    DendType type;

    void toASC(std::string tab, std::ofstream& file) const;
    void toSWC(int& counter,std::ofstream& file) const;

    void removeEmptySections();

    void removeOnly1SubDend();

    void moveFisrtSpines();
};

class NEUROUTILS_API ASC2SWCV2 {
public:

    ASC2SWCV2(const std::string &inputFile, bool useSoma);

    void toASC(const std::vector<std::vector<Eigen::Vector3f>> &contours, const Eigen::Vector3f &displacement,
               const std::string &outFile);

    void toASC(const std::string &outFile);

    MeshVCG *getSomaMesh() const;

    const std::vector<Spine*> &getSpines() const;

    void toSWC(const std::string &filename);
private:
    MeshVCG *_somaMesh;
    std::vector<Spine*> spines;

    std::vector<std::vector<OpenMesh::Vec3d>> contours;
    SimplePoint *soma;
    std::vector<Dendrite> dendrites;

    void procesSomaPart(std::ifstream &file, std::vector<std::vector<OpenMesh::Vec3d>> &countours);

    static SubDendrite processDendrite(std::ifstream &inputStream, std::vector<Spine*> &spines,Eigen::Vector3d lastPoint = MAXPOINT);

    SimplePoint *calcSoma(std::vector<Dendrite> &vector1);

    SimplePoint *calcSoma2();

    double calcSommaRadius(OpenMesh::Vec3d center);

    void joinApicals(std::vector<Dendrite> &apicals, const OpenMesh::Vec3d &somaCenter);

    static bool
    checkPointInsideSoma(MeshVCG &soma, const OpenMesh::Vec3d &point, std::vector<OpenMesh::Vec3d> &intersectionPoints);

};




#endif //NEURONIZE_ASC2SWCV2_H
