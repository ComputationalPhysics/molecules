import QtQuick 2.0

QtObject {
    property string name: "Unnamed"
    property string description: "No description"
    property string longDescription: "No description"
    property string folder: ""
    property url stateSource: (folder && identifier) ? folder + "/" + identifier + ".lmp" : ""
    property url screenshotSource: (folder && identifier) ? folder + "/" + identifier + ".png" : ""
    property real pan: -10
    property real tilt: -20
    property real zoom: -20
    property string identifier: {
        var names = folder.split("/")
        if(folder === "") {
            return ""
        }

        return names[names.length - 1]
    }
}
