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

        MouseArea {
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
                molecularDynamics.incrementRotation(deltaPan, deltaTilt)
                lastPosition = Qt.point(mouse.x, mouse.y)
            }

            onWheel: {
                molecularDynamics.incrementZoom(wheel.angleDelta.y / 720)
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
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width * 0.2
        color: Qt.rgba(239 / 255, 243 / 255, 255 / 255, 200 / 255)

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
