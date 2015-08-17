SAMPLE_DIR=sample
LIBDIR=lib
DETECTION_DIR=module/detection
TRAINING_DIR=module/training

DETECTION_OBJFILES=detection.o param.o intg_chn_feature.o util.o
TRAINING_OBJFILES=ped_db_util.o

CC=g++

CPPFLAGS=-O3#

LIBS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml

LIBFILE=libpedetect.a
LIB_TRAINFILE=libpedtrain.a

.PHONY: all clean $(LIBDIR)

ifdef BUILDTRAIN
all: $(LIBDIR)/$(LIBFILE) $(LIBDIR)/$(LIB_TRAINFILE) 
else
all: $(LIBDIR)/$(LIBFILE)
endif

%: %.cpp
%: %.o

${LIBDIR}/${LIBFILE}: ${addprefix ${LIBDIR}/,${DETECTION_OBJFILES}}
	@mkdir -p $(@D)
	ar cr ${LIBDIR}/${LIBFILE} $^

${LIBDIR}/${LIB_TRAINFILE}: ${addprefix ${LIBDIR}/,${TRAINING_OBJFILES}}
	@mkdir -p $(@D)
	ar cr ${LIBDIR}/${LIB_TRAINFILE} $^


${LIBDIR}/%.o: ${SAMPLE_DIR}/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) -c -o $@ $<

${LIBDIR}/%.o: ${DETECTION_DIR}/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) -c -o $@ $<

${LIBDIR}/%.o: ${TRAINING_DIR}/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) -c -o $@ $<


${LIBDIR}/%.d: ${DETECTION_DIR}/%.cpp
	@mkdir -p $(@D)
	g++ -MF $@ -MM -MT $(@:.d=.o) $<

${LIBDIR}/%.d: ${TRAINING_DIR}/%.cpp
	@mkdir -p $(@D)
	g++ -MF $@ -MM -MT $(@:.d=.o) $<

ifneq ($(MAKECMDGOALS),clean)
    -include $(addprefix $(LIBDIR)/, $(DETECTION_OBJFILES:.o=.d))
    ifdef BUILDTRAIN
        -include $(addprefix $(LIBDIR)/, $(TRAINING_OBJFILES:.o=.d))
    endif
endif

clean:
	rm -rf ${LIBDIR}


