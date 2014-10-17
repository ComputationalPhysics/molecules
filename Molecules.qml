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
                molecularDynamics.t = mouse.x
            }
        }
    }
}
