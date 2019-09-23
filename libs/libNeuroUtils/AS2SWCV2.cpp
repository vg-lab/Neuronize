//
// Created by ivelascog on 10/10/18.
//
#include <fstream>
#include <stack>
#include <iomanip>
#include <unordered_set>
#include <clocale>
#include "MeshVCG.h"
#include "AS2SWCV2.h"


std::tuple<MeshVCG*,std::vector<Spine>> AS2SWCV2::asc2swc(const std::string &inputFile, const std::string &outFile, bool useSoma) {
        std::setlocale(LC_ALL, "en_US.UTF-8");
        std::vector<std::vector<OpenMesh::Vec3d>> contours;
        std::vector<Spine> spines;
        int counter = 2; //Saltamos el soma
        std::ifstream inputStream;
        inputStream.open(inputFile,std::ios::in);
        std::vector<Dendrite > basals;
        std::vector<Dendrite > apicals;
        std::vector<Dendrite > dendrites;
        std::vector<int> parentsCount;
        parentsCount.push_back(0); // el soma no importa pero se rellena para simplificar el acceso.
        int apiPos = -1;

        std::string line;
        while (inputStream >> line) {
            if (line.find("MBFObjectType") !=
                std::string::npos) { //Esto es soma. hay otro soma pero no nos vale ya que tiene solo un contorno
                inputStream >> line;
                procesSomaPart(inputStream, contours);
            }
            if (line.find("Dendrite") != std::string::npos) {
                basals.push_back(processDendrite(inputStream, counter, 3, parentsCount, spines));
            }
            if (line.find("Apical") != std::string::npos) {
                if (apiPos == -1) {
                    apiPos = static_cast<int>(basals.empty() ? 0 : basals.size());
                }
                apicals.push_back(processDendrite(inputStream, counter, 4, parentsCount, spines));

            }
        }
        inputStream.close();

        SimplePoint* auxSoma = calcSoma2(basals);
        joinApicals(apicals,auxSoma->point);
        dendrites = basals;
        dendrites.insert(dendrites.begin() + apiPos,apicals.begin(),apicals.end());

        OpenMesh::Vec3d center;
        MeshVCG* finalSoma = nullptr;
        SimplePoint* soma;
        if (contours.size() >1 && useSoma) {
            MeshVCG  somaMesh (contours);
            MeshVCG somaConvex;
            somaMesh.convexHull(somaConvex);
            center = somaConvex.getCenter();
            auto radius = calcSommaRadius(center,dendrites);
            soma = new SimplePoint(center[0],center[1],center[2],radius);
            //Desplazar los puntos fuera del soma
            int erasedPoints = 0;
            for (auto &dendrite: dendrites) {
                std::vector<OpenMesh::Vec3d> intersectionPointLast;
                std::vector<OpenMesh::Vec3d> intersectionPointCur;
                if (checkPointInsideSoma(somaConvex, dendrite[0].point,intersectionPointCur)) {
                    int firstPoint = 0;
                    bool inside = true;
                    while (inside && firstPoint < dendrite.size()) {
                        intersectionPointLast = intersectionPointCur;
                        intersectionPointCur.clear();
                        firstPoint++;
                        inside = checkPointInsideSoma(somaConvex, dendrite[firstPoint].point,intersectionPointCur);
                    }

                    OpenMesh::Vec3d intersectionSoma;
                    auto rayDir = dendrite[firstPoint].point - dendrite[firstPoint-1].point;
                    intersectionPointLast.clear();
                    somaConvex.rayIntersects(dendrite[firstPoint - 1].point, rayDir,intersectionPointLast);
                    intersectionSoma = intersectionPointLast[0];


                    dendrite.erase(dendrite.begin(), dendrite.begin() + firstPoint - 1); //dejamos un punto para modificarlo.
                    dendrite[0].point = intersectionSoma;
                    dendrite[0].parent = 1;

                    erasedPoints += firstPoint -1;
                }

                //Ajustamos indices
                if (erasedPoints > 0) {
                    dendrite[0].counter -= erasedPoints;
                    for (int i=1; i< dendrite.size();i++) {
                        auto &node = dendrite[i];
                        node.counter -= erasedPoints;
                        node.parent -= erasedPoints;
                    }
                }
            }


            somaConvex.center();

            finalSoma = new MeshVCG();
            somaConvex.remesh(*finalSoma);
        }else {
            soma = calcSoma2(dendrites);
        }
        toSWC(outFile,dendrites,soma);
    return std::make_tuple(finalSoma,spines);

}

