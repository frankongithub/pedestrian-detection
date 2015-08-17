#ifndef UTIL_HPP______kfgk4988822
#define UTIL_HPP______kfgk4988822

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "param.hpp"
#include "intg_chn_feature.hpp"

namespace fk {
//==============================================================================//
class filehandler {
public:
    int m_count;
    const std::vector<std::string> m_extname;
    const std::string m_srcdir;
    filehandler(const std::string srcdir, const std::vector<std::string> extname);
    virtual void operator()(const std::string fullname){};
    virtual ~filehandler(){};
};
//==============================================================================//
class util {
public:
    static bool readdir(filehandler& fhand);
    static void draw_box(cv::Mat src, cv::Rect box, float vote, int linewidth);
    static void save_boost_model(const std::string filename, CvBoost * boost,
                                 int objsize[], int raw_feanum, IntgParam intgParam,
                                 int posnum, int negnum, std::vector<float> pos_stat,
                                 std::vector<float> neg_stat);
    static CvBoost * load_boost_model(const std::string filename);
    static void increase_box_number(std::vector<std::vector<int> >& box) {
        int i, j;
        int box_size=box.size();
        for(i=0; i<box_size; ++i) {
            float cx=(box[i][0]+box[i][1])/2.;
            float cy=(box[i][2]+box[i][3])/2.;
            for(j=0; j<3; ++j) {
                std::vector<int> temp;
                float eratiox=1.1, eratioy=1.05;
                if(j==0) eratiox=1;
                if(j==1) eratioy=1;
                temp.push_back( (box[i][0]-cx)*eratiox+cx );
                temp.push_back( (box[i][1]-cx)*eratiox+cx );
                temp.push_back( (box[i][2]-cy)*eratioy+cy );
                temp.push_back( (box[i][3]-cy)*eratioy+cy );
                box.push_back(temp);
            }
        }
    }
    static void fit_obj_aspect(std::vector<std::vector<int> >& box, 
                               int objw, int objh, float max_extend_ratio[]) {
        int i;
        
        for(i=0; i<box.size(); ++i) {
            float w = box[i][1]-box[i][0];
            float h = box[i][3]-box[i][2];
            float rw = w/objw;
            float rh = h/objh;
            float rw_h = rw/rh;
            float rh_w = rh/rw;
            float ratio = 1;
            if(rw_h<1) {
                ratio = 1/rw_h;
                if(ratio>max_extend_ratio[0])
                    ratio = max_extend_ratio[0];
            } else {
                ratio = 1/rh_w;
                if(ratio>max_extend_ratio[1])
                    ratio = max_extend_ratio[1];
            }
            if(rw_h<1) {
                float cx = (box[i][0]+box[i][1])/2.0f;
                box[i][0] = (box[i][0]-cx)*ratio+cx;
                box[i][1] = (box[i][1]-cx)*ratio+cx;
            } else {
                float cy = (box[i][2]+box[i][3])/2.0f;
                box[i][2] = (box[i][2]-cy)*ratio+cy;
                box[i][3] = (box[i][3]-cy)*ratio+cy;
            }
        }
    }
    static void extend_box_size(std::vector<std::vector<int> > box, 
                                std::vector<cv::Rect>& extend_obj_rect, 
                                std::vector<cv::Rect>& obj_rect,
                                float box_extend_ratio[],
                                cv::Mat src) {
        int i, xmin, ymin, xmax, ymax, k;
        int exmin, eymin, exmax, eymax;
   
        extend_obj_rect.clear();
        obj_rect.clear();

        for(i=0; i<box.size(); ++i) {
            xmin=box[i][0];
            xmax=box[i][1];
            ymin=box[i][2];
            ymax=box[i][3];
            
            float cx=(xmax+xmin)/2.f;
            float cy=(ymax+ymin)/2.f;
            exmin=(xmin-cx)*(box_extend_ratio[0])+cx;
            exmax=(xmax-cx)*(box_extend_ratio[0])+cx;
            eymin=(ymin-cy)*(box_extend_ratio[1])+cy;
            eymax=(ymax-cy)*(box_extend_ratio[1])+cy;        
    
            if( cv::Point(exmin,eymin).inside(cv::Rect(0,0,src.cols,src.rows)) 
              &&cv::Point(exmax,eymax).inside(cv::Rect(0,0,src.cols,src.rows)) ) {
                cv::Rect rect(exmin, eymin, exmax-exmin+1, eymax-eymin+1);
                cv::Rect rect2(xmin,  ymin,   xmax-xmin+1,   ymax-ymin+1);
                extend_obj_rect.push_back(rect);
                obj_rect.push_back(rect2);
            }
        }
    }
    static void compute_feature_with_flip(cv::Mat src, cv::Rect extend_obj_rect,
                                          int objw, int objh, 
                                          std::vector<std::vector<float> >& fea,
                                          feaExtractor * fExtr) {
        fea.clear();
        cv::Mat huMat=src(extend_obj_rect);
            
        cv::resize(huMat, huMat, cv::Size(objw, objh), 0, 0, cv::INTER_AREA);
        huMat.convertTo(huMat, PIXEL_T);
        huMat/=255.f;
        std::vector<pixel_t> tempfea;

         fExtr->preCompute(huMat);
        (*fExtr)(0, 0, tempfea);
        fea.push_back(tempfea);

        //cv::imshow("img", huMat);
        //cv::waitKey(-1);

        //flip images
        cv::flip(huMat, huMat, 1);

        //cv::imshow("img", huMat);
        //cv::waitKey(-1);
         fExtr->preCompute(huMat);
        (*fExtr)(0, 0, tempfea);
        fea.push_back(tempfea); 
    }
private:
    static const std::string m_boost_tree_name;
    
};

}
#endif
