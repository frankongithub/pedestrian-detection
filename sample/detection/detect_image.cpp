#include <cstdlib>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../include/ped.hpp"

int main(void) {

    std::string image_name="./sample.jpg";
    std::cout << "filename: " << image_name << "\n";

    cv::Mat src = cv::imread(image_name, CV_LOAD_IMAGE_COLOR);
    if(src.empty()) 
        std::cout << "can not open image file : " << image_name << "\n", std::exit(1);

    src.convertTo(src, PIXEL_T);
    src/=255.f;

    std::string modelname="../../model/boost_weak100_pos23266_neg15624_icf780_obj80x160_cell12_12_4_1.xml";

    std::string modelname2="../../model/boost_weak2000_pos23266_neg15624_icf9240_obj80x160_cell12_16_4_2.xml";

    fk::PedestrianDetector pedDetector(modelname, modelname2); 

    std::vector<cv::Rect> box;
    std::vector<float> vote;
    int min_h=src.rows/3, max_h=src.rows;
    try {
        pedDetector.detect(src, box, vote, min_h, max_h);
    }
    catch(...) {
        std::cout << "*** can not load model " << modelname << "\n";
    }
    
    int i, linewidth=src.rows/640;
    linewidth+=2;
    for(i=0; i<box.size(); ++i) {
        fk::util::draw_box(src, box[i], vote[i], linewidth);
    }

    cv::imwrite("result.jpg", src*255);

    std::cout << "detected pedestrian num: " << box.size() << "\n";
    cv::namedWindow("det", cv::WINDOW_NORMAL);
    cv::imshow("det", src);
    cv::waitKey(-1);

    return 0;
}

