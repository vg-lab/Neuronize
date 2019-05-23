//
// Created by ivelascog on 20/05/19.
//

#ifndef NEURONIZE_BBDD_H
#define NEURONIZE_BBDD_H

#include <sqlite3.h>
#include <string>
#include <vector>

class BBDD {
    sqlite3 *_db;
    char* _err;
public:
    explicit BBDD(const std::string& filename);
    void addSpine(const std::string& name, float area, float volume, const std::string& neuronName, int origin, const std::string& file);
private:
    bool tableExist(const std::string& tableName);
    void createTableSpines();
    void createTableSomas();
    static std::vector<char> readBytes(const std::string& filename);

};


#endif //NEURONIZE_BBDD_H
