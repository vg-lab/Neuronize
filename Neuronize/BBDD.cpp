//
// Created by ivelascog on 20/05/19.
//

#include "BBDD.h"
#include <boost/format.hpp>
#include <fstream>

#define ERRCHECK {if (_err!=NULL) {printf("%s\n",_err); sqlite3_free(_err);}}

static int existCallback(void *exist, int columns, char **data, char **columnNames) {
    *((bool *) exist) = true;
    return 0;

}

std::vector<char> BBDD::readBytes(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(result.data(), pos);

    return result;
}

BBDD::BBDD(const std::string& filename) {
    sqlite3_open(filename.c_str(),&_db);
    if (!tableExist("SPINES")){
        createTableSpines();
    }

    if(!tableExist("SOMAS")){
        createTableSomas();
    }

}


bool BBDD::tableExist(const std::string &tableName_) {
    std::string Query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName_+"';";
    bool exist;
    sqlite3_exec(_db,Query.c_str(), existCallback,&exist,&_err);
    ERRCHECK
    return exist;
}

void BBDD::createTableSpines() {
    std::string query = "CREATE TABLE SPINES (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                        "                    NAME TEXT NOT NULL,\n"
                        "                    AREA REAL,\n"
                        "                    VOLUME REAL,\n"
                        "                    NEURON TEXT NOT NULL,\n"
                        "                    ORIGIN INT,\n"
                        "                    MODEL BLOB);";
    sqlite3_exec(_db,query.c_str(), nullptr, nullptr,&_err);
    ERRCHECK
}

void BBDD::createTableSomas() {
    std::string query = "CREATE TABLE SOMAS (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\n"
                        "                    NAME TEXT,\n"
                        "                    AREA REAL\n"
                        "                    VOLUME REAL,\n"
                        "                    NEURON TEXT NOT NULL,\n"
                        "                    ORIGIN INT,\n"
                        "                    MODEL BLOB);";
    sqlite3_exec(_db,query.c_str(),nullptr, nullptr,&_err);
    ERRCHECK


}

void BBDD::addSpine(const std::string& name, float area, float volume, const std::string& neuronName, int origin, const std::string& file) {
    std::string query = "INSERT INTO SPINES (NAME, AREA, VOLUME, NEURON, ORIGIN, MODEL) VALUES ('%s',%f,%f,'%s',%i,'%x')";
    std::string file2 = readBytes(file).data();
    std::string formatedQuery = str(boost::format(query) %name %area %volume %neuronName %origin %readBytes(file).data());
    sqlite3_exec(_db,formatedQuery.c_str(), nullptr, nullptr,&_err);
    ERRCHECK

}




