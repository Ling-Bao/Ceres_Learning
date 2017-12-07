//
// Created by bl on 17-11-26.
//

#ifndef CERES_LEARNING_COMMON_H
#define CERES_LEARNING_COMMON_H

#include <fstream>
#include <string>

#include <iostream>
#include<iomanip>

template<typename T>
int PrintHit(std::string commit, T hit, int num_group) {
    if (num_group != 1) {
        std::cout << "Something wrong for input data!" << std::endl;

        return -1;
    }

    std::cout << commit << hit << std::endl;

    return 0;
}

template<typename T>
int PrintHitTemplate(std::string commit, const T &hit, int num_group) {
    if (hit.size() % num_group != 0) {
        std::cout << "Something wrong for input data!" << std::endl;

        return -1;
    }

    std::cout << commit << std::endl;

    for (int i = 0; i < hit.size();) {
        for (int j = 0; j < num_group; ++j) {
            std::cout << std::setprecision(5) << hit[i++] << "\t";
        }
        std::cout << std::endl;
    }
}

template<class Type>
Type StringToNum(const std::string &str) {
    std::istringstream iss(str);
    Type num;
    iss >> num;

    return num;
}

#endif //CERES_LEARNING_COMMON_H
