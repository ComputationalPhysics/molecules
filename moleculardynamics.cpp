/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <moleculardynamics.h>

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QGLFormat>
#include <QOpenGLContext>
#include <iostream>
#include <cmath>
#include "simulator/unitconverter.h"
#include "simulator/mdio.h"
using namespace std;

MolecularDynamicsRenderer::MolecularDynamicsRenderer() :
    m_tilt(0),
    m_pan(0),
    m_roll(0),
    m_positions(0),
    m_zoom(-4),
    m_program(0)
{
    m_glQuads = new CPGLQuads();
}

MolecularDynamicsRenderer::~MolecularDynamicsRenderer()
{
    delete m_program;
}

void MolecularDynamicsRenderer::resetProjection()
{
    // Calculate aspect ratio
    qreal aspect = qreal(m_viewportSize.width()) / qreal(m_viewportSize.height() ? m_viewportSize.height() : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 65 degrees
    const qreal zNear = 2.0, zFar = 200.0, fov = 65.0;

    // Reset projection
    m_projection.setToIdentity();

    // Set perspective projection
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void MolecularDynamicsRenderer::incrementRotation(double deltaPan, double deltaTilt, double deltaRoll)
{
    m_pan += deltaPan;
    m_tilt += deltaTilt;
    m_tilt = max(-90.0, min(90.0, m_tilt)); // Clamp so that upside-down is not possible
    m_roll += deltaRoll;
}

void MolecularDynamicsRenderer::incrementZoom(double deltaZoom)
{
    m_zoom += deltaZoom;

}

void MolecularDynamicsRenderer::paint()
{
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 a_position;\n"
                                           "attribute highp vec3 a_color;\n"
                                           "attribute highp vec2 a_texcoord;\n"
                                           "uniform highp mat4 modelViewProjectionMatrix;\n"
                                           "uniform highp mat4 lightModelViewProjectionMatrix;\n"
                                           "uniform highp float systemSizeZ;\n"
                                           "varying highp vec2 coords;\n"
                                           "varying highp float light;\n"
                                           "varying highp vec3 color;\n"
                                           "void main() {\n"
                                           "    gl_Position = modelViewProjectionMatrix*a_position;\n"
                                           "    highp vec4 lightPosition = lightModelViewProjectionMatrix*a_position;\n"
                                           "    light = clamp((systemSizeZ * 0.7 - lightPosition.z) / (systemSizeZ * 0.7), 0.0, 1.0);\n"
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
    m_program->bind();

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    QMatrix4x4 lightMatrix;
    float systemSizeX = m_simulator.m_system.systemSize().x();
    float systemSizeY = m_simulator.m_system.systemSize().y();
    float systemSizeZ = m_simulator.m_system.systemSize().z();

    float zoom = m_zoom;

    matrix.translate(0,0,zoom-systemSizeZ);

    matrix.rotate(-90, 1, 0, 0);
    matrix.rotate(m_tilt, 1, 0, 0);
    matrix.rotate(m_pan, 0, 0, 1);
    matrix.rotate(m_roll, 0, 1, 0);

    lightMatrix.translate(0,0,-systemSizeZ / 2.0);

    lightMatrix.rotate(-90, 1, 0, 0);
    lightMatrix.rotate(m_tilt, 1, 0, 0);
    lightMatrix.rotate(m_pan, 0, 0, 1);
    lightMatrix.rotate(m_roll, 0, 1, 0);

    // Set modelview-projection matrix
    m_program->setUniformValue("systemSizeZ", systemSizeZ);
    m_program->setUniformValue("modelViewProjectionMatrix", m_projection * matrix);
    m_program->setUniformValue("lightModelViewProjectionMatrix", m_projection * lightMatrix);

    int n = 3*m_simulator.m_system.num_atoms;
    m_glQuads->setModelViewMatrix(matrix);
    m_glQuads->update(&(m_simulator.m_system.positions[0]), &(m_simulator.m_system.atom_type[0]), n, -systemSizeX/2.0, -systemSizeY/2.0, -systemSizeZ/2.0);
    m_glQuads->render(m_program);

    glDepthMask(GL_TRUE);

    m_program->release();
}

double MolecularDynamicsRenderer::zoom() const
{
    return m_zoom;
}

void MolecularDynamicsRenderer::setZoom(double zoom)
{
    m_zoom = zoom;
}

void MolecularDynamics::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void MolecularDynamics::incrementRotation(double deltaPan, double deltaTilt, double deltaRoll)
{
    if(!m_renderer) {
        return;
    }
    m_renderer->incrementRotation(deltaPan, deltaTilt, deltaRoll);
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::incrementZoom(double deltaZoom) {
    if(!m_renderer) {
        return;
    }
    m_renderer->incrementZoom(deltaZoom);
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::setThermostatValue(double arg)
{
    if (m_thermostatValue != arg) {
        m_thermostatValue = arg;
        emit thermostatValueChanged(arg);
    }
}

void MolecularDynamics::setThermostatEnabled(bool arg)
{
    if (m_thermostatEnabled != arg) {
        m_thermostatEnabled = arg;
        emit thermostatEnabledChanged(arg);
    }
}

void MolecularDynamics::sync()
{
    if (!m_renderer) {
        m_renderer = new MolecularDynamicsRenderer();
        connect(window(), SIGNAL(beforeRendering()), m_renderer, SLOT(paint()), Qt::DirectConnection);
        m_renderer->m_positions = &m_positions;
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->resetProjection();
}

MolecularDynamics::MolecularDynamics()
    : m_renderer(0),
      m_thermostatValue(1.0),
      m_thermostatEnabled(false)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));

    int Nx = 10;
    int Ny = 10;
    int Nz = 10;
    m_positions.resize(3*Nx*Ny*Nz);
    for(int i=0; i<Nx; i++) {
        for(int j=0; j<Ny; j++) {
            for(int k=0; k<Nz; k++) {
                int index = i*Ny*Nz + j*Nz + k;
                m_positions[3*index + 0] = i-Nx/2.0;
                m_positions[3*index + 1] = j-Ny/2.0;
                m_positions[3*index + 2] = k;
            }
        }
    }
}

void MolecularDynamics::step(double dt)
{
    if(!m_renderer) {
        return;
    }
    double safeDt = min(0.02, dt);
    if(m_thermostatEnabled) {
        double systemTemperature = m_renderer->m_simulator.m_system.unit_converter->temperature_from_SI(m_thermostatValue);
        m_renderer->m_simulator.m_thermostat->relaxation_time = 0.1;
        m_renderer->m_simulator.m_thermostat->apply(m_renderer->m_simulator.m_sampler, &(m_renderer->m_simulator.m_system), systemTemperature, false);
    }

    m_renderer->m_simulator.m_system.setDt(safeDt);
    m_renderer->m_simulator.step();
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::save(QString fileName)
{
    if(!m_renderer) {
        return;
    }
    m_renderer->m_simulator.m_system.mdio->save_state_to_file_binary(fileName);
}

void MolecularDynamics::load(QString fileName)
{
    if(!m_renderer) {
        return;
    }
    m_renderer->m_simulator.m_system.mdio->load_state_from_file_binary(fileName);
}

double MolecularDynamics::thermostatValue() const
{
    return m_thermostatValue;
}

bool MolecularDynamics::thermostatEnabled() const
{
    return m_thermostatEnabled;
}

void MolecularDynamics::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}




