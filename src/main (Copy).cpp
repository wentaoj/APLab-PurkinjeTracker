#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <doh_tracker.hpp>
#include "tm_tracker.hpp"
#include "image_generator.hpp"

nlohmann::json results_to_json(const vector<TrackingResult> &results)
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

int main(void)
{
    int width = 2048;
    int height = 996;
    int noise_level = 80;
    int num_images = 500;

    float amplitude1 = 2.0f;
    float sigma1 = 30.0f;
    float amplitude2 = 0.5f;
    float sigma2 = 10.0f;

    std::string video_src = "../results/output_video.avi";

    ImageGenerator generator(width, height, amplitude1, sigma1, amplitude2, sigma2, noise_level, num_images);

    cv::VideoWriter video(video_src, cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 10, cv::Size(width, height), false);

    ddpi::TemplateMatchTracker tracker;
    tracker.setup(
        width,
        height,
        1.0f,   // downsize factor
        0,      // gpu device
        20,     // noise
        3.0f,   // overlay scale
        160,    // p1 roi
        220,    // p1 threshold
        128,    // p4 roi
        128,    // intensity
        sigma2, // radius
        1.0f,   // GUI scale
        true    // tracking
    );

    nlohmann::json tracking_data;

    while (generator.hasNext())
    {
        cv::Mat image = generator.next();
        // video.write(image);

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
        tracking_data.push_back(frame_tracking_info);
    }
    video.release();

    generator.write_json("../results/groundtruth.json");

    std::ofstream moment_tracking_file("../results/moments_tracker.json");
    moment_tracking_file << tracking_data.dump(2);

    // DoH Tracker (TODO: read by frame)
    DoHTracker doh_tracker(50, 3, 20, 0.005, 0);
    vector<TrackingResult> results = doh_tracker.get_tracking_results(video_src);
    std::ofstream doh_tracking_file("../results/doh_tracker.json");
    doh_tracking_file << results_to_json(results).dump(2);
    return 0;
}
