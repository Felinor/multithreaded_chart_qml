import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: window
    visible: true
    width: 1000
    height: 800
    minimumWidth: 600
    minimumHeight: 400
    title: "Word Counter Histogram"

    property int progressValue: 0
    property string progressBarText

    Connections {
        target: fileProcessor
        function onFileProcessed(words) {
            var tempPoint = []
            for (var i = 0, keys = Object.keys(words), ii = keys.length; i < ii; i++) {
                var key = keys[i]
                var color = chart.colorMap[key] || Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
                // Сохраняем цвет в colorMap, если его еще нет
                chart.colorMap[key] = color

                tempPoint.push({ x: key, y: words[key], color: color })
            }
            chart.points = tempPoint
        }

        function onPauseChanged(isPause) {
            progressBarText = "Пауза %1 %  (¯ . ¯٥)"
        }

        function onReadStarted() {
            progressBarLoader.active = true
            progressBarText = "Идет чтение файла... %1 % (* ^ ω ^)"
        }

        function onReadFinished() {
            timer.start()
        }

        function onProgressUpdated(progress) {
            progressValue = progress
        }

        function onReadingCanceled(progress) {
            progressBarText = "Идет завершение... %1 %  ┐(￣ヮ￣)┌"
        }
    }

    // Чтобы ProgressBar успел отобразить прогресс
    Timer {
        id: timer
        running: false
        repeat: false
        interval: 1000
        onTriggered: progressBarLoader.active = false
    }

    Component {
        id: progressBarComponent
        CustomProgressBar {
            id: progressBar
            controlImplicitHeight: window.height * 0.05
            controlImplicitWidth: window.width * 0.4
            from: 0
            to: 100
            value: progressValue
        }
    }

    Loader {
        id: progressBarLoader
        active: false
        visible: status == Loader.Ready
        anchors.centerIn: parent
        sourceComponent: progressBarComponent
        z: 1
    }

    BarChart {
        id: chart
        anchors { fill: parent; margins: 100 }
        title:  'Word Counter Histogram'
        yLabel: 'Frequency'
        xLabel: 'Word list'
    }

    Row {
        anchors {
            bottom: parent.bottom
            bottomMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 10

        CustomButton {
            controlImplicitHeight: window.height * 0.05
            controlImplicitWidth: window.width * 0.1
            text: "open file"
            onClicked: fileDialog.open()
        }
        CustomButton {
            controlImplicitHeight: window.height * 0.05
            controlImplicitWidth: window.width * 0.1
            text: "start"
            onClicked: fileProcessor.start()
        }
        CustomButton {
            controlImplicitHeight: window.height * 0.05
            controlImplicitWidth: window.width * 0.1
            text: "pause"
            onClicked: fileProcessor.togglePaused()
        }
        CustomButton {
            controlImplicitHeight: window.height * 0.05
            controlImplicitWidth: window.width * 0.1
            text: "cancel"
            onClicked: fileProcessor.cancel()
        }
    }

    FileDialog {
        id: fileDialog
        onAccepted: fileProcessor.filePath = fileUrl
    }
}
