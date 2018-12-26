#-------------------------------------------------------
# 
# author : lambor.cai <lamborcai@gmail.com>
#
# Copyright (c) 2018-2025 lanbery,NBS Co.
#  
#-------------------------------------------------------
QT	+= core gui network

greaterThan(QT_MAJOR_VERSION,4): QT += widgets

TARGET = RegisterServer

TEMPLATE = app

# Qt defualt deployment rules target
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android : target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# other files
OTHER_FILES += $$PWD/images/logo.ico \
    $$PWD/images/icon.rc

HEADERS += \
    header.h \
    peerinfo.h \
    registerconnection.h \
    registerserver.h \
    window.h

SOURCES += \
    main.cpp \
    peerinfo.cpp \
    registerconnection.cpp \
    registerserver.cpp \
    window.cpp

# ICON
RC_FILE += images/icon.rc
