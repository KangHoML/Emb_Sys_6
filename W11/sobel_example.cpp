#include <iostream>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    VideoCapture cap;

    // Video Device Open
    cap.open("/dev/video0", CAP_V4L2);
    if (!cap.isOpened()) {
        printf("Can't open camera!\n");
        return -1;
    }

    // Default Resolution
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    // Define the codec the create VideoWriter object
    VideoWriter video("sobel_video.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));

    // Define the variables for video image process
    int count = 0; int max;
    float r_val, g_val, b_val;
    float average_gray;
    
    Mat img;
    Mat gray(frame_height, frame_width, CV_8UC1);
    Mat sobelX, sobelY, sobel_gray, sobel_color;

    printf("Open Camera!\n");
    if (argc > 1) {
        max = int(argv[1]);
    }
    else {
        max = 50;
    }
    
    while(count <= max) {
        cap.read(img);
        if (img.empty()) break;

        for(int i = 0; i < frame_height; i++) {
            for(int j = 0; j < frame_width; j++) {
                r_val = img.at<Vec3b>(i, j)[2];
                g_val = img.at<Vec3b>(i, j)[1];
                b_val = img.at<Vec3b>(i, j)[0];

                average_gray = (int)((r_val + g_val + b_val) / 3);
                gray.at<uchar>(i,j) = average_gray;
            }
        }

        Sobel(gray, sobelX, CV_8U, 1, 0);
        Sobel(gray, sobelY, CV_8U, 0, 1);
        sobel_gray = abs(sobelX) + abs(sobelY);

        cvtColor(sobel_gray, sobel_color, COLOR_GRAY2BGR);
        video.write(sobel_color);
        count++;
    }
    
    cap.release();
    video.release();
    return 0;
}


