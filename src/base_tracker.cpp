#include "iostream"
#include "fstream"
#include "cmath"
#include "vector"
#include "string"
#include "map"
#include "opencv2/opencv.hpp"
// #include "nlohmann/json.hpp"
#include "base_tracker.hpp"

using namespace std;
using namespace cv;

vector<pair<Mat, int>> IntermediaryTracker::read_frames(string &src)
{
    VideoCapture vidcap(src);
    vector<pair<Mat, int>> frames;
    Mat frame;
    int frame_num = 0;
    while (vidcap.read(frame))
    {
        frames.push_back({frame.clone(), frame_num});
        frame_num++;
    }
    return frames;
}

Mat IntermediaryTracker::image_proc(Mat &frame)
{
    Mat image_i, image_g, image_y, image;
    frame.convertTo(image_i, 5);
    cvtColor(image_i, image_g, COLOR_BGR2GRAY, 5);
    pow(image_g, 0.5, image_y);
    image_y.convertTo(image_g, 5);
    normalize(image_g, image, 0, 1, NORM_MINMAX, 5);
    return image;
}

TrackerData IntermediaryTracker::save_results(int frame_num, vector<Point> &centroids)
{
    float dx = abs(centroids[0].x - centroids[1].x);
    float dy = abs(centroids[0].y - centroids[1].y);
    return {frame_num, centroids[0], centroids[1], dx, dy};
}

// void save_preds(vector<TrackerData> &data_vec, string &src_video)
// {
//     string json_file_name = "pred_" + src_video + "_doh.json";
//     nlohmann::json json_arr;
//     for (const auto &data : data_vec)
//     {
//         nlohmann::json json;
//         json["frame#"] = data.frame_num;
//         json["P1"] = {data.P1.first, data.P1.second};
//         json["P4"] = {data.P4.first, data.P4.second};
//         json["dx"] = data.dx;
//         json["dy"] = data.dy;
//         json_arr.push_back(json);
//     }
//     ofstream file(json_file_name);
//     if (file.is_open())
//     {
//         file << json_arr.dump(2);
//         file.close();
//         cout << "Saved: data/" << json_file_name << endl;
//     }
//     else
//     {
//         cerr << "Failed: data/" << json_file_name << endl;
//     }
// }
