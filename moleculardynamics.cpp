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
#include <QOpenGLFramebufferObjectFormat>
#include "simulator/unitconverter.h"
#include "simulator/mdio.h"
#include "simulator/atom_types.h"
using namespace std;

MolecularDynamicsRenderer::MolecularDynamicsRenderer() :
    m_skipNextFrame(false),
    m_syncCount(0),
    m_renderCount(0),
    m_dirtyCount(0),
    m_atomCount(0)
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

void MolecularDynamicsRenderer::setModelViewMatrices(double zoom, double tilt, double pan, double roll, const QVector3D &systemSize)
{
    m_systemSize = systemSize;
    float systemSizeX = systemSize.x();
    float systemSizeY = systemSize.y();
    float systemSizeZ = systemSize.z();
    float systemSizeMax = sqrt(systemSizeX*systemSizeX + systemSizeY*systemSizeY + systemSizeZ*systemSizeZ);

    m_modelViewMatrix.setToIdentity();
    m_modelViewMatrix.translate(0,0,zoom);
    m_modelViewMatrix.rotate(90, 1, 0, 0);
    m_modelViewMatrix.rotate(tilt, 1, 0, 0);
    m_modelViewMatrix.rotate(pan, 0, 0, 1);
    m_modelViewMatrix.rotate(roll, 0, 1, 0);

    m_lightModelViewMatrix.setToIdentity();
    m_lightModelViewMatrix.translate(0,0,-systemSizeMax / 2.0);
    m_lightModelViewMatrix.rotate(90, 1, 0, 0);
    m_lightModelViewMatrix.rotate(tilt, 1, 0, 0);
    m_lightModelViewMatrix.rotate(pan, 0, 0, 1);
    m_lightModelViewMatrix.rotate(roll, 0, 1, 0);
}

