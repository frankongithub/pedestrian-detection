#ifndef DETECTION_HPP______9jilkfskn328ihrlijn
#define DETECTION_HPP______9jilkfskn328ihrlijn

#include <vector>
#include <opencv2/opencv.hpp>
#include "param.hpp"


namespace fk {
class feaExtractor;

class ObjDetector {
public:
    ObjDetector(CvBoost* model, int objsize[2], feaExtractor * fExtr,
                SlidingWinParam sparam, float threshold, 
                bool remove_overlap, float overlap_thres);
    void detect(cv::Mat src, std::vector<cv::Rect>& box, 
                std::vector<float>& vote);
    void setSlidingWinParam(SlidingWinParam slidParam) {
        m_slidParam = slidParam;
    }
    void setOverlapParam(bool remove_overlap, float overlap_thres) {
        m_remove_overlap = remove_overlap;
        m_overlap_thres = overlap_thres;
    }
    void setThres(float thres) {
        m_threshold = thres;
    }

protected:
    void detect_sliding(cv::Mat src, feaExtractor * fExtr, CvBoost * model, 
                        int objsize[2], int slid_step[2], std::vector<cv::Rect>&,
                        std::vector<float>&, float threshold);
    void detect_boxes(cv::Mat src, feaExtractor * fExtr, CvBoost * model, int objsize[2], 
                 std::vector<cv::Rect> rect2detect, std::vector<cv::Rect>& ,
                 std::vector<float>& , float threshold);
    float detect_box(cv::Mat src, feaExtractor * fExtr, CvBoost * model, 
                     bool returnsum, int x, int y);
    virtual void detect_image(cv::Mat src, float scale, 
                      std::vector<cv::Rect>& box, std::vector<float>& vote);
protected:
    CvBoost* m_model;
    int m_objsize[2];
    float m_threshold;
    feaExtractor* m_fExtr;
    SlidingWinParam m_slidParam;
    bool m_remove_overlap;
    float m_overlap_thres;
};
//=============================================================================//
class ObjDetector_2stage : public ObjDetector {
public:
    CvBoost* m_model2;
    feaExtractor* m_fExtr2;
    float m_threshold2;

    ObjDetector_2stage(CvBoost* model, CvBoost * model2, 
                 int objsize[2], feaExtractor * fExtr, feaExtractor * fExtr2,
                 SlidingWinParam sparam, float threshold, float threshold2,
                 bool remove_overlap, float overlap_thres)
                 : ObjDetector(model, objsize, fExtr, sparam, threshold,
                               remove_overlap, overlap_thres),
                   m_fExtr2(fExtr2), m_threshold2(threshold2), m_model2(model2) {}
    void setThres2(float thres2) {
        m_threshold2 = thres2;
    }
protected:
    virtual void detect_image(cv::Mat src, float scale, 
                      std::vector<cv::Rect>& box, std::vector<float>& vote);
};
//========================================================================//
class PedestrianDetector {
public:
    PedestrianDetector(const std::string model_name, const std::string model_name2);
    ~PedestrianDetector();
    void detect(cv::Mat src, std::vector<cv::Rect>& box, std::vector<float>& vote,
                int min_height=120, int max_height=-1, 
                float threshold1 = -1, float threshold2 = 0, 
                //int step_size = 6, float scale_step = 1.1, 
                int step_size = 8, float scale_step = 1.1, 
                //int step_size = 8, float scale_step = 1.1, 
                bool remove_overlap = true, float overlap_thres = 0.4);
    
protected:
    int m_objsize[2];
    CvBoost *m_boost, *m_boost2;
    feaExtractor *m_fExtr, *m_fExtr2;
    fk::ObjDetector_2stage *m_detector;    
};
//========================================================================//
}
#endif
