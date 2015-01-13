import QtQuick 2.0
import "style"

Item {
    id: simulationSelectionView
    property alias model: listView.model
    property string title: "Select simulation"

    ListModel {
        id: simulationsModel

        ListElement {
            folder: "simulations/diffusion/simple_diffusion"
        }

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
        anchors {
            top: parent.top
            bottom: dots.top
            left: parent.left
            right: parent.right
            bottomMargin: Style.touchableSize * 0.1
        }

        orientation: Qt.Horizontal
        snapMode: ListView.SnapOneItem
        model: simulationsModel
        cacheBuffer: 999999

        onContentXChanged: {
            dotsRepeater.currentIndex = indexAt(contentX + width / 2, contentY)
        }

        delegate: SimulationButton {
            width: listView.width
            height: listView.height
            folder: model.folder
            onLoadSimulation: {
                systemsViewRoot.loadSimulation(simulation)
            }
        }
    }

    Row {
        id: dots
        anchors {
            bottom: parent.bottom
            bottomMargin: Style.touchableSize * 0.7
            horizontalCenter: parent.horizontalCenter
        }
        height: Style.baseSize * 0.3
        spacing: Style.baseSize * 0.2

        Repeater {
            id: dotsRepeater
            property int currentIndex: 0
            model: simulationsModel.count

            function refresh() {
                for(var i = 0; i < count; i++) {
                    itemAt(i).selected = false
                }
                itemAt(currentIndex).selected = true
            }

            Component.onCompleted: {
                refresh()
            }

            onCurrentIndexChanged: {
                refresh()
            }

            Rectangle {
                id: dot
                property bool selected: false
                anchors.verticalCenter: parent.verticalCenter
                width: Style.baseSize * 0.2
                height: width
                color: "grey"

                radius: width / 2

                states: [
                    State {
                        name: "selected"
                        when: selected
                        PropertyChanges {
                            target: dot
                            color: "white"
                        }
                        PropertyChanges {
                            target: dot
                            width: Style.baseSize * 0.3
                        }
                    }
                ]

                transitions: [
                    Transition {
                        ParallelAnimation {
                            NumberAnimation {
                                target: dot
                                property: "width"
                                duration: 400
                                easing.type: Easing.InOutQuad
                            }
                            NumberAnimation {
                                target: dot
                                property: "height"
                                duration: 400
                                easing.type: Easing.InOutQuad
                            }
                            ColorAnimation {
                                duration: 400
                            }
                        }
                    }
                ]
            }
        }
    }
}

