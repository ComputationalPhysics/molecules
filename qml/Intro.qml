import QtQuick 2.0
import QtGraphicalEffects 1.0
import "style"

Item {
    id: introRoot
    default property alias experiments: experimentsRow.children
    property bool running: false
    property alias title: welcomeText.text
    property int currentExperimentIndex: 0
    width: 100
    height: 62

    Component.onCompleted: {
        resetRunning()
    }

    onRunningChanged: resetRunning()

    function resetRunning() {
        for(var i in experimentsRow.children) {
            var child = experimentsRow.children[i]
            if(introRoot.running && currentExperimentIndex === parseInt(i)) {
                child.running = true
            } else {
                child.running = false
            }
        }
    }

    onCurrentExperimentIndexChanged: {
        if(currentExperimentIndex >= experimentsRow.children.length) {
            currentExperimentIndex = experimentsRow.children.length - 1
            endRectangleAnimation.restart()
        } else if(currentExperimentIndex < 0) {
            currentExperimentIndex = 0
            startRectangleAnimation.restart()
        }
        resetRunning()
    }

    state: "welcome"

    Row {
        id: experimentsRow
        x: - introRoot.width * currentExperimentIndex
        width: parent.width * children.length
        anchors.top: parent.top
        height: parent.height
        Behavior on x {
            NumberAnimation {
                duration: 500
                easing.overshoot: 0.5
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle {
        id: background
        color: "black"
        anchors.fill: parent
        opacity: 1
    }

    Heading {
        id: welcomeText
        text: "Atomify."
    }

    MouseArea {
        id: skipArea
        anchors.fill: parent
        onClicked: {
            transition.enabled = false
            introRoot.state = "tmp"
            introRoot.state = "started"
            introRoot.running = true
            skipArea.enabled = false
        }
    }

    RadialGradient {
        id: startRectangle

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        horizontalOffset: -width * 0.8

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#98F8F8"
            }
            GradientStop {
                position: 1
                color: "transparent"
            }
        }

        width: parent.width / 15
        opacity: 0
        SequentialAnimation {
            id: startRectangleAnimation
            NumberAnimation {
                target: startRectangle
                property: "opacity"
                from: 0
                to: 1
                duration: 200
            }
            NumberAnimation {
                target: startRectangle
                property: "opacity"
                from: 1
                to: 0
                duration: 600
            }
        }
    }

    RadialGradient {
        id: endRectangle

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        horizontalOffset: width * 0.8

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#98F8F8"
            }
            GradientStop {
                position: 1
                color: "transparent"
            }
        }

        width: parent.width / 15
        opacity: 0
        SequentialAnimation {
            id: endRectangleAnimation
            NumberAnimation {
                target: endRectangle
                property: "opacity"
                from: 0
                to: 1
                duration: 200
            }
            NumberAnimation {
                target: endRectangle
                property: "opacity"
                from: 1
                to: 0
                duration: 600
            }
        }
    }

    states: [
        State {
            name: "welcome"
            AnchorChanges {
                target: welcomeText
                anchors.horizontalCenter: introRoot.horizontalCenter
                anchors.verticalCenter: introRoot.verticalCenter
            }
            PropertyChanges {
                target: welcomeText
                opacity: 0
            }
            PropertyChanges {
                target: experimentsRow
                anchors.topMargin: introRoot.height
            }
        },
        State {
            name: "started"
            AnchorChanges {
                target: welcomeText
                anchors.top: introRoot.top
            }
            PropertyChanges {
                target: welcomeText
                opacity: 1
                anchors.topMargin: Style.touchableSize
            }
            PropertyChanges {
                target: background
                opacity: 0
            }
            PropertyChanges {
                target: experimentsRow
                anchors.topMargin: 0
            }
        }
    ]

    transitions: [
        Transition {
            id: transition
            from: "welcome"
            to: "started"
            SequentialAnimation {
                NumberAnimation {
                    target: welcomeText
                    property: "opacity"
                    duration: 2000
                    easing.type: Easing.InOutQuad
                }
                ParallelAnimation {
                    AnchorAnimation {
                        duration: 2000
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: experimentsRow
                        properties: "anchors.topMargin"
                        duration: 2000
                        easing.type: Easing.InOutQuad
                    }
                }
                NumberAnimation {
                    target: background
                    property: "opacity"
                    duration: 1000
                    easing.type: Easing.InOutQuad
                }
            }
            onRunningChanged: {
                if(!running) {
                    skipArea.enabled = false
                    introRoot.running = true
                }
            }
        }
    ]
}

