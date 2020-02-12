//
// Created by ivelascog on 10/10/18.
//
#include <fstream>
#include <stack>
#include <iomanip>
#include <unordered_set>
#include <clocale>
#include "MeshVCG.h"
#include "ASC2SWCV2.h"

ASC2SWCV2::ASC2SWCV2(const std::string &inputFile, bool useSoma) {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    std::ifstream inputStream;
    inputStream.open(inputFile, std::ios::in);
    std::vector<Dendrite> apicals;

    if (!inputStream.is_open()) {
        throw std::runtime_error("Error: Can read the ASC file: " + inputFile);
    }

    std::string line;
    while (inputStream >> line) {
        if (line.find("MBFObjectType") !=
            std::string::npos) { //Esto es soma. hay otro soma pero no nos vale ya que tiene solo un contorno
            inputStream >> line;
            procesSomaPart(inputStream, contours);
        }
        if (line.find("Dendrite") != std::string::npos) {
            Dendrite dendrite;
            dendrite.type = Dendrite::Basal;
            dendrite.dendrite = processDendrite(inputStream, spines);
            dendrites.push_back(dendrite);
        }
        if (line.find("Apical") != std::string::npos) {
            Dendrite dendrite;
            dendrite.type = Dendrite::Apical;
            dendrite.dendrite = processDendrite(inputStream, spines);
            apicals.push_back(dendrite);
        }
    }
    inputStream.close();


    joinApicals(apicals, soma->point);
    dendrites.insert(dendrites.end(), apicals.begin(),apicals.end());

    for (auto& dendrite:this->dendrites) {
        dendrite.removeEmptySections();
        dendrite.removeOnly1SubDend();
    }
    soma = calcSoma2();

    OpenMesh::Vec3d center;
    _somaMesh = nullptr;
    if (contours.size() > 1 && useSoma) {
        MeshVCG somaMesh(contours);
        MeshVCG somaConvex;
        somaMesh.convexHull(somaConvex);
        center = somaConvex.getCenter();
        auto radius = calcSommaRadius(center);
        soma = new SimplePoint(center[0], center[1], center[2], radius);
        //Desplazar los puntos fuera del soma
        for (auto &dendrite: dendrites) {
            std::vector<OpenMesh::Vec3d> intersectionPointLast;
            std::vector<OpenMesh::Vec3d> intersectionPointCur;
            if (checkPointInsideSoma(somaConvex, dendrite.dendrite.section[0]->point, intersectionPointCur)) {
                int firstPoint = 0;
                bool inside = true;
                while (inside && firstPoint < dendrite.dendrite.section.size()) {
                    intersectionPointLast = intersectionPointCur;
                    intersectionPointCur.clear();
                    firstPoint++;
                    inside = checkPointInsideSoma(somaConvex, dendrite.dendrite.section[firstPoint]->point,
                                                  intersectionPointCur);
                }
                OpenMesh::Vec3d intersectionSoma;
                auto rayDir =
                        dendrite.dendrite.section[firstPoint]->point - dendrite.dendrite.section[firstPoint - 1]->point;
                intersectionPointLast.clear();
                somaConvex.rayIntersects(dendrite.dendrite.section[firstPoint - 1]->point, rayDir,
                                         intersectionPointLast);
                intersectionSoma = intersectionPointLast[0];


                dendrite.dendrite.section.erase(dendrite.dendrite.section.begin(),
                                                dendrite.dendrite.section.begin() + firstPoint -
                                                1); //dejamos un punto para modificarlo.
                dendrite.dendrite.section[0]->point = intersectionSoma;

            }

        }

        somaConvex.center();

        _somaMesh = new MeshVCG();
        somaConvex.remesh(*_somaMesh);
    } else {
        this->soma = calcSoma2();
    }
}

bool ASC2SWCV2::checkPointInsideSoma(MeshVCG& soma, const OpenMesh::Vec3d& point, std::vector<OpenMesh::Vec3d>& intersectionPoints) {
    auto center = soma.getCenter();
    OpenMesh::Vec3d v = center - point;
    v.normalize();
    soma.rayIntersects(point,v,intersectionPoints);
    return intersectionPoints.size() == 1;
}

