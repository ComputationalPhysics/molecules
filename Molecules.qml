import QtQuick 2.0
import QtQuick.Controls 1.2
import MolecularDynamics 1.0

Item {
    id: moleculesRoot
    width: 1280
    height: 800

    MolecularDynamics {
        id: molecularDynamics
        anchors.fill: parent

        thermostatEnabled: thermostatCheckBox.checked
        thermostatValue: thermostatSlider.value

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
                onPressed: {
                    lastPosition = Qt.point(mouse.x, mouse.y)
                }

                onPositionChanged: {
                    var deltaX = mouse.x - lastPosition.x
                    var deltaY = mouse.y - lastPosition.y
                    var deltaPan = deltaX / width * 360 // max 3 rounds
                    var deltaTilt = deltaY / height * 180 // max 0.5 round
                    molecularDynamics.incrementRotation(deltaPan, deltaTilt, 0)
                    lastPosition = Qt.point(mouse.x, mouse.y)
                }

                onWheel: {
                    molecularDynamics.incrementZoom(wheel.angleDelta.y / 720)
                }
            }
        }

        Timer {
            id: timer
            property real lastTime: Date.now()
            running: true
            repeat: true
            interval: 10
            onTriggered: {
                var currentTime = Date.now()
                var dt = currentTime - lastTime
                dt /= 1000
                molecularDynamics.step(dt)
                lastTime = currentTime
            }
        }
    }

    Rectangle {
        id: settings

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        width: parent.width * 0.1
        color: Qt.rgba(239 / 255, 243 / 255, 255 / 255, 200 / 255)

        MouseArea {
            property bool isLarge
            anchors.fill: parent
            onPressed: {
                // It seems like the width and height doesn't switch places in portrait/landscape
                if(mouse.y > width*0.2) {
                    if(isLarge) {
                        settings.width = moleculesRoot.width*0.1
                    } else {
                        settings.width = moleculesRoot.width*0.5
                    }

                    isLarge = !isLarge
                }
            }
        }

        Column {
            anchors {
                fill: parent
                margins: moleculesRoot.width * 0.01
            }
            CheckBox {
                id: thermostatCheckBox
                text: "Thermostat: " + thermostatSlider.value.toFixed(1) + " K"
            }

            Slider {
                id: thermostatSlider
                width: parent.width
                minimumValue: 1
                maximumValue: 500
            }
        }
    }
}
