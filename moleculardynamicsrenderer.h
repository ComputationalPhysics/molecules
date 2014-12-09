#ifndef MOLECULARDYNAMICSRENDERER_H
#define MOLECULARDYNAMICSRENDERER_H

#include "cpglquads.h"
#include "cpglcube.h"

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QQuickFramebufferObject>
#include <QThread>
#include <QMutex>

class MolecularDynamicsRenderer : public QQuickFramebufferObject::Renderer
{
public:
    MolecularDynamicsRenderer();
    ~MolecularDynamicsRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void resetProjection();
    void setModelViewMatrices(double zoom, double tilt, double pan, double roll, const QVector3D &systemSize);

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();

private:
    QSize m_viewportSize;

    QVector3D m_systemSize;
    int m_atomCount;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_modelViewMatrix;
    QMatrix4x4 m_lightModelViewMatrix;
    CPGLQuads *m_glQuads;
    CPGLCube *m_glCube;

    std::vector<atomDataType> m_positions;
    std::vector<unsigned long> m_atomTypes;

    bool m_skipNextFrame;

    int m_syncCount;
    int m_renderCount;
    int m_dirtyCount;
};

#endif // MOLECULARDYNAMICSRENDERER_H