void ASC2SWCV2::procesSomaPart(std::ifstream &file, std::vector<std::vector<OpenMesh::Vec3d>> &countours) {
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

SubDendrite ASC2SWCV2::processDendrite(std::ifstream &inputStream, std::vector<Spine*>& spines,Eigen::Vector3d lastPoint) {
    std::string line;
    double x, y, z, d;
    Eigen::Vector3d actualPoint;
    SubDendrite subDendrite;
    float threshold = 1.25f;
    while(inputStream >> line) {
        if (line.find('<') != std::string::npos) {
            inputStream >> line >> line >> line >> line >> line >> line >> line >> line;
            inputStream >> x;
            inputStream >> y;
            inputStream >> z;
            inputStream >> d;
            inputStream >> line;
            SimplePoint initPoint (lastPoint[0], lastPoint[1], lastPoint[2]);
            auto spine = new Spine (initPoint, SimplePoint(x,y,z,d));
            spines.push_back(spine);
            subDendrite.section.push_back(spine);
        } else if (line.find('(') != std::string::npos) {
            inputStream >> line;
            if (line.find(';') != std:: string::npos) {
                inputStream.ignore(1000, '\n');
                inputStream >> line;
            }

            if (line.find('(') != std::string::npos) {
                inputStream.putback('('); //Restauramos para que sean todas iguales.
                subDendrite.subDendrites.push_back(processDendrite(inputStream, spines,lastPoint));
            } else {
                x = std::stod(line);
                inputStream >> y;
                inputStream >> z;
                inputStream >> d;
                inputStream >> line;
                actualPoint = {x, y, z};
                if ((lastPoint - actualPoint).norm() > threshold) {
                    subDendrite.section.push_back(new SimplePoint(x, y, z, d));
                    lastPoint = actualPoint;
                }
            }
        } else if (line.find('|') != std::string::npos) {
            inputStream.putback('('); //Asi la llamada a esta interpreta que debe construir una nueva subdendrita.
            return subDendrite;
        } else if (line.find(')') != std::string::npos) {
            return subDendrite;
        }
    }
}


SimplePoint* ASC2SWCV2::calcSoma(std::vector<Dendrite> &dentrites) {
    int nPoints = static_cast<int>(dentrites.size());
    OpenMesh::Vec3d center (0,0,0);
    for (const auto &dendrite: dentrites) {
        center += dendrite.dendrite.section[0]->point;
    }
    center /= nPoints;
    auto radius = calcSommaRadius(center);

    return new SimplePoint(center[0], center[1], center[2], radius);
}
// TODO leer un unico contorno y no invetarnos el soma completamente
SimplePoint * ASC2SWCV2::calcSoma2() {
    if (dendrites.size() == 1) { //Special case
        auto dendrite = dendrites[0];
        OpenMesh::Vec3d firstPoint = dendrite.dendrite.section[0]->point;
        OpenMesh::Vec3d secondPoint = dendrite.dendrite.section[1]->point;
        OpenMesh::Vec3d dir = firstPoint - secondPoint;
        dir.normalize();

        OpenMesh::Vec3d somaCener = firstPoint + dir;
        return new SimplePoint(somaCener[0],somaCener[1],somaCener[2],0.95f);
    } else {
        OpenMesh::Vec3d maxPoint{-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
                                 -std::numeric_limits<double>::max()};
        OpenMesh::Vec3d minPoint = -maxPoint;
        for (const auto &dendrite:dendrites) {
            maxPoint[0] = std::max(maxPoint[0], dendrite.dendrite.section[0]->point[0]);
            maxPoint[1] = std::max(maxPoint[1], dendrite.dendrite.section[0]->point[1]);
            maxPoint[2] = std::max(maxPoint[2], dendrite.dendrite.section[0]->point[2]);

            minPoint[0] = std::min(minPoint[0], dendrite.dendrite.section[0]->point[0]);
            minPoint[1] = std::min(minPoint[1], dendrite.dendrite.section[0]->point[1]);
            minPoint[2] = std::min(minPoint[2], dendrite.dendrite.section[0]->point[2]);
        }

        OpenMesh::Vec3d center = minPoint + (maxPoint - minPoint) / 2;

        double radius = calcSommaRadius(center);
        return new SimplePoint(center[0], center[1], center[2], radius);
    }
}

double ASC2SWCV2::calcSommaRadius(OpenMesh::Vec3d center) {

    double minDist = (center - dendrites[0].dendrite.section[0]->point).norm();
    for (int i = 1 ; i < dendrites.size();i++) {
        auto dist = (center - dendrites[i].dendrite.section[0]->point).norm();
        minDist = minDist > dist ? dist : minDist;
    }
    if (minDist == 0) {
        minDist++;
    }
    return minDist;

}
void ASC2SWCV2::joinApicals(std::vector<Dendrite>& apicals, const OpenMesh::Vec3d& somaCenter) {
    if (apicals.size() > 1) {
        //Comprobar que dendrita esta mas cerca del soma
        const Dendrite *principalApical = nullptr;
        float minDist = std::numeric_limits<float>::max();
        for (const Dendrite &api: apicals) {
            float dist = (api.dendrite.section[0]->point - somaCenter).norm();

            if (minDist > dist) {
                principalApical = &api;
                minDist = dist;
            }
        }

        std::vector<std::pair<int, SubDendrite *>> joinPoints;

        for (auto &api: apicals) {
            if (&api != principalApical) {
                minDist = std::numeric_limits<float>::max();
                int mini = -1;
                SubDendrite *minSubdendrite;
                std::stack<SubDendrite> subdendrites;
                subdendrites.push(api.dendrite);
                while (!subdendrites.empty()) {
                    auto subdendrite = subdendrites.top();
                    subdendrites.pop();
                    for (int i = 0; i < subdendrite.section.size(); i++) {
                        auto point = subdendrite.section[i]->point;
                        float dist = (point - api.dendrite.section[0]->point).norm();
                        if (minDist > dist) {
                            mini = i;
                            minDist = dist;
                            minSubdendrite = &subdendrite;
                        }
                    }

                    for (const auto &child: subdendrite.subDendrites) {
                        subdendrites.push(child);
                    }

                    joinPoints.emplace_back(mini, minSubdendrite);
                }

            }
        }

        for (int i = 0; i < joinPoints.size(); i++ ) {
            auto joinPoint = joinPoints[i];
            auto subdendrite = joinPoint.second;
            int conectionPoint = joinPoint.first;
            SubDendrite newSubDendrite;
            newSubDendrite.section.insert(newSubDendrite.section.begin(),subdendrite->section.begin() + conectionPoint,subdendrite->section.end());
            newSubDendrite.subDendrites = subdendrite->subDendrites;
            subdendrite->subDendrites.clear();

            subdendrite->section.erase(subdendrite->section.begin() + conectionPoint,subdendrite->section.end());
            subdendrite->subDendrites.push_back(apicals[i].dendrite);
            subdendrite->subDendrites.push_back(newSubDendrite);
        }
    }
}

MeshVCG *ASC2SWCV2::getSomaMesh() const {
    return _somaMesh;
}

const std::vector<Spine*> &ASC2SWCV2::getSpines() const {
    return spines;
}


void Dendrite::removeEmptySections(){
    this->dendrite.removeEmptySections(nullptr, 0);
}

bool SubDendrite::removeEmptySections(SubDendrite *parent, int pos) {
    while (!this->section.empty() && this->section[0]->isSpine()) {
        parent->section.push_back(this->section[0]);
        this->section.erase(this->section.begin());
    }


    if (this->section.empty()) {
        std::vector<int> toDel;
        for (size_t i = 0; i < this->subDendrites.size(); i++) {
            if (this->subDendrites[i].removeEmptySections(this, i)){
                toDel.push_back(i);
            }
        }

        parent->subDendrites.insert(parent->subDendrites.end(), this->subDendrites.begin(), this->subDendrites.end());

        for (int i = toDel.size() - 1; i >= 0; i--) {
            this->subDendrites.erase(this->subDendrites.begin() + toDel[i]);
        }

        return true;
    } else {
        std::vector<int> toDel;
        int size = this->subDendrites.size(); // OJO: Necesario para que this.subDendrites.size() no cambie de valor en las llamadas posteriores.
        for (size_t i = 0; i < size; i++) {
            if (this->subDendrites[i].removeEmptySections(this, i)){
                toDel.push_back(i);
            }
        }

        for (int i = toDel.size() - 1; i >= 0; i--) {
            this->subDendrites.erase(this->subDendrites.begin() + toDel[i]);
        }
        return false;
    }
}

void Dendrite::removeOnly1SubDend() {
    this->dendrite.removeOnly1SubDend(nullptr,0);
}

void SubDendrite::removeOnly1SubDend(SubDendrite *parent, int pos) {
    if (this->subDendrites.size() == 1) {
        auto section = this->subDendrites[0].section;
        section.insert(section.begin(),this->section.begin(),this->section.end());
        this->subDendrites[0].removeOnly1SubDend(this,0);
        parent->subDendrites.push_back(this->subDendrites[0]);
        parent->subDendrites.erase(parent->subDendrites.begin() + pos);
    } else {
        for (size_t i = 0; i < this->subDendrites.size(); i++) {
            subDendrites[i].removeOnly1SubDend(this,i);
        }
    }
}


/** ------------ TOSWC Methods --------------**/

void ASC2SWCV2::toSWC(const std::string &filename) {
    int counter = 1;
    std::ofstream file;
    file.open(filename,std::ofstream::out);
    if (!file.is_open()) {
        throw "Error to write a SWC file: Not able to open file";
    }

    this->soma->toSWC(counter, -1, 1,file);
    counter++;
    for(const auto& dendrite: this->dendrites) {
        dendrite.toSWC(counter,file);
    }

}

void SimplePoint::toSWC(int counter, int parent, int type, std::ofstream& file) const {
    file << std::setprecision(10) << counter << " " << type << " " << std::fixed << this->point[0] << " "
         << this->point[1] << " " << this->point[2] << " " << this->r << " " << parent << std::endl;
}

void Dendrite::toSWC(int &counter, std::ofstream &file) const {
    int type = this->type == Dendrite::Apical ? 4:3;
    return this->dendrite.toSWC(counter, 1, type,file);
}


void SubDendrite::toSWC(int &counter, int parent, int type, std::ofstream &file) const {

    if (this->section.empty()) {
        std::cerr << "[ERROR] Found a empty section, this its a error on ASC2SWC converter" << std::endl;
    }

    for (const auto& point: this->section) {
        if (!point->isSpine()) {
            point->toSWC(counter, parent, type, file);
            parent = counter;
            counter++;
        }
    }

    if (this->subDendrites.size() > 2) {
        int auxParent = parent;
        for (size_t i = 2; i < this->subDendrites.size(); i++) {
            subDendrites[i].toSWC(counter,auxParent - static_cast<int>(i) + 1,type,file);
        }
    }

    for (size_t i = 0; i < 2 && i < this->subDendrites.size(); i++ ) {
        subDendrites[i].toSWC(counter, parent, type,file);
    }

}

/** ------------ TOASC Methods --------------**/
void ASC2SWCV2::toASC(const std::string &outFile) {
    std::ofstream file;
    file.open(outFile, std::ofstream::out);
    std::string tab;
    for (const auto& dendrite:dendrites) {
        dendrite.toASC(tab,file);
    }
}

void ASC2SWCV2::toASC(const std::vector<std::vector<Eigen::Vector3f>> &contours, const Eigen::Vector3f &displacement,
                      const std::string &outFile) {
    std::ofstream file;
    file.open(outFile, std::ofstream::out);
    std::string tab;

    for (const auto &contour: contours) {
        file << "(\"Soma\" " << std::endl;
        file << "\t" << "(Closed)" << std::endl;
        file << "\t" << "(FillDensity 0)" << std::endl;
        file << "\t" << "(MBFObjectType 5)" << std::endl;
        for (const auto &point: contour) {
            file << "\t" << "(\t" << point[0] + displacement[0] << "\t" << point[1] + displacement[1] << "\t"
               << point[2] + displacement[2] << ")" << std::endl;
        }
        file << ")" << std::endl;
    }

    for (const auto& dendrite:dendrites) {
        dendrite.toASC(tab,file);
    }

    file.close();
}

void Dendrite::toASC(std::string tab, std::ofstream& file) const {
    std::string color = this->type== Dendrite::Apical ? "Green": "Blue";
    std::string type = this->type == Dendrite::Apical ? "Apical" : "Dendrite";
    file << tab << "(\t(Color " << color << ")" << std::endl;
    tab += "\t";
    file << tab << "(" << type << ")" << std::endl;
    this->dendrite.toASC(tab, file);

    file << tab << ") ; End of Tree" << std::endl;
}

void SubDendrite::toASC(std::string tab, std::ofstream &file) const {
    tab += "\t";

    if (this->section.empty()) {
        std::cerr << "[ERROR] Found a empty section, this its a error on ASC2SWC converter" << std::endl;
    }
    int n = 1;
    for (const auto &point : this->section) {
        point->toASC(tab, file);
        file << "\t;\t" << n << std::endl;
        n++;
    }
    if (!this->subDendrites.empty()) {
        file << tab << "(" << std::endl;
        subDendrites[0].toASC(tab, file);
        for (int i = 1; i < this->subDendrites.size(); i++) {
            file << tab << "|" << std::endl;
            this->subDendrites[i].toASC(tab, file);
        }
        file << tab << ")   ; End of Split" << std::endl;
    }

}

void SimplePoint::toASC(const std::string &tab, std::ofstream &file) const {
    file << std::setprecision(10) << std::fixed << tab << "( " << this->point[0] << "\t" << this->point[1] << "\t"
         << this->point[2] << "\t" << this->r << ")";
}



void Spine::toASC(const std::string &tab, std::ofstream &file) const  {
    file << tab << "<\t(Class 4 \"none\")" << std::endl;
    file << tab << "(Color Red)" << std::endl;
    file << tab << "(Generated 0)" << std::endl;
    file << std::setprecision(10) << std::fixed << tab << "( " << this->finalPoint.point[0] << "\t"
         << this->finalPoint.point[1] << "\t" << this->finalPoint.point[2] << "\t" << this->finalPoint.r << " )>";

}
