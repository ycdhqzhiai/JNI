#pragma once
#ifndef SLIM_H
#define SLIM_H
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "common.h"
#include "Interpreter.hpp"
#include "sys/time.h"
#include "MNNDefine.h"
#include "Tensor.hpp"
#include "ImageProcess.hpp"
#define POINTS 68

typedef struct ld_info_
{
    float landmarks[POINTS*2];
    float pitch;
    float roll;
    float yall;
    bool lEState;
    bool rEState;
    bool Mouth;
}ld_info_;

class Slim
{
public:
    Slim(const std::string &mnn_path,
              int input_width, int input_length, float width_extend_,
              float height_extend, float eye_close_thres_,
              float mouth_close_thres_, int num_thread_ = 4);

    ~Slim();
    int loadModel();
    int run(const uint8_t *image, float roix1, float roiy1, float roix2, float roiy2, int target_width, int target_height, ld_info_& results);

    float height_extend;
    float width_extend;
    float eye_close_thres;
    float mouth_close_thres;
    int in_w;
    int in_h;
private:
    std::shared_ptr<MNN::Interpreter> slim_interpreter;
    MNN::Session *slim_session = nullptr;
    MNN::Tensor *input_tensor = nullptr;

    std::string mnn_path_;
    bool initialized_;
    int num_thread;
    const float mean_vals[3] = {127, 127, 127};
    const float norm_vals[3] = {1.0 / 127, 1.0 / 127, 1.0 / 127};
};

#endif /* Slim_hpp */
