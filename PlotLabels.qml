import QtQuick 2.0

Item {
    property real maximumValue: 100
    property real minimumValue: -100
    property string title: ""
    width: 100
    height: 62

    Text {
        anchors {
            left: parent.left
            top: parent.top
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: maximumValue.toFixed(0)
        color: "white"
    }

    Text {
        anchors {
            left: parent.left
            bottom: parent.bottom
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: minimumValue.toFixed(0)
        color: "white"
    }

    Text {
        anchors {
            right: parent.right
            top: parent.top
            margins: parent.height * 0.04
        }
        font.pixelSize: 12
        text: title
        color: "white"
    }
}
