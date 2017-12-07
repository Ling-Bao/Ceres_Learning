#include "common.hpp"
#include "hello_word.hpp"
#include "fitting_curve.hpp"
#include "csv.hpp"
#include "photogrammetry.hpp"
#include "example_one.h"
#include "powell_quartic.h"

using namespace std;

/**
 * 1 非约束最优化问题(AutoDiffCostFunction)
 */
void SolveProblem1() {
    PrintHit("------------Problem (1)------------", "AutoDiffCostFunction", 1);
    double x0 = 5.0; // 设置初值
    double x;
    double result;

    HelloWorld helloWorld(x0);
    x = helloWorld.solve();
    result = helloWorld.GetOptimizerValue();

    PrintHit("Solve x = ", x, 1);
    PrintHit("Optimization value = ", result, 1);
}

/**
 * 2 曲线拟合
 */
void SolveProblem2() {
    PrintHit("------------Problem (2)------------", "", 1);
    double a0 = 0.3; // 设置初值
    double b0 = 0.1;
    vector<double> ab;
    vector<double> result;

    FittingCurve fittingCurve;
    fittingCurve.DataGenerate(a0, b0);
    ab = fittingCurve.solve();
    result = fittingCurve.GetOptimizerValue();

    PrintHit("Solve a = ", ab[0], 1);
    PrintHit("Solve b = ", ab[1], 1);
    PrintHitTemplate("Optimization value = ", result, 2);
}

/**
 * 3 测试CSV类
 */
void TestCSV() {
    PrintHit("------------Testing CSV------------", "", 1);

    vector<vector<string>> content{{"bl", "24"},
                                   {"cm", "23"}};
    string filename = "/home/bl/MyWorkspace/OpenSource/Ceres_Learning/support/data/test.csv";
    vector<vector<string>> csvData;
    CSV csv;

    csv.WriteCSV(content, filename.c_str());

    csvData = csv.ReadCSV(filename.c_str());
    for (auto it1 = csvData.begin(); it1 != csvData.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (it2 != it1->end() - 1) {
                cout << it2->data() << ",";
            }
            else if (it1 != csvData.end() - 1) {
                cout << it2->data() << std::endl;
            }
            else {
                cout << it2->data();
            }
        }
    }
}

/**
 * 3 Photogrammetry
 */
void SolveProblem3() {
    string filename = "/home/bl/MyWorkspace/OpenSource/Ceres_Learning/support/data/photogrammetry.csv";
    double focal = 153.24;
    double ratio = 1. / 40000;

    vector<double> res;

    PhotoGrammetry photoGrammetry(filename.c_str(), focal, ratio);
    photoGrammetry.solve();
    res = photoGrammetry.GetOptimizerParameters();


    PrintHit("Optimization parameters: Xs\tYs\tZs\tPhi\tOmega\tKappa", "None", 1);
    for (int i = 0; i < res.size(); ++i) {
        cout << res[i] << "\t";
    }
    cout << endl;
}

/**
 * 4 非约束最优化问题(NumericDiffCostFunction)
 */
void SolveProblem4() {
    PrintHit("------------Problem (4)------------", "NumericDiffCostFunction", 1);
    double x0 = 5.0; // 设置初值
    double x;
    double result;

    ExampleOne exampleOne(x0);
    x = exampleOne.solve();
    result = exampleOne.GetOptimizerValue();

    PrintHit("Solve x = ", x, 1);
    PrintHit("Optimization value = ", result, 1);
}

/**
 * 5 Powell's quartic function
 */
void SolveProblem5() {
    PrintHit("------------Problem (5)------------", "Powell's quartic function", 1);
    double x1 = 3.0;
    double x2 = -1.0;
    double x3 = 0.0;
    double x4 = 1.0;

    vector<double> params;
    double res;
    PowellQuartic powellQuartic(x1, x2, x3, x4);
    params = powellQuartic.solve();
    res = powellQuartic.GetOptimizerValue();

    PrintHit("Optimization parameters: x1\tx2\tx3\tx4", "", 1);
    for (int i = 0; i < params.size(); ++i) {
        cout << params[i] << "\t";
    }
    cout << endl;

    PrintHit("Optimization value = ", res, 1);
}

int main() {
//    SolveProblem1();
//    SolveProblem2();
//    TestCSV();
//    SolveProblem3();
//    SolveProblem4();
    SolveProblem5();

    return 0;
}