bool AS2SWCV2::checkPointInsideSoma(MeshVCG& soma, const OpenMesh::Vec3d& point,std::vector<OpenMesh::Vec3d>& intersectionPoints) {
    auto center = soma.getCenter();
    OpenMesh::Vec3d v = center - point;
    v.normalize();
    soma.rayIntersects(point,v,intersectionPoints);
    return intersectionPoints.size() == 1;
}

void AS2SWCV2::procesSomaPart(std::ifstream &file,std::vector<std::vector<OpenMesh::Vec3d>> &countours) {
    std::string line;
    std::vector<OpenMesh::Vec3d> contour;
    double x, y, z, d;
    while (file >> line) {
        if (line == "(") {
            file >> x;
            file >> y;
            file >> z;
            file >> d;
            file >> line; // Quitamos ultimo parentesis
            contour.emplace_back(x,y,z);
        } else if (line == ")") {
            countours.push_back(contour);
            break; // Fin del contorno
        }
    }
}

// TODO Problema con dendritas apicales construidas a trozos. Buscar forma de unir dendritas  en una sola.
Dendrite AS2SWCV2::processDendrite(std::ifstream &inputStream, int &counter, int type, std::vector<int>& parentsCount,std::vector<Spine>& spines) {
    Dendrite dendrite;
    char name[256];
    double minDistance = 0.5;
    int initCounter = counter;
    std::unordered_set<int> usedParents;
    Eigen::Vector3d lastPoint;

    int parent = 1;
    std::stack<int> parents;
    int brachs = 1;
    double x, y, z, d;
    Eigen::Vector3d actualPoint;
    std::string line;

    // Rellenamos el primer punto
    lastPoint = {std::numeric_limits<double>::max(),std::numeric_limits<double>::max(),std::numeric_limits<double>::max()};


    while (inputStream >> line) {
        if (line.find('<') != std::string::npos) {//Espina
            inputStream >> line >> line >> line >> line >> line >> line >> line >> line;
            inputStream >> x;
            inputStream >> y;
            inputStream >> z;
            inputStream >> d;
            inputStream >> line;
            SimplePoint initPoint (lastPoint[0], lastPoint[1], lastPoint[2]);
            Spine spine (initPoint, SimplePoint(x,y,z), d);
            spines.push_back(spine);
        } else if (line == "(") {
            inputStream >> line;
            while (line == ";") {
                inputStream.getline(name,256);
                inputStream >> line;
            }
            if (line == "(") { //Nueva SubDendrita
                parents.push(counter - 1);
                brachs++;
                inputStream >> x;
                inputStream >> y;
                inputStream >> z;
                inputStream >> d;
            } else {
                x = std::stod(line);
                inputStream >> y;
                inputStream >> z;
                inputStream >> d;
            }
            actualPoint = {x,y,z};
            double dist  = (lastPoint - actualPoint).norm();
            inputStream >> line; //saltamos parentesis final
            if ( (lastPoint - actualPoint).norm() > minDistance ) { // comprobamos que el punto anterior no sea igual a este
                dendrite.emplace_back(x, y, z, d,counter, parent, type);
                parent = counter;
                counter++;
                lastPoint  = actualPoint;
            }
        } else if (line == "|") {
            parent = parents.top();
            auto point = dendrite[(parent - initCounter)].point;
            lastPoint = {point[0],point[1],point[2]};
            while (usedParents.find(parent) != usedParents.end()) { // buscamos un punto libre para relizar la conexion
                parent -= 1;
            }
            usedParents.emplace(parent);
        } else if (line == ")") {
            if (!parents.empty()){
                parents.pop();
            }
            brachs--;
            if (brachs == 0) {
                break;
            }
        } else if (line ==";") {
            inputStream.getline(name,256);
        }
    }
    return dendrite;
}


