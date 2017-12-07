//
// Created by bl on 17-11-25.
//
/**
 * Solve non-constrain optimization problem (2)
 * $$\begin{equation} f(x)=\sum_{n=1}^{N}a^{n}cos(b^{n} \pi x) \tag{2} \end{equation}$$
 */

#ifndef CERES_LEARNING_FITTING_CURVE_H
#define CERES_LEARNING_FITTING_CURVE_H

#include <cmath>

#include <vector>
#include <ceres/ceres.h>

class FittingCurve {
public:
    FittingCurve() {

    }

    void DataGenerate(double a0, double b0);

    std::vector<double> solve();

    std::vector<double> GetOptimizerValue();

    std::vector<double> ab;

private:
    struct CostFunctionFC {
        CostFunctionFC(double x, double y) : x_(x), y_(y) {

        }

        template<typename T>
        bool operator()(const T *const a, const T *const b, T *residual) const {
            /*微分困难
            T sum = T(0.0);
            for (int i = 0; i < 1000; i++) {
                sum += ceres::pow(a[0], T(i)) * ceres::cos(ceres::pow(b[0], T(i)) * M_PI * T(x_));
            }
            residual[0] = T(y_) - sum;
             */

            /*非线性可求微分，初始值难以选取
            residual[0] = T(y_) - ceres::exp(a[0] * a[0] * T(x_) * T(x_) + b[0] * T(x_));
             */

            residual[0] = T(y_) - ceres::exp(a[0] * T(x_) + b[0]);

            return true;
        }

    private:
        double x_;
        double y_;
    };

    double a;
    double b;
    std::vector<double> data;
};

void FittingCurve::DataGenerate(double a0, double b0) {
    if (!data.empty()) {
        data.clear();
    }

    for (int i = 0; i < 50; i++) {
        double x = (1.2 / 50) * i;
        data.push_back(x);

        double sum = 0.0;
        /*微分困难
        for (int j = 0; j < 1000; j++) {
            sum += std::pow(a0, double(j)) * std::cos(std::pow(b0, double(j)) * M_PI * x);
        }
         */

        /*非线性可求微分，初始值难以选取
        sum = std::exp(a0 * a0 * x * x + b0 * x);
         */

        sum = std::exp(a0 * x + b0);

        data.push_back(sum);
    }
}

std::vector<double> FittingCurve::solve() {
    //0 初始值
    this->a = 0.0;
    this->b = 0.0;

    //1 定义问题
    ceres::Problem problem;

    //2 构建CostFunction
    for (int i = 0; i < data.size() / 2; ++i) {
        ceres::CostFunction *costFunction = new ceres::AutoDiffCostFunction<CostFunctionFC, 1, 1, 1>(
                new CostFunctionFC(data[2 * i], data[2 * i + 1]));
        problem.AddResidualBlock(costFunction, NULL, &a, &b);
    }

    //3 设置优化选项以及概要
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;

    //4 求解方程
    ceres::Solve(options, &problem, &summary);


    //5 结果封装
    ab.push_back(a);
    ab.push_back(b);

    return ab;
}

std::vector<double> FittingCurve::GetOptimizerValue() {
    DataGenerate(a, b);

    return data;
}

#endif //CERES_LEARNING_FITTING_CURVE_H
