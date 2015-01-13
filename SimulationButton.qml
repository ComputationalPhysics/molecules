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
            topMargin: Style.baseMargin
        }
        text: simulation ? simulation.name : ""
        color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
        font.pixelSize: parent.width * 0.05
        font.weight: Font.Light

        renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering


        MouseArea {
            anchors.fill: parent
            onClicked: {
                buttonRoot.loadSimulation(simulation)
            }
        }
    }

    Item {
        id: container
        anchors {
            top: nameText.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: parent.width * 0.05
            topMargin: parent.width * 0.03
            bottomMargin: parent.width * 0.05
            rightMargin: parent.width * 0.05
        }

        Rectangle {
            id: imageContainer
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: parent.height * 0.7
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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    buttonRoot.loadSimulation(simulation)
                }
            }

//            Rectangle {
//                anchors {
//                    left: parent.left
//                    bottom: parent.bottom
//                    right: parent.right
//                    top: startText.top
//                }
//                color: "black"
//                border.color: "#ababab"
//                border.width: 1.0
//            }

            Text {
                id: startText
                anchors {
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                    bottomMargin: parent.height * 0.05
                }
                text: "Start simulation"
//                font.capitalization: Font.AllUppercase
                font.pixelSize: buttonRoot.width * 0.02
                color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
            }
        }

        Flickable {
            id: descriptionFlickable
            clip: true
            width: parent.width * 0.45
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            boundsBehavior: Flickable.StopAtBounds

            contentWidth: descriptionText.width
            contentHeight: descriptionText.height

            Text {
                id: descriptionText
                width: descriptionFlickable.width
                color: "#dedede"
                font.pixelSize: buttonRoot.width * 0.03
                font.weight: Font.Light
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                text: simulation.description
            }
        }
    }
}
