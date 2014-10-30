#include "cpglcube.h"
#include "system.h"

CPGLCube::CPGLCube() :
    m_funcs(0),
    m_program(0)
{
    generateVBOs();
}


CPGLCube::~CPGLCube()
{
    if(m_funcs) delete m_funcs;
    if(m_program) delete m_program;
}

void CPGLCube::update(System *system, const QVector3D &offset)
{
    ensureInitialized();

    m_vertices.clear();
    m_indices.clear();
    QVector3D systemSize = system->systemSize();
    m_vertices.push_back(QVector3D(offset.x(), offset.y(), offset.z())); // 0,0,0
    m_vertices.push_back(QVector3D(systemSize.x()+offset.x(), offset.y(), offset.z())); // 1,0,0
    m_vertices.push_back(QVector3D(offset.x(), offset.y(), systemSize.z() + offset.z())); // 0,0,1
    m_vertices.push_back(QVector3D(systemSize.x()+offset.x(), offset.y(), systemSize.z() + offset.z())); // 1,0,1

    m_vertices.push_back(QVector3D(offset.x(), systemSize.y()+offset.y(), offset.z())); // 0,1,0
    m_vertices.push_back(QVector3D(systemSize.x()+offset.x(), systemSize.y()+offset.y(), offset.z())); // 1,1,0
    m_vertices.push_back(QVector3D(offset.x(), systemSize.y()+offset.y(), systemSize.z() + offset.z())); // 0,1,1
    m_vertices.push_back(QVector3D(systemSize.x()+offset.x(), systemSize.y()+offset.y(), systemSize.z() + offset.z())); // 1,1,1

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

void CPGLCube::render(const QMatrix4x4 &modelViewProjectionMatrix)
{
    if(m_vertices.size() == 0) return;
    ensureInitialized();
    createShaderProgram();
    m_program->bind();
    m_program->setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix);

    // Tell OpenGL which VBOs to use
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_funcs->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), 0);

    // Draw cube geometry using indices from VBO 1
    m_funcs->glDrawElements(GL_LINES, m_indices.size(), GL_UNSIGNED_SHORT, 0);

    m_program->disableAttributeArray(vertexLocation);
    m_program->release();
}

void CPGLCube::ensureInitialized()
{
    if(!m_funcs) m_funcs = new QOpenGLFunctions(QOpenGLContext::currentContext());
}

void CPGLCube::createShaderProgram()
{
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 a_position;\n"
                                           "uniform highp mat4 modelViewProjectionMatrix;\n"
                                           "void main() {\n"
                                           "    gl_Position = modelViewProjectionMatrix*a_position;\n"
                                           "}");

        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "void main() {\n"
                                           "    gl_FragColor = vec4(0.5, 0.6, 0.53, 1.0);\n"
                                           "}");


        m_program->link();
    }
}

void CPGLCube::generateVBOs()
{
    ensureInitialized();
    m_funcs->glGenBuffers(2, m_vboIds);
}
