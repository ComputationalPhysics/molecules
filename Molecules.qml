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
            running: true
            repeat: true
            interval: 1
            onTriggered: {
                molecularDynamics.step()
            }
        }
    }
}
