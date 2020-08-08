# The name of your application
TARGET = qdevel-su

CONFIG += sailfishapp
QT += dbus
PKGCONFIG += nemonotifications-qt5
LIBS += -lutil

HEADERS += \
    src/qdevelsu.h

SOURCES += \
    src/main.cpp \
    src/qdevelsu.cpp

DISTFILES += \
    qml/qdevel-su.qml \
    rpm/qdevel-su.spec

