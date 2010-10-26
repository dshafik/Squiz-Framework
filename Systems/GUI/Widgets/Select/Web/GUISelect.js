/**
 * JS Class for the Select Widget.
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
 * Construct GUISelect JS object.
 *
 * @param {String} id ID of the widget.
 * @param {Object} settings Settings for the widget.
 * @return {Void}
 */
function GUISelect(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.currentValue = [];

    // Add itemSelected widget event.
    GUI.addWidgetEvent(this, 'change');

    this.init();

}

GUISelect.prototype = {
    init: function()
    {
        var self      = this;
        var selectBox = dfx.getId(this.id);

        selectBox.getSelectedItems = function() {
            var selected = [];

            var len = this.options.length;
            for (var i = 0; i < len; i++) {
                var option = this.options[i];

                if (option.selected === true) {
                    selected.push(option.value);
                }
            }

            return selected;
        }

        self.currentValue = selectBox.getSelectedItems();

        dfx.addEvent(selectBox, 'change', function(e) {
            var newValue = this.getSelectedItems();
            self.setValue(newValue, e);
        });

    },

    getValue: function()
    {
        return this.currentValue;

    },

    setValue: function(newValue, domEvent)
    {
        if ((newValue instanceof Array) === false) {
            newValue = [newValue];
        }

        // If programmatic change, pass a null event.
        domEvent = domEvent || null;

        var selectBox = dfx.getId(this.id);

        selectBox.setSelectedItems = function(selected) {
            if (typeof selected !== 'object') {
                selected = [selected];
            }

            var len = this.options.length;
            for (var i = 0; i < len; i++) {
                var option = this.options[i];

                if (selected.inArray(option.value)) {
                    option.selected = true;
                } else {
                    option.selected = false;
                }
            }
        }

        var self = this;

        if (self.currentValue !== newValue) {
            self.currentValue = newValue;
            selectBox.setSelectedItems(newValue);
            GUI.setModified(self, true);

            self.fireChangeCallbacks(newValue, domEvent);
        }

    },

    revert: function()
    {
        this.setValue(this.settings.selected);
        GUI.setModified(self, false);
    }

};
