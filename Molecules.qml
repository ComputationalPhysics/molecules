import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import MolecularDynamics 1.0
import "style" 1.0
//import MoleculesStyle 1.0

Item {
    id: moleculesRoot
    width: 1280
    height: 800

    focus: true

    function resetStyle() {
        Style.windowWidth = width
        Style.windowHeight = height
        console.log("Styled: " + Style.windowHeight)
    }

    onWidthChanged: {
        resetStyle()
    }
    onHeightChanged: {
        resetStyle()
    }
    Component.onCompleted: {
        resetStyle()
    }

    MolecularDynamics {
        id: molecularDynamics
        anchors.fill: parent
        property bool zoomSetByLoad: false
        property bool isSettingSystemSize: false
        property real volume: systemSize.x*systemSize.y*systemSize.z
        property real density: atomCount / volume
        property real totalEnergy: kineticEnergy + potentialEnergy

        thermostatEnabled: dashboard.thermostatEnabled
        thermostatValue: dashboard.thermostatValue

        forceEnabled: dashboard.forceEnabled
        forceValue: dashboard.forceValue

        pan: 30
        tilt: 30
        zoom: -40

        running: dashboard.running

        Behavior on zoom {
            enabled: molecularDynamics.zoomSetByLoad
            NumberAnimation {
                duration: 400
                easing.type: Easing.InOutQuad
                onRunningChanged: {
                    if(!running) {
                        molecularDynamics.zoomSetByLoad = false
                    }
                }
            }
        }

        onAtomCountChanged: {
            dashboard.minimumSystemSizeSliderValue = Math.pow(atomCount, 1.0/3)
        }

        onSystemSizeChanged: {
            if(dashboard.isSettingSystemSize) {
                return
            }

            isSettingSystemSize = true
            dashboard.systemSizeX = systemSize.x
            dashboard.systemSizeY = systemSize.y
            dashboard.systemSizeZ = systemSize.z
            isSettingSystemSize = false
        }

        onDidScaleVelocitiesDueToHighValuesChanged: {
//            dashboard.thermostatValue = 1000
            dashboard.thermostatEnabled = true
        }

        PinchArea {
            id: pinchArea
            anchors.fill: parent
            enabled: true

            property double previousScale: 1.0

            onPinchStarted: {
                previousScale = (pinch.scale - 1.0)
            }

            onPinchUpdated: {
                var correctScale = pinch.scale >= 1.0 ? (pinch.scale - 1.0) : (-1.0 / pinch.scale + 1.0)
                var deltaScale = correctScale - previousScale
                molecularDynamics.zoomSetByLoad = false
                molecularDynamics.zoom += 10 * deltaScale
                previousScale = correctScale
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                property point lastPosition
                property point pressedPosition
                property real pressedTime
                property bool movedTooFarToHide: false

                onPressed: {
                    systemsView.revealed = false
                    dashboard.revealed = false
                    lastPosition = Qt.point(mouse.x, mouse.y)
                    pressedPosition = Qt.point(mouse.x, mouse.y)
                    pressedTime = Date.now()
                    movedTooFarToHide = false
                }

                onPositionChanged: {
                    var pressedDeltaX = mouse.x - pressedPosition.x
                    var pressedDeltaY = mouse.y - pressedPosition.y
                    if(pressedDeltaX*pressedDeltaX + pressedDeltaY*pressedDeltaY > moleculesRoot.width * 0.1) {
                        movedTooFarToHide = true
                    }

                    var deltaX = mouse.x - lastPosition.x
                    var deltaY = mouse.y - lastPosition.y
                    var deltaPan = deltaX / width * 360 // max 3 rounds
                    var deltaTilt = deltaY / height * 180 // max 0.5 round
//                    molecularDynamics.incrementRotation(deltaPan, deltaTilt, 0)
                    molecularDynamics.pan += deltaPan
                    molecularDynamics.tilt += deltaTilt
                    molecularDynamics.tilt = Math.max(-90, Math.min(90, molecularDynamics.tilt))
                    lastPosition = Qt.point(mouse.x, mouse.y)
                }

                onWheel: {
//                    molecularDynamics.incrementZoom(wheel.angleDelta.y / 360)
                    molecularDynamics.zoomSetByLoad = false
                    molecularDynamics.zoom += wheel.angleDelta.y / 180
                }
            }
        }

        Timer {
            id: timer
            property real lastTime: Date.now()
            property real lastSampleTime: Date.now()
            running: true
            repeat: true
            interval: 1
            onTriggered: {
                var currentTime = Date.now()
                var dt = currentTime - lastTime
                dt /= 1000
                molecularDynamics.step(dt)

                var sampleTimeDifference = (currentTime - lastSampleTime)/1000
                if(sampleTimeDifference > 0.1) {
                    lastSampleTime = currentTime
                    dashboard.addTemperature(molecularDynamics.temperature)
                    dashboard.addKineticEnergy(molecularDynamics.kineticEnergy / molecularDynamics.atomCount)
                    dashboard.addPotentialEnergy(molecularDynamics.potentialEnergy / molecularDynamics.atomCount)
                    dashboard.addTotalEnergy(molecularDynamics.kineticEnergy / molecularDynamics.atomCount
                                             + molecularDynamics.potentialEnergy / molecularDynamics.atomCount)
                    dashboard.addPressure(molecularDynamics.pressure * 1e-6)
                }
                lastTime = currentTime
            }
        }
    }

    Dashboard {
        id: dashboard
        property bool isSettingSystemSize: false

        temperature: molecularDynamics.temperature
        pressure: molecularDynamics.pressure
        kineticEnergy: molecularDynamics.kineticEnergy
        potentialEnergy: molecularDynamics.potentialEnergy
        totalEnergy: molecularDynamics.totalEnergy
        time: molecularDynamics.time

        function updateSystemSize() {
            if(molecularDynamics.isSettingSystemSize) {
                return
            }
            isSettingSystemSize = true
            molecularDynamics.systemSize = Qt.vector3d(systemSizeX, systemSizeY, systemSizeZ)
            isSettingSystemSize = false
        }

        onSystemSizeXChanged: {
            updateSystemSize()
        }

        onSystemSizeYChanged: {
            updateSystemSize()
        }

        onSystemSizeZChanged: {
            updateSystemSize()
        }
    }

    SimulationsView {
        id: systemsView
        onLoadSimulation: {
            revealed = false
            molecularDynamics.load(fileName)
            var systemSizeMax = Math.max(molecularDynamics.systemSize.x, molecularDynamics.systemSize.y, molecularDynamics.systemSize.z)
            molecularDynamics.zoomSetByLoad = true
            molecularDynamics.zoom = -10 - systemSizeMax
            dashboard.resetControls()
        }
    }

    Keys.onPressed: {
        if(event.modifiers & Qt.ControlModifier && event.key === Qt.Key_S) {
            var saveFileName = "state-"+Date.now()+".lmp"
            console.log("Saving state to " + saveFileName)
            molecularDynamics.save("state-"+Date.now()+".lmp")
        }
    }
}
