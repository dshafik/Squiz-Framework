/**
 * JS Class for the Table Widget.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

function GUITable(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.elem = dfx.getId(this.id);

    this._loadingRow = null;

    this.init();

}

GUITable.prototype = {

    init: function()
    {
        if (this.settings.sortable === 'true') {
            $("#" + this.id + "").tablesorter();
        }

        var self      = this;
        var tableElem = dfx.getId(this.id);

        if (this.settings.selectable === 'true') {
            // Rows are selectable add the click event.
            dfx.addEvent(tableElem, 'click', function(e) {
                var target = dfx.getMouseEventTarget(e);
                while (target && dfx.isTag(target, 'table') === false) {
                    if (dfx.isTag(target, 'td') === true) {
                        self._columnClicked(target);
                        // Note that we do not break here so that rowClicked event is
                        // also fired.
                    } else if (dfx.isTag(target, 'tr') === true) {
                        self._rowClicked(target);
                        break;
                    }

                    target = target.parentNode;
                }
            });
        }

    },

    _columnClicked: function(col) {},

    _rowClicked: function(row)
    {
        if (!row) {
            return;
        }

        var parent = row.parentNode;
        for (var node = parent.firstChild; node; node = node.nextSibling) {
            dfx.removeClass(node, 'selected');
        }

        dfx.addClass(row, 'selected');

    },


    /**
     * Generates a new row using a channel and specified row data.
     *
     * This method should be used for complex table rows (e.g. rows with widgets),
     * the channel specified must return array of column contents.
     *
     * @param {object}   rowsData Data for the columns.
     * @param {function} callback The function to call once the row is added.
     * @param {string}   system   The name of the system that action belongs to. Optional.
     * @param {string}   action   The name of the action to call. Optional.
     *
     * @return {void}
     */
    generateRows: function(rowsData, callback, system, action)
    {
        var channel = this.settings.rowGenerator;
        if (system && action) {
            channel = {
                system: system,
                action: action
            };
        }

        var tbody = dfx.getTag('tbody', this.elem)[0];

        if (!this._loadingRow) {
            this._loadingRow = document.createElement('tr');
            dfx.addClass(this._loadingRow, 'loadingRow');

            var colCount = 0;
            dfx.foreach(this.settings.columns, function() {
                colCount++;
            });

            dfx.setHtml(this._loadingRow, '<td colspan="' + colCount + '"><img src="' + GUI.getWidgetURL('GUI/Table') + '/ajax-loader.gif" /></td>');
        }

        tbody.appendChild(this._loadingRow);

        var params = {
            rowsData: dfx.jsonEncode(rowsData),
            channel: dfx.jsonEncode(channel),
            settings: dfx.jsonEncode(this.settings)
        };

        var self = this;
        GUI.sendRequest('GUITable', 'generateRows', params, function(data) {
            var rowHTML = data.result;
            var tmp     = document.createElement('div');
            dfx.setStyle(tmp, 'display', 'none');

            // Add the tmp div to DOM so that any JS script retrieved will work with
            // dfx.getId() etc.
            document.body.appendChild(tmp);

            dfx.setHtml(tmp, rowHTML);

            // Remove loading row.
            dfx.remove(self._loadingRow);

            // Get the last row.
            var currentRowCount = tbody.childNodes.length;

            var rows = [];
            while (tmp.firstChild) {
                currentRowCount++;

                rows.push(tmp.firstChild);
                if ((currentRowCount % 2) === 0) {
                    // Even.
                    dfx.removeClass(tmp.firstChild, 'rowOdd');
                    dfx.addClass(tmp.firstChild, 'rowEven');
                } else {
                    // Odd.
                    dfx.removeClass(tmp.firstChild, 'rowEven');
                    dfx.addClass(tmp.firstChild, 'rowOdd');
                }

                tbody.appendChild(tmp.firstChild);
            }

            dfx.remove(tmp);

            if (callback) {
                callback.call(self, rows);
            }
        });

    },

    getValue: function()
    {
        var elem = dfx.getId(this.id);
        if (!elem) {
            return null;
        }

        var tds = dfx.getTag('td', elem);
        var ln  = tds.length;

        var value = {};

        for (var i = 0; i < ln; i++) {
            var td    = tds[i];
            var colid = td.getAttribute('colid');
            if (!colid || colid.indexOf(this.id) === -1) {
                continue;
            }

            var colWidget = GUI.getWidget(colid);
            if (!colWidget) {
                continue;
            }

            // Get column rowid.
            var rowid = td.parentNode.getAttribute('rowid');
            if (!value[rowid]) {
                value[rowid] = {}
            }

            var colIndex = colid.replace(this.id + '-' + rowid + '-', '');

            var widgetValue        = colWidget.getValue();
            value[rowid][colIndex] = widgetValue;
        }//end for

        return value;

    },

    removeWidget: function()
    {
        // Remove all the widgets in each column.
        this.processColumns(function(colid, colWidget, td) {
            GUI.removeWidget(colWidget.id);
        });

    },

    processColumns: function(callback)
    {
        var elem = dfx.getId(this.id);
        if (!elem) {
            return null;
        }

        var tds = dfx.getTag('td', elem);
        var ln  = tds.length;

        var value = {};

        for (var i = 0; i < ln; i++) {
            var td    = tds[i];
            var colid = td.getAttribute('colid');
            if (!colid || colid.indexOf(this.id) === -1) {
                continue;
            }

            var colWidget = GUI.getWidget(colid);
            if (!colWidget) {
                continue;
            }

            callback.call(this, colid, colWidget, td);
        }

    }

};
