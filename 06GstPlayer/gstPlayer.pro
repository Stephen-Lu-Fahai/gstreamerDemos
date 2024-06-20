QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        display.cpp \
        main.cpp \
        videoSrc.cpp


CUDA_SOURCES += kernel.cu

#gstreamer
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
INCLUDEPATH += /usr/include/gstreamer-1.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include
LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0



# OpenCV libs
INCLUDEPATH += /opt/OpenCv4.7/include/opencv4
LIBS += -L/opt/OpenCv4.7/lib/
LIBS += -lopencv_core -lopencv_imgproc -lopencv_calib3d -lopencv_imgcodecs -lopencv_features2d -lopencv_flann -lopencv_photo -lopencv_freetype\
        -lopencv_cudaimgproc -lopencv_cudafilters -lopencv_cudaarithm -lopencv_cudafeatures2d -lopencv_cudawarping -lopencv_cudastereo


CUDA_ARCH = -arch=sm_75 \
            -gencode=arch=compute_75,code=sm_75 \
            -gencode=arch=compute_86,code=sm_86 \
            -gencode=arch=compute_89,code=sm_89 \
            -gencode=arch=compute_89,code=compute_89

# CUDA settings <-- may change depending on your system
CUDA_SDK = "/usr/local/cuda"   # Path to cuda SDK install
CUDA_DIR = "/usr/local/cuda"   # Path to cuda toolkit install

# DO NOT EDIT BEYOND THIS UNLESS YOU KNOW WHAT YOU ARE DOING....
SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'
SYSTEM_TYPE = 64            # '32' or '64', depending on your system

NVCC_OPTIONS = --use_fast_math -Xcompiler -fPIC

# include paths
INCLUDEPATH += $$CUDA_DIR/include

# library directories
QMAKE_LIBDIR += $$CUDA_DIR/lib64
QMAKE_LIBDIR += $$CUDA_DIR/lib64/stubs

CUDA_OBJECTS_DIR = ./

# Add the necessary libraries
CUDA_LIBS = -lcuda -lcudart -lnppc -lnpps -lnppig -lnppidei -lnppif -lnppicc -lnppitc -lnppim -lnppist -lnppial

# The following makes sure all path names (which often include spaces) are put between quotation marks
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')
LIBS += $$CUDA_LIBS

# Configuration of the Cuda compiler
CONFIG(debug, debug|release) {
    # Debug mode
    cuda_d.input = CUDA_SOURCES
    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}.o
    cuda_d.commands = $$CUDA_DIR/bin/nvcc -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE $$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda_d
}
else {
    # Release mode
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}.o
    cuda.commands = $$CUDA_DIR/bin/nvcc $$NVCC_OPTIONS $$CUDA_INC $$NVCC_LIBS --machine $$SYSTEM_TYPE $$CUDA_ARCH -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    display.h \
    videoSrc.h
