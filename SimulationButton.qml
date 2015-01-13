import QtQuick 2.0
import Qt.labs.settings 1.0
import "style"

Item {
    id: buttonRoot
    property Simulation simulation: simulationLoader.item
//    property string simulationSource
    property string folder
    signal loadSimulation(var fileName)

    Loader {
        id: simulationLoader

        function identifier(folder) {
            var names = folder.split("/")
            if(folder === "") {
                return ""
            }

            return names[names.length - 1]
        }

        source: folder + "/" + identifier(folder) + ".qml"
        onLoaded: {
            item.folder = buttonRoot.folder
        }
    }

    Text {
        id: nameText
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: Style.touchableSize
        }
        text: simulation ? simulation.name : ""
        color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
        font.pixelSize: parent.width * 0.05
        font.weight: Font.Light

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
    }


    Rectangle {
        id: container
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            margins: buttonRoot.height * 0.05
        }
        height: parent.height * 0.45
        width: parent.width * 0.45

        color: "black"
        border.color: "#ababab"
        border.width: 1.0

        Image {
            anchors {
                fill: parent
                margins: buttonRoot.width * 0.05
            }
            fillMode: Image.PreserveAspectFit
            source: simulation.screenshotSource
        }
    }

    Text {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
        }
        width: parent.width * 0.45
        color: "#dedede"
        font.pixelSize: parent.width * 0.03

        text: simulation.description
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            buttonRoot.loadSimulation(simulation)
        }
    }
}
