#ifndef INTG_CHN_FEATURE_HPP______4jdfg589y79023
#define INTG_CHN_FEATURE_HPP______4jdfg589y79023

#include <opencv2/opencv.hpp>
#include <cstdlib>
#include "param.hpp"

namespace fk {
//========================================================================//
class IntgChn {
public:
    static void getIntgChnFea(cv::Mat src, int fsize1[2], int fsize2[2], int fstep[2], 
                              int overlap, std::vector<pixel_t>& fea);
    static void getIntgChnFea_fromIntg(cv::Mat intg, int fsize1[2], int fsize2[2],
                              int fstep[2], int overlap, std::vector<pixel_t>& fea);
};
//========================================================================//
class feaExtractor {
public:
    static void getXYGrad(cv::Mat src, cv::Mat& xgrad, cv::Mat& ygrad);
    static void getGrad(cv::Mat xgrad, cv::Mat ygrad, cv::Mat& grad);
    static void getGradOrient(cv::Mat xgrad, cv::Mat ygrad,
                              std::vector<cv::Mat>& orient, int orient_total=6);
    static void getLUV(cv::Mat src, cv::Mat& L, cv::Mat& U, cv::Mat& V);
    static void getIntgChnFea10(cv::Mat src, std::vector<pixel_t>& fea, 
                                int fsize1[2], int fsize2[2], int fstep[2],
                                int overlap, int orient_total=6);
    static void getIntgChnFea10(cv::Mat src, std::vector<pixel_t>& fea, 
                                IntgParam iparam, int orient_total=6);
    static void getIntgChnFea10_fromIntg(cv::Rect box,
                              cv::Mat grad, std::vector<cv::Mat>& orient, cv::Mat L, 
                              cv::Mat U, cv::Mat V, std::vector<pixel_t>& fea, 
                              IntgParam iparam, int orient_total=6);
    virtual void operator()(int x, int y, std::vector<pixel_t>& fea) = 0;
    virtual void preCompute(cv::Mat src) {}

};
//========================================================================//
class IntgChnFea10Extractor : public feaExtractor{
public:
    IntgChnFea10Extractor(IntgParam iparam, 
                              int objsize[2], int orient_total=6)
                       :m_iparam(iparam), m_orient_total(orient_total)
    {
        m_objsize[0]=objsize[0];
        m_objsize[1]=objsize[1];
    }
    virtual void preCompute(cv::Mat src);
    virtual void operator()(int x, int y, std::vector<pixel_t>& fea);
protected:
    IntgParam m_iparam; 
    int m_orient_total;
    int m_objsize[2];
    cv::Mat m_intgGrad, m_intgL, m_intgU, m_intgV;
    std::vector<cv::Mat> m_intgOrient;
};
//========================================================================//
}

#endif
