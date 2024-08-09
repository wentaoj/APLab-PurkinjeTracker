//
// Created by aplab on 7/25/24.
//
#include "nlohmann/json.hpp"
#include "tm_tracker.hpp"
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

struct gaussian2d_t
{
    float center_x;
    float center_y;
    float amplitude;
    float sigma;
};

float gaussian2(const gaussian2d_t &params, float x, float y)
{
    return params.amplitude * std::exp(-((std::pow(x - params.center_x, 2.0f) + std::pow(y - params.center_y, 2.0f)) /
                                         (2.0f * (params.sigma * params.sigma))));
}

void createImage(unsigned char *img, int width, int height, const gaussian2d_t &p1, const gaussian2d_t &p4)
{
    auto max = static_cast<float>(std::numeric_limits<unsigned char>::max());

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float val = gaussian2(p1, static_cast<float>(x), static_cast<float>(y)) + gaussian2(p4, static_cast<float>(x), static_cast<float>(y));
            val = val > 1.0f ? 1.0f : val;

            img[y * width + x] = static_cast<unsigned char>(val * max);
        }
    }
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

    auto *img = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    gaussian2d_t p1{
        .center_x = 253.0f,
        .center_y = 32.0f,
        .amplitude = 2.0f,
        .sigma = 10.0f};

    gaussian2d_t p4{
        .center_x = 738.0f,
        .center_y = 521.0f,
        .amplitude = 0.5f,
        .sigma = 3.0f};

    createImage(img, width, height, p1, p4);

    // save image
    cv::imwrite("../test/image.png", cv::Mat(height, width, CV_8U, img));

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

    tracker.upload(img, false);
    tracker.track();
    ddpi::DDPIdata_t data;
    tracker.getCurrentData(data);
    std::cout << "P1: <" << data.p1.x << ", " << data.p1.y << ">\n"
              << "P4: <" << data.p4.x << ", " << data.p4.y << ">\n"
              << "P4_score = " << data.p4.score << "\n";

    //// check P1 template and actual image
    int wid, hei;
    float *p1_image = new float[128 * 128];
    tracker.getP1Image(p1_image, wid, hei);
    // cv::Mat p1_transpose;
    cv::Mat p1_image_mat(hei, wid, CV_32F, p1_image);
    // cv::transpose(p1_image_mat, p1_transpose);

    cv::imwrite("../test/p1_T.png", p1_image_mat);

    //// check P4 template and actual image
    uchar *p4_template = new uchar[200 * 200];
    tracker.getP4Template(p4_template, wid, hei);
    cv::imwrite("../test/p4_template.png", cv::Mat(hei, wid, CV_8U, p4_template));

    float *p4_image = new float[200 * 200];
    tracker.getP4Image(p4_image, wid, hei);
    cv::imwrite("../test/p4_image.png", cv::Mat(wid, hei, CV_32FC1, p4_image));

    return 0;
    ////// TODO: Why P4 is nan??
}
