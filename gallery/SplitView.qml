/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Christian Dywan <christian.dywan@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import U1db 1.0 as U1db
import Ubuntu.Components 0.1
import QtWebKit 3.0
import QtWebKit.experimental 1.0

Tab {
    id: exampleView
    property string example
    title: "Example %1".arg(example)
    Row {
        id: splitView
        property string qml: Qt.resolvedUrl('../examples/u1db-qt-example-%1.qml'.arg(example))
        property string html: 'file:////usr/share/u1db-qt/examples/u1db-qt-example-%1.html'.arg(example)
        anchors.fill: parent
        Loader {
            width: parent.width / 3
            source: splitView.qml
            asynchronous: true
        }
        // TODO: syntax highlighting
        // FIXME: switching tabs with web views may crash lp#1124065
        WebView {
            width: parent.width / 3
            height: parent.height
            url: splitView.qml
            // FIXME: default font size is extremely small lp#1169989
            experimental.preferences.minimumFontSize: units.dp(24)
        }
        WebView {
            width: parent.width / 3
            height: parent.height
            url: splitView.html
            experimental.preferences.minimumFontSize: units.dp(24)
            // TODO: open help browser onNavigationRequested: { url }
        }
    }
}


