QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        gstLive.cpp \
        gstMediaSrc.cpp \
        gstRtspServer.cpp \
        main.cpp

HEADERS += \
    gstLive.h \
    gstMediaSrc.h \
    gstRtspServer.h \

#gstreamer
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
INCLUDEPATH += /usr/include/gstreamer-1.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include
LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0

#gstreamer rtsp server
INCLUDEPATH += $$PWD/ots-lib/gstRtspServer
LIBS += -L$$PWD/ots-lib -lgstrtspserver-1.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
