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
 * Construct GUITextArea JS object.
 *
 * @param {String} id ID of the widget.
 * @param {Object} settings Settings for the widget.
 * @return {Void}
 */
function GUITextArea(id, settings)
{
    this.id           = id;
    this.settings     = settings;
    var widgetElement = dfx.getId(self.id);
    this.textArea     = dfx.getClass('textarea', widgetElement)[0];

    // Add itemSelected widget event.
    GUI.addWidgetEvent(this, 'keyUp');

    this.init();

}

GUITextArea.prototype = {
    init: function()
    {
        var self          = this;
        var widgetElement = dfx.getId(self.id);
        var textArea      = dfx.getClass('textarea', widgetElement)[0];

        dfx.addEvent(textArea, 'blur', function() {
            dfx.removeClass(textArea, 'selected');
        });

        dfx.addEvent(textArea, 'focus', function() {
            dfx.addClass(textArea, 'selected');
        });

        var value = textArea.value;
        dfx.addEvent(textArea, 'keyup', function(e) {
            if (value !== this.value) {
                // If the value of the box has changed, set it modified.
                value = this.value;
                GUI.setModified(self, true);
            }

            // Fire textChanged widget event when a new value is selected.
            self.fireKeyUpCallbacks(value, e);
        });

    },

    getValue: function()
    {
        var widgetElement = dfx.getId(this.id);
        var textArea      = dfx.getClass('textarea', widgetElement)[0];
        return textArea.value;

    },

    setValue: function(value)
    {
        var widgetElement = dfx.getId(this.id);
        var textArea      = dfx.getClass('textarea', widgetElement)[0];
        if (value !== textArea.value) {
            // Only do something if the value is different from the current value.
            textArea.value = value;
            GUI.setModified(this, true);
        }

    },

    focus: function()
    {
        this.textArea.focus();

    },

    select: function()
    {
        this.textArea.select();

    }

}
