QT += core sql
QT -= gui

TARGET = ImportTree_txt2DB
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

HEADERS     = logger.h \
              TableColumnDataModel.h \
              TableRowDataModel.h

SOURCES     = main.cpp \
              TableColumnDataModel.cpp \
              TableRowDataModel.cpp
