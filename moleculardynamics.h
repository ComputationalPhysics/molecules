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

#ifndef SQUIRCLE_H
#define SQUIRCLE_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include "cpglquads.h"
#include "cpglcube.h"
#include "simulator.h"

//! [1]
class MolecularDynamicsRenderer : public QObject {
    Q_OBJECT
public:
    Simulator m_simulator;
    MolecularDynamicsRenderer();
    ~MolecularDynamicsRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void resetProjection();
//    void incrementRotation(double deltaPan, double deltaTilt, double deltaRoll);
//    void incrementZoom(double deltaZoom);

//    double zoom() const;
    void setModelViewMatrices(double zoom, double tilt, double pan, double roll);

//    double pinchScale() const;
//    void setPinchScale(double pinchScale);

public slots:
    void paint();

private:
    QSize m_viewportSize;
//    double m_tilt;
//    double m_pan;
//    double m_roll;
//    double m_zoom;

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_modelViewMatrix;
    QMatrix4x4 m_lightModelViewMatrix;
    CPGLQuads *m_glQuads;
    CPGLCube *m_glCube;
};
//! [1]

//! [2]
class MolecularDynamics : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(double thermostatValue READ thermostatValue WRITE setThermostatValue NOTIFY thermostatValueChanged)
    Q_PROPERTY(bool thermostatEnabled READ thermostatEnabled WRITE setThermostatEnabled NOTIFY thermostatEnabledChanged)
    Q_PROPERTY(bool forceEnabled READ forceEnabled WRITE setForceEnabled NOTIFY forceEnabledChanged)
    Q_PROPERTY(double forceValue READ forceValue WRITE setForceValue NOTIFY forceValueChanged)
    Q_PROPERTY(QVector3D systemSize READ systemSize WRITE setSystemSize NOTIFY systemSizeChanged)
    Q_PROPERTY(double tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(double pan READ pan WRITE setPan NOTIFY panChanged)
    Q_PROPERTY(double roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int atomCount READ atomCount NOTIFY atomCountChanged)
    Q_PROPERTY(bool didScaleVelocitiesDueToHighValues READ didScaleVelocitiesDueToHighValues NOTIFY didScaleVelocitiesDueToHighValuesChanged)
    Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(double kineticEnergy READ kineticEnergy NOTIFY kineticEnergyChanged)
    Q_PROPERTY(double potentialEnergy READ potentialEnergy NOTIFY potentialEnergyChanged)
    Q_PROPERTY(double pressure READ pressure NOTIFY pressureChanged)
    Q_PROPERTY(double time READ time NOTIFY timeChanged)

public:
    MolecularDynamics();
    Q_INVOKABLE void step(double dt);
    Q_INVOKABLE void save(QString fileName);
    Q_INVOKABLE void load(QString fileName);

    double thermostatValue() const;
    bool thermostatEnabled() const;
    bool forceEnabled() const;
    double forceValue() const;
    QVector3D systemSize() const;

    double tilt() const;
    double pan() const;
    double roll() const;
    double zoom() const;

    bool running() const;

    int atomCount() const
    {
        return m_atomCount;
    }

    bool didScaleVelocitiesDueToHighValues() const
    {
        return m_didScaleVelocitiesDueToHighValues;
    }

    double temperature() const
    {
        return m_temperature;
    }

    double kineticEnergy() const
    {
        return m_kineticEnergy;
    }

    double potentialEnergy() const
    {
        return m_potentialEnergy;
    }

    double pressure() const
    {
        return m_pressure;
    }

    double time() const
    {
        return m_time;
    }

public slots:
    void sync();
    void cleanup();
    void incrementRotation(double deltaPan, double deltaTilt, double deltaRoll);
    void incrementZoom(double deltaZoom);

    void setThermostatValue(double arg);
    void setThermostatEnabled(bool arg);
    void setForceEnabled(bool arg);
    void setForceValue(double arg);
    void setSystemSize(QVector3D arg);

    void setTilt(double arg);
    void setPan(double arg);
    void setRoll(double arg);
    void setZoom(double arg);

    void setRunning(bool arg);

private slots:

    void setDidScaleVelocitiesDueToHighValues(bool arg)
    {
        if (m_didScaleVelocitiesDueToHighValues == arg)
            return;

        m_didScaleVelocitiesDueToHighValues = arg;
        emit didScaleVelocitiesDueToHighValuesChanged(arg);
    }

    void setTemperature(double arg)
    {
        if (m_temperature == arg)
            return;

        m_temperature = arg;
        emit temperatureChanged(arg);
    }

    void setKineticEnergy(double arg)
    {
        if (m_kineticEnergy == arg)
            return;

        m_kineticEnergy = arg;
        emit kineticEnergyChanged(arg);
    }

    void setPotentialEnergy(double arg)
    {
        if (m_potentialEnergy == arg)
            return;

        m_potentialEnergy = arg;
        emit potentialEnergyChanged(arg);
    }

    void setPressure(double arg)
    {
        if (m_pressure == arg)
            return;

        m_pressure = arg;
        emit pressureChanged(arg);
    }

    void setTime(double arg)
    {
        if (m_time != arg) {
            m_time = arg;
            emit timeChanged(arg);
        }
    }

signals:
    void thermostatValueChanged(double arg);
    void thermostatEnabledChanged(bool arg);
    void forceEnabledChanged(bool arg);
    void forceValueChanged(double arg);
    void systemSizeChanged(QVector3D arg);

    void tiltChanged(double arg);
    void panChanged(double arg);
    void rollChanged(double arg);
    void zoomChanged(double arg);

    void runningChanged(bool arg);

    void atomCountChanged(int arg);

    void didScaleVelocitiesDueToHighValuesChanged(bool arg);

    void temperatureChanged(double arg);

    void kineticEnergyChanged(double arg);

    void potentialEnergyChanged(double arg);

    void pressureChanged(double arg);

    void timeChanged(double arg);

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    MolecularDynamicsRenderer *m_renderer;
    double m_xPoint;
    double m_yPoint;

    double m_thermostatValue;
    bool m_thermostatEnabled;
    bool m_forceEnabled;
    double m_forceValue;
    QVector3D m_systemSize;
    double m_tilt;
    double m_pan;
    double m_roll;
    double m_zoom;
    QElapsedTimer m_timer;
    bool m_running;
    int m_atomCount;

    void setAtomCount(int arg)
    {
        if (m_atomCount == arg)
            return;

        m_atomCount = arg;
        emit atomCountChanged(arg);
    }
    bool m_didScaleVelocitiesDueToHighValues;
    double m_temperature;
    double m_kineticEnergy;
    double m_potentialEnergy;
    double m_pressure;
    double m_time;
};
//! [2]

#endif // SQUIRCLE_H
