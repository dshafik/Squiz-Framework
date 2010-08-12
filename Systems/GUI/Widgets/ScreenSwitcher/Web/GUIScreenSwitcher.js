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

    this.currScreen = {
        system: null,
        id: null
    };

    this.includedFiles = {};
    this.loadedWidgets = [];

    GUI.addWidgetEvent(this, 'screenLoadStarts');
    GUI.addWidgetEvent(this, 'screenLoadFinished');

    this.init();

}

GUIScreenSwitcher.prototype = {

    init: function()
    {
        var self = this;

        dfx.foreach(this.settings.items, function(idx) {
            var screenid = self.id + '-' + self.settings.items[idx].id;
            dfx.addEvent(dfx.getId(screenid), 'click', function(evt) {
                if (self.currScreen.id !== self.settings.items[idx].id) {
                    if (self.settings.items[idx].disabled !== 'true') {
                        var oldScreenid = self.id + '-' + self.currScreen.id;

                        self.currScreen = {
                            system: self.settings.items[idx].system,
                            id: self.settings.items[idx].id
                        },

                        dfx.removeClass(dfx.getId(oldScreenid), 'hover');
                        dfx.addClass(dfx.getId(oldScreenid), 'inactive');
                        dfx.removeClass(dfx.getId(oldScreenid), 'selected');

                        dfx.addClass(this, 'selected');
                        dfx.removeClass(this, 'inactive');
                        dfx.removeClass(this, 'hover');

                        self.loadDynamic(
                            self.settings.items[idx].system,
                            self.settings.items[idx].id
                        );
                    }//end if
                }//end if
            });

            dfx.addEvent(dfx.getId(screenid), 'mouseover', function(evt) {
                if (self.currScreen.id !== self.settings.items[idx].id) {
                    if (self.settings.items[idx].disabled !== 'true') {
                        dfx.addClass(this, 'hover');
                    }
                }
            });

            dfx.addEvent(dfx.getId(screenid), 'mouseout', function(evt) {
                if (self.currScreen.id !== self.settings.items[idx].id) {
                    if (self.settings.items[idx].disabled !== 'true') {
                        dfx.removeClass(this, 'hover');
                    }
                }
            });

            return true;
        });

    }

};

dfx.noInclusionInherits('GUIScreenSwitcher', 'GUIContentSwitcher', true);
