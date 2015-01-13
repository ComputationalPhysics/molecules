import QtQuick 2.0
import "style"

Item {
    id: buttonRoot
    signal clicked
    property alias text: buttonText.text
    property real maximumRotation: Math.random() * 5 + 5

    smooth: true

    transform: Rotation { id: rotationTransform
        origin.x: 30
        origin.y: 30
        axis {
            x: 0
            y: 1
            z: 0
        }
        angle: 0
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0.2, 0.2, 0.2, 0.9)
        antialiasing: true
    }

    Text {
        id: buttonText
        anchors {
            left: parent.left
            leftMargin: Style.touchableSize
            verticalCenter: parent.verticalCenter
        }

        text: "Begin"
        font.pixelSize: 40
        font.weight: Font.Light
        renderType: Text.QtRendering
        color: "#dedede"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            buttonRoot.clicked()
        }
    }

//    ParallelAnimation {
//        loops: Animation.Infinite
//        running: true
//        SequentialAnimation {
//            NumberAnimation {
//                id: forwardAnimation
//                target: rotationTransform
//                property: "angle"
//                duration: Math.random() * 6000 + 8000
//                easing.type: Easing.InOutQuad
//                from: -buttonRoot.maximumRotation
//                to: buttonRoot.maximumRotation
//            }
//            NumberAnimation {
//                id: backAnimation
//                target: rotationTransform
//                property: "angle"
//                duration: Math.random() * 6000 + 6000
//                easing.type: Easing.InOutQuad
//                from: buttonRoot.maximumRotation
//                to: -buttonRoot.maximumRotation
//            }
//        }
//        SequentialAnimation {
//            NumberAnimation {
//                target: rotationTransform
//                property: "axis.x"
//                duration: Math.random() * 6000 + 6000
//                easing.type: Easing.InOutQuad
//                from: 0
//                to: 1
//            }
//            NumberAnimation {
//                target: rotationTransform
//                property: "axis.x"
//                duration: Math.random() * 6000 + 8000
//                easing.type: Easing.InOutQuad
//                from: 1
//                to: 0
//            }
//        }
//    }
}
