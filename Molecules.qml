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
            onPositionChanged: {
                molecularDynamics.mouseX = mouse.x
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
