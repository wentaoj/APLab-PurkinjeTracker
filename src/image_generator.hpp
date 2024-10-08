#pragma once

#include <random>
#include <vector>
#include <opencv2/opencv.hpp>
#include "nlohmann/json.hpp"

struct gaussian2d_t
{
    float center_x;
    float center_y;
    float amplitude;
    float sigma;
};

class ImageGenerator
{
public:
    float position_P1_x;
    float position_P1_y;
    float position_P4_x;
    float position_P4_y;
    
    ImageGenerator(int width, int height, float amplitude1, float sigma1, float amplitude2, float sigma2, int noise_level, int num_images);
    bool hasNext() const;
    cv::Mat next();
    void write_json(const std::string &filename);
    int getCurrentImage() const;

private:
    void createImage(unsigned char *img, int width, int height, const gaussian2d_t &p1, const gaussian2d_t &p4, int noise_level, std::default_random_engine &generator);
    void addNoise(unsigned char *img, int width, int height, int noise_level, std::default_random_engine &generator);
    float gaussian2(const gaussian2d_t &params, float x, float y);

    int width;
    int height;
    float amplitude1;
    float sigma1;
    float amplitude2;
    float sigma2;
    int noise_level;
    int num_images;
    int current_image;

    // Add the missing member variables
    int direction_P1;
    int direction_P4;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution_y;
    nlohmann::json frame_data;
};
