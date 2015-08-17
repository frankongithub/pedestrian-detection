#include "param.hpp"
#include <string>
#include <iostream>

namespace fk {
//========================================================================//
IntgParam::IntgParam(int size1, int size2, int size_step, int overlap)
{
    m_fsize1[0]=m_fsize1[1]=size1;
    m_fsize2[0]=m_fsize2[1]=size2;
    m_fsize_step[0]=m_fsize_step[1]=size_step;
    m_foverlap = overlap;
}
//========================================================================//
IntgParam::IntgParam()
{
    m_fsize1[0]=m_fsize1[1]=8;
    m_fsize2[0]=m_fsize2[1]=12;
    m_fsize_step[0]=m_fsize_step[1]=4;
    m_foverlap = 1;
}
//========================================================================//
IntgParam::IntgParam(int fsize1[2], int fsize2[2],
               int fsize_step[2], int foverlap)
{
    m_fsize1[0]=fsize1[0]; m_fsize1[1]=fsize1[1];
    m_fsize2[0]=fsize2[0]; m_fsize2[1]=fsize2[1];
    m_fsize_step[0]=fsize_step[0];
    m_fsize_step[1]=fsize_step[1];
    m_foverlap = foverlap;
}
//========================================================================//
IntgParam ParamFactory::getIntgParam(const std::string s) 
{
    if(s.compare("1")==0) { //5760 dim
        return IntgParam(8,12,4,1);
        //param1=IntgParam(8,12,4,1);
        //return param1;
    }
    if(s.compare("2")==0) {
        return IntgParam(8,8,4,1);
    }
    if(s.compare("3")==0) { //second stage 1
        return IntgParam(12,16,4,2);
    }
    if(s.compare("4")==0) {
        return IntgParam(12,12,4,2);
    }
    if(s.compare("5")==0) { //first stage
        return IntgParam(16,16,4,1);
    }
    if(s.compare("6")==0) { //second stage 2
        return IntgParam(12,20,4,2);
    }
    if(s.compare("7")==0) { //first stage
        return IntgParam(12,12,4,1);
    }
    std::cout << "ParamFactory::getIntgParam() invalid param: " << s << "\n";
    std::exit(1);
}
//========================================================================//
SlidingWinParam::SlidingWinParam(int scale_num, int slid_step[2], 
                                 float init_scale, float shrink_ratio)
                                 : m_scale_num(scale_num), m_init_scale(init_scale), 
                                   m_shrink_ratio(shrink_ratio)
{
    m_slid_step[0] = slid_step[0];
    m_slid_step[1] = slid_step[1];
}
//========================================================================//
SlidingWinParam ParamFactory::getSlidingWinParam(const std::string s) 
{
    if(s.compare("1")==0) {
        int scale_num=15;
        float shrink_ratio=0.9;
        float init_scale=1;
        int slid_step[2]={80/15, slid_step[0]/float(80)*160};
        return SlidingWinParam(scale_num, slid_step, init_scale, shrink_ratio);
    }
    if(s.compare("2")==0) {
        int scale_num=15;
        //float shrink_ratio=0.9;
        float shrink_ratio=0.85;
        float init_scale=1;
        int slid_step[2]={12, 12};
        //int slid_step[2]={80/10, slid_step[0]};
        //int slid_step[2]={80/10, slid_step[0]/float(80)*160};
        return SlidingWinParam(scale_num, slid_step, init_scale, shrink_ratio);
    }
    if(s.compare("3")==0) {
        int scale_num=15;
        float shrink_ratio=0.85;
        float init_scale=1;
        int slid_step[2]={80/7, slid_step[0]/float(80)*160};
        return SlidingWinParam(scale_num, slid_step, init_scale, shrink_ratio);
    }
    std::cout << "ParamFactory::getSlidingWinParam() invalid param: " << s << "\n";
    std::exit(1);
}
//========================================================================//
void ParamFactory::getObjSize(const std::string s, int win_size[]) 
{
    if(s.compare("1")==0) {
        win_size[0]=80;
        win_size[1]=160;
        return;
    }
    std::cout << "ParamFactory::getSlidingWinParam() invalid param: " << s << "\n";
    std::exit(1);
}
//========================================================================//
void ParamFactory::get_box_extend_ratio(const std::string s, float ratio[2]) {
    if( s.compare("1")==0 ) {
        ratio[0]=1.5; ratio[1]=1.1;
    } else if ( s.compare("2")==0 ) {
//        ratio[0]=1.3; ratio[1]=1.1;
        ratio[0]=ratio[1]=1.1;
        //ratio[0]=1.1; ratio[1]=1.05;
    } else {
        std::cout << "unsupport extend ratio: " << s << "\n";
        std::exit(1);
    }
}
//========================================================================//
std::ostream& operator<<(std::ostream& os, const SlidingWinParam& swp)
{
    os << "init scale/shrink/#scale/step: " << swp.m_init_scale << "/"
       << swp.m_shrink_ratio << "/" << swp.m_scale_num << "/(" 
       << swp.m_slid_step[0] << "," << swp.m_slid_step[1] << ")\n";
    return os;
}
//========================================================================//

}
