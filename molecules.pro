TEMPLATE = app

android {
    TARGET = Atomify
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
} else {
    TARGET = atomify
}

QT += qml quick widgets opengl openglextensions
CONFIG += c++11
INCLUDEPATH += simulator
SOURCES += main.cpp \
    moleculardynamics.cpp \
    cpglquads.cpp \
    simulator/random.cpp \
    simulator/settings.cpp \
    simulator/statisticssampler.cpp \
    simulator/system.cpp \
    simulator/thermostat.cpp \
    simulator/unitconverter.cpp \
    simulator/simulator.cpp \
    simulator/mdio.cpp \
    vec3.cpp \
    cpglcube.cpp \
    moleculardynamicsrenderer.cpp

RESOURCES += qml/qml.qrc \
    images.qrc \
    fonts.qrc \
    simulations/diffusion/diffusion.qrc \
    simulations/fun/fun.qrc \
    simulations/pressure/pressure.qrc \
    simulations/fractures/fractures.qrc

OTHER_FILES += \
    android/com/compphys/atomify/AtomifyActivity.java \
    android/src/com/compphys/atomify/AtomifyActivity.java

# DEFINES += MD_DEBUG

ios {
    QMAKE_INFO_PLIST = iOS.plist
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    moleculardynamics.h \
    cpglquads.h \
    simulator/atom_types.h \
    simulator/potential_lennard_jones.h \
    simulator/random.h \
    simulator/settings.h \
    simulator/statisticssampler.h \
    simulator/system.h \
    simulator/thermostat.h \
    simulator/unitconverter.h \
    simulator/simulator.h \
    simulator/mdio.h \
    vec3.h \
    cpglcube.h \
    moleculardynamicsrenderer.h

OTHER_FILES += \
    iOS.plist

OTHER_FILES += \
    android/AndroidManifest.xml

