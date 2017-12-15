//
// Created by bl on 17-12-15.
//

#ifndef CERES_LEARNING_BUNDLE_ADJUSTMENT_H
#define CERES_LEARNING_BUNDLE_ADJUSTMENT_H

#include <cmath>
#include <cstdio>
#include <iostream>

#include <ceres/ceres.h>
#include <ceres/rotation.h>

class BAProblem {
public:
    ~BAProblem() {
        delete[] point_index_;
        delete[] camera_index_;
        delete[] observations_;
        delete[] parameters_;
    }

    int num_observations() const { return num_observations_; }

    const double *observations() const { return observations_; }

    double *mutable_cameras() { return parameters_; }

    double *mutable_points() { return parameters_ + 9 * num_cameras_; }

    double *mutable_camera_for_observation(int i) { return mutable_cameras() + camera_index_[i] * 9; }

    double *mutable_point_for_observation(int i) { return mutable_points() + point_index_[i] * 3; }

    bool LoadFile(const char *filename) {
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            return false;
        }

        FscanfOrDie(fp, "%d", &num_cameras_);
        FscanfOrDie(fp, "%d", &num_points_);
        FscanfOrDie(fp, "%d", &num_observations_);

        point_index_ = new int[num_observations_];
        camera_index_ = new int[num_observations_];
        observations_ = new double[2 * num_observations_];

        num_parameters_ = 9 * num_cameras_ + 3 * num_points_;
        parameters_ = new double[num_parameters_];

        for (int i = 0; i < num_observations_; ++i) {
            FscanfOrDie(fp, "%d", camera_index_ + i);
            FscanfOrDie(fp, "%d", point_index_ + i);
            for (int j = 0; j < 2; ++j) {
                FscanfOrDie(fp, "%lf", observations_ + 2 * i + j);
            }
        }

        for (int i = 0; i < num_parameters_; ++i) {
            FscanfOrDie(fp, "%lf", parameters_ + i);
        }

        return true;
    }

private:
    template<typename T>
    void FscanfOrDie(FILE *fp, const char *format, T *value) {
        int num_scanned = fscanf(fp, format, value);
        if (num_scanned != 1) {
            LOG(FATAL) << "Invalid data file.";
        }
    }

    int num_cameras_; //> 相机数目（图片数量）
    int num_points_; //> 点数量
    int num_observations_; //> 观察点数量
    int num_parameters_; //> 优化参数数量

    int *point_index_; //> 点索引
    int *camera_index_; //> 相机索引
    double *observations_; //> 观察点矩阵
    double *parameters_; //> 优化参数
};

struct SnavelyReprojectionError {
    SnavelyReprojectionError(double observed_x, double observed_y) : observed_x_(observed_x), observed_y_(observed_y) {}

    template<typename T>
    bool operator()(const T *const camera, const T *const point, T *residuals) const {
        T p[3];

        // 世界坐标系-->相机坐标系
        ceres::AngleAxisRotatePoint(camera, point, p);
        p[0] += camera[3];
        p[1] += camera[4];
        p[2] += camera[5];

        // 相机坐标系-->归一化坐标系
        T xp = -p[0] / p[2];
        T yp = -p[1] / p[2];

        // 畸变矫正（）
        /**************************************************************************************************************
         *     We use a pinhole camera model; the parameters we estimate for each camera area rotation R, a translation
         * t, a focal length f and two radial distortion parameters k1 and k2. The formula for projecting a 3D point X
         * into a camera R,t,f,k1,k2 is:
         *     P  =  R * X + t       (conversion from world to camera coordinates)
         *     p  = -P / P.z         (perspective division)
         *     p' =  f * r(p) * p    (conversion to pixel coordinates)
         *
         *     where P.z is the third (z) coordinate of P. In the last equation, r(p) is a function that computes a
         * scaling factor to undo the radial distortion:
         *     r(p) = 1.0 + k1 * ||p||^2 + k2 * ||p||^4.
         *
         *     camera[9] = {yaw, pitch, roll, t1, t2, t3, focal, k1, k2}
         *************************************************************************************************************/
        const T &k1 = camera[7];
        const T &k2 = camera[8];
        T r2 = xp * xp + yp * yp;
        T distortion = 1.0 + r2 * (k1 + k2 * r2);

        const T &focal = camera[6];
        T predicted_x = focal * distortion * xp;
        T predicated_y = focal * distortion * yp;

        // Error
        residuals[0] = predicted_x - observed_x_;
        residuals[1] = predicated_y - observed_y_;

        return true;
    }

    // Factory
    static ceres::CostFunction *Create(const double observed_x, const double observed_y) {
        return (new ceres::AutoDiffCostFunction<SnavelyReprojectionError, 2, 9, 3>(
                new SnavelyReprojectionError(observed_x, observed_y)));
    }

    double observed_x_;
    double observed_y_;
};

int SolveBA(const char *filename) {
    BAProblem ba_problem;
    if (!ba_problem.LoadFile(filename)) {
        std::cerr << "ERROR: unable to open file " << filename << "\n";

        return -1;
    }

    const double *observations = ba_problem.observations();

    ceres::Problem problem;
    for (int i = 0; i < ba_problem.num_observations(); ++i) {
        ceres::CostFunction *cost_function =
                SnavelyReprojectionError::Create(observations[2 * i + 0],
                                                 observations[2 * i + 1]);
        problem.AddResidualBlock(cost_function,
                                 NULL,
                                 ba_problem.mutable_camera_for_observation(i),
                                 ba_problem.mutable_point_for_observation(i));
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_SCHUR;
    options.minimizer_progress_to_stdout = true;

    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
    std::cout << summary.FullReport() << "\n";

    return 0;
}

#endif //CERES_LEARNING_BUNDLE_ADJUSTMENT_H
