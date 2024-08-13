#include "kalman_filter.hpp"

KalmanFilterTracker::KalmanFilterTracker(float dt, float process_noise, float measurement_noise, float state_estimate_error, const std::vector<float> &initial_positions)
    : dt(dt), x(8), P(8, 8), Q(8, 8), R(4, 4), F(8, 8), H(4, 8)
{
    // Initialize state vector
    x << initial_positions[0], initial_positions[1], 0, 0, initial_positions[2], initial_positions[3], 0, 0;

    // state covariance matrix P
    P = state_estimate_error * Eigen::MatrixXf::Identity(8, 8);

    // process noise covariance Q
    Q = Eigen::MatrixXf::Identity(8, 8) * process_noise;

    // measurement noise covariance R
    R = Eigen::MatrixXf::Identity(4, 4) * measurement_noise;

    // state transition matrix F
    F << 1, 0, dt, 0, 0, 0, 0, 0,
        0, 1, 0, dt, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, dt, 0,
        0, 0, 0, 0, 0, 1, 0, dt,
        0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 1;

    // measurement matrix H
    H << 1, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0;
}

Eigen::VectorXf KalmanFilterTracker::predict()
{
    // Predict the next state and Cov
    x = F * x;
    P = F * P * F.transpose() + Q;
    return x;
}

Eigen::VectorXf KalmanFilterTracker::update(const Eigen::VectorXf &z)
{
    // Measurement residual
    Eigen::VectorXf y = z - H * x;
    // Residual covariance
    Eigen::MatrixXf S = H * P * H.transpose() + R;
    // Kalman gain
    Eigen::MatrixXf K = P * H.transpose() * S.inverse();

    // state estimate + covariance
    x = x + K * y;
    Eigen::MatrixXf I = Eigen::MatrixXf::Identity(8, 8);
    P = (I - K * H) * P;

    return x;
}