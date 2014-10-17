TEMPLATE = app

QT += qml quick widgets opengl
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
    simulator/simulator.cpp

RESOURCES += qml.qrc

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
    simulator/simulator.h
