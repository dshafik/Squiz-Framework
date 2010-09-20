/**
 * JS Class for the Content Switcher Widget.
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

function GUIContentSwitcher(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = 'GUIContentSwitcher';

    this.loadedWidgets = [];

    this.current = null;
    var self     = this;

    if (settings.initialItem) {

        dfx.foreach(settings._items, function(idx) {
            var item = settings._items[idx];

            if (item.id === settings.initialItem) {
                self.current = {
                    system: item.system,
                    modeid: item.id
                };
            }
        });
    }

    GUI.addWidgetEvent(this, 'itemChanged');

    GUI.addReloadTemplateCallback(function(template) {
        if (!self.current) {
            return;
        }

        var currentTemplate = self.current.system + ':' + self.current.modeid;
        if (currentTemplate === template) {
            self.reload();
        }
    });

    this.init();

}

GUIContentSwitcher.prototype = {

    init: function()
    {
        this.addEvents();

    },

    addEvents: function()
    {
        var parentElem = dfx.getId(this.id);
        if (!parentElem) {
            return;
        }

        var buttons = dfx.getTag('li', parentElem);

        var self = this;
        dfx.addEvent(buttons, 'click', function(e) {
            self.buttonClicked(e.target);
        });

    },

    buttonClicked: function(button)
    {
        var modeid = button.getAttribute('modeid');
        var system = button.getAttribute('system');

        this._removeSelectedClass();
        dfx.addClass(button.parentNode, 'selected');

        this.fireItemChangedCallbacks(modeid, system);
        this.loadMode(system, modeid);

    },

    getVisibleContentElement: function()
    {
        var visibleElements = dfx.getClass(this.className + '-itemContent visible', dfx.getId(this.id + '-itemContents'));

        if (visibleElements.length > 0) {
            return visibleElements[0];
        }

        return null;

    },

     _removeSelectedClass: function()
    {
        dfx.removeClass(dfx.getTag('li', dfx.getId(this.id)), 'selected');

    },

    loadMode: function(system, modeid)
    {
        if (this.settings.loadType === 'static') {
            this.showMode(system, modeid);
        } else {
            this.loadDynamic(system, modeid);
        }

    },

    showMode: function(system, modeid)
    {
        var itemContentElem = dfx.getId(this.id + '-itemContent-' + modeid);
        if (!itemContentElem) {
            // Mode should have been loaded and its element should exist.
            return false;
        }

        var parent = dfx.getId(this.id + '-itemContents');
        if (!parent) {
            // Item contents wrapper not found.
            return false;
        }

        for (var node = parent.firstChild; node; node = node.nextSibling) {
            dfx.removeClass(node, 'visible');
        }

        dfx.addClass(itemContentElem, 'visible');

        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));

        this.current = {
            system: system,
            modeid: modeid
        };

    },

    loadDynamic: function(system, modeid)
    {
        var params = {
            system: system,
            modeid: modeid
        };

        GUI.loadContent(this.className, 'getDynamicItemContent', dfx.getId(this.settings.target), params);

        // Add the loaded template.
        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));

        this.current = {
            system: system,
            modeid: modeid
        };

    },

    reload: function()
    {
        this.loadMode(this.current.system, this.current.modeid);

    }

}