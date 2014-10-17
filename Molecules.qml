import QtQuick 2.0
import MolecularDynamics 1.0

Item {
    width: 640
    height: 480

    MolecularDynamics {
        anchors.fill: parent
    }

    Rectangle {
        width: 100
        height: 200
        color: "blue"
        opacity: 0.5
    }
}
