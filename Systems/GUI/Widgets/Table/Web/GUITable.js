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

/**
 * GUITable widget.
 *
 * @param {string} id       The id of the widget.
 * @param {object} settings The settings of the widget.
 *
 * @constructor
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
                } else if (dfx.isTag(target, 'div') === true && dfx.hasClass(target, 'GUI-delete') === true) {
                    self.toggleRow(target.parentNode.parentNode.parentNode);
                    break;
                } else if (dfx.hasClass(target, 'GUITable-colRemovedOverlay') === true) {
                    // Delete overlay was clicked, do nothing.
                    break;
                }

                target = target.parentNode;
            }
        });

    },

    _columnClicked: function(col) {},

    _rowClicked: function(row)
    {
        if (!row) {
            return;
        }

        if (this.settings.selectable === 'true') {
            var parent = row.parentNode;
            for (var node = parent.firstChild; node; node = node.nextSibling) {
                dfx.removeClass(node, 'selected');
            }

            dfx.addClass(row, 'selected');
        }

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

            dfx.setHtml(this._loadingRow, '<td colspan="' + colCount + '"></td>');
        }

        // Hide no Items msg.
        this.hideNoItemsMsg();

        tbody.appendChild(this._loadingRow);

        var params = {
            rowsData: dfx.jsonEncode(rowsData),
            channel: dfx.jsonEncode(channel),
            settings: dfx.jsonEncode(this.settings)
        };

        var self = this;
        GUI.sendRequest('GUITable', 'generateRows', params, function(rowHTML) {
            var tmp = document.createElement('div');
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
            self._setModified.call(self, true);

            if (callback) {
                callback.call(self, rows);
            }
        }, 'raw', function(exception) {
             // There was an error.. Remove the loading row.
            dfx.remove(self._loadingRow);
        });

    },


    /**
     * Generates new rows in bulk using a channel and specified params.
     *
     * This method should be used for multiple complex table rows (e.g. rows with
     * widgets), the channel specified must return array of rows with their column
     * contents.
     *
     * @param {object}   params   Params for the data provider.
     * @param {function} callback The function to call once the row is added.
     * @param {string}   system   The name of the system that action belongs to. Optional.
     * @param {string}   action   The name of the action to call. Optional.
     *
     * @return {void}
     */
    generateBulkRows: function(params, callback, system, action)
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

            dfx.setHtml(this._loadingRow, '<td colspan="' + colCount + '"></td>');
        }

        // Hide no Items msg.
        this.hideNoItemsMsg();

        tbody.appendChild(this._loadingRow);

        params.channel  = channel;
        params.settings = this.settings;

        for (var i in params) {
            params[i] = dfx.jsonEncode(params[i]);
        };

        var self = this;
        GUI.sendRequest('GUITable', 'generateBulkRows', params, function(rowHTML) {
            var tmp = document.createElement('div');
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
            self._setModified.call(self, true);

            if (callback) {
                callback.call(self, rows);
            }
        }, 'raw');

    },

    /**
     * Toggles removed row state.
     *
     * @param {DOMNode} rowElement The row element that needs to be removed/added.
     *
     * @returns {void}
     */
    toggleRow: function(rowElement)
    {
        if (dfx.hasClass(rowElement, 'deleted') === false) {
            dfx.addClass(rowElement, 'deleted');
        } else {
            dfx.removeClass(rowElement, 'deleted');
        }

        this._setModified(true);

    },

    getValue: function()
    {
        var elem = dfx.getId(this.id);
        if (!elem) {
            return null;
        }

        var tds = dfx.getTag('td', elem);
        var ln  = tds.length;

        var value   = {};
        var removed = {};

        for (var i = 0; i < ln; i++) {
            var td    = tds[i];
            var colid = td.getAttribute('colid');
            if (!colid || colid.indexOf(this.id) === -1) {
                continue;
            }

            var rowid = td.parentNode.getAttribute('rowid');

            if (dfx.hasClass(td.parentNode, 'deleted') === true) {
                removed[rowid] = true;
                continue;
            }

            var colIndex    = colid.replace(this.id + '-' + rowid + '-', '');
            var widgetValue = null;

            // Get column rowid.
            if (!value[rowid]) {
                value[rowid] = {}
            }

            var colWidget = GUI.getWidget(colid);
            if (colWidget) {
                widgetValue = colWidget.getValue();
            }

            value[rowid][colIndex] = widgetValue;
        }//end for

        var retValue = {
            items: value,
            removed: removed
        };

        return retValue;

    },

    saved: function()
    {
        // Table widget was saved so do some processing.
        // Remove the elements that were marked as deleted.
        var tbody    = dfx.getTag('tbody', this.elem)[0];
        var toRemove = [];
        for (var node = tbody.firstChild; node; node = node.nextSibling) {
            if (dfx.hasClass(node, 'deleted') === true) {
                toRemove.push(node);
            }
        }

        if (toRemove.length > 0) {
            dfx.remove(toRemove);
        }

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

    },

    showNoItemsMsg: function()
    {
        if (!this.settings.noItemsMsg) {
            return;
        }

        dfx.addClass(this.elem, 'noItems');

        var noItemsid  = this.id + '-noItemsMsg';
        var noItemsDiv = dfx.getId(noItemsid);
        if (!noItemsDiv) {
            noItemsDiv    = document.createElement('div');
            noItemsDiv.id = noItemsid;
            dfx.addClass(noItemsDiv, 'GUIList-noItemsMsg');
            dfx.setHtml(noItemsDiv, this.settings.noItemsMsg);
            dfx.insertAfter(this.elem, noItemsDiv);
        }

        dfx.addClass(noItemsDiv, 'noItems');

    },

    hideNoItemsMsg: function()
    {
        var noItemsDiv = dfx.getId(this.id + '-noItemsMsg');
        if (noItemsDiv) {
            dfx.removeClass(noItemsDiv, 'noItems');
        }

        dfx.removeClass(this.elem, 'noItems');

    },

    _setModified: function(status)
    {
        // If this widget does not require save then there is no reason to call setModified.
        if (this.settings.requiresSave === false) {
            return;
        }

        if (this.settings.enablesSaveButton === false) {
            GUI.setModified(this, status, true);
        } else {
            GUI.setModified(this, status, false);
        }

    }

};
