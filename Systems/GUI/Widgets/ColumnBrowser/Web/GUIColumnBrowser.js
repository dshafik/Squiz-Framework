/**
 * JS Class for the GUIColumnBrowser Widget.
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
  * Column Browser widget.
  *
  * @param {string} id       ID of the widget.
  * @param {object} settings Settings for this widget.
  *
  * @constructor
  */
function GUIColumnBrowser(id, settings)
{
    this.id       = id;
    this.settings = settings;
    this.elem     = dfx.getId(this.id);

    this._loaderElem = null;

    this._lineageWidget = null;
    if (this.settings.lineageWidgetId) {
        this._lineageWidget = GUI.getWidget(this.settings.lineageWidgetId);
    }

    GUI.addWidgetEvent(this, 'itemClicked');

    this.init();

}

GUIColumnBrowser.prototype = {

    init: function()
    {
        var self = this;
        dfx.addEvent(this.elem, 'click', function(e) {
            self._handleClick(e);
        });

        // If a lineage widgetid is set then set its initial items.
        if (this._lineageWidget) {
            this._lineageWidget.setLineage(this.getLineage());
            this._lineageWidget.addItemClickedCallback(function(itemid, index) {
                self.lineageItemClicked(itemid, index);
            });
        }

        if (dfx.isset(this.settings.initialItemid) === true) {
            this.selectItem(this.settings.initialItemid, 0);
        }

    },

    _handleClick: function(e)
    {
        var target = dfx.getMouseEventTarget(e);
        var elem   = null;
        if ((elem = this._hasClass(target, 'GUIColumnBrowser-item', this.elem, true)) !== null) {
            this.itemClicked(elem, e);
        }

    },

    _hasClass: function(elem, className, stopElem, returnElement)
    {
        if (dfx.hasClass(elem, className) === true) {
            if (returnElement === true) {
                return elem;
            } else {
                return true;
            }
        } else if (elem.parentNode && elem.parentNode !== stopElem) {
            return this._hasClass(elem.parentNode, className, stopElem, returnElement);
        }

        return null;

    },

    selectItem: function(itemid, columnIndex)
    {
        if (columnIndex < 0) {
            // Last visible column.
            var visibleColumns = dfx.getClass('GUIColumnBrowser-column visible', this.elem);
            if (visibleColumns.length >= (columnIndex * -1)) {
                columnIndex = (visibleColumns.length + columnIndex);
            } else {
                return false;
            }
        }

        // First find the relevant column.
        var lookupClass = 'GUIColumnBrowser-column level-' + columnIndex;
        var columnElem  = dfx.getClass(lookupClass, this.elem);
        if (columnElem.length === 0) {
            return false;
        }

        // Now find the specified item in that column and call the itemClicked function
        // to select it.
        var items = dfx.getClass('GUIColumnBrowser-item', columnElem[0]);
        var iln   = items.length;

        for (var i = 0; i < iln; i++) {
            if (dfx.attr(items[i], 'itemid').toString() === itemid.toString()) {
                this.itemClicked(items[i]);
                return true;
            }
        }

        return false;

    },

    itemClicked: function(itemElement, e)
    {
        if (this.settings.enableMultiSelect !== true || !e || (e.ctrlKey !== true && e.metaKey !== true)) {
            dfx.removeClass(dfx.getClass('selected', itemElement.parentNode.parentNode), 'selected');
        }

        dfx.addClass(itemElement, 'selected');

        // Remove lastClicked class from all elements and add it to the new clicked element.
        // This class is used to determine the element that is the parent of next column,
        // since there can be more than 1 selected item we cannot use that..
        dfx.removeClass(dfx.getClass('lastClicked', itemElement.parentNode), 'lastClicked');
        dfx.addClass(itemElement, 'lastClicked');

        // Show the children of this item in the next column.
        this.showChildren(itemElement);

        this.fireItemClickedCallbacks(dfx.attr(itemElement, 'itemid'), dfx.attr(itemElement, 'childcount'), itemElement);

    },

    showChildren: function(parentElement, forceReload)
    {
        var parentid   = dfx.attr(parentElement, 'itemid');
        var childCount = dfx.attr(parentElement, 'childcount');

        // Get its level.
        var parentColumnElement = parentElement.parentNode.parentNode;
        var parentLevel         = parseInt(parentColumnElement.className.match(/level-(\d+)/)[1]);

        var lookupClass = 'GUIColumnBrowser-column level-' + (parentLevel + 1);

        var childColumnElement = dfx.getClass(lookupClass, this.elem);
        if (forceReload !== true && childColumnElement && childColumnElement.length > 0) {
            childColumnElement = childColumnElement[0];
            // Now find the card that needs to be shown.
            var cards = dfx.getClass('GUIColumnBrowser-column-card', childColumnElement);
            var cln   = cards.length;

            var card = null;
            for (var i = 0; i < cln; i++) {
                if (dfx.attr(cards[i], 'parentid') !== parentid) {
                    dfx.removeClass(cards[i], 'visible');
                } else {
                    card = cards[i];
                }
            }

            if (card === null && this.settings.dynamicLoading === true && childCount !== '0') {
                // Load card dynamically.
                this.loadChildren(parentElement);

                // Return here and let the callback call this function once it gets data back.
                return;
            }

            // Show the card and the column.
            dfx.addClass(card, 'visible');
            dfx.addClass(childColumnElement, 'visible');

            dfx.removeClass(dfx.getClass('GUIColumnBrowser-item selected', childColumnElement), 'selected');

            // The 'last' class represents the column that is shown last.
            dfx.removeClass(dfx.getClass('GUIColumnBrowser-column last', this.elem), 'last');
            dfx.addClass(childColumnElement, 'last');
        } else if (this.settings.dynamicLoading === true && childCount !== '0') {
            // Load card dynamically.
            this.loadChildren(parentElement);

            // Return here and let the callback call this function once it gets data back.
            return;
        }//end if

        dfx.removeClass(dfx.getClass('GUIColumnBrowser-column active', this.elem), 'active');
        dfx.addClass(parentColumnElement, 'active');

        var self = this;
        setTimeout(function() {
            jQuery(self.elem).scrollTo('100%', 200, {axis:'x'});
        }, 150);

        // Hide all other columns that shouldnt be visible.
        this._hideColumn((parentLevel + 2));

        // Update lineage widget if its available.
        if (this._lineageWidget) {
            this._lineageWidget.setLineage(this.getLineage());
        }

    },

    loadChildren: function(parentElement)
    {
        var parentid            = dfx.attr(parentElement, 'itemid');
        var parentColumnElement = parentElement.parentNode.parentNode;
        var parentLevel         = parseInt(parentColumnElement.className.match(/level-(\d+)/)[1]);

        var params = {
            settings: dfx.jsonEncode(this.settings),
            parentid: parentid
        };

        // Show the loading animation.
        if (!this._loaderElem) {
            this._loaderElem = document.createElement('div');
            dfx.addClass(this._loaderElem, 'GUIColumnBrowser-column-loader');
            dfx.setHtml(this._loaderElem, '<img src="' + GUI.getWidgetURL('GUI/AssetPicker') + '/white.gif" />');
        }

        // Set the wrapper width so that 'overflow: auto' works when the loader is added.
        var columnWrapperElem = dfx.getClass('GUIColumnBrowser-columnWrapper', this.elem)[0];
        var currentWidth      = dfx.getElementWidth(columnWrapperElem);
        var parentColWidth    = dfx.getElementWidth(parentColumnElement);
        dfx.setStyle(columnWrapperElem, 'width', currentWidth + parentColWidth + 'px');

        // Add the loader after the last visible column.
        dfx.insertAfter(parentColumnElement, this._loaderElem);

        var self = this;
        GUI.sendRequest('GUIColumnBrowser', 'getChildrenContents', params, function(contents) {
            var childLevel  = (parentLevel + 1);
            var lookupClass = 'GUIColumnBrowser-column level-' + childLevel;

            var childColumnElement = dfx.getClass(lookupClass, self.elem);
            if (!childColumnElement || childColumnElement.length === 0) {
                var childColumnElement = document.createElement('div');
                dfx.addClass(childColumnElement, 'GUIColumnBrowser-column');
                dfx.addClass(childColumnElement, 'level-' + childLevel);
                dfx.insertAfter(parentColumnElement, childColumnElement);
            } else {
                childColumnElement = childColumnElement[0];
            }

            var card = document.createElement('div');
            dfx.addClass(card, 'GUIColumnBrowser-column-card');
            dfx.attr(card, 'parentid', parentid);
            childColumnElement.appendChild(card);

            dfx.setHtml(card, contents);

            dfx.remove(self._loaderElem);

            // Now call showChildren again to apply classes.
            self.showChildren(parentElement);
        }, 'raw');

    },

    _hideColumn: function(colIndex)
    {
        var columns = dfx.getClass('GUIColumnBrowser-column', this.elem);
        var cln     = columns.length;

        var width = 0;
        for (var i = 0; i < cln; i++) {
            if (i >= colIndex) {
                dfx.removeClass(columns[i], 'visible');
                dfx.removeClass(dfx.getClass('GUIColumnBrowser-column-card visible', columns[i]), 'visible');
                dfx.removeClass(dfx.getClass('GUIColumnBrowser-item selected', columns[i]), 'selected');
            } else {
                width += dfx.getElementWidth(columns[i]);
            }
        }

        // Set the parent width so that 'overflow: auto' works.
        dfx.setStyle(dfx.getClass('GUIColumnBrowser-columnWrapper', this.elem), 'width', width + 'px');

    },

    getLineage: function(asArray)
    {
        var selectedItems = dfx.getClass('GUIColumnBrowser-item selected lastClicked', this.elem);
        var sln           = selectedItems.length;
        var lineage       = null;
        if (asArray === true) {
            lineage = [];
        } else {
            lineage = {};
        }

        for (var i = 0; i < sln; i++) {
            var selected = selectedItems[i];
            if (dfx.hasClass(selected.parentNode, 'visible') !== true) {
                continue;
            }

            // Find the last clicked element.
            var itemid = dfx.attr(selected, 'itemid');
            var title  = dfx.attr(selected, 'itemtitle');

            if (!itemid) {
                itemid = null;
            }

            if (asArray === true) {
                lineage.push(itemid);
            } else {
                lineage[itemid] = title;
            }
        }

        if (asArray === true) {
            lineage.unshift(null);
        }

        return lineage;

    },

    lineageItemClicked: function(itemid, index)
    {
        this.selectItem(itemid, index);

    },

    getSelectedElements: function()
    {
        var col = dfx.getClass('GUIColumnBrowser-column active', this.elem);

        // Now find the selected elements.
        var selectedElems = dfx.getClass('GUIColumnBrowser-item selected', col);

        return selectedElems;

    },

    getValue: function()
    {
        var selectedElems = this.getSelectedElements();
        var sln           = selectedElems.length;
        var selection     = [];
        for (var i = 0; i < sln; i++) {
            selection.push(dfx.attr(selectedElems[i], 'itemid'));
        }

        return selection;

    },

    /**
     * Reloads the current selection's parent.
     */
    reload: function()
    {
        var selected = this.getValue();
        var lineage  = this.getLineage(true);

        var params = {
            settings: dfx.jsonEncode(this.settings),
            lineage: dfx.jsonEncode(lineage)
        };

        var self = this;
        GUI.sendRequest('GUIColumnBrowser', 'reloadContents', params, function(contents) {
            var wrapper = dfx.getClass('GUIColumnBrowser-columnWrapper', self.elem)[0];
            dfx.empty(wrapper);
            dfx.setHtml(wrapper, contents);

            var selection = dfx.getClass('GUIColumnBrowser-item selected lastClicked', this.elem);
            selected      = selection.pop();
            dfx.trigger(selected, 'click');
        }, 'raw');

    }

};
