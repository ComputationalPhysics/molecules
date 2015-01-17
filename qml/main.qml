import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Molecules")

    Molecules {
        anchors.fill: parent
    }
}
