#include "iostream"
#include "vector"
#include "utility"
#include "numeric"
#include "opencv2/opencv.hpp"
#include "blob.hpp"

using namespace std;
using namespace cv;

vector<Point3f> DoH::get_blobs(Mat &in_image)
{
    Mat image_g, image_n, image_i, image;
    normalize(in_image, image_g, 0, 1, NORM_MINMAX, 5);
    integral(image_g, image_n, 5);
    image_i = image_n(Range(1, image_n.rows), Range(1, image_n.cols)).clone();
    image_i.convertTo(image, 5);
    auto sigmas = logspace(log10(min_sigma), log10(max_sigma), num_sigma);
    vector<Mat> image_2d;
    for (int j = 0; j < sigmas.size(); j++)
    {
        image_2d.push_back(Mat::zeros(image.rows, image.cols, 5));
    }
    for (int i = 0; i < sigmas.size(); i++)
    {
        hessian_mat(image, sigmas[i]).copyTo(image_2d[i]);
    }
    auto lm = local_max(image_2d, threshold);
    if (lm.empty())
    {
        return {};
    }
    for (int i = 0; i < lm.size(); i++)
    {
        lm[i].z = sigmas[lm[i].z];
    }
    return get_prune(lm, overlap);
}

vector<float> logspace(float start, float stop, int num)
{
    vector<float> result;
    float delta = (stop - start) / (num - 1);
    for (int i = 0; i < num; i++)
    {
        result.push_back(pow(10, start + i * delta));
    }
    return result;
}

vector<int> argsort(vector<float> &inten)
{
    vector<int> idx(inten.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&inten](int i1, int i2)
         { return inten[i1] < inten[i2]; });
    return idx;
}

vector<Point3f> DoH::get_prune(vector<Point3f> &blob, float overlap)
{
    vector<Rect> pos;
    vector<float> conf;
    vector<int> mat_i;
    vector<Point3f> out;

    if (blob.empty())
    {
        return blob;
    }
    for (int i = 0; i < blob.size(); i++)
    {
        float y = blob[i].y;
        float x = blob[i].x;
        float r = blob[i].z;
        float radius = r * sqrt(2);
        float tlx = x - radius;
        float tly = y - radius;
        float width = 2 * radius;
        float height = 2 * radius;
        pos.push_back(Rect(tlx, tly, width, height));
        conf.push_back(r);
    }
    dnn::NMSBoxes(pos, conf, 0.7f, overlap, mat_i);
    if (mat_i.size() == 0)
    {
        return {};
    }
    for (int i = 0; i < mat_i.size(); i++)
    {
        out.push_back(blob[mat_i[i]]);
    }
    return out;
}

float DoH::integral_img(Mat &image, int r, int c, int rl, int cl)
{
    r = max(0, min(r, image.rows - 1));
    c = max(0, min(c, image.cols - 1));
    int r2 = max(0, min(r + rl, image.rows - 1));
    int c2 = max(0, min(c + cl, image.cols - 1));
    return max(0.0f, image.at<float>(r, c) + image.at<float>(r2, c2) - image.at<float>(r, c2) - image.at<float>(r2, c));
}

vector<Point3f> DoH::get_peak(vector<Mat> &image, vector<Mat> &mask, int num_peaks)
{
    vector<float> peak;
    vector<Point3f> coords;
    vector<Point3f> top_cords;
    vector<Point> points;

    for (int z = 0; z < image.size(); z++)
    {
        findNonZero(mask[z], points);
        for (int i = 0; i < points.size(); i++)
        {
            coords.emplace_back(points[i].x, points[i].y, z);
            peak.push_back(-image[z].at<float>(points[i]));
        }
    }
    auto sort = argsort(peak);
    for (int i = 0; i < num_peaks; i++)
    {
        top_cords.push_back(coords[sort[i]]);
    }
    return top_cords;
}

vector<Mat> DoH::get_mask(vector<Mat> &image, vector<Mat> &footprint, float threshold)
{
    vector<Mat> out;

    if (footprint.size() == 1 || image.size() == 1)
    {
        for (int i = 0; i < image.size(); i++)
        {
            Mat mask;
            compare(image[i], threshold, mask, CMP_GT);
            out.push_back(mask / 255);
        }
        return out;
    }

    for (int i = 0; i < image.size(); i++)
    {
        Mat image_mask = image[i].clone();
        for (int j = -1; j <= 1; j++)
        {
            int curr = i + j;
            if (curr >= 0 && curr < image.size())
            {
                Mat layer;
                dilate(image[curr], layer, footprint[j + 1]);
                max(image_mask, layer, image_mask);
            }
        }

        Mat mask1, mask2, mask;
        compare(image[i], image_mask, mask1, CMP_EQ);
        compare(image[i], threshold, mask2, CMP_GE);
        bitwise_and(mask1, mask2, mask);
        out.push_back(mask);
    }
    return out;
}

vector<Point3f> DoH::local_max(vector<Mat> &image, float threshold, int num_peaks)
{
    double image_max = 0.0;
    for (int i = 0; i < image.size(); i++)
    {
        double mi, mx;
        minMaxLoc(image[i], &mi, &mx);
        image_max = max(mx, image_max);
    }

    float image_peak = image_max / num_peaks;
    vector<Mat> footprint;
    for (int i = 0; i < 3; i++)
    {
        footprint.push_back(Mat::ones(3, 3, 5));
    }
    threshold = max(threshold, image_peak);

    auto mask = get_mask(image, footprint, threshold);
    return get_peak(image, mask, num_peaks);
}

Mat DoH::hessian_mat(Mat &image, float sigma)
{
    int size, height, width, s2, s3, w;
    size = 3 * sigma;
    height = image.rows;
    width = image.cols;
    s2 = (size - 1) / 2;
    s3 = size / 3;
    w = size;

    Mat out = Mat(image.size(), 5);

    float w_i = 1.0 / (size * size);

    if (size % 2 == 0)
    {
        size += 1;
    }
    for (int r = 0; r < height; r++)
    {
        for (int c = 0; c < width; c++)
        {
            float tl = integral_img(image, r - s3, c - s3, s3, s3);
            float br = integral_img(image, r + 1, c + 1, s3, s3);
            float bl = integral_img(image, r - s3, c + 1, s3, s3);
            float tr = integral_img(image, r + 1, c - s3, s3, s3);
            float mid, side, dxx, dxy, dyy;
            dxy = bl + tr - tl - br;
            dxy = -dxy * w_i;
            mid = integral_img(image, r - s3 + 1, c - s2, 2 * s3 - 1, w);
            side = integral_img(image, r - s3 + 1, c - s3 / 2, 2 * s3 - 1, s3);
            dxx = mid - 3 * side;
            dxx = -dxx * w_i;
            mid = integral_img(image, r - s2, c - s3 + 1, w, 2 * s3 - 1);
            side = integral_img(image, r - s3 / 2, c - s3 + 1, s3, 2 * s3 - 1);
            dyy = mid - 3 * side;
            dyy = -dyy * w_i;

            out.at<float>(r, c) = dxx * dyy - 0.81 * (dxy * dxy);
        }
    }
    return out;
}