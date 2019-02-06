#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "config.hpp"
#include "macros.hpp"

using namespace cv;
using namespace std;

// #define USE_CAMERA

#ifdef USE_CAMERA
constexpr int WAITKEY_DELAY = 16;
#else
constexpr int WAITKEY_DELAY = 0;
#endif

int main(int argc, char **argv)
{
    namedWindow("params");
    ALGORITHM_PARAM(hlow, 54, 255);
    ALGORITHM_PARAM(hhigh, 77, 255);
    ALGORITHM_PARAM(slow, 137, 255);
    ALGORITHM_PARAM(shigh, 255, 255);
    ALGORITHM_PARAM(vlow, 0, 255);
    ALGORITHM_PARAM(vhigh, 89, 255);
    ALGORITHM_PARAM(minTargetRectArea, 100, 500);
    ALGORITHM_PARAM(minTargetFullness, 500, 1000);

#ifdef USE_CAMERA
    VideoCapture cam(0);
#else
    cv::String path("../c2018/vision/test-img/*.jpg"); //select only jpg
    vector<cv::String> fn;
    vector<cv::Mat> data;
    cv::glob(path, fn, true);
    size_t k = 0;
    if (fn.empty())
    {
        return 0;
    }
#endif
    Mat raw;
    Mat resized;
    Mat undistorted;
    Mat blurred;
    Mat hsv;
    Mat mask;
    vector<vector<Point>> contours;
    vector<Point> convex_cnt;
    vector<Point> poly;
    vector<RotatedRect> targets;

    for (;;)
    {
#ifdef USE_CAMERA
        cam >> raw;
#else
        raw = cv::imread(fn[min(k, fn.size() - 1)]);
#endif
        SHOW("raw", raw);

        resize(raw, resized, Size(320, 240));
        SHOW("resized", resized);

        // TODO undistort
        resized.copyTo(undistorted);
        SHOW("undistorted", undistorted)

        GaussianBlur(undistorted, blurred, Size(3, 3), 0, 0);
        SHOW("blurred", blurred);

        cvtColor(blurred, hsv, CV_RGB2HSV);
        SHOW("hsv", hsv);

        inRange(hsv, Scalar(hlow, slow, vlow), Scalar(hhigh, shigh, vhigh), mask);
        SHOW("mask", mask);

        cout << "test" << endl;
        contours.clear();
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);

        drawContours(resized, contours, -1, Scalar(255, 255, 0));
        SHOW("contours", resized);

        targets.clear();
        for (auto &cnt : contours)
        {
            // determine if contour is a valid target
            convex_cnt.clear();
            convexHull(cnt, convex_cnt);
            RotatedRect rect = minAreaRect(convex_cnt);

            if (rect.size.area() < minTargetRectArea)
            {
                cout << "area too small" << endl;
                continue;
            }
            float area = static_cast<float>(contourArea(convex_cnt));
            if (area / rect.size.area() < static_cast<float>(minTargetFullness) / 1000)
            {
                cout << "fullness too low" << endl;
                continue;
            }

            targets.push_back(rect);
            // store target info
        }

        for (RotatedRect &rect : targets)
        {

            // perform target matching
            // output found vertical angle and horizontal angle
            cv::Mat pts;
            // vector<vector<Point>> vec2;
            // vec2.push_back(v);
            boxPoints(rect, pts);
            // polylines(resized, pts, true, Scalar(0, 0, 255));

            Point2f rect_points[4];
            rect.points(rect_points);
            for (int j = 0; j < 4; j++)
                line(resized, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);

            cout << "test" << endl;
        }
        SHOW("boxes", resized);

        switch (waitKey(WAITKEY_DELAY))
        {
        case 27:
            return 0;
#ifndef USE_CAMERA
        case 'n':
            k++;
            break;
        case 'b':
            k--;
            break;
#endif
        }
    };
    return 0;
}