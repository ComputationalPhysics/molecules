import QtQuick 2.0

Item {
    id: simulationSelectionView
    property alias model: listView.model
    property string title: "Select simulation"

    ListModel {
        id: simulationsModel

        ListElement {
            folder: "simulations/fun/bullets"
        }

        ListElement {
            folder: "simulations/diffusion/chamber"
        }

        ListElement {
            folder: "simulations/pressure/crystal"
        }
    }

    ListView {
        id: listView
        anchors.centerIn: parent
        width: parent.width
        height: systemsViewRectangle.height
        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
        model: simulationsModel

        delegate: SimulationButton {
            width: listView.width
            height: listView.height
            folder: model.folder
            onLoadSimulation: {
                systemsViewRoot.loadSimulation(simulation)
            }
        }
    }
}

