#-------------------------------------------------
#
# Project created by QtCreator 2019-04-11T21:45:30
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SputterMachineSimulator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    Hofi_operation.cpp \
    KJL2_operation.cpp \
    KJL_operation.cpp \
    MFC_operation.cpp \
    cesar_operation.cpp \
        main.cpp \
        mainwindow.cpp \
    process_serial_data.cpp \
    serial_config.cpp \
    sps_config.cpp

HEADERS += \
    Hofi_operation.h \
    KJL2_operation.h \
    KJL_operation.h \
    MFC_operation.h \
    cesar_operation.h \
        mainwindow.h \
    process_serial_data.h \
    serial_config.h \
    sps_config.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img_resources.qrc
