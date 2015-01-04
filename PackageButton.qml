import QtQuick 2.0

Item {
    id: buttonRoot

    property string name
    property string imageSource

    Rectangle {
        id: container
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: nameText.top
            margins: buttonRoot.height * 0.05
        }

        color: "black"
        border.color: "darkgrey"
        border.width: 1.0

        Image {
            anchors {
                fill: parent
                margins: buttonRoot.width * 0.05
            }
            fillMode: Image.PreserveAspectFit
            source: imageSource ? imageSource : ""
        }
    }

    Text {
        id: nameText
        anchors {
            horizontalCenter: parent.horizontalCenter
            margins: buttonRoot.width * 0.02
            bottom: parent.bottom
        }
        text: name ? name : ""
        color: "white"
        font.pixelSize: buttonRoot.height * 0.1

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }
}
