import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import QtQuick.Window 2.1

SystemDialog {
    id: systemDialog
    category: SystemDialog.Dialog
    contentHeight: flickable.height
    allowedOrientations: __silica_applicationwindow_instance.defaultAllowedOrientations

    SilicaFlickable {
        id: flickable
        width: parent.width
        height: Math.min(systemDialog.screenHeight*4/5, keyboardY, contentHeight)
        contentWidth: width
        contentHeight: column.height
        clip: true

        property bool isPortrait: (systemDialog.orientation === Qt.PortraitOrientation) ||
                                  (systemDialog.orientation === Qt.InvertedPortraitOrientation)
        property int keyboardY: {
            if (!Qt.inputMethod.visible)
                return Screen.height
            var rec = Qt.inputMethod.keyboardRectangle
            return isPortrait ? rec.y : (Screen.width - rec.width)
        }

        Behavior on height {
            NumberAnimation { duration: 200}
        }

        Column {
            id: column
            width: flickable.width

            SystemDialogHeader {
                title: qdevelSu.title
                description: qdevelSu.description
            }

            TextField {
                id: passwordField
                width: parent.width
                label: "Password"
                placeholderText: "Password"
                inputMethodHints: Qt.ImhNoPredictiveText
                focus: true
                echoMode: TextInput.Password
                passwordMaskDelay: 2000

                Keys.onReturnPressed: writePasswd()
            }

            Row {
                height: childrenRect.height
                SystemDialogTextButton {
                    text: "Cancel"
                    width: column.width/2
                    onClicked: qdevelSu.exit(1)
                }

                SystemDialogTextButton {
                    text: "Accept"
                    width: column.width/2
                    onClicked: writePasswd()
                }
            }
        }
    }


    function writePasswd() {
        qdevelSu.writePasswd(passwordField.text)
        systemDialog.dismiss()
    }

    onDismissed: qdevelSu.exit(1)

    layoutItem.onVisibleChanged: {
        if (layoutItem.visible)
            focusTimer.start()
    }

    Timer {
        id: focusTimer
        interval: 0
        onTriggered: passwordField.forceActiveFocus()
    }

    Connections {
        target: qdevelSu
        onShowWindow: systemDialog.activate()
    }
}
