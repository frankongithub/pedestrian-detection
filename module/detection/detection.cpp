#include "detection.hpp"
#include "util.hpp"
#include "param.hpp"
#include "util.hpp"
#include "intg_chn_feature.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "unistd.h"

namespace fk {
//==============================================================//
ObjDetector::ObjDetector(CvBoost* model, int objsize[2], feaExtractor * fExtr,
                         SlidingWinParam sparam, float threshold,
                         bool remove_overlap, float overlap_thres)
                         : m_model(model), m_slidParam(sparam), 
                           m_fExtr(fExtr), m_threshold(threshold),
                           m_remove_overlap(remove_overlap),
                           m_overlap_thres(overlap_thres)
{    
    m_objsize[0]=objsize[0]; m_objsize[1]=objsize[1];
}
//==============================================================//
float overlapRatio(cv::Rect rect1, cv::Rect rect2) {
    cv::Rect intersect=rect1&rect2;
    cv::Rect unionrect=rect1|rect2;
    return intersect.area()/float(unionrect.area());
}
//==============================================================//
void ObjDetector::detect_image(cv::Mat src, float scale, 
                     std::vector<cv::Rect>& box, std::vector<float>& vote)
{
    int i;

    box.clear();
    vote.clear();
    detect_sliding(src, m_fExtr, m_model, m_objsize, m_slidParam.m_slid_step, 
                   box, vote, m_threshold);
    std::cout << "image size/box num: " << src.size() << "/" << box.size() << "\n";
    for(i=0; i<box.size(); ++i) {
        box[i].x/=scale;
        box[i].y/=scale;
        box[i].width/=scale;
        box[i].height/=scale;
    }
}
//==============================================================//
void ObjDetector_2stage::detect_image(cv::Mat src, float scale, 
                     std::vector<cv::Rect>& box, std::vector<float>& vote)
{
    int i, j;

    box.clear();
    vote.clear();
    detect_sliding(src, m_fExtr, m_model, m_objsize, m_slidParam.m_slid_step, 
                   box, vote, m_threshold);
    std::cout << "(first  stage)image size/box num: " << src.size() << "/" << box.size() << "\n";
    if(box.size()>0) 
    {
        std::vector<cv::Rect> box2;
        std::vector<float> vote2;
        detect_boxes(src, m_fExtr2, m_model2, m_objsize, box, 
                     box2, vote2, m_threshold2);
        std::cout << "(second stage)image size/box num: " << src.size() << "/" << box2.size() << "\n";

        for(j=0; j<box2.size(); ++j) {
            box2[j].x/=scale;
            box2[j].y/=scale;
            box2[j].width/=scale;
            box2[j].height/=scale;
        }
        box=box2;
        vote=vote2;
    }
}
//==============================================================//
void ObjDetector::detect(cv::Mat src, std::vector<cv::Rect>& box,
                          std::vector<float>& vote) 
{
    int i, j;



    float m=m_slidParam.m_init_scale;

    box.clear();
    vote.clear();
    std::cout << "original size: " << src.size() << std::endl;
    std::cout << "objsize: " << m_objsize[0] << "/" << m_objsize[1] << "\n";

    for(i=0; i<m_slidParam.m_scale_num; ++i, m*=m_slidParam.m_shrink_ratio) {

        cv::Size newSize(src.cols*m, src.rows*m);

        //std::cout << "objsize: " << m_objsize[0] << "/" << m_objsize[1] << "\n";
        std::cout << i << ": scale " << m << "\n";

        int maxlen = 2048;
        if(newSize.width>maxlen || newSize.height>maxlen) {
            std::cout << "**** image size > " << maxlen << "\n";
            std::cout << "**** stop detection\n";
            continue;
        }

        if(newSize.width<m_objsize[0] || newSize.height<m_objsize[1]) {
            break;
        }
        cv::Mat resized;        
        cv::resize(src, resized, newSize, 0, 0, cv::INTER_AREA);

        std::vector<cv::Rect> tempbox;
        std::vector<float> tempvote;

        detect_image(resized, m, tempbox, tempvote);

        box.insert(box.end(), tempbox.begin(), tempbox.end());
        vote.insert(vote.end(), tempvote.begin(), tempvote.end());
    }
    //remove overlapping area
    if(m_remove_overlap) {
        for(i=0; i<vote.size(); ++i) {
            bool toremove=false;
            for(j=0; j<vote.size(); ++j) {
                if(i!=j) 
                { 
                    if(  vote[i]<=vote[j]
                      && overlapRatio(box[i],box[j])>m_overlap_thres) 
                    {
                        toremove=true;
                        break;
                    }   

                    //if( (box[i]&box[j])==box[i] )
                     // && overlapRatio(box[i],box[j])>m_overlap_thres-0.1)
                    if( (box[i]&box[j]).area()/float(box[i].area())>0.85)
                    {
                        float cx = box[j].x+box[j].width/2.0;
                        float cy = box[j].y+box[j].height/2.0;
                        if( (box[i].x<cx && box[i].x+box[i].width >cx) 
                          ||(box[i].y<cy && box[i].y+box[i].height>cy) )
                        {
                            if(vote[i]<=vote[j]) { 
                                toremove=true;
                                break;
                            }
                            else {
                                vote.erase(vote.begin()+j);
                                box.erase(box.begin()+j);
                                --j;
                            }
                        }
                    }
                }
            }
            if(toremove) {
                vote.erase(vote.begin()+i);
                box.erase(box.begin()+i);
                --i;
            }
        }  
    }
}
//==============================================================//
float ObjDetector::detect_box(cv::Mat src, feaExtractor * fExtr, CvBoost * model,
                              bool returnsum, int x, int y) {
    std::vector<pixel_t> tempfea;

    (*fExtr)(x, y, tempfea);
    cv::Mat temp(tempfea,true);
    temp=temp.t();
    return model->predict(temp, cv::Mat(), cv::Range::all(), false, returnsum );
}
//========================================================================//
void ObjDetector::detect_sliding(cv::Mat src, feaExtractor * fExtr, CvBoost * model, 
                           int objsize[2], 
                           int slid_step[2], std::vector<cv::Rect>& box,
                           std::vector<float>& vote, float threshold) 
{
    int i, j;

    box.clear();
    fExtr->preCompute(src);

    for(i=0; i<src.rows-objsize[1]; i+=slid_step[1]) {
        for(j=0; j<src.cols-objsize[0]; j+=slid_step[0]) {
            bool returnsum=true;
            float score = detect_box(src, fExtr, model, returnsum, j, i);
            if(score > threshold) {
                vote.push_back(score);
                box.push_back(cv::Rect(j,i,objsize[0],objsize[1]));
            }
        }
    }
    //std::cout << "src size/box num: " << src.size() << "/" << box.size() << "\n";
}
//==============================================================//
void ObjDetector::detect_boxes(cv::Mat src, feaExtractor * fExtr, 
                 CvBoost * model, int objsize[2],
                 std::vector<cv::Rect> rect2detect, std::vector<cv::Rect>& box,
                 std::vector<float>& vote, float threshold) 
{
    int i, j;

    box.clear();
    fExtr->preCompute(src);

    for(i=0; i<rect2detect.size(); ++i) {
        int x=rect2detect[i].x;
        int y=rect2detect[i].y;
        bool returnsum=true;
        float score = detect_box(src, fExtr, model, returnsum, x, y);
        if(score > threshold) {
            vote.push_back(score);
            box.push_back(rect2detect[i]);          
        }
    }
    //std::cout << "src size/box num: " << src.size() << "/" << box.size() << "\n";
}
//==============================================================//
PedestrianDetector::PedestrianDetector(const std::string model_name, 
                                       const std::string model_name2) 
{
    fk::ParamFactory::getObjSize("1", m_objsize);

    if(access(model_name.c_str(), R_OK)!=0)
        std::cout << "can not open model " << model_name << "\n", std::exit(1);

    if(access(model_name2.c_str(), R_OK)!=0)
        std::cout << "can not open model " << model_name2 << "\n", std::exit(1);

    m_boost = util::load_boost_model(model_name);
    m_boost2 = util::load_boost_model(model_name2);

    SlidingWinParam slidingParam;
    slidingParam.m_scale_num = 30;
    slidingParam.m_slid_step[0] = slidingParam.m_slid_step[1] = 10;
    slidingParam.m_shrink_ratio = 0.9;
    slidingParam.m_init_scale = 1;

    bool remove_overlap = true;
    float overlap_thres = 0.4;

    float threshold=-1;
    float threshold2=0;

    fk::IntgParam intgParam1 = fk::ParamFactory::getIntgParam("7"); //780
    m_fExtr = new fk::IntgChnFea10Extractor(intgParam1, m_objsize);
                
    fk::IntgParam intgParam2 = fk::ParamFactory::getIntgParam("3"); //9240
    m_fExtr2 = new fk::IntgChnFea10Extractor(intgParam2, m_objsize);
       
    m_detector = new fk::ObjDetector_2stage(m_boost, m_boost2, m_objsize, 
                                            m_fExtr, m_fExtr2, slidingParam, 
                                            threshold, threshold2,
                                            remove_overlap, overlap_thres);
}
//==============================================================//
PedestrianDetector::~PedestrianDetector() {
    delete m_boost;
    delete m_boost2;
    delete m_fExtr;
    delete m_fExtr2;
    delete m_detector;
}
//==============================================================//
void PedestrianDetector::detect(cv::Mat src, std::vector<cv::Rect>& box,
            std::vector<float>& vote,
            int min_height, int max_height, 
            float threshold1, float threshold2, 
            int step_size, 
            float scale_step, 
            bool remove_overlap, float overlap_thres) 
{
    int i;
    if(src.channels()!=3) 
        std::cout << "color space is not BGR\n", std::exit(1);
    if(src.cols < m_objsize[0]/2 || src.rows < m_objsize[1]/2) 
        std::cout << "image size shoud be at least " << m_objsize[0]/2 
                  << "X" << m_objsize[1]/2 << "\n", std::exit(1);
    if( src.rows < m_objsize[1]/2 ) 
         std::cout << "image height < " << m_objsize[1]/2 
                   << std::endl, std::exit(1);
                
    if(max_height==-1)            max_height=src.rows;
    if(min_height<m_objsize[1]/2) min_height=m_objsize[1]/2;
    if(max_height>src.rows)       max_height=src.rows;
    if(max_height<min_height)     max_height=min_height;         
     
    SlidingWinParam slidingParam;
    slidingParam.m_slid_step[0] = slidingParam.m_slid_step[1] = step_size;
    slidingParam.m_shrink_ratio = 1.0/scale_step;
    slidingParam.m_init_scale = m_objsize[1]/float(min_height);
    float temp = min_height;
    slidingParam.m_scale_num = 1;
    while( temp < max_height ) {
        temp /= slidingParam.m_shrink_ratio;
        ++slidingParam.m_scale_num;
    }
    std::cout << "min/max obj height: " << min_height << "/" << max_height 
              << std::endl;
    std::cout << slidingParam;
    m_detector->setSlidingWinParam(slidingParam);
    m_detector->setOverlapParam(remove_overlap, overlap_thres);
    m_detector->setThres(threshold1);
    m_detector->setThres2(threshold2);
    m_detector->detect(src, box, vote);
}
//==============================================================//
};
