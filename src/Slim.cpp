#include "Slim.h"
#include <opencv2/opencv.hpp>
#define clip(x, y) (x < 0 ? 0 : (x > y ? y : x))

Slim::Slim(const std::string &mnn_path,
                int input_width, int input_length, float width_extend_,
                float height_extend_, float eye_close_thres_,
                float mouth_close_thres_, int num_thread_)
{
    num_thread = num_thread_;
    in_w = input_width;
    in_h = input_length;
    mnn_path_ = mnn_path;
    width_extend = width_extend_;
    height_extend = height_extend_;
    eye_close_thres = eye_close_thres_;
    mouth_close_thres = mouth_close_thres_;
    initialized_ = false;
}

Slim::~Slim()
{
    slim_interpreter->releaseModel();
    slim_interpreter->releaseSession(slim_session);
}

int Slim::loadModel()
{
    slim_interpreter = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(mnn_path_.c_str()));
    if (!slim_interpreter)
        return -1;
    MNN::ScheduleConfig config;
    config.numThread = num_thread;
    MNN::BackendConfig backendConfig;
    backendConfig.precision = (MNN::BackendConfig::PrecisionMode) 2;
    config.backendConfig = &backendConfig;
    slim_session = slim_interpreter->createSession(config);
    input_tensor = slim_interpreter->getSessionInput(slim_session, nullptr);
    // if (ultraface.load_param(param_path_.c_str()) == -1 ||
    //     ultraface.load_model(bin_path_.c_str()) == -1)
    // {
    //     GLOG(G_ERROR) << "Load slim model fail!";
    //     return ERROR_load_face_model_fail;
    // }
    initialized_ = true;
    return 0;
}


int Slim::run(const uint8_t *image, float roix1, float roiy1, float roix2, float roiy2, int target_width, int target_height, ld_info_& results)
{
    if (!initialized_)
    {
        return -1;
    }

    if (!image)
    {
        return -2;
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long cr = tv.tv_sec * 1000000 + tv.tv_usec;
    MNN::Tensor* roi_tensor = mat_pixel_roi_resize_bgr(image, roix1, roiy1, roix2 - roix1, roiy2 - roiy1, target_width, target_height);
    gettimeofday(&tv, NULL);
    long c1r = tv.tv_sec * 1000000 + tv.tv_usec;
    long c2r = (c1r - cr)/1000;
    printf("roi time1 : %ld ms\n",c2r);
#if 0

    auto hwc_data   = roi_tensor->host<float>();
    auto hwc_size   = roi_tensor->size();

    //std::cout << input_tensor->size() << std::endl;
    cv::Mat roi = cv::Mat::zeros(roi_tensor->height(), roi_tensor->width(), CV_8UC3);
    ::memcpy(roi.data, hwc_data, hwc_size);
    cv::imwrite("roi.jpg", roi);
#endif

    MNN::CV::ImageProcess::Config config;
    config.sourceFormat = MNN::CV::RGB;
    config.destFormat = MNN::CV::RGB;
    config.filterType = MNN::CV::BILINEAR;
    memcpy(config.mean, mean_vals, 3 * sizeof(float));
    memcpy(config.normal, norm_vals, 3 * sizeof(float));



    // struct timeval tv;
    gettimeofday(&tv, NULL);
    long c = tv.tv_sec * 1000000 + tv.tv_usec;
    // get input tensor
    MNN::CV::Matrix transform;
    transform.setScale((float)(roix2 - roix1-1)/(in_w-1), (float)(roiy2 - roiy1-1)/(in_h-1));
    std::shared_ptr<MNN::CV::ImageProcess> pretreat(MNN::CV::ImageProcess::create(config));
    pretreat->setMatrix(transform);
    pretreat->convert(roi_tensor->host<unsigned char>(), roix2 - roix1, roiy2 - roiy1, 0, input_tensor);

    slim_interpreter->runSession(slim_session);
    std::string output = "output1";
    MNN::Tensor *tensor_out = slim_interpreter->getSessionOutput(slim_session, output.c_str());
    MNN::Tensor tensor_out_host(tensor_out, tensor_out->getDimensionType());
    tensor_out->copyToHostTensor(tensor_out);

    gettimeofday(&tv, NULL);
    long c1 = tv.tv_sec * 1000000 + tv.tv_usec;
    long c2 = (c1 - c)/1000;	
    printf("inference time1 : %ld ms\n",c2);
    for (int i = 0; i < POINTS; ++i)
    {
        results.landmarks[2*i] = tensor_out->host<float>()[2 * i] * (roix2 - roix1)+ roix1;
        results.landmarks[2*i + 1] = tensor_out->host<float>()[2 * i + 1] * (roiy2 - roiy1) + roiy1;
    }
    results.pitch = tensor_out->host<float>()[136];
    results.yall = tensor_out->host<float>()[137];
    results.roll = tensor_out->host<float>()[138];

    float lEDist = cal_distance(results.landmarks, 41, 37, 40, 38, 39, 36);
    float rEDist = cal_distance(results.landmarks, 47, 43, 46, 44, 45, 42);
    float mEDist = cal_distance(results.landmarks, 67, 61, 65, 63, 64, 60);

    if (lEDist < eye_close_thres)
        results.lEState = true;
    else
        results.lEState = false;
    if (rEDist < eye_close_thres)
        results.rEState = true;
    else
        results.rEState = false;
    if (mEDist < mouth_close_thres)
        results.Mouth = true;
    else
        results.Mouth = false;
    return 0;
}