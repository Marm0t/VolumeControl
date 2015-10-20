TEMPLATE = app
TARGET = VolumeControl

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Lib" -lole32 -lUser32


SOURCES += \
    volumechanger.cpp\
    tray.cpp    \
    main.cpp \
    keylistener.cpp



QMAKE_LFLAGS += /INCREMENTAL:NO

HEADERS += \
    logging.h \
    volumechanger.h \
    tray.h \
    runnable.h \
    keylistener.h
