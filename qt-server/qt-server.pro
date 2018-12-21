#-------------------------------------------------------
# 
# author : lambor.cai <lamborcai@gmail.com>
#
# Copyright (c) 2018-2025 lanbery,NBS Co.
#  
#-------------------------------------------------------
QT	+= core gui network

greaterThan(QT_MAJOR_VERSION,5): QT +=widgets

TARGER = RegisterServer

TEMPLATE = app

# Qt defualt deployment rules target
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android : target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


