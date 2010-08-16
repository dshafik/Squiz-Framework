/**
 * JS Class for the Save Button Widget.
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

function GUISaveButton(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = settings.widget.type;

    this._saveButton   = null;
    this._revertButton = null;

    this.init();

}

GUISaveButton.prototype = {

    init: function()
    {
        var self           = this;
        this._saveButton   = dfx.getId(this.id + '-save');
        this._revertButton = dfx.getId(this.id + '-revert');

        dfx.addEvent(this._saveButton, 'click', function() {
            GUI.save();
        });

        dfx.addEvent(this._revertButton, 'click', function() {
            GUI.revert();
        });

        GUI.addModifiedCallback(function(widgetid, modified) {console.info(modified);
            if (modified === true) {
                self.enable();
            } else {
                self.disable();
            }
        });

    },

    enable: function()
    {
        dfx.attr(this._saveButton, 'disabled', false);
        dfx.attr(this._revertButton, 'disabled', false);

        dfx.addClass(dfx.getId(this.id), this.className + 'Active');

    },

    disable: function()
    {
        dfx.attr(this._saveButton, 'disabled', true);
        dfx.attr(this._revertButton, 'disabled', true);
        dfx.removeClass(dfx.getId(this.id), this.className + 'Active');

    }

};