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
import Ubuntu.Components 0.1

MainView {
    width: units.gu(60)
    height: units.gu(80)

    Page {
       Tabs {
            /* FIXME: lp#1167568 Repeater {
                model: ["1", "2", "2b", "3", "4", "5"]
                SplitView {
                    example: modelData
                }
            } */
            SplitView { example: "1" }
            SplitView { example: "2" }
            SplitView { example: "2b" }
            SplitView { example: "3" }
            SplitView { example: "4" }
            SplitView { example: "5" }
        }
    }
}

