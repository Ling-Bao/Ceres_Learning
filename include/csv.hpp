//
// Created by bl on 17-11-26.
//
/**
 * 实现csv文件读写
 */

#ifndef CERES_LEARNING_CSV_H
#define CERES_LEARNING_CSV_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "common.hpp"

class CSV {
public:
    std::vector<std::vector<std::string>> &ReadCSV(const char *filename);

    int WriteCSV(std::vector<std::vector<std::string>> &content, const char *filename);

    std::vector<std::vector<double >> &StringToVector();

    int nRow;
    int nCol;
    std::vector<std::vector<std::string>> table;
    std::vector<std::vector<double>> vtable;
};

std::vector<std::vector<std::string>> &CSV::ReadCSV(const char *filename) {
    std::ifstream in;
    in.open(filename, std::ios::in);

    std::string lineStr;
    std::vector<std::vector<std::string>> strArray;
    while (std::getline(in, lineStr)) {
        std::stringstream ss(lineStr);
        std::string str;
        std::vector<std::string> lineArray;
        while (std::getline(ss, str, ',')) {
            lineArray.push_back(str);
        }

        nCol = lineArray.size();
        strArray.push_back(lineArray);
    }

    nRow = strArray.size();
    table = strArray;

    return table;
}

int CSV::WriteCSV(std::vector<std::vector<std::string>> &content, const char *filename) {
    std::ofstream out;
    out.open(filename, std::ios::out);

    for (auto it1 = content.begin(); it1 != content.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {

            if (it2 != it1->end() - 1) {
                out << it2->data() << ",";
            }
            else if (it1 != content.end() - 1) {
                out << it2->data() << std::endl;
            }
            else {
                out << it2->data();
            }
        }
    }
}

std::vector<std::vector<double>> &CSV::StringToVector() {
    /*
    double **data = new double*[csv.nRow];
    for (int i = 0; i < csv.nCol; ++i) {
        data[i] = new double[csv.nCol];
    }

    for (int i = 0; i < csv.nRow; ++i) {
        for (int j = 0; j < csv.nCol; ++j) {
            data[i][j] = StringToNum<double>(csvData[i][j]);
        }
    }
     */

    for (int i = 0; i < nRow; ++i) {
        std::vector<double> v;
        for (int j = 0; j < nCol; ++j) {
            v.push_back(StringToNum<double>(table[i][j]));
        }
        vtable.push_back(v);
    }

    return vtable;
}

#endif //CERES_LEARNING_CSV_H
