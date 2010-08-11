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
        this.loadMode(system, modeid);

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

        if (this.current !== null) {
            GUI.unloadTemplate(this.current.system + ':' + dfx.ucFirst(this.current.modeid));
        }

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

        if (this.current !== null) {
            GUI.unloadTemplate(this.current.system + ':' + dfx.ucFirst(this.current.modeid));
        }

        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));

        this.current = {
            system: system,
            modeid: modeid
        };

    }

}