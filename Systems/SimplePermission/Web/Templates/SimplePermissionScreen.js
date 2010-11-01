/**
 * JS Class for the Simple Permission Screen.
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
 * @subpackage SimplePermission
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var SimplePermissionPermissionScreen = new function()
{
    var _usersFolderid = null;

    this.initScreen = function(usersFolderid) {
        _usersFolderid = usersFolderid;

        var self = this;
        GUI.getWidget('projectList').addItemSelectedCallback(function(selectedProjectid) {
            // When the item is clicked show the roles on the right side.
            self.showRolesForProject(selectedProjectid);
        });
    };

    this.showRolesForProject = function(projectid) {
        // Check if its already loaded.
        var elem = dfx.getId('PermissionScreen-rolesLists-' + projectid);
        if (elem) {
            dfx.removeClass(dfx.getClass('PermissionScreen-rolesLists'), 'active');
            dfx.addClass(elem, 'active');
            return;
        }

        var wrapperDiv = dfx.getId('PermissionScreenMainContent');

        elem = document.createElement('div');
        wrapperDiv.appendChild(elem);

        var params = {
            resourceid: projectid
        };

        dfx.removeClass(dfx.getClass('PermissionScreen-rolesLists'), 'active');
        GUI.loadContent('Permission', 'getPermissionListForScreen', elem, params, function() {
            wrapperDiv.appendChild(elem);
        });
    };

    this.addNew = function(resourceid, roleid) {
        var options = {
            modal: true
        };

        var templateSettings = {
            rootNode: _usersFolderid
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(function(selection) {
                self.addSelectedUsers(resourceid, roleid, selection);
            });
        }, options);
    },

    this.addSelectedUsers = function(resourceid, roleid, userids) {
        var rowsData = {};
        dfx.foreach(userids, function(i) {
            var rowid = userids[i];
            rowsData['new-' + rowid] = userids[i];
        });

        // Get the Table widget and add a new row.
        var listid = 'PermissionScreen-role-' + resourceid + '-' + roleid + '-list';
        GUI.getWidget(listid).generateItems(rowsData);
    };

    this.saved = function() {
        GUI.reloadTemplate('SimplePermission:PermissionScreen');
    };

};