void AS2SWCV2::toSWC(const std::string &filename,std::vector<Dendrite> &dendrites, SimplePoint* &soma) {
    std::ofstream outputStream;
    outputStream.open(filename, std::ios::out);
    outputStream << std::setprecision(10) << 1 << "\t" << 2 << "\t" << soma->point[0]<< "\t" << soma->point[1] <<
                    "\t" << soma->point[2] << "\t" << soma->d << "\t" << -1 << std::endl;

    for (const auto &dendrite: dendrites){
        for (const auto &point: dendrite) {
            outputStream << std::setprecision(10) << point.counter << "\t" << point.type << "\t" << point.point[0] << "\t" <<
                            point.point[1] << "\t" << point.point[2] << "\t" << point.d << "\t" << point.parent << std::endl;

        }
    }

    outputStream.close();
}

SimplePoint* AS2SWCV2::calcSoma(std::vector<Dendrite> &dentrites) {
    int nPoints = static_cast<int>(dentrites.size());
    OpenMesh::Vec3d center (0,0,0);
    for (const auto &dendrite: dentrites) {
        center += dendrite[0].point;
    }
    center /= nPoints;
    auto radius = calcSommaRadius(center,dentrites);

    return new SimplePoint(center[0], center[1], center[2], radius);
}

SimplePoint* AS2SWCV2::calcSoma2(std::vector<Dendrite> &dendrites) {
    OpenMesh::Vec3d maxPoint {-std::numeric_limits<double >::max(),-std::numeric_limits<double >::max(),-std::numeric_limits<double >::max()};
    OpenMesh::Vec3d minPoint = -maxPoint;
    for (const auto &dendrite:dendrites){
        maxPoint[0] = std::max(maxPoint[0],dendrite[0].point[0]);
        maxPoint[1] = std::max(maxPoint[1],dendrite[0].point[1]);
        maxPoint[2] = std::max(maxPoint[2],dendrite[0].point[2]);

        minPoint[0] = std::min(minPoint[0],dendrite[0].point[0]);
        minPoint[1] = std::min(minPoint[1],dendrite[0].point[1]);
        minPoint[2] = std::min(minPoint[2],dendrite[0].point[2]);
    }

    OpenMesh::Vec3d center = minPoint + (maxPoint-minPoint)/2;

    double radius = calcSommaRadius(center,dendrites);
    return new SimplePoint(center[0],center[1],center[2], radius);


}

double AS2SWCV2::calcSommaRadius(OpenMesh::Vec3d center,std::vector<Dendrite> &dentrites){
    double minDist = (center - dentrites[0][0].point).norm();
    for (int i = 1 ; i < dentrites.size();i++) {
        auto dist = (center - dentrites[i][0].point).norm();
        minDist = minDist > dist ? dist : minDist;
    }
    if (minDist == 0) {
        minDist++;
    }
    return minDist;

}

void AS2SWCV2::joinApicals(std::vector<Dendrite>& apicals,const OpenMesh::Vec3d& somaCenter) {

    if (apicals.size() > 1) {
        //Comprobar que dendrita esta mas cerca del soma
        const Dendrite* principalApical = nullptr;
        float minDist = std::numeric_limits<float>::max();
        for (const Dendrite& api: apicals) {
            float dist = (api[0].point - somaCenter).norm();

            if (minDist > dist) {
                principalApical = &api;
                minDist = dist;
            }
        }


        for (auto &api : apicals) {
            if (&api != principalApical) {
                minDist = std::numeric_limits<float>::max();
                int mini = -1;
                for (int i = 0; i < principalApical->size(); i++) {
                    float dist = (api[0].point - (*principalApical)[i].point).norm();
                    if (minDist > dist) {
                        mini = i;
                        minDist = dist;
                    }
                }


                api[0].parent = (*principalApical)[mini].counter;
            }
        }
    }
}
