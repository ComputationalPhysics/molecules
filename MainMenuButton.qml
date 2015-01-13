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
        angle: 72
    }

    Rectangle {
        anchors.fill: parent
        color: "#252525"
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

    SequentialAnimation {
        loops: Animation.Infinite
        running: true
        NumberAnimation {
            id: forwardAnimation
            target: rotationTransform
            property: "angle"
            duration: Math.random() * 2000 + 8000
            easing.type: Easing.InOutQuad
            from: 0
            to: buttonRoot.maximumRotation
        }
        NumberAnimation {
            id: backAnimation
            target: rotationTransform
            property: "angle"
            duration: Math.random() * 2000 + 12000
            easing.type: Easing.InOutQuad
            from: buttonRoot.maximumRotation
            to: 0
        }
    }
}
