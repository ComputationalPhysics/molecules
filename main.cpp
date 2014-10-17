#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include "moleculardynamics.h"
#include <QSurfaceFormat>
#include <QGLFormat>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
    qmlRegisterType<MolecularDynamics>("MolecularDynamics", 1, 0, "MolecularDynamics");

    QApplication app(argc, argv);

    QGLFormat glFormat;
    glFormat.setVersion(4, 2);
    glFormat.setProfile(QGLFormat::CoreProfile);

    // Set the default GL format to OpenGL 3.2 Core
    QGLFormat::setDefaultFormat(glFormat);

//    qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();

//    QGLFormat glFormat;
//    glFormat.setVersion( 4, 2 );
//    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
//    QOpenGLContext context;
//    context.setFormat(f);

//    QGLWidget* qglWidget = new QGLWidget(glFormat);

//    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
//    qDebug() << "Driver Version String:" << versionString;
//    qDebug() << "Current Context:" << qglWidget->format();

//    QOpenGLVersionProfile::setVersion(4,2);
//    QOpenGLVersionProfile::setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    // QOpenGLContext::versionFunctions()

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

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