QOpenGLFramebufferObject *MolecularDynamicsRenderer::createFramebufferObject(const QSize &size) {
    m_viewportSize = size;
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void MolecularDynamicsRenderer::synchronize(QQuickFramebufferObject* item)
{
    m_syncCount++;
    MolecularDynamics *molecularDynamics = (MolecularDynamics*)item; // TODO: Use correct casting method
    if(!molecularDynamics) {
        return;
    }
    resetProjection();
    setModelViewMatrices(molecularDynamics->zoom(), molecularDynamics->tilt(), molecularDynamics->pan(), molecularDynamics->roll(), molecularDynamics->systemSize());
    if(molecularDynamics->simulatorOutputDirty()) {
        qDebug() << "Setting positions";
        molecularDynamics->m_simulatorOutputMutex.lock();
        m_atomCount = molecularDynamics->m_atomCount;
        m_positions = molecularDynamics->m_positions;
        m_atomTypes = molecularDynamics->m_atomTypes;
        molecularDynamics->setSimulatorOutputDirty(false);
        molecularDynamics->m_simulatorOutputMutex.unlock();
        m_dirtyCount++;
    }
}

void MolecularDynamicsRenderer::render()
{
    m_renderCount++;
    glDepthMask(true);

    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Calculate model view transformation
    //    QMatrix4x4 matrix;
    //    QMatrix4x4 lightMatrix;
    float systemSizeX = m_systemSize.x();
    float systemSizeY = m_systemSize.y();
    float systemSizeZ = m_systemSize.z();

    float systemSizeMax = sqrt(systemSizeX*systemSizeX + systemSizeY*systemSizeY + systemSizeZ*systemSizeZ);

    QMatrix4x4 modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix;
    QMatrix4x4 lightModelViewProjectionMatrix = m_projectionMatrix * m_lightModelViewMatrix;

    QVector3D offset(-systemSizeX/2.0, -systemSizeY/2.0, -systemSizeZ/2.0);

    int n = m_atomCount * 3;
    m_glQuads->setModelViewMatrix(m_modelViewMatrix);
    m_glQuads->update(&(m_positions[0]), &(m_atomTypes[0]), n, offset);
    m_glQuads->render(systemSizeMax, modelViewProjectionMatrix, lightModelViewProjectionMatrix);

    m_glCube->update(m_systemSize, offset);
    m_glCube->render(modelViewProjectionMatrix);

    glDepthMask(GL_TRUE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void MolecularDynamicsSimulator::step()
{
    m_simulator.step();
    emit stepCompleted();
}

MolecularDynamics::MolecularDynamics()
    :
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
      m_potentialEnergy(0),
      m_systemSizeIsDirty(false),
      m_stepRequested(false),
      m_previousStepCompleted(true),
      m_simulatorOutputDirty(false)
{
    m_timer.start();

    m_mdSimulator.moveToThread(&m_simulatorWorker);
    connect(this, &MolecularDynamics::requestStep, &m_mdSimulator, &MolecularDynamicsSimulator::step);
    connect(&m_mdSimulator, &MolecularDynamicsSimulator::stepCompleted, this, &MolecularDynamics::finalizeStep);
    m_simulatorWorker.start();
}

MolecularDynamics::~MolecularDynamics()
{
    m_simulatorInputMutex.unlock();
    m_simulatorOutputMutex.unlock();
    m_simulatorRunningMutex.unlock();
    m_simulatorWorker.quit();
    m_simulatorWorker.wait();
}

MolecularDynamicsRenderer *MolecularDynamics::createRenderer() const
{
    return new MolecularDynamicsRenderer();
}

void MolecularDynamics::incrementRotation(double deltaPan, double deltaTilt, double deltaRoll)
{
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::incrementZoom(double deltaZoom) {
    if(window()) {
        window()->update();
    }
}

void MolecularDynamics::setThermostatValue(double arg)
{
    if (m_thermostatValue != arg) {
        m_thermostatValue = arg;
        emit thermostatValueChanged(arg);
        update();
    }
}

void MolecularDynamics::setThermostatEnabled(bool arg)
{
    if (m_thermostatEnabled != arg) {
        m_thermostatEnabled = arg;
        emit thermostatEnabledChanged(arg);
        update();
    }
}

void MolecularDynamics::setForceEnabled(bool arg)
{
    if (m_forceEnabled == arg)
        return;

    m_forceEnabled = arg;
    emit forceEnabledChanged(arg);
    update();
}

void MolecularDynamics::setForceValue(double arg)
{
    if (m_forceValue == arg)
        return;

    m_forceValue = arg;
    //    m_renderer->m_simulator.m_settings.gravity_force = m_forceValue*1e-3; // Nice scaling
    emit forceValueChanged(arg);
    update();
}

void MolecularDynamics::setSystemSize(QVector3D arg)
{
    if (m_systemSize == arg)
        return;

    m_systemSize = arg;
    m_systemSizeIsDirty = true;
    emit systemSizeChanged(arg);
    update();
}

void MolecularDynamics::setTilt(double arg)
{
    if (m_tilt == arg)
        return;

    m_tilt = arg;
    emit tiltChanged(arg);
    update();
}

void MolecularDynamics::setPan(double arg)
{
    if (m_pan == arg)
        return;

    m_pan = arg;
    emit panChanged(arg);
    update();
}

void MolecularDynamics::setRoll(double arg)
{
    if (m_roll == arg)
        return;

    m_roll = arg;
    emit rollChanged(arg);
    update();
}

void MolecularDynamics::setZoom(double arg)
{
    if (m_zoom == arg)
        return;

    m_zoom = arg;
    emit zoomChanged(arg);
    update();
}

void MolecularDynamics::setRunning(bool arg)
{
    if (m_running == arg)
        return;

    m_running = arg;
    emit runningChanged(arg);
    update();
}

Simulator *MolecularDynamics::simulator()
{
    return &(m_mdSimulator.m_simulator);
}

void MolecularDynamics::step()
{
    if(!m_running || m_simulatorOutputDirty) {
        return;
    }

    if(m_simulatorRunningMutex.tryLock()) {
        bool didLoadNewSystem = loadIfPlanned();

        if(!didLoadNewSystem) {
            if(thermostatEnabled()) {
                float temperatureKelvin = thermostatValue() + 273.15;
                double systemTemperature = m_mdSimulator.m_simulator.m_system.unit_converter->temperature_from_SI(temperatureKelvin);
                m_mdSimulator.m_simulator.m_thermostat->relaxation_time = 1;
                m_mdSimulator.m_simulator.m_thermostat->apply(m_mdSimulator.m_simulator.m_sampler, &(m_mdSimulator.m_simulator.m_system), systemTemperature, ARGON);
            }
            if(m_systemSizeIsDirty) {
                m_mdSimulator.m_simulator.m_system.setSystemSize(systemSize());
            }
        }

        double dt = m_timer.restart() / 1000.0;
        double safeDt = min(0.02, dt);
        m_mdSimulator.m_simulator.m_system.setDt(safeDt);
        emit requestStep();
    }

}

void MolecularDynamics::finalizeStep()
{
    m_simulatorOutputMutex.lock();
    setDidScaleVelocitiesDueToHighValues(m_mdSimulator.m_simulator.m_system.didScaleVelocitiesDueToHighValues());
    setAtomCount(m_mdSimulator.m_simulator.m_system.numAtoms());
    float temperatureCelsius = m_mdSimulator.m_simulator.m_system.unit_converter->temperature_to_SI(m_mdSimulator.m_simulator.m_sampler->temperature_free_atoms) - 273.15;
    setTemperature(temperatureCelsius);
    setKineticEnergy(m_mdSimulator.m_simulator.m_system.unit_converter->energy_to_ev(m_mdSimulator.m_simulator.m_sampler->kinetic_energy));
    setPotentialEnergy(m_mdSimulator.m_simulator.m_system.unit_converter->energy_to_ev(m_mdSimulator.m_simulator.m_sampler->potential_energy));
    setPressure(m_mdSimulator.m_simulator.m_system.unit_converter->pressure_to_SI(m_mdSimulator.m_simulator.m_sampler->pressure));
    setTime(m_mdSimulator.m_simulator.m_system.unit_converter->time_to_SI(m_mdSimulator.m_simulator.m_system.time()));
    m_positions = m_mdSimulator.m_simulator.m_system.positions;
    m_atomTypes = m_mdSimulator.m_simulator.m_system.atom_type;
    m_previousStepCompleted = true;
    m_simulatorOutputDirty = true;
    m_simulatorOutputMutex.unlock();
    m_simulatorRunningMutex.unlock();
    update();
}

void MolecularDynamics::save(QString fileName)
{
    qWarning() << "Save not yet implemented!";
}

bool MolecularDynamics::loadIfPlanned() {
    if(m_systemToLoad.size()) {
        m_mdSimulator.m_simulator.m_system.mdio->load_state_from_file_binary(m_systemToLoad);
        setAtomCount(m_mdSimulator.m_simulator.m_system.numAtoms());
        setSystemSize(m_mdSimulator.m_simulator.m_system.systemSize());
        emit systemSizeChanged(m_systemSize);
        m_systemToLoad = "";
        emit loaded();
        return true;
    }
    return false;
}

void MolecularDynamics::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        //        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
        //        connect(win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}

void MolecularDynamics::setAtomCount(int arg)
{
    if (m_atomCount == arg)
        return;

    m_atomCount = arg;
    emit atomCountChanged(arg);
}

// ********************************************
// ******* Basic setters and getters **********
// ********************************************

void MolecularDynamics::setPreviousStepCompleted(bool arg)
{
    if (m_previousStepCompleted != arg) {
        m_previousStepCompleted = arg;
        emit previousStepCompletedChanged(arg);
    }
}

void MolecularDynamics::setSimulatorOutputDirty(bool arg)
{
    m_simulatorOutputDirty = arg;
}

void MolecularDynamics::setDidScaleVelocitiesDueToHighValues(bool arg)
{
    if (m_didScaleVelocitiesDueToHighValues == arg)
        return;

    m_didScaleVelocitiesDueToHighValues = arg;
    emit didScaleVelocitiesDueToHighValuesChanged(arg);
}

void MolecularDynamics::setTemperature(double arg)
{
    if (m_temperature == arg)
        return;

    m_temperature = arg;
    emit temperatureChanged(arg);
}

void MolecularDynamics::setKineticEnergy(double arg)
{
    if (m_kineticEnergy == arg)
        return;

    m_kineticEnergy = arg;
    emit kineticEnergyChanged(arg);
}

void MolecularDynamics::setPotentialEnergy(double arg)
{
    if (m_potentialEnergy == arg)
        return;

    m_potentialEnergy = arg;
    emit potentialEnergyChanged(arg);
}

void MolecularDynamics::setPressure(double arg)
{
    if (m_pressure == arg)
        return;

    m_pressure = arg;
    emit pressureChanged(arg);
}

void MolecularDynamics::setTime(double arg)
{
    if (m_time != arg) {
        m_time = arg;
        emit timeChanged(arg);
    }
}

void MolecularDynamics::load(QString fileName)
{
    m_systemToLoad = fileName;
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

int MolecularDynamics::atomCount() const
{
    return m_atomCount;
}

bool MolecularDynamics::didScaleVelocitiesDueToHighValues() const
{
    return m_didScaleVelocitiesDueToHighValues;
}

double MolecularDynamics::temperature() const
{
    return m_temperature;
}

double MolecularDynamics::kineticEnergy() const
{
    return m_kineticEnergy;
}

double MolecularDynamics::potentialEnergy() const
{
    return m_potentialEnergy;
}

double MolecularDynamics::pressure() const
{
    return m_pressure;
}

double MolecularDynamics::time() const
{
    return m_time;
}

bool MolecularDynamics::previousStepCompleted() const
{
    return m_previousStepCompleted;
}

bool MolecularDynamics::simulatorOutputDirty() const
{
    return m_simulatorOutputDirty;
}
