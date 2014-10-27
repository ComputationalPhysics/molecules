import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: systemsViewRoot
    property bool revealed: true
    signal loadSimulation(var fileName)
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
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: systemsViewRectangle
                opacity: 0.0
                scale: 0.85
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
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 250
                    easing.type: Easing.InOutQuad
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
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    properties: "scale"
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }
        }

    ]

    MouseArea {
        enabled: systemsViewRoot.revealed
        anchors.fill: parent
        onPressed: {
            systemsViewRoot.revealed = false
        }
    }

    Rectangle {
        id: systemsViewRectangle
        enabled: revealed // Keeps button presses from being caught by internal MouseAreas
        anchors {
            centerIn: parent
        }
        height: parent.height * 0.8
        width: parent.width * 0.7

        border.width: 1.0
        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)
        color: Qt.rgba(0.05, 0.05, 0.05, 0.8)

        MouseArea {
            // Dummy mouse area to avoid interacting with underlying simulation
            enabled: systemsViewRoot.revealed
            anchors.fill: parent
        }

        Flickable {
            anchors {
                fill: parent
                margins: parent.width * 0.07
            }
            clip: true
            contentHeight: systemsViewColumn.height
            ColumnLayout {
                id: systemsViewColumn
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                spacing: systemsViewRectangle.width * 0.02

                Text {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Welcome to Molecules!"
                    font.pixelSize: parent.width * 0.04
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

                Grid {
                    id: systemsGrid

                    property real elementWidth: systemsViewColumn.width / columns - spacing
                    property real elementHeight: elementWidth * 9.0 / 16.0
                    property var simulations: [
                        { identifier: "default", name: "Default"},
                        { identifier: "diffusion", name: "Diffusion"},
                        { identifier: "chamber", name: "Chamber"},
                        { identifier: "cylinder", name: "Cylinder"},
                        { identifier: "fracture", name: "Fracture"},
                        { identifier: "wallcrash", name: "Wall crash"},
                        { identifier: "bullets", name: "Bullets"},
                    ]

                    columns: 3
                    spacing: systemsViewRectangle.width * 0.01

                    Component.onCompleted: {
                        for(var i in simulations) {
                            var simulation = simulations[i]
                            var component = Qt.createComponent("SimulationButton.qml")
                            var properties = {
                                width: Qt.binding(function() {return systemsGrid.elementWidth}),
                                height: Qt.binding(function() {return systemsGrid.elementHeight}),
                                identifier: simulation.identifier,
                                name: simulation.name
                            }
                            var button = component.createObject(systemsGrid, properties)
                            button.loadSimulation.connect(systemsViewRoot.loadSimulation)
                        }
                    }
                }
            }
        }
    }
}
