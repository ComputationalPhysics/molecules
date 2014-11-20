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
    cpglcube.cpp \
    simulator/atom.cpp \
    simulator/celllist.cpp \
    simulator/cpelapsedtimer.cpp \
    simulator/io.cpp \
    simulator/mdio.cpp \
    simulator/neighborlist.cpp \
    simulator/statisticssampler.cpp \
    simulator/system.cpp \
    simulator/unitconverter.cpp \
    simulator/integrators/integrator.cpp \
    simulator/integrators/velocityverlet.cpp \
    simulator/math/random.cpp \
    simulator/math/vec3.cpp \
    simulator/modifiers/berendsenthermostat.cpp \
    simulator/potentials/lennardjones.cpp \
    simulator/potentials/potential.cpp \
    simulator/simulator.cpp

RESOURCES += qml.qrc \
    images.qrc \
    simulations.qrc \
    fonts.qrc

OTHER_FILES += \
    android/com/compphys/atomify/AtomifyActivity.java \
    android/src/com/compphys/atomify/AtomifyActivity.java

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
    cpglcube.h \
    simulator/atom.h \
    simulator/celllist.h \
    simulator/cpelapsedtimer.h \
    simulator/io.h \
    simulator/mdio.h \
    simulator/neighborlist.h \
    simulator/statisticssampler.h \
    simulator/system.h \
    simulator/unitconverter.h \
    simulator/integrators/integrator.h \
    simulator/integrators/velocityverlet.h \
    simulator/math/random.h \
    simulator/math/vec3.h \
    simulator/modifiers/berendsenthermostat.h \
    simulator/potentials/lennardjones.h \
    simulator/potentials/potential.h \
    simulator/simulator.h

OTHER_FILES += \
    iOS.plist

OTHER_FILES += \
    android/AndroidManifest.xml

