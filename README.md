Pedestrian detection in still images.

This project uses AdaBoost classifiers to detect pedestrians in still images.
A two-stage approach is adopted for acceleration - a simple first-stage 
AdaBoost classifier is used to eliminate most negative samples and then a more 
complex second-stage one is used to detect pedestrians. The weak learners for
AdaBoost classifiers are two-level decision trees whose nodes are labeled with
integral channel features computed from LUV channels, gradient magnitude, and
gradient magnitude with six orientations. The numbers of weak learners for the
first and second stage classifiers are 100 and 2,000 respectively.


License
========================================
for research use only


Requirements
========================================
OpenCV 2.4.9
build-essential


OS
========================================
Ubuntu 14.04 LTS


Pre-trained models
========================================
first-stage: model/boost_weak100_pos23266_neg15624_icf780_obj80x160_cell12_12_4_1.xml
second-stage: model/boost_weak2000_pos23266_neg15624_icf9240_obj80x160_cell12_16_4_2.xml";


Build the library
========================================
1. navigate into project root directory
2. run the command 'make' to build libpedetect.a (located in project_dir/lib/)


Build and Run the sample pedestrian detection program
========================================
1. navigate into sample/detection/
2. run 'build.sh'
3. run './detect_image'
this program detects pedestrians in sample/detection/sample.jpg
and saves the result in sample/detection/result.jpg

