#include "cpglcube.h"
#include "system.h"

CPGLCube::CPGLCube()
{
}

CPGLCube::~CPGLCube()
{
    delete m_funcs;
}

void CPGLCube::update(System *system, float deltaX, float deltaY, float deltaZ)
{
    m_vertices.clear();
    m_indices.clear();
    vec3 systemSize = system->systemSize();
    m_vertices.push_back(QVector3D(deltaX, deltaY, deltaZ)); // 0,0,0
    m_vertices.push_back(QVector3D(systemSize.x()+deltaX, deltaY, deltaZ)); // 1,0,0
    m_vertices.push_back(QVector3D(deltaX, deltaY, systemSize.z() + deltaZ)); // 0,0,1
    m_vertices.push_back(QVector3D(systemSize.x()+deltaX, deltaY, systemSize.z() + deltaZ)); // 1,0,1

    m_vertices.push_back(QVector3D(deltaX, systemSize.y()+deltaY, deltaZ)); // 0,1,0
    m_vertices.push_back(QVector3D(systemSize.x()+deltaX, systemSize.y()+deltaY, deltaZ)); // 1,1,0
    m_vertices.push_back(QVector3D(deltaX, systemSize.y()+deltaY, systemSize.z() + deltaZ)); // 0,1,1
    m_vertices.push_back(QVector3D(systemSize.x()+deltaX, systemSize.y()+deltaY, systemSize.z() + deltaZ)); // 1,1,1

    m_indices.push_back(0);
    m_indices.push_back(1);

    m_indices.push_back(0);
    m_indices.push_back(2);

    m_indices.push_back(2);
    m_indices.push_back(3);

    m_indices.push_back(3);
    m_indices.push_back(1);

    m_indices.push_back(0);
    m_indices.push_back(4);

    m_indices.push_back(2);
    m_indices.push_back(6);

    m_indices.push_back(3);
    m_indices.push_back(7);

    m_indices.push_back(1);
    m_indices.push_back(5);

    m_indices.push_back(4);
    m_indices.push_back(5);

    m_indices.push_back(4);
    m_indices.push_back(6);

    m_indices.push_back(6);
    m_indices.push_back(7);

    m_indices.push_back(5);
    m_indices.push_back(7);
    // Transfer vertex data to VBO 0
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(QVector3D), &m_vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    m_funcs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLushort), &m_indices[0], GL_STATIC_DRAW);
}

void CPGLCube::render(QOpenGLShaderProgram *program)
{
    if(m_vertices.size() == 0) return;

    // Tell OpenGL which VBOs to use
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    m_funcs->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), 0);

    // Draw cube geometry using indices from VBO 1
    m_funcs->glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
}

void CPGLCube::initializeOpenGLFunctions()
{
    m_funcs = new QOpenGLFunctions(QOpenGLContext::currentContext());
}

void CPGLCube::generateVBOs()
{
    m_funcs->glGenBuffers(2, m_vboIds);
}
