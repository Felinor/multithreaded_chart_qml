import QtQuick 2.15
import QtQuick.Controls 2.15

ProgressBar {
    id:control
    property double controlImplicitWidth: 300
    property double controlImplicitHeight: 40

    background: Rectangle {
        implicitWidth: controlImplicitWidth
        implicitHeight: controlImplicitHeight
        color: "#e6e6e6"
        radius: 3
    }
    contentItem: Item {
        implicitWidth: 200
        implicitHeight: 4

        Rectangle {
            width: progressBar.visualPosition * parent.width
            height: parent.height
            radius: 2
            color: "#2F54EB"
        }

        Text {
            anchors.centerIn: parent
            text: value !== to ? progressBarText.arg(position*100) : "Готово! ♡( ◡‿◡ )"
            color: value !== to ? "black" : "white"
            font.pixelSize: parent.height * 0.5
        }
    }
}
