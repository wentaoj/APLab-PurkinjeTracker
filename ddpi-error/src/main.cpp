#include "iostream"
#include "vector"
#include "tm_tracker.hpp"
#include "ddpi_definition.hpp"
#include "utils.hpp"
#include "opencv2/opencv.hpp"

using namespace ddpi;
using namespace cv;
using namespace std;

void tm_tracker(string &src)
{
    Frame generator(src);
    TemplateMatchTracker *tm_tracker = new TemplateMatchTracker();

    Mat in_image;
    Mat image_gray;
    uchar *image = nullptr;
    size_t image_size = 0;
    int frame_num = 0;

    try
    {
        if (generator.hasNext())
        {
            // auto [frame, _] = generator.next();
            // cvtColor(frame.clone(), image_gray, COLOR_BGR2GRAY);
            // image_size = image_gray.rows * image_gray.cols * sizeof(uchar);
            // cudaMalloc((void **)&image, image_size);

            unsigned char *frame = new unsigned char[600 * 800];

            for (auto i = 0; i < 600; i++)
            {
                for (auto j = 0; j < 800; j++)
                {
                    frame[i * 800 + j] = (unsigned char)((float)(i) / 600.0f * 255.0f);
                }
            }

            tm_tracker->setup(
                600,
                800,
                1.0f / 1.0f, // downsamplerate
                0,           // device
                50,          // _noise: parameter. as threshold in localizeP4 function; localizeP1 always have 128.
                4.0f,        // overlayscale: when draw bbox rectangle
                70,          // P1 roi size
                200,         // P1 threshold
                64,          // P4 ROI size 64
                153,         // P4 intensity 70
                10.0f,       // P4 sigm
                1.0f,        // P4 GUI scale
                true);

            do
            {
                // cvtColor(frame.clone(), image_gray, COLOR_BGR2GRAY);

                // // TEST 2 original height x width
                // int mem_count = 0;
                // uchar *image_data = new uchar[image_size];
                // for (int row = 0; row < image_gray.rows; ++row) // # height
                // {
                //     for (int col = 0; col < image_gray.cols; ++col) // # width
                //     {
                //         if (image_gray.at<uchar>(row, col) > 0)
                //             mem_count++;

                //         image_data[row * image_gray.cols + col] = image_gray.at<uchar>(row, col);
                //     }
                // }

                // cudaMemcpy(image, image_data, image_size, cudaMemcpyHostToDevice);

                // delete[] image_data;

                // // END 2

                // // TEST 3 transpose X
                // int mem_count = 0;
                // uchar *image_data = new uchar[image_size];
                // for (int col = 0; col < image_gray.cols; ++col)
                // {
                //     for (int row = 0; row < image_gray.rows; ++row)
                //     {
                //         if (image_gray.at<uchar>(row, col) > 0)
                //             mem_count++;
                //         image_data[col * image_gray.rows + row] = image_gray.at<uchar>(row, col);
                //     }
                // }

                // cudaMemcpy(image, image_data, image_size, cudaMemcpyHostToDevice);

                // delete[] image_data;

                //// END 3

                ///////////////////// Original
                // cudaMemcpy(image, image_gray.data, image_size, cudaMemcpyHostToDevice);

                tm_tracker->upload(frame, false);
                int track_result = tm_tracker->track();
                /////////////////////

                imwrite("../image_host/frame" + to_string(frame_num) + ".png", Mat(600, 800, CV_8UC1, frame));

                if (track_result == 0) // DDPI_OK
                {
                    DDPIdata_t data;
                    tm_tracker->getCurrentData(&data);
                    cout << "DDPIdata_t: " << data.p1.x << ", " << data.p1.y << ", " << data.p4.x << ", " << data.p4.y << endl;

                    //// check BBox
                    int x, y, width, height;
                    tm_tracker->getP1BoundingBox(x, y, width, height);
                    cout << "BBoxP1: (" << x << ", " << y << ", " << width << ", " << height << ")" << endl;

                    tm_tracker->getP4BoundingBox(x, y, width, height);
                    cout << "BBoxP4: (" << x << ", " << y << ", " << width << ", " << height << ")" << endl;

                    float *p1_image = new float[128 * 128];
                    tm_tracker->getP1Image(p1_image, width, height);
                    cv::Mat p1_transpose;
                    cv::Mat p1_image_mat(height, width, CV_32FC1, p1_image);
                    cv::transpose(p1_image_mat, p1_transpose);

                    imwrite("../image_host/P1_image/" + to_string(frame_num) + ".png", p1_transpose);

                    //// check P4 template and actual image
                    uchar *p4_template = new uchar[256 * 256];
                    tm_tracker->getP4Template(p4_template, width, height);
                    imwrite("../image_host/P4_template/" + to_string(frame_num) + ".png", Mat(height, width, CV_8UC1, p4_template));

                    float *p4_image = new float[256 * 256]; // cout
                    tm_tracker->getP4Image(p4_image);
                    imwrite("../image_host/P4_image/" + to_string(frame_num) + ".png", Mat(width, height, CV_32FC1, p4_image));

                    ////// TEST END

                    ///// Below: default
                    float x1, y1, x4, y4;
                    tm_tracker->getP1(x1, y1);
                    tm_tracker->getP4(x4, y4);

                    break;

                    cout << "Processing Frame# " << frame_num << endl
                         << "Position P1: (" << x1 << ", " << y1 << ")" << endl
                         << "Position P4: (" << x4 << ", " << y4 << ")" << endl;
                }
                else
                {
                    cout << "Error raised." << endl;
                }
                frame_num++;
                if (!generator.hasNext())
                    break;
                auto next_frame_info = generator.next();
                // frame = next_frame_info.first;
            } while (frame_num < 100);
        }
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }

    cudaFree(image);
    tm_tracker->shutdown();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <video_path>" << std::endl;
        return 1;
    }

    string video = "../vid/" + string(argv[1]);
    timingdecorator(tm_tracker, video);
    return 0;
}