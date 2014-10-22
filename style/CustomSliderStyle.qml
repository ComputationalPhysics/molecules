import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0
import "../style" 1.0

SliderStyle {
    id: sliderStyle


    handle: Rectangle {
        implicitWidth: Style.touchableSize
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
