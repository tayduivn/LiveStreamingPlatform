import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.14
import QtQuick.Controls.Material 2.12

import RTMPStreamer 1.0

ApplicationWindow {
    visible: true
    width: 400
    height: 260
    title: qsTr("Live")
    Material.theme: Material.Dark
    Material.primary: Material.BlueGrey
    Material.accent: Material.Amber
    ColumnLayout{
        anchors.centerIn: parent
        Layout.preferredWidth: 300
        Layout.preferredHeight: 100
        Layout.maximumWidth: 400
        Label{
            text: "Output URL"
            font.pixelSize: 20
        }
        TextArea{
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            onEditingFinished: {
                streamer.outUrl = text
            }
            Component.onCompleted: {
                streamer.outUrl = "rtmp://127.0.0.1:443/live/home"
                text = "rtmp://127.0.0.1:443/live/home"
            }
            font.pixelSize: 16
        }

        RowLayout{
            Button{
                text: "Start"
                Layout.fillWidth: true
                onClicked: {
                    streamer.start()
                }
            }
            Button{
                text: "Stop"
                Layout.fillWidth: true
                onClicked: {
                    streamer.stop()
                }
            }
        }
    }
    RTMPStreamer{
        id:streamer
    }
}
