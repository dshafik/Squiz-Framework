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

function GUISelect(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.currentValue = [];

    this.init();
}

GUISelect.prototype = {
    init: function()
    {
        var self = this;
        var selectBox = dfx.getId(this.id);

        selectBox.getSelectedItems = function() {
            var selected = [];

            for (var i = 0; i < this.options.length; i++) {
                var option = this.options[i];

                if (option.selected === true) {
                    selected.push(option.value);
                }
            }
            return selected;
        }

        self.currentValue = selectBox.getSelectedItems();

        dfx.addEvent(selectBox, 'change', function() {
            var newValue = this.getSelectedItems();
            self.setValue(newValue);
        });
    },

    getValue: function()
    {
        return this.currentValue;

    },

    setValue: function(newValue)
    {
        var self = this;

        if (self.currentValue !== newValue) {
            self.currentValue = newValue;
            GUI.setModified(self, true);
        }
    }

};
