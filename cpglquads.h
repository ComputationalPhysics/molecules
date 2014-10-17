#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <vector>

struct VertexData
{
    QVector3D position;
    QVector2D textureCoord;
};

class CPGLQuads
{
private:
    GLuint m_vboIds[2];
    std::vector<VertexData> m_vertices;
    std::vector<GLushort> m_indices;
    QOpenGLFunctions *m_funcs;

    void generateVBOs();

public:
    CPGLQuads();
    ~CPGLQuads();
    // void update(std::vector<float> &positions);
    void update(double *positions, int n, float deltaX, float deltaY, float deltaZ);
    void render(QOpenGLShaderProgram *program);
    void initializeOpenGLFunctions();
};
