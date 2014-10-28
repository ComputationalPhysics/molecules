import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import "style"
import "controls"

Item {
    id: dashboardRoot
    property bool revealed: false
    property bool running: true
    property alias thermostatValue: thermostat.value
    property alias thermostatEnabled: thermostat.activated

    //    property alias forceValue: forceSlider.value
    //    property alias forceEnabled: forceCheckbox.checked

    property double forceValue: 0
    property bool forceEnabled: false

    property alias systemSizeX: systemSizeXSlider.value
    property alias systemSizeY: systemSizeYSlider.value
    property alias systemSizeZ: systemSizeZSlider.value
    property real  minimumSystemSizeSliderValue: 1.0

    property real temperature: 0.0
    property real pressure: 0.0
    property real kineticEnergy: 0.0
    property real potentialEnergy: 0.0
    property real totalEnergy: 0.0
    property real time: 0.0

    property bool othersPressed: false
    property real hiddenOpacity: 0.2
    property real hiddenOpacityForPlots: 0.4

    function resetControls() {
        running = true
        thermostatEnabled = false
        thermostatValue = 100
        forceEnabled = false
        forceValue = 0

        temperaturePlot.clearData()
        pressurePlot.clearData()
        totalEnergyPlot.clearData()
        kineticEnergyPlot.clearData()
        potentialEnergyPlot.clearData()
    }

    function addTemperature(temperature) {
        temperaturePlot.addValue(temperature)
    }

    function addKineticEnergy(kineticEnergy) {
        kineticEnergyPlot.addValue(kineticEnergy)
    }

    function addPotentialEnergy(potentialEnergy) {
        potentialEnergyPlot.addValue(potentialEnergy)
    }

    function addTotalEnergy(totalEnergy) {
//        totalEnergyPlot.addValue(totalEnergy)
        resetRange()
    }

    function addPressure(pressure) {
        pressurePlot.addValue(pressure)
    }

    function min(data) {
        var minValue = Infinity
        for(var i in data) {
            if(!isNaN(data[i])) {
                minValue = Math.min(minValue, data[i])
            }
        }
        return minValue
    }

    function max(data) {
        var maxValue = -Infinity
        for(var i in data) {
            if(!isNaN(data[i])) {
                maxValue = Math.max(maxValue, data[i])
            }
        }
        return maxValue
    }

    function minMaxExponent(minValue, maxValue) {
        var logMinValue = Math.log(Math.abs(minValue)) / Math.LN10
        var logMaxValue = Math.log(Math.abs(maxValue)) / Math.LN10

        var minExponent = Math.floor(logMinValue)
        var maxExponent = Math.floor(logMaxValue)

        var sharedExponent = Math.max(minExponent, maxExponent)

        var powMinExponent = Math.pow(10, sharedExponent)
        var minValueOverExponentMinusOne = minValue / powMinExponent
        var minValueRoundedDown = Math.floor(minValueOverExponentMinusOne)
        var minValueOut = minValueRoundedDown * powMinExponent

        var powMaxExponent = Math.pow(10, sharedExponent)
        var maxValueOverExponentMinusOne = maxValue / powMaxExponent
        var maxValueRoundedUp = Math.ceil(maxValueOverExponentMinusOne)
        var maxValueOut = maxValueRoundedUp * powMaxExponent

        return [minValueOut, maxValueOut]
    }

    function resetRange() {
//        var minValue = Math.min(min(kineticEnergyPlot.values), Math.min(min(potentialEnergyPlot.values), min(totalEnergyPlot.values)))
//        var maxValue = Math.max(max(kineticEnergyPlot.values), Math.max(max(potentialEnergyPlot.values), max(totalEnergyPlot.values)))
        var minValue = Math.min(min(kineticEnergyPlot.values), min(potentialEnergyPlot.values))
        var maxValue = Math.max(max(kineticEnergyPlot.values), max(potentialEnergyPlot.values))

        var minMaxExponents = minMaxExponent(minValue, maxValue)
        minValue = minMaxExponents[0]
        maxValue = minMaxExponents[1]

        kineticEnergyPlot.minimumValue = minValue
        kineticEnergyPlot.maximumValue = maxValue
        potentialEnergyPlot.minimumValue = minValue
        potentialEnergyPlot.maximumValue = maxValue
//        totalEnergyPlot.minimumValue = minValue
//        totalEnergyPlot.maximumValue = maxValue

        var minTemperature = min(temperaturePlot.values)
        var maxTemperature = max(temperaturePlot.values)

        var minMaxTemperatureExponents = minMaxExponent(minTemperature, maxTemperature)
        minTemperature = minMaxTemperatureExponents[0]
        maxTemperature = minMaxTemperatureExponents[1]

        temperaturePlot.minimumValue = minTemperature
        temperaturePlot.maximumValue = maxTemperature

        var minPressure = min(pressurePlot.values)
        var maxPressure = max(pressurePlot.values)

        var minMaxPressureExponents = minMaxExponent(minPressure, maxPressure)
        minPressure = minMaxPressureExponents[0]
        maxPressure = minMaxPressureExponents[1]

        pressurePlot.minimumValue = minPressure
        pressurePlot.maximumValue = maxPressure
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

    MouseArea {
        enabled: dashboardRoot.revealed
        anchors.fill: parent
        onPressed: {
            dashboardRoot.revealed = false
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
            onPressed: {
                othersPressed = true
            }
            onReleased: {
                othersPressed = false
            }
        }

        Image {
            id: playButton
            anchors {
                top: parent.top
                left: parent.left
                margins: parent.width*0.01
            }

            width: Style.touchableSize
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
        Text {
            anchors {
                verticalCenter: playButton.verticalCenter
                left: playButton.right
                margins: dashboardRoot.width * 0.01
            }
            color: "white"

            opacity: othersPressed ? hiddenOpacity : 1.0

            text: (dashboardRoot.time * 1e12).toFixed(1) + " ps"
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
            opacity: othersPressed ? hiddenOpacityForPlots : 1.0

            spacing: anchors.margins

            Behavior on opacity {
                OpacityAnimation {

                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "black"
                        border.color: "white"
                        border.width: 1.0

                        Plot {
                            id: temperaturePlot
                            anchors {
                                fill: parent
                                margins: dashboardRoot.width * 0.01
                            }
                            minimumValue: 0
                            maximumValue: 1100
                            strokeStyle: "#a6cee3"
                        }

                        PlotLabels {
                            anchors.fill: parent
                            minimumValue: temperaturePlot.minimumValue
                            maximumValue: temperaturePlot.maximumValue
                            title: "T [K]"
                        }
                    }

                    Text {
                        text: "T = " + dashboardRoot.temperature.toFixed(1) + " K"
                        color: temperaturePlot.strokeStyle
                        font.pixelSize: 12
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "black"
                        border.color: "white"
                        border.width: 1.0

                        Plot {
                            id: pressurePlot
                            anchors {
                                fill: parent
                                margins: dashboardRoot.width * 0.01
                            }
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

                    Text {
                        text: "P = " + (dashboardRoot.pressure * 1e-6).toFixed(1) + " MPa"
                        color: pressurePlot.strokeStyle
                        font.pixelSize: 12
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "black"
                        border.color: "white"
                        border.width: 1.0

                        Plot {
                            id: kineticEnergyPlot
                            anchors {
                                fill: parent
                                margins: dashboardRoot.width * 0.01
                            }
                            minimumValue: -1
                            maximumValue: 1
                            strokeStyle: "#b2df8a"
                        }

                        Plot {
                            id: potentialEnergyPlot
                            anchors {
                                fill: parent
                                margins: dashboardRoot.width * 0.01
                            }
                            minimumValue: kineticEnergyPlot.minimumValue
                            maximumValue: kineticEnergyPlot.maximumValue
                            strokeStyle: "#a6cee3"
                        }

                        PlotLabels {
                            anchors.fill: parent
                            minimumValue: kineticEnergyPlot.minimumValue
                            maximumValue: kineticEnergyPlot.maximumValue
                            title: "E/N [eV]"
                        }
                    }
                    Row {
                        spacing: 12
                        Text {
                            text: "Ek = " + dashboardRoot.kineticEnergy.toFixed(0) + " eV"
                            color: kineticEnergyPlot.strokeStyle
                            font.pixelSize: 12
                        }
                        Text {
                            text: "Ep = " + dashboardRoot.potentialEnergy.toFixed(0) + " eV"
                            color: potentialEnergyPlot.strokeStyle
                            font.pixelSize: 12
                        }
                    }
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
                margins: dashboardRoot.width * 0.03
            }

            Thermostat {
                id: thermostat
                Layout.fillHeight: true
                Layout.preferredWidth: height
                secondaryValue: dashboardRoot.temperature
                minimumValue: 0
                maximumValue: 1500 + 273.15
                activated: false
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Image {
                property bool checked: false
                Layout.fillHeight: true
                Layout.preferredWidth: height

                source: "images/systemsize.png"
                smooth: true
                antialiasing: true

                fillMode: Image.PreserveAspectFit
                opacity: !othersPressed ? 1.0 : hiddenOpacity

                Behavior on opacity {
                    OpacityAnimation {

                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.preferredWidth: dashboardRoot.width*0.01
            }

            ColumnLayout {
                Slider {
                    id: systemSizeXSlider
                    orientation: Qt.Horizontal
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
                        handleLabel: "X"
                    }

                    Behavior on opacity {
                        OpacityAnimation {

                        }
                    }
                }

                Slider {
                    id: systemSizeYSlider
                    orientation: Qt.Horizontal
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
                        handleLabel: "Y"
                    }

                    Behavior on opacity {
                        OpacityAnimation {

                        }
                    }
                }

                Slider {
                    id: systemSizeZSlider
                    orientation: Qt.Horizontal
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
                        handleLabel: "Z"
                    }

                    Behavior on opacity {
                        OpacityAnimation {

                        }
                    }
                }
            }
        }
    }


}
