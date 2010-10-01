/**
 * JS Class for the List Widget.
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
  * GUIList widget.
  *
  * @param {string} id       The id of the widget.
  * @param {object} settings The settings of the widget.
  *
  * @constructor
  */
function GUIList(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.elem         = dfx.getId(id);
    this._loadingItem = null;

    if (settings.sortable === true) {
        jQuery(this.elem).sortable({
            axis: 'y',
            handle: 'div.GUIList-dragHandle',
            stop: function() {}
        });
    }

}

GUIList.prototype = {
    toggleDelete: function(itemElement)
    {
        var deleteIcon = dfx.getClass('GUI-delete', itemElement)[0];
        if (!deleteIcon) {
            return;
        }

        if (dfx.hasClass(deleteIcon, 'deleted') === false) {
            dfx.addClass(deleteIcon, 'deleted');
        } else {
            dfx.removeClass(deleteIcon, 'deleted');
        }

        GUI.setModified(this, true);

    },

    getValue: function()
    {
        var items   = {};
        var removed = {};
        for (var node = this.elem.firstChild; node; node = node.nextSibling) {
            if (dfx.isTag(node, 'li') === false) {
                continue;
            }

            var itemid = dfx.attr(node, 'itemid');

            // Check if item is deleted.
            var deleteIcon = dfx.getClass('GUI-delete', node)[0];
            if (dfx.hasClass(deleteIcon, 'deleted') === true) {
                removed[itemid] = true;
            } else {
                items[itemid] = true;
            }
        }

        var retValue = {
            items: items,
            removed: removed
        };

        return retValue;

    },

    /**
     * Generates new items using a channel and specified item data.
     *
     * @param {object}   itemsData Data for the columns.
     * @param {function} callback  The function to call once the row is added.
     * @param {string}   system    The name of the system that action belongs to. Optional.
     * @param {string}   action    The name of the action to call. Optional.
     *
     * @return {void}
     */
    generateItems: function(itemsData, callback, system, action)
    {
        var channel = this.settings.itemsGenerator;
        if (system && action) {
            channel = {
                system: system,
                action: action
            };
        }

        if (!this._loadingItem) {
            this._loadingItem = document.createElement('li');
            dfx.addClass(this._loadingItem, 'loadingItem');

            var loadingItemCont = '<div class="GUIList-dragHandle"></div>';
            loadingItemCont    += '<div class="GUIList-itemContent"><img src="' + GUI.getWidgetURL('GUI/Table') + '/ajax-loader.gif" /></div>';
            loadingItemCont    += '<div class="GUI-delete"><span class="GUI-deleteIcon" onclick="GUI.getWidget(\'' + this.id + '\').toggleDelete(this.parentNode.parentNode);"/></div>';
            dfx.setHtml(this._loadingItem, loadingItemCont);
        }

        this.elem.appendChild(this._loadingItem);

        var params = {
            itemsData: dfx.jsonEncode(itemsData),
            channel: dfx.jsonEncode(channel),
            settings: dfx.jsonEncode(this.settings)
        };

        var self = this;
        GUI.sendRequest('GUIList', 'generateItems', params, function(rowHTML) {
            var tmp = document.createElement('div');
            dfx.setStyle(tmp, 'display', 'none');

            // Add the tmp div to DOM so that any JS script retrieved will work with
            // dfx.getId() etc.
            document.body.appendChild(tmp);

            dfx.setHtml(tmp, rowHTML);

            // Remove loading row.
            dfx.remove(self._loadingItem);

            self.elem.appendChild(tmp.firstChild);

            dfx.remove(tmp);
            GUI.setModified(self, true);

            if (callback) {
                callback.call(self, rows);
            }
        }, 'raw');

    }

};

