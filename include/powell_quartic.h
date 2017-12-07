//
// Created by bl on 17-11-29.
//

#ifndef CERES_LEARNING_POWELLQUARTIC_H
#define CERES_LEARNING_POWELLQUARTIC_H

#include <ceres/ceres.h>

class PowellQuartic {
public:
    PowellQuartic(double x1, double x2, double x3, double x4);

    std::vector<double> solve();

    double GetOptimizerValue();

public:
    std::vector<double> param;
    double result; ///> 最优值

private:
    struct CostFunctionPQ1 {
        template<typename T>
        bool operator()(const T *const x1, const T *const x2, T *residual) const {
            residual[0] = x1[0] + T(10.0) * x2[0];

            return true;
        }
    };

    struct CostFunctionPQ2 {
        template<typename T>
        bool operator()(const T *const x3, const T *const x4, T *residual) const {
            residual[0] = T(sqrt(5.0)) * (x3[0] - x4[0]);

            return true;
        }
    };

    struct CostFunctionPQ3 {
        template<typename T>
        bool operator()(const T *const x2, const T *const x3, T *residual) const {
            residual[0] = (x2[0] - 2.0 * x3[0]) * (x2[0] - 2.0 * x3[0]);

            return true;
        }
    };

    struct CostFunctionPQ4 {
        template<typename T>
        bool operator()(const T *const x1, const T *const x4, T *residual) const {
            residual[0] = T(sqrt(10.0)) * (x1[0] - x4[0]) * (x1[0] - x4[0]);

            return true;
        }
    };

    double x1_, x2_, x3_, x4_;
};

PowellQuartic::PowellQuartic(double x1, double x2, double x3, double x4) :
        x1_{x1}, x2_{x2}, x3_{x3}, x4_{x4} {

}

std::vector<double> PowellQuartic::solve() {
    //0 设置初始值

    //1 定义ceres问题
    ceres::Problem problem;

    //2 构建CostFunction
    // CostFunction, residual维数, 优化变量维数
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CostFunctionPQ1, 1, 1, 1>
                                     (new CostFunctionPQ1), NULL, &x1_, &x2_);
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CostFunctionPQ2, 1, 1, 1>
                                     (new CostFunctionPQ2), NULL, &x3_, &x4_);
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CostFunctionPQ3, 1, 1, 1>
                                     (new CostFunctionPQ3), NULL, &x2_, &x3_);
    problem.AddResidualBlock(new ceres::AutoDiffCostFunction<CostFunctionPQ4, 1, 1, 1>
                                     (new CostFunctionPQ4), NULL, &x1_, &x4_);

    //3 设置优化选项以及概要
    ceres::Solver::Options options;
    options.max_num_iterations = 100;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;

    //4 求解方程
    ceres::Solve(options, &problem, &summary);

    std::cout << summary.FullReport() << "\n";

    param.push_back(x1_);
    param.push_back(x2_);
    param.push_back(x3_);
    param.push_back(x4_);

    return param;
}

double PowellQuartic::GetOptimizerValue() {
    result = pow(x1_ + 10.0 * x2_, 2) + 5 * pow(x3_ - x4_, 2) + pow(x2_ - 2 * x3_, 4) + 10 * pow(x1_ - x4_, 4);

    return result;
}

#endif //CERES_LEARNING_POWELLQUARTIC_H
