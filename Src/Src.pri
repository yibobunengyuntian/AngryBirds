INCLUDEPATH += $$PWD

FORMS += \
    $$PWD/mainwin.ui

HEADERS += \
    $$PWD/defines.h \
    $$PWD/mainwin.h

SOURCES += \
    $$PWD/mainwin.cpp


include($$PWD/Dialog/Dialog.pri)
include($$PWD/HomeWgt/HomeWgt.pri)
include($$PWD/LevelWgt/LevelWgt.pri)
include($$PWD/MaskWgt/MaskWgt.pri)
