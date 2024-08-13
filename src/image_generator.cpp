#include <cmath>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>
#include "image_generator.hpp"

using namespace std;

const int MOVEMENT_CONSTANT = 1.2;

ImageGenerator::ImageGenerator(int width, int height, float amplitude1, float sigma1, float amplitude2, float sigma2, int noise_level, int num_images)
    : width(width), height(height), amplitude1(amplitude1), sigma1(sigma1), amplitude2(amplitude2), sigma2(sigma2), noise_level(noise_level), num_images(num_images), current_image(0),
      position_P1_x(50), position_P1_y(height / 2), position_P4_x(450), position_P4_y(height / 2), direction_P1(1), direction_P4(1)
{
    generator = std::default_random_engine(42);
    distribution_y = std::uniform_real_distribution<float>(30, height - 30);
}

bool ImageGenerator::hasNext() const
{
    return current_image < num_images;
}

cv::Mat ImageGenerator::next()
{
    // update
    position_P1_x += direction_P1 * (10 + MOVEMENT_CONSTANT);
    if (position_P1_x <= 50 || position_P1_x >= 1024 - 50)
    {
        direction_P1 = -direction_P1;
    }

    position_P4_x += direction_P4 * 10; // Move P4 horizontally
    if (position_P4_x <= 450 || position_P4_x >= width - 50)
    {
        direction_P4 = -direction_P4;
    }

    if (direction_P1 > 0)
    {
        position_P1_x = std::min(position_P1_x, position_P4_x - 10);
    }
    else
    {
        position_P1_x = std::max(position_P1_x, position_P4_x + 10);
    }

    gaussian2d_t p1{
        .center_x = position_P1_x,
        .center_y = position_P1_y,
        .amplitude = amplitude1,
        .sigma = sigma1};

    gaussian2d_t p4{
        .center_x = position_P4_x,
        .center_y = position_P4_y,
        .amplitude = amplitude2,
        .sigma = sigma2};

    auto *img = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    createImage(img, width, height, p1, p4, noise_level, generator);
    cv::Mat image(height, width, CV_8UC1, img);

    nlohmann::json frame_info;
    frame_info["frame#"] = current_image;
    frame_info["P1"] = {p1.center_x, p1.center_y};
    frame_info["P4"] = {p4.center_x, p4.center_y};
    frame_info["dx"] = std::abs(p4.center_x - p1.center_x);
    frame_info["dy"] = std::abs(p4.center_y - p1.center_y);
    frame_data.push_back(frame_info);

    current_image++;
    return image;
}

void ImageGenerator::write_json(const std::string &filename)
{
    std::ofstream file(filename);
    file << frame_data.dump(2);
}

int ImageGenerator::getCurrentImage() const
{
    return current_image - 1;
}

void ImageGenerator::createImage(unsigned char *img, int width, int height, const gaussian2d_t &p1, const gaussian2d_t &p4, int noise_level, std::default_random_engine &generator)
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

    addNoise(img, width, height, noise_level, generator);
}

void ImageGenerator::addNoise(unsigned char *img, int width, int height, int noise_level, std::default_random_engine &generator)
{
    std::uniform_int_distribution<int> distribution(0, noise_level);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int noise = distribution(generator);
            int index = y * width + x;
            int new_value = img[index] + noise;
            img[index] = static_cast<unsigned char>(std::min(new_value, 255));
        }
    }
}

float ImageGenerator::gaussian2(const gaussian2d_t &params, float x, float y)
{
    return params.amplitude * std::exp(-((std::pow(x - params.center_x, 2.0f) + std::pow(y - params.center_y, 2.0f)) / (2.0f * (params.sigma * params.sigma))));
}