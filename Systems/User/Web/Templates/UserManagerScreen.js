/**
 * JS Class for the UserManager Screen.
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

var UserUserManagerScreen = new function()
{
    this.init = function()
    {
        // Add the itemClicked event call back for the column browser.
        var browser = GUI.getWidget('userManager-assetBrowser');
        if (browser) {
            browser.addItemClickedCallback(function(itemid, childCount, itemElement) {
                // Get edit template contents for the item.
                _loadEditContents(itemid);
            });
        }

    };

    var _loadEditContents = function(assetid) {
        var params = {
            assetid: assetid
        };

        GUI.loadContent('User', 'loadUserManagerEditContents', dfx.getId('userManager-editPane'), params);

    };

    this.cancelCreateUser = function() {
        // Show the Group details.
        dfx.hideElement(dfx.getId('UserManagerScreen-editUserGroup-createUser'));
        dfx.showElement(dfx.getId('UserManagerScreen-editUserGroup-groupInfo'));
        dfx.showElement(dfx.getId('UserManagerScreen-editFolder-groupInfo'));
    };

    this.createNewUser = function() {
        dfx.hideElement(dfx.getId('UserManagerScreen-editUserGroup-groupInfo'));
        dfx.hideElement(dfx.getId('UserManagerScreen-editFolder-groupInfo'));
        dfx.showElement(dfx.getId('UserManagerScreen-editUserGroup-createUser'));
    };

    this.createUser = function() {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');

        var userDetails = {
            username: GUI.getWidget('UserManagerScreen-editUserGroup-newUser-username').getValue(),
            firstName: GUI.getWidget('UserManagerScreen-editUserGroup-newUser-firstName').getValue(),
            lastName: GUI.getWidget('UserManagerScreen-editUserGroup-newUser-lastName').getValue(),
            email: GUI.getWidget('UserManagerScreen-editUserGroup-newUser-email').getValue(),
            password: GUI.getWidget('UserManagerScreen-editUserGroup-newUser-password').getValue(),
            userGroups: dfx.jsonEncode(browserWidget.getValue())
        };

        GUI.sendRequest('User', 'createUser', userDetails, function(response) {
            if (response && response.result) {
                browserWidget.reload();
            }
        });

    };

    this.addExistingUser = function() {
        var options = {
            modal: true
        };

        // TODO: Get users folder id.
        var templateSettings = {
            rootNode: 1
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(function(selection) {
                if (selection.length <= 0) {
                    return;
                }

                var browserWidget = GUI.getWidget('userManager-assetBrowser');
                var params = {
                    userids: dfx.jsonEncode(selection),
                    userGroups: dfx.jsonEncode(browserWidget.getValue())
                };

                GUI.sendRequest('User', 'addUsersToGroups', params, function(response) {
                    if (response && response.result) {
                        browserWidget.reload();
                    }
                });

            });
        }, options);

    };

    this.resetPassword = function() {
        var elem   = dfx.getId('UserManagerScreen-editUser-editExistingUser-resetPasswordBox');
        var button = dfx.getId('UserManagerScreen-editUser-resetPassword');
        dfx.remove(button);
        dfx.showElement(elem);
    };

    this.toggleUserStatus = function() {
        var status = GUI.getWidget('UserManagerScreen-editUser-toggleStatus').getValue();
        var header = dfx.getClass('UserManagerScreen-editUser-status-header')[0];
        if (status === true) {
            dfx.removeClass(header, 'Inactive');
        } else {
            dfx.addClass(header, 'Inactive');
        }

    };


};
