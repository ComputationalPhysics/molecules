#include <cpglquads.h>
#include <QOpenGLFunctions>
#include <QColor>
#include <QRgb>
#include "simulator/atom_types.h"

using std::vector;

CPGLQuads::CPGLQuads() :
    m_funcs(0),
    m_program(0)
{
    generateVBOs();
}

CPGLQuads::~CPGLQuads()
{
    if(m_funcs) delete m_funcs;
    if(m_program) delete m_program;
}

void CPGLQuads::setModelViewMatrix(QMatrix4x4 &matrix)
{
    m_modelViewMatrix = matrix;
}

void CPGLQuads::generateVBOs()
{
    ensureInitialized();
    m_funcs->glGenBuffers(2, m_vboIds);
}

void CPGLQuads::ensureInitialized()
{
    if(!m_funcs) m_funcs = new QOpenGLFunctions(QOpenGLContext::currentContext());
}

QVector3D CPGLQuads::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}

// void CPGLQuads::update(vector<float> &positions)
void CPGLQuads::update(atomDataType *positions, long unsigned int* atomType, int n, const QVector3D &offset)
{
    ensureInitialized();
    int numPoints = n / 3; // x,y,z

    QVector3D right;
    right.setX(m_modelViewMatrix(0,0));
    right.setY(m_modelViewMatrix(0,1));
    right.setZ(m_modelViewMatrix(0,2));
    QVector3D up;
    up.setX(m_modelViewMatrix(1,0));
    up.setY(m_modelViewMatrix(1,1));
    up.setZ(m_modelViewMatrix(1,2));

    float scale = 0.4;
    QVector3D ul = (0.5*up - 0.5*right)*scale;
    QVector3D ur = (0.5*up + 0.5*right)*scale;
    QVector3D dl = (-0.5*up - 0.5*right)*scale;
    QVector3D dr = (-0.5*up + 0.5*right)*scale;

    int numberOfVertices = numPoints*4;
    m_vertices.resize(numberOfVertices);
    m_indices.resize(6*numPoints);

//    QVector3D normalColor =  QVector3D(0.25490196,  0.71372549,  0.76862745);
//    QVector3D frozenColor =  QVector3D(0.13333333333333333, 0.3686274509803922, 0.6588235294117647);
//    QVector3D fixedColor =  QVector3D(0.1450980392156863, 0.20392156862745098, 0.5803921568627451);
//    QVector3D unknownColor =  QVector3D(1.0, 0.0, 0.0);

    QVector3D normalColor = vectorFromColor(QColor("#41b6c4"));
//    QVector3D normalColor = vectorFromColor(QColor("#7fcdbb"));
    QVector3D frozenColor =  vectorFromColor(QColor("#1f78b4"));
//    QVector3D fixedColor =  vectorFromColor(QColor("#33a02c"));
    QVector3D fixedColor =  vectorFromColor(QColor("#1f78b4"));
    QVector3D unknownColor =  vectorFromColor(QColor("#ff0000"));

    for(int i=0; i<numPoints; i++) {
        // NOTE: Y and Z are swapped!
        QVector3D position(positions[3*i + 0] + offset.x(), positions[3*i + 1] + offset.y(), positions[3*i + 2] + offset.z());

        m_vertices[4*i + 0].position = position + dl;
        m_vertices[4*i + 0].textureCoord= QVector2D(0,1);

        m_vertices[4*i + 1].position = position + dr;
        m_vertices[4*i + 1].textureCoord= QVector2D(1,1);

        m_vertices[4*i + 2].position = position + ur;
        m_vertices[4*i + 2].textureCoord= QVector2D(1,0);

        m_vertices[4*i + 3].position = position + ul;
        m_vertices[4*i + 3].textureCoord= QVector2D(0,0);

        QVector3D color;

        if(atomType[i] == FROZEN) {
            color = frozenColor;
        } else if(atomType[i] == FIXED) {
            color = fixedColor;
        } else if(atomType[i] == ARGON) {
            color = normalColor;
        } else {
            color = unknownColor;
        }

        m_vertices[4*i + 0].color = color;
        m_vertices[4*i + 1].color = color;
        m_vertices[4*i + 2].color = color;
        m_vertices[4*i + 3].color = color;

        m_indices [6*i + 0] = 4*i+0;
        m_indices [6*i + 1] = 4*i+1;
        m_indices [6*i + 2] = 4*i+2;

        m_indices [6*i + 3] = 4*i+2;
        m_indices [6*i + 4] = 4*i+3;
        m_indices [6*i + 5] = 4*i+0;
    }

    // Transfer vertex data to VBO 0
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexData), &m_vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    m_funcs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLushort), &m_indices[0], GL_STATIC_DRAW);
}

void CPGLQuads::createShaderProgram() {
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 a_position;\n"
                                           "attribute highp vec3 a_color;\n"
                                           "attribute highp vec2 a_texcoord;\n"
                                           "uniform highp mat4 modelViewProjectionMatrix;\n"
                                           "uniform highp mat4 lightModelViewProjectionMatrix;\n"
                                           "uniform highp float lightFalloffDistance;\n"
                                           "varying highp vec2 coords;\n"
                                           "varying highp float light;\n"
                                           "varying highp vec3 color;\n"
                                           "void main() {\n"
                                           "    gl_Position = modelViewProjectionMatrix*a_position;\n"
                                           "    highp vec4 lightPosition = lightModelViewProjectionMatrix*a_position;\n"
                                           "    light = clamp((lightFalloffDistance * 0.85 - lightPosition.z) / (lightFalloffDistance * 0.7), 0.4, 1.0);\n"
                                           "    coords = a_texcoord;\n"
                                           "    color = a_color;\n"
                                           "}");

        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "varying highp vec2 coords;\n"
                                           "varying highp float light;\n"
                                           "varying highp vec3 color;\n"
                                           "highp vec2 center = vec2(0.5, 0.5);\n"
                                           "void main() {\n"
                                           "    highp vec2 delta = coords - center;\n"
                                           "    highp float r2 = delta.x*delta.x + delta.y*delta.y;\n"
                                           "    highp float gradient = 1.0 - (r2*r2)/0.07;\n"
                                           "    highp vec3 color1 = color;\n"
                                           "    highp vec3 color2 = 0.5 * vec3(color.r*0.1, color.g*0.4, color.b*0.2);\n"
                                           "    highp vec3 finalColor = (color1 * gradient + color2 * (1.0 - gradient));\n"
                                           "    highp float alpha = float(r2<0.25);\n"
                                           "    if(alpha < 0.999) { discard; }\n"
                                           "    gl_FragColor = vec4(finalColor * light, 1.0);\n"
                                           "}");


        m_program->link();
    }
}

void CPGLQuads::render(float lightFalloffDistance, const QMatrix4x4 &modelViewProjectionMatrix, const QMatrix4x4 &lightModelViewProjectionMatrix)
{
    if(m_vertices.size() == 0) return;
    ensureInitialized();
    createShaderProgram();

    m_program->bind();

    // Set modelview-projection matrix
    m_program->setUniformValue("lightFalloffDistance", lightFalloffDistance);
    m_program->setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix);
    m_program->setUniformValue("lightModelViewProjectionMatrix", lightModelViewProjectionMatrix);

    // Tell OpenGL which VBOs to use
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_funcs->glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = m_program->attributeLocation("a_color");
    m_program->enableAttributeArray(colorLocation);
    m_funcs->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    m_funcs->glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
    // glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
    m_funcs->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);

    m_program->release();
}
