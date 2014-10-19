import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import MolecularDynamics 1.0

Item {
    id: moleculesRoot
    width: 1280
    height: 800

    focus: true

    MolecularDynamics {
        id: molecularDynamics
        anchors.fill: parent

        thermostatEnabled: toolsView.thermostatEnabled
        thermostatValue: toolsView.thermostatValue

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
                molecularDynamics.incrementZoom(5 * deltaScale)
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
                    molecularDynamics.incrementRotation(deltaPan, deltaTilt, 0)
                    lastPosition = Qt.point(mouse.x, mouse.y)
                }

                onReleased: {
                    var currentTime = Date.now()
                    var timeDiff = (currentTime - pressedTime) / 1000
                    if(timeDiff < 0.6 && !movedTooFarToHide) {
                        toolsView.revealed = false
                    }
                }

                onWheel: {
                    molecularDynamics.incrementZoom(wheel.angleDelta.y / 360)
                }
            }
        }

        Timer {
            id: timer
            property real lastTime: Date.now()
            running: toolsView.running
            repeat: true
            interval: 1
            onTriggered: {
                var currentTime = Date.now()
                var dt = currentTime - lastTime
                dt /= 1000
                molecularDynamics.step(dt)
                lastTime = currentTime
            }
        }
    }

    ToolsView {
        id: toolsView
    }

    SimulationsView {
        id: systemsView
        onLoadSimulation: {
            revealed = false
            molecularDynamics.load(fileName)
        }
    }

    Keys.onPressed: {
        if(event.modifiers & Qt.ControlModifier && event.key === Qt.Key_S) {
            molecularDynamics.save()
        }
    }
}
