/**
 * JS Class for the SuperUsers Screen.
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
 * @subpackage User
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var UserSuperUsersScreen = new function()
{
    var _screenData = null;

    this.init = function(screenData) {
        _screenData = screenData;
    };

    this.addUser = function() {
        var options = {
            modal: true
        };

        // TODO: Use project asset etc..
        var templateSettings = {
            rootNode: _screenData.usersFolderid
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(function(selection) {
                self.addSelectedUsers(selection);
            });
        }, options);
    },

    this.addSelectedUsers = function(userids) {
        var rowsData = {};
        dfx.foreach(userids, function(i) {
            var rowid       = userids[i];
            rowsData[rowid] = userids[i];
        });

        // Get the Table widget and add a new row.
        GUI.getWidget('superUsersList').generateRows(rowsData, function() {});
    };

};
