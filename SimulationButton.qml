import QtQuick 2.0

Item {
    id: buttonRoot
    property string identifier: ""
    property string name: ""
    signal loadSimulation(var fileName)

//    width: 200
//    height: 100

    Rectangle {
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
            source: buttonRoot.identifier !== "" ? "simulations/" + buttonRoot.identifier + ".png" : ""
        }
    }
    Text {
        id: nameText
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }
        text: buttonRoot.name
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(buttonRoot.identifier === "") {
                return
            }

            loadSimulation(":/simulations/" + buttonRoot.identifier + ".lmp")
        }
    }
}
