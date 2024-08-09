#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "doh_tracker.hpp"
#include "tm_tracker.hpp"
#include "image_generator.hpp"

nlohmann::json results_to_json(const std::vector<TrackingResult> &results)
{
    nlohmann::json json_arr;
    for (const auto &result : results)
    {
        nlohmann::json json_obj;
        json_obj["frame_num"] = result.frame_num;
        json_obj["P1"] = {result.P1.x, result.P1.y};
        json_obj["P4"] = {result.P4.x, result.P4.y};
        json_obj["dx"] = result.dx;
        json_obj["dy"] = result.dy;
        json_arr.push_back(json_obj);
    }
    return json_arr;
}

nlohmann::json read_config(const std::string &filename)
{
    std::ifstream file(filename);
    nlohmann::json config;
    file >> config;
    return config;
}

int main(void)
{
    nlohmann::json config = read_config("../src/config.json");

    // For Moments Tracker
    int width = config["width"];   // 2048
    int height = config["height"]; // 996
    float downsize_factor = config["downsize_factor"];
    int gpu_device = config["gpu_device"];
    int noise = config["noise"];
    float overlay_scale = config["overlay_scale"];
    int p1_roi = config["p1_roi"];
    int p1_threshold = config["p1_threshold"];
    int p4_roi = config["p4_roi"];
    int intensity = config["intensity"];
    float radius = config["radius"];
    float gui_scale = config["gui_scale"];
    bool tracking = config["tracking"];

    // For DoH Tracker
    int max_sigma = config["max_sigma"];
    int min_sigma = config["min_sigma"];
    int num_sigma = config["num_sigma"];
    float threshold = config["doh_threshold"];
    float overlap = config["doh_overlap"];

    // For Synthetic Images
    int noise_level = config["noise_level"];
    int num_images = config["num_images"];
    float amplitude1 = config["amplitude1"];
    float sigma1 = config["sigma1"];
    float amplitude4 = config["amplitude4"];
    float sigma4 = config["sigma4"];
    ImageGenerator generator(width, height, amplitude1, sigma1, amplitude4, sigma4, noise_level, num_images);

    // cv::VideoWriter video(vid_path, cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 10, cv::Size(width, height), false);

    ddpi::TemplateMatchTracker tracker;
    tracker.setup(
        width,
        height,
        downsize_factor,
        gpu_device,
        noise,
        overlay_scale,
        p1_roi,
        p1_threshold,
        p4_roi,
        intensity,
        radius,
        gui_scale,
        tracking);

    nlohmann::json moments_tracking_data;
    nlohmann::json doh_tracking_data;

    DoHTracker doh_tracker(max_sigma, min_sigma, num_sigma, threshold, overlap);

    while (generator.hasNext())
    {
        cv::Mat image = generator.next();
        // video.write(image);

        // Template Match Tracker
        tracker.upload(image.data, false);
        tracker.track();
        ddpi::DDPIdata_t data;
        tracker.getCurrentData(data);

        nlohmann::json frame_tracking_info;
        frame_tracking_info["frame#"] = generator.getCurrentImage();
        frame_tracking_info["P1"] = {data.p1.x, data.p1.y};
        frame_tracking_info["P4"] = {data.p4.x, data.p4.y};
        frame_tracking_info["P4_score"] = {data.p4.score}; // 0 if similar to template
        frame_tracking_info["dx"] = std::abs(data.p4.x - data.p1.x);
        frame_tracking_info["dy"] = std::abs(data.p4.y - data.p1.y);
        moments_tracking_data.push_back(frame_tracking_info);

        // DoH Tracker
        try
        {
            std::vector<cv::Point> centroids = doh_tracker.process_frame(image, generator.getCurrentImage());

            if (centroids.size() < 2)
            {
                throw std::runtime_error("Not enough blobs found");
            }

            TrackingResult doh_result;
            doh_result.frame_num = generator.getCurrentImage();
            doh_result.P1 = centroids[0];
            doh_result.P4 = centroids[1];
            doh_result.dx = std::abs(centroids[1].x - centroids[0].x);
            doh_result.dy = std::abs(centroids[1].y - centroids[0].y);

            // Store DoH tracking result
            nlohmann::json doh_tracking_info;
            doh_tracking_info["frame#"] = doh_result.frame_num;
            doh_tracking_info["P1"] = {doh_result.P1.x, doh_result.P1.y};
            doh_tracking_info["P4"] = {doh_result.P4.x, doh_result.P4.y};
            doh_tracking_info["dx"] = doh_result.dx;
            doh_tracking_info["dy"] = doh_result.dy;
            doh_tracking_data.push_back(doh_tracking_info);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing frame " << generator.getCurrentImage() << ": " << e.what() << std::endl;
        }
    }
    // video.release();

    generator.write_json("../results/groundtruth.json");

    std::ofstream moments_tracking_file("../results/moments_tracker.json");
    moments_tracking_file << moments_tracking_data.dump(2);

    std::ofstream doh_tracking_file("../results/doh_tracker.json");
    doh_tracking_file << doh_tracking_data.dump(2);

    return 0;
}