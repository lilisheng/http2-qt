#
# http2-qt
#
# CC0 - PUBLIC DOMAIN
# This work is free of known copyright restrictions.
# http://creativecommons.org/publicdomain/zero/1.0/
#


QT       += core network
QT       -= gui

TARGET = http2c
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    client.cpp \
    ../common/basicframe.cpp \
    ../common/hpack.cpp \
    ../common/connection.cpp \
    ../common/settingsframe.cpp \
    ../common/headersframe.cpp \
    ../common/dataframe.cpp \
    ../common/goawayframe.cpp \
    ../common/pingframe.cpp \
    ../common/windowupdateframe.cpp \
    ../common/huffman.cpp \
    ../common/stream.cpp

HEADERS += \
    client.h \
    ../common/basicframe.h \
    ../common/hpack.h \
    ../common/connection.h \
    ../common/settingsframe.h \
    ../common/headersframe.h \
    ../common/iframe.h \
    ../common/dataframe.h \
    ../common/goawayframe.h \
    ../common/pingframe.h \
    ../common/windowupdateframe.h \
    ../common/huffman.h \
    ../common/huffmantable.h \
    ../common/errorcode.h \
    ../common/stream.h \
    ../common/headertable.h \
    ../common/streamcontext.h \
    ../common/common.h \
    ../common/consts.h \
    app.h

INCLUDEPATH += ../common
