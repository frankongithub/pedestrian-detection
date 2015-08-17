#include "intg_chn_feature.hpp"
#include <opencv2/opencv.hpp>
namespace fk {
//============================================================================//
void feaExtractor::getXYGrad(cv::Mat src, cv::Mat& xgrad, cv::Mat& ygrad)
{
    cv::Sobel(src, xgrad, src.depth(), 1, 0, 5);
    cv::Sobel(src, ygrad, src.depth(), 0, 1, 5);
}
//============================================================================//
void feaExtractor::getGrad(cv::Mat xgrad, cv::Mat ygrad, cv::Mat& grad)
{
    cv::magnitude(xgrad, ygrad, grad);
}
//============================================================================//
void feaExtractor::getGradOrient(cv::Mat xgrad, cv::Mat ygrad, 
                                 std::vector<cv::Mat>& orient,
                                 int orient_total)
{
    int i;
    cv::Mat temp, temp2, grad;

    orient.clear();
    cv::phase(xgrad, ygrad, temp2);
    cv::magnitude(xgrad, ygrad, grad);
    temp=temp2-CV_PI;
    temp.copyTo(temp2, temp2>CV_PI);
    temp2=temp2/(CV_PI/float(orient_total)+0.001);
    temp2.convertTo(temp2, CV_8U);
    temp=temp2-1;
    temp.copyTo(temp2, temp2==orient_total);
    temp2.convertTo(temp2, PIXEL_T);
    for(i=0; i<orient_total; ++i) {
        orient.push_back(cv::Mat((temp2==i)/float(255)));
        orient[i].convertTo(orient[i], cv::DataType<pixel_t>::type);
        orient[i]=orient[i].mul(grad);
    }
}
//============================================================================//
void feaExtractor::getLUV(cv::Mat src, cv::Mat& L, cv::Mat& U, cv::Mat& V)
{
    cv::Mat luv, luv3[3];

    cv::cvtColor(src, luv, CV_BGR2Luv);
    cv::split(luv, luv3);
    L=luv3[0];
    U=luv3[1];
    V=luv3[2];
}
//============================================================================//
void feaExtractor::getIntgChnFea10(cv::Mat src, std::vector<pixel_t>& fea, 
                                   IntgParam iparam, int orient_total) 
{
    getIntgChnFea10(src, fea, iparam.m_fsize1, iparam.m_fsize2, 
                    iparam.m_fsize_step, iparam.m_foverlap, orient_total);
}
//============================================================================//
void feaExtractor::getIntgChnFea10(cv::Mat src, std::vector<pixel_t>& fea,
                                   int fsize1[2], int fsize2[2], int fstep[2],
                                   int overlap, int orient_total)
{
    int i;
    cv::Mat srcGray, xgrad, ygrad, grad, L, U, V;
    std::vector<cv::Mat> orient;
    std::vector<pixel_t> tempfea;

    fea.clear();
    cv::cvtColor(src, srcGray, CV_BGR2GRAY);

    feaExtractor::getXYGrad(srcGray, xgrad, ygrad);
    feaExtractor::getGrad(xgrad, ygrad, grad);
    feaExtractor::getGradOrient(xgrad, ygrad, orient, orient_total);
    feaExtractor::getLUV(src, L, U, V);

    IntgChn::getIntgChnFea(grad, fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea(L, fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea(U, fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea(V, fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    for(i=0; i<orient_total; ++i) {
        IntgChn::getIntgChnFea(orient[i], fsize1, fsize2, fstep, overlap, tempfea);
        fea.insert(fea.end(), tempfea.begin(), tempfea.end());
    }
}
//============================================================================//
void feaExtractor::getIntgChnFea10_fromIntg(cv::Rect box,
                              cv::Mat grad, std::vector<cv::Mat>& orient, cv::Mat L,
                              cv::Mat U, cv::Mat V, std::vector<pixel_t>& fea,
                              IntgParam iparam,
                              int orient_total) {
    int i;
    int fsize1[2] = {iparam.m_fsize1[0], iparam.m_fsize1[1]};
    int fsize2[2] = {iparam.m_fsize2[0], iparam.m_fsize2[1]};
    int fstep[2]  = {iparam.m_fsize_step[0], iparam.m_fsize_step[1]};
    int overlap   = iparam.m_foverlap;
    std::vector<pixel_t> tempfea;

    fea.clear();

    IntgChn::getIntgChnFea_fromIntg(grad(box), fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea_fromIntg(L(box), fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea_fromIntg(U(box), fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    IntgChn::getIntgChnFea_fromIntg(V(box), fsize1, fsize2, fstep, overlap, tempfea);
    fea.insert(fea.end(), tempfea.begin(), tempfea.end());

    for(i=0; i<orient_total; ++i) {
        IntgChn::getIntgChnFea_fromIntg(orient[i](box), fsize1, fsize2, fstep,
                                        overlap, tempfea);
        fea.insert(fea.end(), tempfea.begin(), tempfea.end());
    }
}
//============================================================================//
void IntgChnFea10Extractor::preCompute(cv::Mat src) {
    int i;
    cv::Mat srcGray, xgrad, ygrad;

    cv::cvtColor(src, srcGray, CV_BGR2GRAY);

    feaExtractor::getXYGrad(srcGray, xgrad, ygrad);
    feaExtractor::getGrad(xgrad, ygrad, m_intgGrad);
    feaExtractor::getGradOrient(xgrad, ygrad, m_intgOrient, m_orient_total);
    feaExtractor::getLUV(src, m_intgL, m_intgU, m_intgV);

    cv::integral(m_intgGrad, m_intgGrad, INTG_T);
    for(i=0; i<m_intgOrient.size(); ++i)
        cv::integral(m_intgOrient[i], m_intgOrient[i], INTG_T);

    cv::integral(m_intgL, m_intgL, INTG_T);
    cv::integral(m_intgU, m_intgU, INTG_T);
    cv::integral(m_intgV, m_intgV, INTG_T);
}
//============================================================================//
void IntgChnFea10Extractor::operator()(int x, int y, std::vector<pixel_t>& fea) 
{
    getIntgChnFea10_fromIntg(cv::Rect(x, y, m_objsize[0], m_objsize[1]),
                             m_intgGrad, m_intgOrient, m_intgL, m_intgU, m_intgV,
                             fea, m_iparam, m_orient_total);
}
//============================================================================//
void IntgChn::getIntgChnFea(cv::Mat src, int fsize1[2], int fsize2[2],
                           int fstep[2], int overlap, 
                           std::vector<pixel_t>& fea)
{
    int fsizex, fsizey, i, j;
    cv::Mat integral;

    fea.clear();
    cv::integral(src, integral, INTG_T);

    for(fsizey=fsize1[1]; fsizey<=fsize2[1]; fsizey+=fstep[1]) {
        for(fsizex=fsize1[0]; fsizex<=fsize2[0]; fsizex+=fstep[0]) {
            for(i=0; i<=src.rows-fsizey; i+=fsizey/overlap) {
                for(j=0; j<=src.cols-fsizex; j+=fsizex/overlap) {
                    fea.push_back(integral.at<intg_t>(i+fsizey,j+fsizex)
                                 -integral.at<intg_t>(i+fsizey, j)
                                 -integral.at<intg_t>(i,j+fsizex)
                                 +integral.at<intg_t>(i,j));
                }
            }
        }
    }
}
//============================================================================//
void IntgChn::getIntgChnFea_fromIntg(cv::Mat intg, int fsize1[2], int fsize2[2],
                           int fstep[2], int overlap, 
                           std::vector<pixel_t>& fea)
{
    int fsizex, fsizey, i, j;

    fea.clear();    
    for(fsizey=fsize1[1]; fsizey<=fsize2[1]; fsizey+=fstep[1]) {
        for(fsizex=fsize1[0]; fsizex<=fsize2[0]; fsizex+=fstep[0]) {
#if 0 
            //not faster
            for(i=0; i<=intg.rows-fsizey; i+=fsizey/overlap) {
                intg_t * p = intg.ptr<intg_t>(i);
                intg_t * p2 = intg.ptr<intg_t>(i+fsizey);
                for(j=0; j<=intg.cols-fsizex; j+=fsizex/overlap) {
                    intg_t temp = *p;
                    temp -= *(p+fsizex);
                    //p2+=j;
                    temp -= *p2;
                    temp += *(p2+fsizex);
                    fea.push_back(temp);
                    p+=fsizex/overlap;
                    p2+=fsizex/overlap;
                }
            }
#else
            for(i=0; i<=intg.rows-fsizey; i+=fsizey/overlap) {
                for(j=0; j<=intg.cols-fsizex; j+=fsizex/overlap) {
                    fea.push_back(intg.at<intg_t>(i+fsizey,j+fsizex)
                                 -intg.at<intg_t>(i+fsizey, j)
                                 -intg.at<intg_t>(i,j+fsizex)
                                 +intg.at<intg_t>(i,j));
                }
            }
#endif
        }
    }
}
//========================================================================//
}
