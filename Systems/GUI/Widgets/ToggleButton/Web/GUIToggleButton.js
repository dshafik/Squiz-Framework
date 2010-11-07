/**
 * JS Class for the Toggle Button Widget.
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

function GUIToggleButton(id, settings) {
    this.id       = id;
    this.settings = settings;

    GUI.addWidgetEvent(this, 'toggleOn');
    GUI.addWidgetEvent(this, 'toggleOff');

    this.currentValue = settings.value || false;

    this.init();
}

GUIToggleButton.prototype = {
    init: function()
    {
        var self = this;
        dfx.addEvent(dfx.getId(self.id), 'click', function() {
            if (self.currentValue === true) {
                // move to false
                self.setValue(false);
            } else {
                // move to true
                self.setValue(true);
            }
        });

    },

    getValue: function() {
        return this.currentValue;
    },

    setValue: function(value, isInitValue) {
        var self = this;
        if (value !== self.currentValue) {
            // Only do something if the value is different from the current value
            self.currentValue = value;
            if (value === true) {
                // move to true
                self.switchOn(isInitValue);
            } else {
                // move to false
                self.switchOff(isInitValue);
            }
        }
    },

    switchOff: function(isInitValue) {
        var sec = 300;
        var self     = this;
        var widgetElement = dfx.getId(self.id);
        var glowMask = dfx.getClass('toggle', widgetElement)[0];

        if (isInitValue !== true) {
            self._setModified.call(self, true);
        }

        if (isInitValue === true) {
            var optYes = dfx.getClass('yes', widgetElement)[0];
            dfx.removeClass(optYes, 'selected');
            var optNo = dfx.getClass('no', widgetElement)[0];
            dfx.addClass(optNo, 'selected');
            dfx.setStyle(glowMask, 'left', '50px');
        } else {
            dfx.animate(glowMask, {left: 50}, sec, function() {
                var optYes = dfx.getClass('yes', widgetElement)[0];
                dfx.removeClass(optYes, 'selected');
                var optNo = dfx.getClass('no', widgetElement)[0];
                dfx.addClass(optNo, 'selected');

                // finish, fire event
                if (self.settings.toggleAction) {
                    eval(self.settings.toggleAction);
                }

                self.fireToggleOffCallbacks();
            });
        }
    },

    switchOn: function(isInitValue) {
        var sec = 300;
        var self     = this;
        var widgetElement = dfx.getId(self.id);
        var glowMask = dfx.getClass('toggle', widgetElement)[0];

        if (isInitValue !== true) {
            self._setModified.call(self, true);
        }

        if (isInitValue === true) {
            var optYes = dfx.getClass('yes', widgetElement)[0];
            dfx.addClass(optYes, 'selected');
            var optNo = dfx.getClass('no', widgetElement)[0];
            dfx.removeClass(optNo, 'selected');
            dfx.setStyle(glowMask, 'left', '6px');
        } else {
            dfx.animate(glowMask, {left: 6}, sec, function() {
                var optYes = dfx.getClass('yes', widgetElement)[0];
                dfx.addClass(optYes, 'selected');
                var optNo = dfx.getClass('no', widgetElement)[0];
                dfx.removeClass(optNo, 'selected');

                // finish, fire event
                if (self.settings.toggleAction) {
                    eval(self.settings.toggleAction);
                }

                self.fireToggleOnCallbacks();
            });
        }
    },

    revert: function()
    {
        if (this.getValue() !== this.settings.value) {
            this.setValue(this.settings.value);
        }

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

}
