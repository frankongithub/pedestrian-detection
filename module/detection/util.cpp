#include <dirent.h>
#include <iostream>
#include "util.hpp"
namespace fk {
const std::string util::m_boost_tree_name = "opencv_boost_tree";
//========================================================================//
filehandler::filehandler(const std::string srcdir,
                const std::vector<std::string> extname)
                : m_srcdir(srcdir), m_extname(extname), m_count(0)
{ 
}
//========================================================================//
bool util::readdir(filehandler& fhand) {
    struct dirent * entry;
    int count=0;

    DIR * dir = opendir(fhand.m_srcdir.c_str());
    if( !dir )  {
        std::cout << "can NOT open " << fhand.m_srcdir << std::endl;
        return 1;
    }
    std::cout << "open dir: " << fhand.m_srcdir << "\n";

    while( (entry=::readdir(dir)) ) {
        int i;
        const std::string filename = entry->d_name;
        bool extmatch=false;

        if(entry->d_name[0]=='.') 
            continue;

        for(i=0; i<fhand.m_extname.size(); ++i) {
            if(filename.compare(filename.size()-3,3,fhand.m_extname[i])==0) 
                extmatch=true;
        }

        if(!extmatch) continue;

        std::string fullname=fhand.m_srcdir;
        fullname.append("/");
        fullname.append(entry->d_name);

        fhand(fullname);
    }
}
//========================================================================//
void util::draw_box(cv::Mat src, cv::Rect box, float vote, int linewidth) 
{
    float intensity=std::min(vote/20,1.f);
    cv::rectangle(src, box, cv::Scalar(0,intensity,0), linewidth);
    int w=box.width, h=box.height;
    cv::line(src, cv::Point(box.x, box.y),
                  cv::Point(box.x+w, box.y+h), 
                  cv::Scalar(0,intensity,0), 1);
    cv::line(src, cv::Point(box.x+w, box.y),
                  cv::Point(box.x, box.y+h), 
                  cv::Scalar(0,intensity,0), 1);
}
//========================================================================//
void util::save_boost_model(const std::string filename, CvBoost * boost,
                            int objsize[], int raw_feanum, IntgParam intgParam,
                            int posnum, int negnum, std::vector<float> pos_stat,
                            std::vector<float> neg_stat) 
{
    int i;
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    fs << "obj_size" << "{" << "x" << objsize[0] << "y" << objsize[1] << "}";
    fs << "raw_feature_num" << raw_feanum;
    fs << "feature_param" << "[" 
       << int(intgParam.m_fsize1[0]) << int(intgParam.m_fsize1[1])
       << int(intgParam.m_fsize2[0]) << int(intgParam.m_fsize2[1])
       << int(intgParam.m_fsize_step[0]) << int(intgParam.m_fsize_step[1])
       << int(intgParam.m_foverlap) << "]";
    fs << "positive_sample" << posnum;
    fs << "negative_sample" << negnum;
    fs << "positive_votesX100" << "{";
    for(i=0; i<pos_stat.size(); ++i) {
        std::string name;
        std::stringstream namess(name);
        if(i==0)
            namess << "percentile_" << 1;
        else
            namess << "percentile_" << i*10;
        name=namess.str();  
        fs << name << int(pos_stat[i]*100);
    }
    fs << "}";

    fs << "negative_votesX100" << "{";
    for(i=0; i<neg_stat.size(); ++i) {
        std::string name;
        std::stringstream namess(name);
        if(i==0)
            namess << "percentile_" << 1;
        else
            namess << "percentile_" << i*10;
        name=namess.str();  
        fs << name << int(neg_stat[i]*100);
    }
    fs << "}";

    boost->write(*fs, m_boost_tree_name.c_str());
    fs.release();
}
CvBoost * util::load_boost_model(const std::string filename) 
{
    CvBoost * boost = new CvBoost;
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    boost->read(*fs, *fs[m_boost_tree_name]);
    fs.release();
    return boost;
}
//========================================================================//
}


