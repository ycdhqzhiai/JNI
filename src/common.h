#ifndef COMMON_H
#define COMMON_H
#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>  
#include "Interpreter.hpp"
#include "MNNDefine.h"
#include "Tensor.hpp"
#include "ImageProcess.hpp"
#include <memory>
// inline ncnn::Mat mat_pixel_roi_resize_bgr(ncnn::Mat inputMat, int roix1, int roiy1, int roix2, int roiy2, int target_width, int target_height)
// {
//     ncnn::Mat roi;
//     ncnn::copy_cut_border(inputMat, roi, roiy1, inputMat.h - roiy2, roix1, inputMat.w - roix2);
//     ncnn::Mat roi_resize;
//     ncnn::resize_bilinear(roi, roi_resize, target_width, target_height);
//     return roi_resize;
// }
inline MNN::Tensor * mat_pixel_roi_resize_bgr(const uint8_t *image, int left, int top, int width, int height, int target_width, int target_height)
{
    MNN::CV::ImageProcess::Config config;
    config.sourceFormat = MNN::CV::BGR;
    config.destFormat = MNN::CV::RGB;
    config.filterType = MNN::CV::BILINEAR;
    MNN::CV::Matrix transform;
    std::vector<int> shape = {1, height, width, 3};
    MNN::Tensor * input_tensor = MNN::Tensor::create(shape, halide_type_of<unsigned char>());
    std::shared_ptr<MNN::CV::ImageProcess> pretreat(MNN::CV::ImageProcess::create(config));

    transform.setTranslate(-left,-top);
    transform.invert(&transform);
    pretreat->setMatrix(transform);
    pretreat->convert(image, target_width, target_height, 0, input_tensor);
    return input_tensor;
}

inline float cal_distance(float* result, int index1, int index2, int index3, int index4, int index5, int index6)
{
    float dis_y = abs(result[index1 * 2 + 1] - result[index2 * 2 + 1]) + abs(result[index3 * 2 + 1] - result[index4 * 2 + 1]);
    float dis_x = 2 * abs(result[index5 * 2] - result[index6 * 2]);
    return dis_y / (dis_x + 0.0000001);
}

inline void getEyeCenterPoint(float* landmarks, int begin_index, int img_w, int img_h, int input_w, int input_h, float *eyeRoi)
{
    float cPoint[2];
    float sum_x = 0;
    float sum_y = 0;
    // 37 43
    for(int j = begin_index; j < begin_index + 6; j++)
    {
        sum_x += landmarks[j * 2];
        sum_y += landmarks[j * 2 + 1];
    }

    cPoint[0] = sum_x / 6;
    cPoint[1] = sum_y / 6;
    float eye_w =  1.5 * abs((landmarks[begin_index * 2] - landmarks[(begin_index + 3) * 2]));
    eyeRoi[0] = cPoint[0] - eye_w / 2 > 0 ? cPoint[0] - eye_w / 2 : 0;
    eyeRoi[1] = cPoint[1] - eye_w / 2 > 0 ? cPoint[1] - eye_w / 2 : 0;
    eyeRoi[2] = cPoint[0] + eye_w / 2 < img_w ? cPoint[0] + eye_w / 2 : img_w;
    eyeRoi[3] = cPoint[1] + eye_w / 2 < img_h ? cPoint[1] + eye_w / 2 : img_h;
}
#endif /* COMMON_H */
