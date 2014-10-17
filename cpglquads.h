#pragma once
#include <QtGui/QOpenGLShaderProgram>
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

    void generateVBOs();

public:
    CPGLQuads();
    void update(std::vector<float> &positions);
    void render(QOpenGLShaderProgram *program);
};
