#include <cpglquads.h>
using std::vector;

void CPGLQuads::generateVBOs()
{
    glGenBuffers(2, m_vboIds);
}

// void CPGLQuads::update(vector<float> &positions)
void CPGLQuads::update(double *positions, int n, float deltaX, float deltaY, float deltaZ)
{
    int numPoints = n / 3; // x,y,z
    QVector3D up(0,1,0);
    QVector3D right(1,0,0);
    float scale = 0.3;
    QVector3D ul = (0.5*up - 0.5*right)*scale;
    QVector3D ur = (0.5*up + 0.5*right)*scale;
    QVector3D dl = (-0.5*up - 0.5*right)*scale;
    QVector3D dr = (-0.5*up + 0.5*right)*scale;

    int numberOfVertices = numPoints*4;
    m_vertices.resize(numberOfVertices);
    m_indices.resize(6*numPoints);

    for(int i=0; i<numPoints; i++) {
        QVector3D position(positions[3*i + 0] - deltaX, positions[3*i + 1] - deltaY, positions[3*i + 2] - deltaZ);

        m_vertices[4*i + 0].position = position + dl;
        m_vertices[4*i + 0].textureCoord= QVector2D(0,1);

        m_vertices[4*i + 1].position = position + dr;
        m_vertices[4*i + 1].textureCoord= QVector2D(1,1);

        m_vertices[4*i + 2].position = position + ur;
        m_vertices[4*i + 2].textureCoord= QVector2D(1,0);

        m_vertices[4*i + 3].position = position + ul;
        m_vertices[4*i + 3].textureCoord= QVector2D(0,0);

        m_indices [6*i + 0] = 4*i+0;
        m_indices [6*i + 1] = 4*i+1;
        m_indices [6*i + 2] = 4*i+2;

        m_indices [6*i + 3] = 4*i+2;
        m_indices [6*i + 4] = 4*i+3;
        m_indices [6*i + 5] = 4*i+0;
    }

    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexData), &m_vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLushort), &m_indices[0], GL_STATIC_DRAW);
}

void CPGLQuads::render(QOpenGLShaderProgram *program)
{
    if(m_vertices.size() == 0) return;

    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
    // glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
}

CPGLQuads::CPGLQuads()
{
    generateVBOs();
}
