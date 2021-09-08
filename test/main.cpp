//  Created by Linzaer on 2019/11/15.
//  Copyright © 2019 Linzaer. All rights reserved.

#include "Slim.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <timer.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <mnn .mnn> [image files...]\n", argv[0]);
        return 1;
    }

    string mnn_path = argv[1];
    string image_file = argv[2];
    cv::Mat frame = cv::imread(image_file);
    std::cout << mnn_path << std::endl;
    std::cout << frame.size() << std::endl;
    Slim slim(mnn_path, 128, 128, 0.2,0.2, 0.2,0.2, 1); // config model input
    slim.loadModel();
    ld_info_ ld_info;
    for(int j = 0; j< 100; j++)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        long c = tv.tv_sec * 1000000 + tv.tv_usec;


        slim.run(frame.data, 400,10, 650, 260, frame.cols, frame.rows, ld_info);

        gettimeofday(&tv, NULL);
        long c1 = tv.tv_sec * 1000000 + tv.tv_usec;
        long c2 = (c1 - c)/1000;	
        printf("detect time1 : %ld ms\n",c2);
    }


    for(int i = 0; i < 68; i++)
    {
        cv::Point2f p = cv::Point2f(ld_info.landmarks[2 * i], ld_info.landmarks[2*i + 1]);
        cv::circle(frame, p, 1, cv::Scalar(225, 0, 225), 1, 1);
    }
    cv::imwrite("result.jpg",frame);
    return 0;
}
