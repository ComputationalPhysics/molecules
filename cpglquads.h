#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <vector>
#include <system.h>

struct VertexData
{
    QVector3D position;
    QVector3D color;
    QVector2D textureCoord;
};

class CPGLQuads
{
public:
    CPGLQuads();
    ~CPGLQuads();
    // void update(std::vector<float> &positions);
    void update(atomDataType *positions, unsigned long *atomType, int n, float deltaX, float deltaY, float deltaZ);
    void render(QOpenGLShaderProgram *program);
    void initializeOpenGLFunctions();
    void setModelViewMatrix(QMatrix4x4& matrix);
private:
    GLuint m_vboIds[2];
    std::vector<VertexData> m_vertices;
    std::vector<GLushort> m_indices;
    QOpenGLFunctions *m_funcs;
    QMatrix4x4 m_modelViewMatrix;

    void generateVBOs();
};
