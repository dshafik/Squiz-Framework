/**
 * JS Class for the AssetPicker Widget.
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

 function GUIAssetPicker(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this._selectedAssetsCallback = null;

    this.init();

}

GUIAssetPicker.prototype = {
    init: function()
    {
        var linHeight   = parseInt(dfx.getElementHeight(dfx.getId(this.id + '-lineage')));
        var browserElem = dfx.getId(this.id + '-assetBrowser');

        var midElem    = dfx.getClass('GUIDialog-middle', dfx.getId(this.id + '-dialog'))[0];
        var initHeight = parseInt(dfx.getElementHeight(midElem));

        dfx.setStyle(browserElem, 'height', (initHeight - linHeight - 2) + 'px');

        GUI.getWidget(this.id + '-dialog').addDialogResizedCallback(function(ui, contentSize) {
            dfx.setStyle(browserElem, 'height', (contentSize.height - linHeight) + 'px');
        });

        GUI.getWidget(this.id + '-dialog').addDialogClosedCallback(function() {
            GUI.unloadTemplate('GUIAssetPicker:GUIAssetPicker');
            GUI.dequeueOverlay('normal');
        });

    },

    setSelectAssetsCallback: function(callback)
    {
        this._selectedAssetsCallback = callback;

    },

    selectAssets: function()
    {
        var selection = GUI.getWidget(this.id + '-dialog').getSelection();
        if (this._selectedAssetsCallback) {
            // If the callback method returns false then do not close the picker.
            if (this._selectedAssetsCallback.call(this, selection) === false) {
                return;
            }
        }

        this.close();

    },

    close: function()
    {
        GUI.getWidget(this.id + '-dialog').close();

    }

};
