import QtQuick 2.0
import "style"

Text {
    id: welcomeText
    color: Style.font.heading.color
    font.weight: Font.Light
    font.family: "Roboto"
    font.pixelSize: Style.font.heading.size
    renderType: Qt.platform.os === "linux" ? Text.NativeRendering : Text.QtRendering
}

