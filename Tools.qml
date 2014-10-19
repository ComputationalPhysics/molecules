import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: toolsViewRoot

    property bool revealed: false
    property bool running: true
    property alias thermostatValue: thermostatSlider.value
    property alias thermostatEnabled: thermostatCheckBox.checked

    anchors {
        top: parent.top
        bottom: parent.bottom
        right: parent.right
    }

    width: parent.width * 0.3
    height: parent.height
    state: revealed ? "revealed" : "hidden"

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: toolsColumn
                anchors.rightMargin: -toolsColumn.width
//                    anchors.bottomMargin: -toolsColumn.height
            }
            PropertyChanges {
                target: revealToggleButton
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
            }
        },
        State {
            name: "revealed"
            PropertyChanges {
                target: toolsColumn
                anchors.rightMargin: -toolsColumn.border.width
                anchors.bottomMargin: -toolsColumn.border.width
            }
            PropertyChanges {
                target: revealToggleButton
                anchors.rightMargin: -revealToggleButton.width
                anchors.bottomMargin: -revealToggleButton.height
            }
        }

    ]

    transitions: [
        Transition {
            from: "hidden"
            to: "revealed"
            ParallelAnimation {
                NumberAnimation {
                    target: toolsColumn
                    properties: "anchors.rightMargin"
                    duration: 250
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    properties: "anchors.bottomMargin"
                    duration: 400
                    easing.type: Easing.InQuad
                }
            }
        },
        Transition {
            from: "revealed"
            to: "hidden"
            ParallelAnimation {
                NumberAnimation {
                    properties: "anchors.bottomMargin"
                    duration: 300
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    properties: "anchors.rightMargin"
                    duration: 400
                    easing.type: Easing.InQuad
                }
            }
        }
    ]

    Item {
        id: revealToggleButton
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height * 0.1
        width: height

        Image {
            anchors {
                fill: parent
                margins: parent.width * 0.2
            }
            source: "images/tools.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: {
                toolsViewRoot.revealed = !toolsViewRoot.revealed
            }
        }
    }

    Rectangle {
        id: toolsColumn
        anchors {
            bottom: parent.bottom
            bottomMargin: -toolsColumn.border.width
            right: parent.right
        }
        height: parent.height + border.width * 2
        width: parent.width + border.width * 2

        border.width: 1.0
        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)

        color: Qt.rgba(0.05, 0.05, 0.05, 0.93)

        MouseArea {
            // Dummy area to keep input on background from being passed through to MD object
            anchors.fill: parent
        }

        Column {
            anchors {
                fill: parent
                margins: toolsViewRoot.width * 0.1
            }
            spacing: toolsViewRoot.width * 0.05

            Item {
                width: 10
                height: parent.height * 0.2
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.2
                height: width
                source: toolsViewRoot.running ? "images/pause.png" : "images/play.png"

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        toolsViewRoot.running = !toolsViewRoot.running
                    }
                }
            }

            Item {
                width: 10
                height: parent.height * 0.1
            }

            CheckBox {
                id: thermostatCheckBox
                text: "Thermostat: " + thermostatSlider.value.toFixed(1) + " K"
                style: CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }
            }

            Slider {
                id: thermostatSlider
                width: parent.width
                minimumValue: 1
                maximumValue: 500
            }
        }
    }
}
