/**
 * JS Class for the Viper Redo Plugin.
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
 * @package    CMS
 * @subpackage Editing
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

function ViperRedoPlugin(viper)
{
    ViperPlugin.call(this, viper);
    ViperPluginManager.addKeyPressListener('CTRL+Z', this, 'handleUndo');
    ViperPluginManager.addKeyPressListener('CTRL+Y', this, 'handleRedo');

}

ViperRedoPlugin.prototype = {
    start: function()
    {
        var self           = this;
        this.toolbarPlugin = ViperPluginManager.getPlugin('ViperToolbarPlugin');
        if (dfx.isset(this.toolbarPlugin) === true) {
            var name = 'Redo';

            var ctrlName = 'CTRL';
            if (navigator.platform.toLowerCase().indexOf('mac') !== false) {
                ctrlName = 'CMD';
            }

            this.toolbarPlugin.addButton(name, 'undo', 'Undo (' + ctrlName + ' + Z)', function () {
                return self.handleUndo();
            });
            this.toolbarPlugin.setButtonDisabled('undo');

            this.toolbarPlugin.addButton(name, 'redo', 'Redo (' + ctrlName + ' + Y)', function () {
                return self.handleRedo();
            });

            this.toolbarPlugin.setButtonDisabled('redo');
        }

        this.viper.registerCallback('ViperUndoManager:newUndoTask', 'ViperRedoPlugin', function() {
            self._updateButtonStates();
        });

    },

    handleUndo: function()
    {
        ViperUndoManager.undo();
        this._updateButtonStates();

        return true;

    },

    handleRedo: function()
    {
        ViperUndoManager.redo();
        this._updateButtonStates();

        return true;

    },

    _updateButtonStates: function()
    {
        if (!this.toolbarPlugin) {
            return;
        }

        if (ViperUndoManager.getUndoCount() > 1) {
            this.toolbarPlugin.setButtonInactive('undo');
        } else {
            this.toolbarPlugin.setButtonDisabled('undo');
        }

        if (ViperUndoManager.getRedoCount() > 0) {
            this.toolbarPlugin.setButtonInactive('redo');
        } else {
            this.toolbarPlugin.setButtonDisabled('redo');
        }

    }

};

dfx.noInclusionInherits('ViperRedoPlugin', 'ViperPlugin', true);
