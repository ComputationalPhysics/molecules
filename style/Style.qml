pragma Singleton
import QtQuick 2.0

QtObject {
    property real windowWidth
    property real windowHeight
    property real minimumTouchableSize: windowWidth / 25
    property real maximumTouchableSize: windowWidth / 10
    property real pixelDensity: 72
    property real touchableSize: {
        if(pixelDensity === 0) {
            // Assume we're running on a "standard" computer monitor
            return 72
        }
        return pixelDensity * 6
    }
    property real baseSize: {
        if(pixelDensity === 0) {
            // Assume we're running on a "standard" computer monitor
            return 36
        }
        return pixelDensity * 4
    }
    property real baseMargin: {
        if(pixelDensity === 0) {
            // Assume we're running on a "standard" computer monitor
            return 36
        }
        return pixelDensity * 4
    }
}
