import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    width: 1280
    height: 800
    title: qsTr("Molecules")

    Molecules {
        anchors.fill: parent
    }
}
