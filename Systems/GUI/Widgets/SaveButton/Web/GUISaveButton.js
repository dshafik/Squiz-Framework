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

/**
 * GUISaveButton.
 *
 * @param {string} id       The id of the widget.
 * @param {Array}  settings Settings of the widget.
 *
 * @return void
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

    /**
     * Initialise the save button.
     *
     * Adds save button click events.
     *
     * @returns {void}
     */
    init: function()
    {
        var self           = this;
        this._saveButton   = dfx.getId(this.id + '-save');
        this._revertButton = dfx.getId(this.id + '-revert');

        GUI.addModifiedCallback(function(widgetid, modified) {
            if (modified === true) {
                self.enable();
            } else if (self._canDisable() === true) {
                self.disable();
            }
        });

        GUI.addRevertedCallback(function() {
            self.disable();
        });

        GUI.addTemplateRemovedCallback(function() {
            if (self._canDisable() === true) {
                self.disable();
            }
        });

        GUI.addSavedCallback(function() {
            if (self._canDisable() === true) {
                self.disable();
            }
        });

    },

    _canDisable: function()
    {
        // Check if there are modifed templates or widgets.
        if (GUI.hasModifiedTemplates() === false && GUI.hasModifiedWidgets() === false) {
            return true;
        }

        return false;

    },

    revert: function()
    {
        GUI.getWidget(this.id + '-intervention').show(this._revertButton);

    },

    /**
     * Enable the save button.
     *
     * @returns {void}
     */
    enable: function()
    {
        dfx.attr(this._saveButton, 'disabled', false);
        dfx.attr(this._revertButton, 'disabled', false);

        dfx.addClass(dfx.getId(this.id), this.className + 'Active');

    },

    /**
     * Disable the save button.
     *
     * @returns {void}
     */
    disable: function()
    {
        dfx.attr(this._saveButton, 'disabled', true);
        dfx.attr(this._revertButton, 'disabled', true);
        dfx.removeClass(dfx.getId(this.id), this.className + 'Active');

    }

};
