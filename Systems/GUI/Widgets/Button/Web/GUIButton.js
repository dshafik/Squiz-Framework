/**
 * JS Class for the Button Widget.
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
 * Construct GUIButton JS object.
 *
 * @param {String} id ID of the widget.
 * @param {Object} settings Settings for the widget.
 * @return {Void}
 */
function GUIButton(id, settings)
{
    this.id       = id;
    this.settings = settings;
    this.button   = dfx.getId(this.id);

    // Add itemSelected widget event.
    GUI.addWidgetEvent(this, 'click');

    this.init();

}

GUIButton.prototype = {
    init: function()
    {
        var self = this;

        dfx.addEvent(this.button, 'click', function(e) {
            self.fireClickCallbacks(self.button, e);
        });

    },

    enable: function()
    {
        this.button.disabled = false;

    },

    disable: function()
    {
        this.button.disabled = true;

    },

    setValue: function(val)
    {
        this.button.value = val;
        var label = dfx.getTag('span', this.button)[0];
        dfx.setHtml(label, val);

    },

    getValue: function()
    {
        return this.button.value;

    }

}
