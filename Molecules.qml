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
            id: mousey
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

            onPressAndHold: {
                pinchy.enabled = true
            }
        }

        PinchArea {
            id: pinchy
            anchors.fill: parent
            property double xPoint
            property double yPoint
            property double pinchScale
            property double pinchRotation
            property point lastPosition
            property double lastRoll
            enabled: false

            onPinchUpdated: {
                molecularDynamics.xPoint = pinch.center.x
                molecularDynamics.yPoint = pinch.center.y
                molecularDynamics.pinchRotation = pinch.rotation
                molecularDynamics.pinchScale = pinch.scale

                var deltaX = pinch.center.x - lastPosition.x
                var deltaY = pinch.center.y - lastPosition.y
                var deltaPan = deltaX / width * 360 // max 3 rounds
                var deltaTilt = deltaY / height * 180 // max 0.5 round
                var deltaRoll = -(pinch.rotation - lastRoll)
                deltaPan = 0 // Disable rotation on pinch
                deltaTilt = 0

                molecularDynamics.incrementRotation(deltaPan, deltaTilt, deltaRoll)
                // console.log("deltaPan" + deltaPan)
                // console.log("deltaTilt" + deltaTilt)

                lastPosition = Qt.point(pinch.center.x, pinch.center.y)
                lastRoll = pinch.rotation
            }

            onPinchFinished: {
                molecularDynamics.onPinchedFinished();
                pinchy.enabled = false
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
