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
    void update(System *system, float deltaX, float deltaY, float deltaZ);
    void render(QOpenGLShaderProgram *program);
    void initializeOpenGLFunctions();

private:
    GLuint m_vboIds[2];
    std::vector<QVector3D> m_vertices;
    std::vector<GLushort> m_indices;
    QOpenGLFunctions *m_funcs;

    void generateVBOs();

};
