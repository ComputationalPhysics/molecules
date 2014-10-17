#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include "moleculardynamics.h"
#include <QSurfaceFormat>
#include <QGLFormat>
#include <QOpenGLContext>

#include <settings.h>
#include <system.h>
#include <thermostat.h>
#include <statisticssampler.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    qmlRegisterType<MolecularDynamics>("MolecularDynamics", 1, 0, "MolecularDynamics");

    QApplication app(argc, argv);
    QQuickView view;

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///Molecules.qml"));
    view.show();

    return app.exec();
}

//int main(int argc, char **argv)
//{
//    QGuiApplication app(argc, argv);

//    qmlRegisterType<MolecularDynamics>("MolecularDynamics", 1, 0, "MolecularDynamics");

//    QQuickView view;
//    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.setSource(QUrl("qrc:///Molecules.qml"));
//    view.show();

//    return app.exec();
//}
