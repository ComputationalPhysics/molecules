#pragma once
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QVector3D>

class System;
class CPGLCube
{
public:
    CPGLCube();
    ~CPGLCube();
    void update(const QVector3D &systemSize, const QVector3D &offset);
    void render(const QMatrix4x4 &modelViewProjectionMatrix);
    void ensureInitialized();

private:
    GLuint m_vboIds[2];
    std::vector<QVector3D> m_vertices;
    std::vector<GLushort> m_indices;
    QOpenGLFunctions *m_funcs;
    QOpenGLShaderProgram *m_program;

    void createShaderProgram();
    void generateVBOs();

};
