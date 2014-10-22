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
#include "simulator/atom_types.h"
using namespace std;

MolecularDynamicsRenderer::MolecularDynamicsRenderer()
{
    m_glQuads = new CPGLQuads();
    m_glCube = new CPGLCube();
}

MolecularDynamicsRenderer::~MolecularDynamicsRenderer()
{

}

void MolecularDynamicsRenderer::resetProjection()
{
    // Calculate aspect ratio
    qreal aspect = qreal(m_viewportSize.width()) / qreal(m_viewportSize.height() ? m_viewportSize.height() : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 65 degrees
    const qreal zNear = 2.0, zFar = 2000.0, fov = 65.0;

    // Reset projection
    m_projectionMatrix.setToIdentity();

    // Set perspective projection
    m_projectionMatrix.perspective(fov, aspect, zNear, zFar);
}

void MolecularDynamicsRenderer::setModelViewMatrices(double zoom, double tilt, double pan, double roll)
{
    float systemSizeX = m_simulator.m_system.systemSize().x();
    float systemSizeY = m_simulator.m_system.systemSize().y();
    float systemSizeZ = m_simulator.m_system.systemSize().z();
    float systemSizeMax = sqrt(systemSizeX*systemSizeX + systemSizeY*systemSizeY + systemSizeZ*systemSizeZ);

    m_modelViewMatrix.setToIdentity();
    m_modelViewMatrix.translate(0,0,zoom);
    m_modelViewMatrix.rotate(-90, 1, 0, 0);
    m_modelViewMatrix.rotate(tilt, 1, 0, 0);
    m_modelViewMatrix.rotate(pan, 0, 0, 1);
    m_modelViewMatrix.rotate(roll, 0, 1, 0);

    m_lightModelViewMatrix.setToIdentity();
    m_lightModelViewMatrix.translate(0,0,-systemSizeMax / 2.0);
    m_lightModelViewMatrix.rotate(-90, 1, 0, 0);
    m_lightModelViewMatrix.rotate(tilt, 1, 0, 0);
    m_lightModelViewMatrix.rotate(pan, 0, 0, 1);
    m_lightModelViewMatrix.rotate(roll, 0, 1, 0);
}

void MolecularDynamicsRenderer::paint()
{
    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate model view transformation
//    QMatrix4x4 matrix;
//    QMatrix4x4 lightMatrix;
    float systemSizeX = m_simulator.m_system.systemSize().x();
    float systemSizeY = m_simulator.m_system.systemSize().y();
    float systemSizeZ = m_simulator.m_system.systemSize().z();

    float systemSizeMax = sqrt(systemSizeX*systemSizeX + systemSizeY*systemSizeY + systemSizeZ*systemSizeZ);

    QMatrix4x4 modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix;
    QMatrix4x4 lightModelViewProjectionMatrix = m_projectionMatrix * m_lightModelViewMatrix;

    QVector3D offset(-systemSizeX/2.0, -systemSizeY/2.0, -systemSizeZ/2.0);

    int n = 3*m_simulator.m_system.numAtoms();
    m_glQuads->setModelViewMatrix(m_modelViewMatrix);
    m_glQuads->update(&(m_simulator.m_system.positions[0]), &(m_simulator.m_system.atom_type[0]), n, offset);
    m_glQuads->render(systemSizeMax, modelViewProjectionMatrix, lightModelViewProjectionMatrix);

    m_glCube->update(&(m_simulator.m_system),offset);
    m_glCube->render(modelViewProjectionMatrix);

    glDepthMask(GL_TRUE);
}

MolecularDynamics::MolecularDynamics()
    : m_renderer(0),
      m_thermostatValue(1.0),
      m_thermostatEnabled(false),
      m_forceEnabled(false),
      m_forceValue(0),
      m_tilt(0),
      m_pan(0),
      m_roll(0),
      m_zoom(-4),
      m_running(true),
      m_atomCount(0),
      m_didScaleVelocitiesDueToHighValues(false),
      m_temperature(0),
      m_pressure(0),
      m_kineticEnergy(0),
      m_potentialEnergy(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    m_timer.start();
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
//    m_renderer->incrementRotation(deltaPan, deltaTilt, deltaRoll);
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::incrementZoom(double deltaZoom) {
    if(!m_renderer) {
        return;
    }
//    m_renderer->incrementZoom(deltaZoom);
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

void MolecularDynamics::setForceEnabled(bool arg)
{
    if(!m_renderer) return;
    if (m_forceEnabled == arg)
        return;

    m_forceEnabled = arg;
    m_renderer->m_simulator.m_settings.gravity_direction = m_forceEnabled ? 2 : -1; // -1 means disabled
    emit forceEnabledChanged(arg);
}

void MolecularDynamics::setForceValue(double arg)
{
    if(!m_renderer) return;
    if (m_forceValue == arg)
        return;

    m_forceValue = arg;
    m_renderer->m_simulator.m_settings.gravity_force = m_forceValue*1e-3; // Nice scaling
    emit forceValueChanged(arg);
}

void MolecularDynamics::setSystemSize(QVector3D arg)
{
    if(!m_renderer) return;
    if (m_systemSize == arg)
        return;

    m_systemSize = arg;
    m_renderer->m_simulator.m_system.setSystemSize(m_systemSize);
    emit systemSizeChanged(arg);
}

void MolecularDynamics::setTilt(double arg)
{
    if (m_tilt == arg)
        return;

    m_tilt = arg;
    emit tiltChanged(arg);
}

void MolecularDynamics::setPan(double arg)
{
    if (m_pan == arg)
        return;

    m_pan = arg;
    emit panChanged(arg);
}

void MolecularDynamics::setRoll(double arg)
{
    if (m_roll == arg)
        return;

    m_roll = arg;
    emit rollChanged(arg);
}

void MolecularDynamics::setZoom(double arg)
{
    if (m_zoom == arg)
        return;

    m_zoom = arg;
    emit zoomChanged(arg);
}

void MolecularDynamics::setRunning(bool arg)
{
    if (m_running == arg)
        return;

    m_running = arg;
    emit runningChanged(arg);
}

void MolecularDynamics::sync()
{
    if (!m_renderer) {
        m_renderer = new MolecularDynamicsRenderer();
        m_systemSize = m_renderer->m_simulator.m_system.systemSize();
        emit systemSizeChanged(m_systemSize);

        connect(window(), SIGNAL(beforeRendering()), m_renderer, SLOT(paint()), Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->resetProjection();
    m_renderer->setModelViewMatrices(m_zoom, m_tilt, m_pan, m_roll);
    double dt = m_timer.restart() / 1000.0;
    double safeDt = min(0.02, dt);
    if(m_thermostatEnabled) {
        double systemTemperature = m_renderer->m_simulator.m_system.unit_converter->temperature_from_SI(m_thermostatValue);
        m_renderer->m_simulator.m_thermostat->relaxation_time = 1;
        m_renderer->m_simulator.m_thermostat->apply(m_renderer->m_simulator.m_sampler, &(m_renderer->m_simulator.m_system), systemTemperature, ARGON);
    }

    m_renderer->m_simulator.m_system.setDt(safeDt);

    if(m_running) {
        m_renderer->m_simulator.step();
    }

    setDidScaleVelocitiesDueToHighValues(m_renderer->m_simulator.m_system.didScaleVelocitiesDueToHighValues());
    setAtomCount(m_renderer->m_simulator.m_system.numAtoms());
    setTemperature(m_renderer->m_simulator.m_system.unit_converter->temperature_to_SI(m_renderer->m_simulator.m_sampler->temperature_free_atoms));
    setKineticEnergy(m_renderer->m_simulator.m_system.unit_converter->energy_to_ev(m_renderer->m_simulator.m_sampler->kinetic_energy));
    setPotentialEnergy(m_renderer->m_simulator.m_system.unit_converter->energy_to_ev(m_renderer->m_simulator.m_sampler->potential_energy));
    setPressure(m_renderer->m_simulator.m_system.unit_converter->pressure_to_SI(m_renderer->m_simulator.m_sampler->pressure));

//    double pressure = m_renderer->m_simulator.m_sampler->pressure;
//    double volume = m_renderer->m_simulator.m_system.volume();
//    double temperature = m_renderer->m_simulator.m_sampler->temperature;
//    qDebug() << "PV/NkT=" << pressure*volume/(atomCount()*temperature);
}

void MolecularDynamics::step(double dt)
{
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
    m_systemSize = m_renderer->m_simulator.m_system.systemSize();
    emit systemSizeChanged(m_systemSize);
}

double MolecularDynamics::thermostatValue() const
{
    return m_thermostatValue;
}

bool MolecularDynamics::thermostatEnabled() const
{
    return m_thermostatEnabled;
}

bool MolecularDynamics::forceEnabled() const
{
    return m_forceEnabled;
}

double MolecularDynamics::forceValue() const
{
    return m_forceValue;
}

QVector3D MolecularDynamics::systemSize() const
{
    return m_systemSize;
}

double MolecularDynamics::tilt() const
{
    return m_tilt;
}

double MolecularDynamics::pan() const
{
    return m_pan;
}

double MolecularDynamics::roll() const
{
    return m_roll;
}

double MolecularDynamics::zoom() const
{
    return m_zoom;
}

bool MolecularDynamics::running() const
{
    return m_running;
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




