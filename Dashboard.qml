import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import "style"

Item {
    id: dashboardRoot
    property bool revealed: false
    property bool running: true
    property alias thermostatValue: thermostatSlider.value
    property alias thermostatEnabled: thermostatCheckbox.checked

    property alias forceValue: forceSlider.value
    property alias forceEnabled: forceCheckbox.checked

    property alias systemSizeX: systemSizeXSlider.value
    property alias systemSizeY: systemSizeYSlider.value
    property alias systemSizeZ: systemSizeZSlider.value
    property real  minimumSystemSizeSliderValue: 1

    property bool othersPressed: false
    property real hiddenOpacity: 0.2

    function addTemperature(temperature) {
        plotTemperature.addPoint(temperature)
    }

    function addKineticEnergy(kineticEnergy) {
        plotKineticEnergy.addPoint(kineticEnergy)
    }

    function addPotentialEnergy(potentialEnergy) {
        plotPotentialEnergy.addPoint(potentialEnergy)
    }

    function addTotalEnergy(totalEnergy) {
        plotTotalEnergy.addPoint(totalEnergy)
    }

    function addPressure(pressure) {
        pressurePlot.addPoint(pressure)
    }

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
            PropertyChanges {
                target: revealToggleButton
                opacity: 0.0
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: dashboardRectangle
                opacity: 0.0
                scale: 0.85
            }
            PropertyChanges {
                target: revealToggleButton
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
        enabled: dashboardRoot.revealed
        anchors.fill: parent
        onPressed: {
            dashboardRoot.revealed = false
        }
    }

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
        color: Qt.rgba(0.05, 0.05, 0.05, othersPressed ? hiddenOpacity : 0.8)

        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }

        MouseArea {
            // Dummy area to keep input on background from being passed through to MD object
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

            opacity: othersPressed ? hiddenOpacity : 1.0

            Behavior on opacity {
                OpacityAnimation {

                }
            }

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
            opacity: othersPressed ? hiddenOpacity : 1.0

            spacing: anchors.margins

            Behavior on opacity {
                OpacityAnimation {

                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                border.color: "white"
                border.width: 1.0

                Plot {
                    id: plotTemperature
                    anchors.fill: parent
                    minimumValue: 0
                    maximumValue: 1100
                    strokeStyle: "#a6cee3"
                }

                PlotLabels {
                    anchors.fill: parent
                    minimumValue: plotTemperature.minimumValue
                    maximumValue: plotTemperature.maximumValue
                    title: "T [K]"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                border.color: "white"
                border.width: 1.0

                Plot {
                    id: pressurePlot
                    anchors.fill: parent
                    minimumValue: -10
                    maximumValue: 1200
                    strokeStyle: "#a6cee3"
                }

                PlotLabels {
                    anchors.fill: parent
                    minimumValue: pressurePlot.minimumValue
                    maximumValue: pressurePlot.maximumValue
                    title: "P [MPa]"
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                border.color: "white"
                border.width: 1.0

                Plot {
                    id: plotKineticEnergy
                    anchors.fill: parent
                    minimumValue: -1
                    maximumValue: 1
                    strokeStyle: "#a6cee3"
                }

                Plot {
                    id: plotPotentialEnergy
                    anchors.fill: parent
                    minimumValue: plotKineticEnergy.minimumValue
                    maximumValue: plotKineticEnergy.maximumValue
                    strokeStyle: "#b2df8a"
                }

                Plot {
                    id: plotTotalEnergy
                    anchors.fill: parent
                    minimumValue: plotKineticEnergy.minimumValue
                    maximumValue: plotKineticEnergy.maximumValue
                    strokeStyle: "#fdbf6f"
                }

                PlotLabels {
                    anchors.fill: parent
                    minimumValue: plotKineticEnergy.minimumValue
                    maximumValue: plotKineticEnergy.maximumValue
                    title: "E/N [eV]"
                }
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
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }

                Behavior on opacity {
                    OpacityAnimation {

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
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CustomSliderStyle {

                }

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            CheckBox {
                id: forceCheckbox
                checked: false
                text: "F"
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CheckBoxStyle {
                    label: Text {
                        text: control.text
                        color: "white"
                    }
                }

                Behavior on opacity {
                    OpacityAnimation {

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
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CustomSliderStyle {

                }

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            Slider {
                id: systemSizeXSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: dashboardRoot.minimumSystemSizeSliderValue
                maximumValue: 100
                value: 10
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CustomSliderStyle {

                }

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            Slider {
                id: systemSizeYSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: dashboardRoot.minimumSystemSizeSliderValue
                maximumValue: 100
                value: 10
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }
                style: CustomSliderStyle {

                }

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            Slider {
                id: systemSizeZSlider
                orientation: Qt.Vertical
                Layout.fillHeight: true
                minimumValue: dashboardRoot.minimumSystemSizeSliderValue
                maximumValue: 100
                value: 10
                opacity: pressed || !othersPressed ? 1.0 : hiddenOpacity
                onPressedChanged: {
                    if(pressed) {
                        othersPressed = true
                    } else {
                        othersPressed = false
                    }
                }

                style: CustomSliderStyle {

                }

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }


}
