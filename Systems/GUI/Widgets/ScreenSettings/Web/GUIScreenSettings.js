/**
 * JS Class for the Screen Settings Widget.
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

function GUIScreenSettings(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.className = 'GUIScreenSettings';

    GUI.addWidgetEvent(this, 'itemSelected');
    GUI.addWidgetEvent(this, 'itemToggled');

    this.init();

}

GUIScreenSettings.prototype = {

    init: function()
    {
        if (this.settings.selectable === true
            || this.settings.removable === true
        ) {
            var self = this;
            dfx.addEvent(dfx.getId(this.id), 'click', function(e) {
                self._handleClickEvent(e)
            });
        }

    },

    _handleClickEvent: function(e)
    {
        var target = dfx.getMouseEventTarget(e);
        if (dfx.hasClass(target, 'GUI-deleteIcon') === true) {
            this.itemToggle(target.parentNode);
        } else if (dfx.hasClass(target, 'GUIScreenSettings-item') === true) {
            this.selectItem(target);
        }

    },

    selectItem: function(itemElement)
    {
        // Remove selected class from previous selected item.
        var prevSelected = dfx.getClass('GUIScreenSettings-item selected', dfx.getId(this.id));
        if (prevSelected.length > 0) {
            prevSelected = prevSelected[0];
        } else {
            prevSelected = null;
        }

        if (prevSelected !== itemElement) {
            dfx.removeClass(prevSelected, 'selected');
            dfx.addClass(itemElement, 'selected');
            this.fireItemSelectedCallbacks(dfx.attr(itemElement, 'itemid'), itemElement, prevSelected);
        }

    },

    getSelectedItemId: function()
    {
        var selectedItem = dfx.getClass('GUIScreenSettings-item selected', dfx.getId(this.id));
        return dfx.attr(selectedItem, 'itemid');

    },

    addItem: function(itemid, title, selected)
    {
        var template = this.settings._templates.item;

        template = template.replace('%title%', title);
        template = template.replace('%itemid%', itemid);

        var tmpEl = document.createElement('div');
        dfx.setHtml(tmpEl, template);
        var newElem = tmpEl.firstChild;

        var listCont = dfx.getClass('GUIScreenSettings-mid', dfx.getId(this.id))[0];
        listCont.appendChild(newElem);

        if (selected === true) {
            this.selectItem(newElem);
        }

        return newElem;

    },

    itemToggle: function(itemElement)
    {
        dfx.toggleClass(itemElement, 'deleted');
        this.fireItemToggledCallbacks(
            dfx.attr(itemElement.parentNode, 'itemid'),
            dfx.hasClass(itemElement, 'deleted'),
            itemElement.parentNode
        );

    },

    getValue: function()
    {
        var items = dfx.getClass(this.className + '-item', dfx.getId(this.id));
        var value = {};
        var self  = this;
        dfx.foreach(items, function(i) {
            var item         = items[i];
            var widgetValues = self.getWidgetValues(item);
            dfx.foreach(widgetValues, function(widgetid) {
                value[widgetid] = widgetValues[widgetid];
            });
        });

        return value;

    },

    getWidgetValues: function(itemElem)
    {
        // Find all the elements with ids.
        var elems  = dfx.find(itemElem, '[id]');
        var ln     = elems.length;
        var values = {};

        if (ln === 0) {
            return null;
        }

        for (var i = 0; i < ln; i++) {
            var elem = elems[i];
            var id   = elem.getAttribute('id');
            if (!id) {
                continue;
            }

            // Check if id is a valid widget id.
            var widget = GUI.getWidget(id);
            if (!widget) {
                continue;
            }

            // Check the widget has getValue() funciton.
            // Some widgets might not have any value to return.
            if (typeof widget.getValue !== 'function') {
                continue;
            }

            values[id] = widget.getValue();
        }//end for

        return values;

    }

};
