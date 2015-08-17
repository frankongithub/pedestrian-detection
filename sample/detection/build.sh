CPPFLAGS=-O3
g++ ${CPPFLAGS} -o detect_image detect_image.cpp\
         ../../lib/libpedetect.a `pkg-config --libs opencv`

