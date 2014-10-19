import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: systemsViewRoot
    property bool revealed: true
    anchors.fill: parent

    state: revealed ? "revealed" : "hidden"

    states: [
        State {
            name: "revealed"
            PropertyChanges {
                target: systemsViewRectangle
                opacity: 1.0
                scale: 1.0
            }
            PropertyChanges {
                target: revealToogleButton
                opacity: 1.0
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: systemsViewRectangle
                opacity: 0.0
                scale: 0.85
            }
            PropertyChanges {
                target: revealToogleButton
                opacity: 1.0
            }
        }
    ]

    transitions: [
        Transition {
            from: "revealed"
            to: "hidden"
            ParallelAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 250
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 250
                    easing.type: Easing.InQuad
                }
            }
        },
        Transition {
            from: "hidden"
            to: "revealed"
            ParallelAnimation {
                NumberAnimation {
                    properties: "opacity"
                    duration: 250
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 100
                    easing.type: Easing.InQuad
                }
            }
        }

    ]

    Item {
        id: revealToogleButton
        anchors {
            top: parent.top
            left: parent.left
        }
        height: parent.height * 0.1
        width: height

        Image {
            anchors {
                fill: parent
                margins: parent.width * 0.2
            }
            source: "images/systems.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                systemsViewRoot.revealed = true
            }
        }
    }

    Rectangle {
        id: systemsViewRectangle
        anchors {
            centerIn: parent
        }
        height: parent.height * 0.8
        width: parent.width * 0.7

        border.width: 1.0
        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)
        color: Qt.rgba(0.05, 0.05, 0.05, 0.8)

        ColumnLayout {
            anchors {
                fill: parent
                margins: parent.width * 0.07
            }
            spacing: systemsViewRectangle.width * 0.03

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Welcome to Molecules!"
                font.pixelSize: parent.width * 0.05
                color: "white"
            }

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Please select a simulation."
                font.pixelSize: parent.width * 0.025
                color: "white"
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Grid {
                id: systemsGrid

                property real elementWidth: width / 4 - spacing
                property real elementHeight: elementWidth * 9.0 / 16.0

                Layout.fillWidth: true
                spacing: systemsViewRectangle.width * 0.01

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }

                Rectangle {
                    width: systemsGrid.elementWidth
                    height: systemsGrid.elementHeight
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
        MouseArea {
            // Dummy mouse area to avoid interacting with underlying simulation
            enabled: systemsViewRoot.revealed
            anchors.fill: parent
        }
    }
}
