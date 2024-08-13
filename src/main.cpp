#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "doh_tracker.hpp"
#include "image_generator.hpp"
#include "kalman_filter.hpp"
#include "utils.hpp"

nlohmann::json read_config(const std::string &filename)
{
    std::ifstream file(filename);
    nlohmann::json config;
    file >> config;
    return config;
}

ImageGenerator main_logic(nlohmann::json &doh_tracking_data, nlohmann::json &kf_tracking_data)
{
    nlohmann::json config = read_config("../src/config.json");

    int width = config["width"];
    int height = config["height"];
    int max_sigma = config["max_sigma"];
    int min_sigma = config["min_sigma"];
    int num_sigma = config["num_sigma"];
    float threshold = config["doh_threshold"];
    float overlap = config["doh_overlap"];
    int noise_level = config["noise_level"];
    int num_images = config["num_images"];
    float amplitude1 = config["amplitude1"];
    float sigma1 = config["sigma1"];
    float amplitude4 = config["amplitude4"];
    float sigma4 = config["sigma4"];
    float dt = config["kf_dt"];
    float process_noise = config["kf_process_noise"];
    float measurement_noise = config["kf_measurement_noise"];
    float state_estimate_error = config["kf_state_estimate_error"];

    ImageGenerator generator(width, height, amplitude1, sigma1, amplitude4, sigma4, noise_level, num_images);
    // cv::VideoWriter video("create.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 10, cv::Size(width, height), false);

    DoHTracker doh_tracker(max_sigma, min_sigma, num_sigma, threshold, overlap);

    // First frame for initialization
    cv::Mat first_image = generator.next();
    std::vector<float> initial_positions = {
        static_cast<float>(generator.position_P1_x),
        static_cast<float>(generator.position_P1_y),
        static_cast<float>(generator.position_P4_x),
        static_cast<float>(generator.position_P4_y)};

    KalmanFilterTracker kf(dt, process_noise, measurement_noise, state_estimate_error, initial_positions);

    while (generator.hasNext())
    {
        cv::Mat image = generator.next();
        // video.write(image);

        try
        {
            std::vector<cv::Point> centroids = doh_tracker.process_frame(image, generator.getCurrentImage());

            if (centroids.size() < 2)
            {
                throw std::runtime_error("Not enough blobs found");
            }

            Eigen::VectorXf measurement(4);
            measurement << centroids[0].x, centroids[0].y, centroids[1].x, centroids[1].y;

            Eigen::VectorXf predicted_positions = kf.predict();
            Eigen::VectorXf updated_positions = kf.update(measurement);

            // Store DoH tracking result
            nlohmann::json doh_tracking_info;
            doh_tracking_info["frame#"] = generator.getCurrentImage();
            doh_tracking_info["P1"] = {centroids[0].x, centroids[0].y};
            doh_tracking_info["P4"] = {centroids[1].x, centroids[1].y};
            doh_tracking_info["dx"] = std::abs(centroids[1].x - centroids[0].x);
            doh_tracking_info["dy"] = std::abs(centroids[1].y - centroids[0].y);
            doh_tracking_data.push_back(doh_tracking_info);

            // Store Kalman Filter result
            nlohmann::json kf_tracking_info;
            kf_tracking_info["frame#"] = generator.getCurrentImage() - 1;
            kf_tracking_info["P1"] = {updated_positions[0], updated_positions[1]};
            kf_tracking_info["P4"] = {updated_positions[4], updated_positions[5]};
            kf_tracking_info["dx"] = std::abs(updated_positions[4] - updated_positions[0]);
            kf_tracking_info["dy"] = std::abs(updated_positions[5] - updated_positions[1]);
            kf_tracking_data.push_back(kf_tracking_info);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    // video.release();

    return generator;
}

int main(void)
{
    nlohmann::json doh_tracking_data;
    nlohmann::json kf_tracking_data;

    ImageGenerator generator = timing_decorator("Trackers", [&]()
                                                { return main_logic(doh_tracking_data, kf_tracking_data); });

    generator.write_json("../results/groundtruth.json");

    std::ofstream doh_tracking_file("../results/doh_tracker.json");
    doh_tracking_file << doh_tracking_data.dump(2);

    std::ofstream kf_tracking_file("../results/kf_tracker.json");
    kf_tracking_file << kf_tracking_data.dump(2);

    return 0;
}