import QtQuick 2.0

Item {
    property string title: "Select package"

    ListModel {
        id: packageListModel

        ListElement {
            name: "Diffusion"
            imageSource: "simulations/pressure/crystal/screenshot.png"
            packageId: "diffusion"
        }

        ListElement {
            name: "Fun"
            imageSource: "simulations/fun/bullets/screenshot.png"
            packageId: "fun"
        }

        ListElement {
            name: "Fun"
            imageSource: "simulations/fun/bullets/screenshot.png"
            packageId: "fun"
        }

        ListElement {
            name: "Fun"
            imageSource: "simulations/fun/bullets/screenshot.png"
            packageId: "fun"
        }

        ListElement {
            name: "Fun"
            imageSource: "simulations/fun/bullets/screenshot.png"
            packageId: "fun"
        }

        ListElement {
            name: "Fun"
            imageSource: "simulations/fun/bullets/screenshot.png"
            packageId: "fun"
        }
    }

    ListView {
        id: listView
        anchors.centerIn: parent
        width: parent.width
        height: systemsViewRectangle.height * 0.6
        model: packageListModel
        cacheBuffer: 10000
        orientation: Qt.Horizontal
//        snapMode: ListView.SnapToItem

        delegate: PackageButton {
            width: listView.width * 0.45
            height: listView.height
            name: model.name
            imageSource: model.imageSource

            Component.onCompleted: {
                console.log(model)
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var targetModel = diffusionModel
                    switch(model.packageId) {
                    case "diffusion":
                        targetModel = diffusionModel
                        break
                    case "fun":
                        targetModel = funModel
                        break
                    case "pressure":
                        targetModel = pressureModel
                        break
                    }
                    stackView.push(simulationSelectionView)
                    stackView.currentItem.model = targetModel
                }
            }
        }
    }
}

