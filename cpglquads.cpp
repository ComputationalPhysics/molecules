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
void CPGLQuads::update(vector<Atom*> &atoms, const QVector3D &offset)
{
    ensureInitialized();
    int numAtoms = atoms.size();
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

    int numberOfVertices = numAtoms*4;
    m_vertices.resize(numberOfVertices);
    m_indices.resize(6*numAtoms);

    QVector3D normalColor = vectorFromColor(QColor("#41b6c4"));
    QVector3D frozenColor =  vectorFromColor(QColor("#bcbddc"));
    QVector3D fixedColor =  vectorFromColor(QColor("#bcbddc"));
    QVector3D unknownColor =  vectorFromColor(QColor("#ff0000"));

    for(int i=0; i<numAtoms; i++) {
        // NOTE: Y and Z are swapped!
        Atom *atom = atoms[i];

        QVector3D position(atom->position[0] + offset.x(), atom->position[1] + offset.y(), atom->position[2] + offset.z());

        m_vertices[4*i + 0].position = position + dl;
        m_vertices[4*i + 0].textureCoord= QVector2D(0,1);

        m_vertices[4*i + 1].position = position + dr;
        m_vertices[4*i + 1].textureCoord= QVector2D(1,1);

        m_vertices[4*i + 2].position = position + ur;
        m_vertices[4*i + 2].textureCoord= QVector2D(1,0);

        m_vertices[4*i + 3].position = position + ul;
        m_vertices[4*i + 3].textureCoord= QVector2D(0,0);

        QVector3D color = normalColor;

//        if(atomType[i] == FROZEN) {
//            color = frozenColor;
//        } else if(atomType[i] == FIXED) {
//            color = fixedColor;
//        } else if(atomType[i] == ARGON) {
//            color = normalColor;
//        } else {
//            color = unknownColor;
//        }

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
                                           "    highp float lightDistance = min(lightPosition.z, gl_Position.z);\n"
                                           "    light = clamp((lightFalloffDistance * 0.85 - lightDistance) / (lightFalloffDistance * 0.7), 0.4, 1.0);\n"
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
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);

    m_program->disableAttributeArray(vertexLocation);
    m_program->disableAttributeArray(colorLocation);
    m_program->disableAttributeArray(texcoordLocation);

    m_program->release();
}
