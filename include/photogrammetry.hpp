//
// Created by bl on 17-11-26.
//

#ifndef CERES_LEARNING_PHOTOGRAMMETRY_H
#define CERES_LEARNING_PHOTOGRAMMETRY_H

#include <ceres/ceres.h>
#include <vector>

#include "csv.hpp"

class PhotoGrammetry {
public:
    PhotoGrammetry(const char *filename, double focal, double ratio) :
            filename_(filename), focal_(focal / 1000), ratio_(ratio) {

    }

    void ReadFromCSV();

    void solve();

    std::vector<double> GetOptimizerParameters();

    std::vector<std::vector<double>> csvData;
    int nRow;
    int nCol;
    std::vector<double> res;

private:
    struct CostFunctionPG {
        CostFunctionPG(double x, double y, double X, double Y, double Z, double f) :
                x_(x), y_(y), X_(X), Y_(Y), Z_(Z), f_(f) {

        }

        template<typename T>
        bool operator()(const T *const pBackCrossParameters, T *residual) const {
            T dXs = pBackCrossParameters[0];
            T dYs = pBackCrossParameters[1];
            T dZs = pBackCrossParameters[2];
            T dPhi = pBackCrossParameters[3];
            T dOmega = pBackCrossParameters[4];
            T dKappa = pBackCrossParameters[5];

            T a1 = ceres::cos(dPhi) * ceres::cos(dKappa) - ceres::sin(dPhi) * ceres::sin(dOmega) * ceres::sin(dKappa);
            T a2 = -ceres::cos(dPhi) * ceres::sin(dKappa) - ceres::sin(dPhi) * ceres::sin(dOmega) * ceres::cos(dKappa);
            T a3 = -ceres::sin(dPhi) * ceres::cos(dOmega);
            T b1 = ceres::cos(dOmega) * ceres::sin(dKappa);
            T b2 = ceres::cos(dOmega) * ceres::cos(dKappa);
            T b3 = -ceres::sin(dOmega);
            T c1 = ceres::sin(dPhi) * ceres::cos(dKappa) + ceres::cos(dPhi) * ceres::sin(dOmega) * ceres::sin(dKappa);
            T c2 = -ceres::sin(dPhi) * ceres::sin(dKappa) + ceres::cos(dPhi) * ceres::sin(dOmega) * ceres::cos(dKappa);
            T c3 = ceres::cos(dPhi) * ceres::cos(dOmega);

            residual[0] = T(x_) + T(f_) * T((a1 * (X_ - dXs) + b1 * (Y_ - dYs) + c1 * (Z_ - dZs)) /
                                            ((a3 * (X_ - dXs) + b3 * (Y_ - dYs) + c3 * (Z_ - dZs))));
            residual[1] = T(y_) + T(f_) * T((a2 * (X_ - dXs) + b2 * (Y_ - dYs) + c2 * (Z_ - dZs)) /
                                            ((a3 * (X_ - dXs) + b3 * (Y_ - dYs) + c3 * (Z_ - dZs))));

            return true;
        }

    private:
        double x_;
        double y_;
        double X_;
        double Y_;
        double Z_;
        double f_;
    };

    const char *filename_;
    double focal_;
    double ratio_;
    double param[6] = {0.0};
};

void PhotoGrammetry::ReadFromCSV() {
    CSV csv;

    csv.ReadCSV(filename_);
    nRow = csv.nRow;
    nCol = csv.nCol;
    csvData = csv.StringToVector();
}

void PhotoGrammetry::solve() {
    //0 从csv文件读取数据
    ReadFromCSV();

    //1 定义问题
    ceres::Problem problem;

    //2 设置初始值
    for (int j = 0; j < 3; ++j) {
        double sum = 0;
        for (int i = 0; i < nRow; ++i) {
            sum += csvData[i][j + 2];
        }
        param[j] = sum / nRow;
    }
    param[2] += focal_ / ratio_;

    //3 构建CostFunction
    for (int i = 0; i < nRow; ++i) {
        CostFunctionPG *costFunctionPG = new CostFunctionPG(csvData[i][0] / 1000, csvData[i][1] / 1000, csvData[i][2],
                                                            csvData[i][3], csvData[i][4], focal_);
        problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CostFunctionPG, 2, 6>(costFunctionPG), NULL, param);
    }

    //4 设置优化选项以及概要
    ceres::Solver::Options options;
    options.max_num_iterations = 50;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;

    //5 求解方程
    ceres::Solve(options, &problem, &summary);

    //6 结果封装
    for (int i = 0; i < 6; ++i) {
        res.push_back(param[i]);
    }
}

std::vector<double> PhotoGrammetry::GetOptimizerParameters() {
    return res;
}

#endif //CERES_LEARNING_PHOTOGRAMMETRY_H
