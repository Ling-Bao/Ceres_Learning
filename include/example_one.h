//
// Created by bl on 17-11-29.
//

#ifndef CERES_LEARNING_EXAMPLE_ONE_H
#define CERES_LEARNING_EXAMPLE_ONE_H

#include <ceres/ceres.h>

class ExampleOne {
public:
    ExampleOne(double x0);

    double solve();

    double GetOptimizerValue();

public:
    double x0_; ///> 初始值
    double result; ///> 最优值

private:
    struct CostFunctionEO {
        bool operator()(const double *const x, double *residual) const {
            residual[0] = 10.0 - x[0];
            return true;
        }
    };

    double x;
};

ExampleOne::ExampleOne(double x0) : x0_{x0} {

}

double ExampleOne::solve() {
    //0 设置初始值
    this->x = 0.0;

    //1 定义ceres问题
    ceres::Problem problem;

    //2 构建CostFunction
    // CostFunction, residual维数, 优化变量维数
    ceres::CostFunction *costFunction = new ceres::NumericDiffCostFunction<CostFunctionEO, ceres::CENTRAL, 1, 1>(
            new CostFunctionEO);
    problem.AddResidualBlock(costFunction, NULL, &x);

    //3 设置优化选项以及概要
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;

    //4 求解方程
    ceres::Solve(options, &problem, &summary);

    return x;
}

double ExampleOne::GetOptimizerValue() {
    result = (10.0 - x) * (10.0 - x) / 2.0;

    return result;
}

#endif //CERES_LEARNING_EXAMPLE_ONE_H
