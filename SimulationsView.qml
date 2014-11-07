import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

import "style"

Item {
    id: systemsViewRoot
    property bool revealed: true
    signal loadSimulation(var simulation)
    anchors.fill: parent

    property list<Simulation> simulations: [
        Simulation{ name: "Chamber"; stateFile: "simulations/chamber.lmp"; imageSource: "simulations/chamber.png"},
        Simulation{ name: "Crystal"; stateFile: "simulations/crystal.lmp"; imageSource: "simulations/crystal.png"},
        Simulation{ name: "Diffusion"; stateFile: "simulations/diffusion.lmp"; imageSource: "simulations/diffusion.png"; zoom: -30},
        Simulation{ name: "Fracture"; stateFile: "simulations/fracture.lmp"; imageSource: "simulations/fracture.png"},
        Simulation{ name: "Wall crash"; stateFile: "simulations/wallcrash.lmp"; imageSource: "simulations/wallcrash.png"},
        Simulation{ name: "Bullets"; stateFile: "simulations/bullets.lmp"; imageSource: "simulations/bullets.png"; zoom: -55; pan: -80; tilt: -15}
    ]

    state: revealed ? "revealed" : "hidden"

    function loadFirstSimulation() {
        loadSimulation(simulations[0])
    }

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
            fill: parent
            margins: Math.min(parent.width, parent.height) * 0.1
        }

        border.width: 1.0
        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)
        color: Qt.rgba(0.05, 0.05, 0.05, 0.8)

        MouseArea {
            // Dummy mouse area to avoid interacting with underlying simulation
            enabled: systemsViewRoot.revealed
            anchors.fill: parent
        }


        Image {
            id: exitButton
            anchors {
                top: parent.top
                right: parent.right
                margins: parent.width*0.01
            }

            width: Style.touchableSize
            height: width
            source: "images/exit.png"

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    simulationsView.revealed = false
                }
            }
        }

        ColumnLayout {
            id: systemsViewColumn
            anchors {
                fill: parent
                margins: systemsViewRoot.width * 0.05
            }

            spacing: systemsViewRectangle.width * 0.02

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Welcome to Atomify"
                font.pixelSize: parent.width * 0.04
                color: "white"

                renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
            }

            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                text: "Please select a simulation."
                font.pixelSize: parent.width * 0.025
                color: "white"

                renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
            }

//            Item {
//                Layout.fillHeight: true
//                Layout.fillWidth: true
//            }


            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true


                Grid {
                    id: systemsGrid
                    anchors.centerIn: parent
                    property real targetAspectRatio: 16*3 / (2*9)
                    property real availableAspectRatio: parent.width / parent.height
                    height: availableAspectRatio > targetAspectRatio ? parent.height : parent.width / targetAspectRatio
                    width: availableAspectRatio > targetAspectRatio ? parent.height * targetAspectRatio : parent.width
                    Layout.fillWidth: true

                    property real elementWidth: systemsGrid.width / columns - (columns-1)/columns*spacing
                    // property real elementHeight: systemsGrid.height / rows - (rows-1)/rows*spacing
                    property real elementHeight: elementWidth*9/16

                    columns: 3
                    rows: 2
                    spacing: systemsViewRectangle.width * 0.01

                    Component.onCompleted: {
                        for(var i in simulations) {
                            var simulation = simulations[i]
                            var component = Qt.createComponent("SimulationButton.qml")
                            var properties = {
                                width: Qt.binding(function() {return systemsGrid.elementWidth}),
                                height: Qt.binding(function() {return systemsGrid.elementHeight}),
                                simulation: simulation
                            }
                            var button = component.createObject(systemsGrid, properties)
                            button.loadSimulation.connect(systemsViewRoot.loadSimulation)
                        }
                    }
                }
            }
        }
    }

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
}
