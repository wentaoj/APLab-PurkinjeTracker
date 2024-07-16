#include "iostream"
#include "fstream"
#include "vector"
#include "utility"
#include "cmath"
#include "algorithm"
#include "string"
#include "opencv2/opencv.hpp"
#include "tracker.hpp"
#include "blob.hpp"

using namespace std;
using namespace cv;

void DoHTracker::track(string &src)
{
    Frame generator(src);
    vector<TrackerData> pred_info;

    while (generator.hasNext())
    {
        auto [image, frame_num] = generator.next();

        cout << "Frame: " << frame_num << endl;
        Mat frame = image_proc(image);
        try
        {
            vector<Point> centroids = process_frame(frame, frame_num);

            if (centroids.size() < 2)
            {
                throw runtime_error("Not enough blobs found");
            }
            pred_info.push_back(save_results(frame_num, centroids));
        }
        catch (const exception &e)
        {
            pred_info.push_back(TrackerData());
        }
    }

    // TODO: implement save_preds to json
    for (const auto &data : pred_info)
    {
        cout << "Frame: " << data.frame_num << endl
             << " P1: " << data.P1 << " P4: " << data.P4
             << " dx: " << data.dx << " dy: " << data.dy << endl;
    }
    // save_preds(pred_info, src);
}

vector<Point> DoHTracker::process_frame(Mat &frame, int frame_num)
{
    Mat image = frame.clone();
    if (image.channels() == 3)
    {
        cvtColor(image, image, COLOR_BGR2GRAY);
    }

    vector<Point> centroids = detect_blobs(image, frame_num);

    if (centroids.size() < 2)
    {
        throw runtime_error("DoH Detection Error: Not enough blobs found at" + to_string(frame_num) + " frame\n");
        return {};
    }
    return centroids;
}

vector<Point> DoHTracker::detect_blobs(Mat &image, int frame_num)
{
    DoH doh(max_sigma, min_sigma, num_sigma, threshold, overlap);
    vector<Point3f> blobs = doh.get_blobs(image);

    for (const auto &blob : blobs)
    {
        cout << "blob: " << blob << endl;
    }

    if (blobs.size() < 2)
    {
        throw runtime_error("DoH Detection Error: Not enough blobs found at" + to_string(frame_num) + "\n");
        return {};
    }

    sort(blobs.begin(), blobs.end(), [](Point3f &a, Point3f &b)
         { return a.z > b.z; });

    vector<Point> centroids;
    for (int i = 0; i < blobs.size(); i++)
    {
        float x = blobs[i].x;
        float y = blobs[i].y;
        centroids.push_back(Point(x, y));
    }
    return centroids;
}
