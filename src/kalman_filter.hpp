#pragma once

#include <eigen3/Eigen/Dense>

class KalmanFilterTracker
{
public:
    KalmanFilterTracker(float dt, float process_noise, float measurement_noise, float state_estimate_error, const std::vector<float> &initial_positions);
    Eigen::VectorXf predict();
    Eigen::VectorXf update(const Eigen::VectorXf &z);

private:
    float dt;
    Eigen::VectorXf x;
    Eigen::MatrixXf P;
    Eigen::MatrixXf Q;
    Eigen::MatrixXf R;
    Eigen::MatrixXf F;
    Eigen::MatrixXf H;
};