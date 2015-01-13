pragma Singleton
import QtQuick 2.0
import QtQuick.Window 2.0

QtObject {
    property real windowWidth
    property real windowHeight
    property real minimumTouchableSize: windowWidth / 25
    property real maximumTouchableSize: windowWidth / 10
    property real touchableSize: {
        if(Screen.pixelDensity === 0) {
            // Assume we're running on a "standard" computer monitor
            return 56
        }
        return Screen.pixelDensity * 8
    }
}
