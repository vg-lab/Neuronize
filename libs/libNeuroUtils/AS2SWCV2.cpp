//
// Created by ivelascog on 10/10/18.
//

#include <fstream>
#include <stack>
#include <iomanip>
#include "AS2SWCV2.h"
#include "MeshVCG.h"

MeshVCG* AS2SWCV2::asc2swc(const std::string &inputFile, const std::string &outFile) {
        std::setlocale(LC_ALL, "en_US.UTF-8");
        std::vector<std::vector<OpenMesh::Vec3d>> contours;
        int counter = 2; //Saltamos el soma
        std::ifstream inputStream;
        inputStream.open(inputFile,std::ios::in);
        std::vector<Dendrite > dentrites;

        std::string line;
        while (inputStream >> line) {
            if (line.find("MBFObjectType") != std::string::npos) { //Esto es soma. hay otro soma pero no nos vale ya que tiene solo un contorno
                inputStream >> line;
                procesSomaPart(inputStream,contours);
            }
            if (line.find("Dendrite") !=  std::string::npos) {
               dentrites.push_back(processDendrite(inputStream, counter, 3));
            }
            if (line.find("Apical") != std::string::npos) {
                dentrites.push_back(processDendrite(inputStream, counter, 4));

            }
        }
        inputStream.close();
        OpenMesh::Vec3d center;
        MeshVCG* finalSoma = nullptr;
        SimplePoint* soma;
        if (contours.size() >1) {
            MeshVCG  somaMesh (contours);
            MeshVCG somaConvex;
            somaMesh.convexHull(somaConvex);
            center = somaConvex.getCenter();
            auto radius = calcSommaRadius(center,dentrites);
            soma = new SimplePoint(center[0],center[1],center[2],radius);

            //Desplazar los puntos fuera del soma

            
            float offset = 0.0;
            for (auto &dendrite: dentrites) {
                double dist= std::numeric_limits<double>::max();
                auto v = dendrite[0].point - center;
                v.normalize();
                std::vector<OpenMesh::Vec3d> intersectPoints;
                somaConvex.RayIntersects(center,v,intersectPoints);
                for (const auto &intersectPoint: intersectPoints) {
                    auto distAux = (intersectPoint - dendrite[0].point).norm();
                    dist = std::min(dist, distAux);
                }
                if (dist > 0 ) {
                    auto desp = v * (dist + offset);
                    for (auto &point :dendrite) {
                        point.point += desp;
                    }
                }
            }

            somaConvex.center();

            finalSoma = new MeshVCG();
            somaConvex.remesh(*finalSoma);




        }else {
            soma = calcSoma(dentrites);
        }



        toSWC(outFile,dentrites,soma);




    return finalSoma;

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
Dendrite AS2SWCV2::processDendrite(std::ifstream &inputStream, int &counter,int type) {
    Dendrite dendrite;
    int parent = 1;
    std::stack<int> parents;
    int brachs = 1;
    double x, y, z, d;
    std::string line;
    while (inputStream >> line) {
        if (line.find('<') != std::string::npos) {  //Estamos ante una espina de momento las ignoramos
            while (line.find('>') == std::string::npos){
                inputStream >> line;
            }
        } else if (line == "(") {
            inputStream >> line;
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
            inputStream >> line; //saltamos parentesis final
            dendrite.emplace_back(x,y,z,d,parent,type);
            parent = counter;
            counter++;

        } else if (line == "|") {
            parent = parents.top();
        } else if (line == ")") {
            if (!parents.empty()){
                parent = parents.top();
                parents.pop();
            }
            brachs--;
            if (brachs == 0) {
                break;
            }
        }
    }
    return dendrite;
}

void AS2SWCV2::toSWC(const std::string &filename,std::vector<Dendrite> &dendrites, SimplePoint* &soma) {
    std::ofstream outputStream;
    outputStream.open(filename, std::ios::out);
    int counter = 1;
    outputStream << std::setprecision(10) << counter << "\t" << 2 << "\t" << soma->point[0]<< "\t" << soma->point[1] <<
                    "\t" << soma->point[2] << "\t" << soma->d << "\t" << -1 << std::endl;
    counter++;
    for (const auto &dendrite: dendrites){
        for (const auto &point: dendrite) {
            outputStream << std::setprecision(10) << counter << "\t" << point.type << "\t" << point.point[0] << "\t" <<
                            point.point[1] << "\t" << point.point[2] << "\t" << point.d << "\t" << point.parent << std::endl;
            counter++;

        }
    }
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

double AS2SWCV2::calcSommaRadius(OpenMesh::Vec3d center,std::vector<Dendrite> &dentrites){
    double minDist = (center - dentrites[0][0].point).norm();
    for (int i = 1 ; i < dentrites.size();i++) {
        auto dist = (center - dentrites[i][0].point).norm();
        minDist = minDist > dist ? dist : minDist;
    }
    return minDist;

}
