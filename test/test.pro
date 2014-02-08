#
# http2-qt
#
# CC0 - PUBLIC DOMAIN
# This work is free of known copyright restrictions.
# http://creativecommons.org/publicdomain/zero/1.0/
#

QT       += network testlib

QT       -= gui

TARGET = tst_http2test
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_http2test.cpp ../common/hpack.cpp ../common/huffman.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../common
