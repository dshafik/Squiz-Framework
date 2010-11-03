/**
 * JS Class for the Screen Switcher Widget.
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

function GUIScreenSwitcher(id, settings)
{
    GUIContentSwitcher.call(this, id, settings);
    this.className = 'GUIScreenSwitcher';

    this.elem       = dfx.getId(this.id);
    this.targetid   = this.settings.target;
    this.targetElem = dfx.getId(this.targetid);

    this.includedFiles = {};
    this.loadedWidgets = [];

    GUI.addWidgetEvent(this, 'screenLoadStarts');
    GUI.addWidgetEvent(this, 'screenLoadFinished');

    // Add the inital item template to GUI's template list.
    var itemsCount = settings._items.length;
    for (var i = 0; i < itemsCount; i++) {
        var itemid = settings._items[i].id;
        if (itemid === settings.initialItem) {
            var itemSystem = settings._items[i].system;
            // Add the initially loaded template.
            GUI.addTemplate(itemSystem + ':' + itemid);
            break;
        }
    }

}

GUIScreenSwitcher.prototype = {

    init: function()
    {
        var self = this;

        dfx.foreach(this.settings._items, function(idx) {
            var itemId     = self.settings._items[idx].id;
            var itemSystem = self.settings._items[idx].system;
            var domId      = self.id + '-' + itemId;

            dfx.addEvent(dfx.getId(domId), 'click', function(evt) {
                var current = self.current;
                if (self.loadMode(itemSystem, itemId) !== false) {
                    if (current !== null) {
                        var oldDomId = self.id + '-' + current.modeid;

                        dfx.removeClass(dfx.getId(oldDomId), 'hover');
                        dfx.addClass(dfx.getId(oldDomId), 'inactive');
                        dfx.removeClass(dfx.getId(oldDomId), 'selected');
                    }//end if

                    dfx.addClass(this, 'selected');
                    dfx.removeClass(this, 'inactive');
                    dfx.removeClass(this, 'hover');

                    self.fireItemChangedCallbacks(itemId, itemSystem);
                }
            });

            dfx.addEvent(dfx.getId(domId), 'mouseover', function(evt) {
                var doEvent = true;
                if (self.current !== null) {
                    if (self.current.modeid === self.settings._items[idx].id) {
                        doEvent = false;
                    }
                }

                if (doEvent === true) {
                    if (self.settings._items[idx].disabled !== 'true') {
                        dfx.addClass(this, 'hover');
                    }
                }
            });

            dfx.addEvent(dfx.getId(domId), 'mouseout', function(evt) {
                var doEvent = true;
                if (self.current !== null) {
                    if (self.current.modeid === self.settings._items[idx].id) {
                        doEvent = false;
                    }
                }

                if (doEvent === true) {
                    if (self.settings._items[idx].disabled !== 'true') {
                        dfx.removeClass(this, 'hover');
                    }
                }
            });

            return true;
        });

    },

    showButton: function(template)
    {
        var parts = template.split(':');
        if (parts.length < 2) {
            return;
        }

        var button = dfx.getId(this.id + '-' + parts[1]);
        if (!button) {
            return;
        }

        return dfx.getClass('GUIScreenSwitcher-icon', button)[0];


    },

    loadMode: function(system, modeid)
    {
        // Unload the current template and call parent function.
        if (this.current !== null) {
            if (GUI.unloadTemplate(this.current.system + ':' + dfx.ucFirst(this.current.modeid)) === false) {
                return false;
            }
        }

        GUI.addTemplate(system + ':' + dfx.ucFirst(modeid));
        GUIContentSwitcher.prototype.loadMode.call(this, system, modeid);
        return true;
    }

};

dfx.noInclusionInherits('GUIScreenSwitcher', 'GUIContentSwitcher', true);
