#ifndef PARAM_HPP______fg48dfgl234j
#define PARAM_HPP______fg48dfgl234j

#include <string>
#include <opencv2/opencv.hpp>

namespace fk {

typedef float pixel_t;
#define PIXEL_T CV_32F
typedef double intg_t ;
#define INTG_T CV_64F
//========================================================================//
class IntgParam {
public:
    IntgParam();
    IntgParam(int size1, int size2, int size_step, int overlap); 
    IntgParam(int fsize1[2], int fsize2[2], int fsize_step[2], int foverlap);
    int m_fsize1[2], m_fsize2[2];
    int m_fsize_step[2], m_foverlap;
};
//========================================================================//
class SlidingWinParam {
public:
    SlidingWinParam() {};
    SlidingWinParam(int scale_num, int slid_step[2], 
                    float init_scale, float shrink_ratio);
    int m_scale_num;
    int m_slid_step[2];
    float m_shrink_ratio;
    float m_init_scale;
};
//========================================================================//
class ParamFactory {
public:
    static IntgParam getIntgParam(const std::string s);
    static SlidingWinParam getSlidingWinParam(const std::string s);
    static void getObjSize(const std::string s, int win_size[]);
    static void get_box_extend_ratio(const std::string s, float ratio[2]);
};
//========================================================================//
std::ostream& operator<<(std::ostream& os, const SlidingWinParam& swp);
//========================================================================//
}
#endif




