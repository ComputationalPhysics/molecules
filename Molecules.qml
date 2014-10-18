import QtQuick 2.0
import MolecularDynamics 1.0

Item {
    width: 640
    height: 480

    MolecularDynamics {
        id: molecularDynamics
        anchors.fill: parent

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
}
