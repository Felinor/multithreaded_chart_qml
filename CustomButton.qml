import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control

    property double controlImplicitHeight: 30
    property double controlImplicitWidth: 100
    property double fontPointSize: controlImplicitHeight * 0.4

    property color borderColor: "white"
    property color cobaltBlue: "#2F54EB"

    background: Rectangle {
        id: backgroundBtn
        implicitHeight: controlImplicitHeight
        implicitWidth: controlImplicitWidth
        color: cobaltBlue
        radius: 8
        border.color: borderColor
    }

    contentItem: Text {
        id: textItem
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "white"
        text: control.text
        font {
            family: "Noto Sans"
            pointSize: fontPointSize
            letterSpacing: 0.25
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onPressed: mouse.accepted = false
    }
}
