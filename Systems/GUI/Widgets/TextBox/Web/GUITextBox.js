/**
 * JS Class for the Text Box Widget.
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
 * Construct GUITextBox JS object.
 *
 * @param {String} id ID of the widget.
 * @param {Object} settings Settings for the widget.
 * @return {Void}
 */
function GUITextBox(id, settings)
{
    this.id           = id;
    this.settings     = settings;
    var widgetElement = dfx.getId(self.id);
    this.textBox      = dfx.getClass('input', widgetElement)[0];

    this.init();

    GUI.addWidgetEvent(this, 'keyPress');
    GUI.addWidgetEvent(this, 'keyUp');

}

GUITextBox.prototype = {
    init: function()
    {
        var self          = this;
        var widgetElement = dfx.getId(self.id);
        var textBox       = dfx.getClass('input', widgetElement)[0];

        dfx.addEvent(textBox, 'blur', function() {
            dfx.removeClass(textBox, 'selected');
        });

        dfx.addEvent(textBox, 'focus', function() {
            dfx.addClass(textBox, 'selected');
        });

        var value = textBox.value;
        dfx.addEvent(textBox, 'keypress', function(e) {
            if (value !== this.value) {
                // If the value of the box has changed, set it modified.
                value = this.value;
                GUI.setModified(self, true);
            }

            self.fireKeyPressCallbacks(value, e);
        });

        dfx.addEvent(textBox, 'keyup', function(e) {
            if (value !== this.value) {
                // If the value of the box has changed, set it modified.
                value = this.value;
                GUI.setModified(self, true);
            }

            self.fireKeyUpCallbacks(value, e);
        });

    },

    getValue: function()
    {
        var widgetElement = dfx.getId(this.id);
        var textBox       = dfx.getClass('input', widgetElement)[0];
        return textBox.value;

    },

    setValue: function(value)
    {
        var widgetElement = dfx.getId(this.id);
        var textBox       = dfx.getClass('input', widgetElement)[0];
        if (value !== textBox.value) {
            // Only do something if the value is different from the current value.
            textBox.value = value;
            GUI.setModified(this, true);
        }

    },

    focus: function()
    {
        this.textBox.focus();

    },

    select: function()
    {
        this.textBox.select();

    },

    revert: function()
    {
        var widgetElement = dfx.getId(this.id);
        var textBox       = dfx.getClass('input', widgetElement)[0];
        textBox.value     = this.settings.value;
        GUI.setModified(this, false);

    }

}
