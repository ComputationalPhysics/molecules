import QtQuick 2.4
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
        Simulation{ name: "Diffusion"; stateFile: "simulations/diffusion.lmp"; imageSource: "simulations/diffusion.png"; zoom: -18.33333333333339; pan: -0.875; tilt: -25.666666666666618},
        Simulation{ name: "Fracture"; stateFile: "simulations/fracture.lmp"; imageSource: "simulations/fracture.png"},
        Simulation{ name: "Wall crash"; stateFile: "simulations/wallcrash.lmp"; imageSource: "simulations/wallcrash.png"},
        Simulation{ name: "Bullets"; stateFile: "simulations/bullets.lmp"; imageSource: "simulations/bullets.png"; zoom: -55; pan: -80; tilt: -15}
    ]

    Component.onCompleted: {
        //        systemsGrid.setup()
    }

    onSimulationsChanged: {
        systemsGrid.setup()
    }

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

        StackView {
            id: stackView
            anchors.fill: parent
            initialItem: mainMenu
            delegate: StackViewDelegate {
                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "x"
                        from: enterItem.width
                        to: 0
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "x"
                        from: 0
                        to: -exitItem.width
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                }
                popTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "x"
                        from: -enterItem.width
                        to: 0
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "x"
                        from: 0
                        to: exitItem.width
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }

        Image {
            id: backButton
            anchors {
                top: parent.top
                left: parent.left
                margins: parent.width*0.01
            }

            width: Style.touchableSize
            height: width
            source: "images/back.png"
            enabled: stackView.depth > 1
            opacity: stackView.depth > 1

            Behavior on opacity {
                NumberAnimation {
                    duration: 1000
                    easing.type: Easing.InOutQuad
                }
            }

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    if(stackView.depth > 1) {
                        stackView.pop()
                    } else {
                        simulationsView.revealed = false
                    }
                }
            }
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
    }

    Component {
        id: mainMenu

        Item {
            id: mainMenuRoot
            Column {
                id: systemsViewColumn
                anchors {
                    fill: parent
                    topMargin: systemsViewRoot.width * 0.02
                    bottomMargin: anchors.topMargin
                    leftMargin: systemsViewRoot.width * 0.05
                    rightMargin: anchors.leftMargin
                }

                clip: true

                spacing: systemsViewRectangle.width * 0.02

                Text {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Atomify"
                    font.pixelSize: parent.width * 0.04
                    color: "white"

                    renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
                }
            }

            Rectangle {
                anchors.fill: simulateText
                anchors.margins: -width / 8
                color: "black"
                border.width: 2
                border.color: "white"

                MouseArea {
                    anchors.fill: parent
                    onClicked: stackView.push(packageSelectionView)
                }
            }

            Text {
                id: simulateText
                anchors.centerIn: parent
                text: "Simulate"
                font.pixelSize: mainMenuRoot.width * 0.04
                color: "white"
            }
        }
    }

    Component {
        id: packageSelectionView

        Item {

            ListModel {
                id: listModel

                ListElement {
                    name: "Diffusion"
                    imageSource: "simulations/diffusion.png"
                }

                ListElement {
                    name: "Fun"
                    imageSource: "simulations/bullets.png"
                }
            }

            Column {
                id: systemsViewColumn
                anchors {
                    fill: parent
                    topMargin: systemsViewRoot.width * 0.02
                    bottomMargin: anchors.topMargin
                    leftMargin: systemsViewRoot.width * 0.05
                    rightMargin: anchors.leftMargin
                }

                clip: true

                spacing: systemsViewRectangle.width * 0.02

                Text {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Select package"
                    font.pixelSize: parent.width * 0.04
                    color: "white"

                    renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
                }

                ListView {
                    id: listView
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: systemsViewRectangle.height * 0.6
                    model: listModel
                    cacheBuffer: 10000
                    orientation: Qt.Horizontal
                    snapMode: ListView.SnapToItem

                    delegate: PackageButton {
                        width: listView.width * 0.45
                        height: listView.height
                        name: model.name
                        imageSource: model.imageSource

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                stackView.push(simulationSelectionView)
                            }
                        }
                    }
                }
            }
        }

    }

    Component {
        id: simulationSelectionView

        Item {
            ListModel {
                id: listModel

                ListElement {
                    simulation: "simulations/Bullets.qml"
                }
                ListElement {
                    simulation: "simulations/Chamber.qml"
                }
                ListElement {
                    simulation: "simulations/Crystal.qml"
                }
            }

            Column {
                id: systemsViewColumn
                anchors {
                    fill: parent
                    topMargin: systemsViewRoot.width * 0.02
                    bottomMargin: anchors.topMargin
                    leftMargin: systemsViewRoot.width * 0.05
                    rightMargin: anchors.leftMargin
                }

                clip: true

                spacing: systemsViewRectangle.width * 0.02

                Text {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Select simulation"
                    font.pixelSize: parent.width * 0.04
                    color: "white"

                    renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
                }

                ListView {
                    id: listView
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: systemsViewRectangle.height * 0.6
                    model: listModel
                    orientation: Qt.Horizontal
                    snapMode: ListView.SnapToItem

                    delegate: SimulationButton {
                        width: listView.width * 0.45
                        height: listView.height
                        simulationSource: model.simulation

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                systemsViewRoot.loadSimulation(simulation)
                            }
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
