import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0
import "../style" 1.0

SliderStyle {
    id: sliderStyle


    handle: Rectangle {
        property double minimumWidth: Style.windowWidth / 30
        property double maximumWidth: Style.windowHeight / 20
        implicitWidth: Math.max(Math.min(Screen.pixelDensity * 4, maximumWidth), minimumWidth)
        implicitHeight: implicitWidth
        radius: width
        border.width: width * 0.1
        border.color: "#f7fbff"
        color: "#deebf7"
    }
    groove: Rectangle {
        id: rect
        implicitWidth: 200
        implicitHeight: 8
        color: "#c6dbef"
        radius: 8

        Rectangle {
            color: "#4292c6"
            anchors {
                bottom: parent.bottom
                top: parent.top
                left: parent.left
            }
            radius: parent.radius
            width: (control.value - control.minimumValue) / (control.maximumValue - control.minimumValue) * rect.width
        }
    }
}
