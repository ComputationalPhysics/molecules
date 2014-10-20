import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Item {
    id: dashboardRoot
    property bool revealed: false
    property bool running: true
    property alias thermostatValue: thermostatSlider.value
    property alias thermostatEnabled: thermostatCheckbox.checked

    property alias forceValue: forceSlider.value
    property alias forceEnabled: forceCheckbox.checked

    anchors.fill: parent
    state: revealed ? "revealed" : "hidden"

    states: [
        State {
            name: "revealed"
            PropertyChanges {
                target: dashboardRectangle
                opacity: 1.0
                scale: 1.0
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: dashboardRectangle
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
                dashboardRoot.revealed = !dashboardRoot.revealed
            }
        }
    }

    Rectangle {
        id: dashboardRectangle
        enabled: revealed
        anchors.centerIn: parent
        height: parent.height * 0.8
        width: parent.width * 0.7
        border.width: 1.0
        border.color: Qt.rgba(0.5, 0.5, 0.5, 0.9)
        color: Qt.rgba(0.05, 0.05, 0.05, 0.8)

        MouseArea {
            anchors.fill: parent
        }

        Image {
            id: playButton
            anchors {
                top: parent.top
                left: parent.left
                margins: parent.width*0.01
            }

            width: parent.width * 0.05
            height: width
            source: dashboardRoot.running ? "images/pause.png" : "images/play.png"

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    dashboardRoot.running = !dashboardRoot.running
                }
            }
        }

        RowLayout {
            id: plotRow
            anchors {
                top: playButton.bottom
                left: parent.left
                right: parent.right
                margins: parent.width*0.05
            }

            height: dashboardRectangle.height*0.3

            spacing: anchors.margins
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        RowLayout {
            id: controlsRow
            anchors {
                top: plotRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: parent.width*0.05
            }

            spacing: anchors.margins

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            CheckBox {
                id: thermostatCheckbox
                checked: false
                text: "T"
                style: CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }
            }

            Slider {
                id: thermostatSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: 1
                maximumValue: 1000
                value: 100
            }

            CheckBox {
                id: forceCheckbox
                checked: false
                text: "F"
                style: CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }
            }

            Slider {
                id: forceSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: -100
                maximumValue: 100
                value: 0
            }

            Slider {
                id: systemSizeXSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: 5
                maximumValue: 100
                value: 10
            }

            Slider {
                id: systemSizeYSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: 5
                maximumValue: 100
                value: 10
            }

            Slider {
                id: systemSizeZSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: 5
                maximumValue: 100
                value: 10
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }


}
