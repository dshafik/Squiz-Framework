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

    this.cancelCreate = function() {
        // Hide the create containers.
        dfx.hideElement(dfx.getId('UserManagerScreen-createSection'));
        dfx.hideElement(dfx.getId('UserManagerScreen-createGroup'));
        dfx.hideElement(dfx.getId('UserManagerScreen-createUser'));
        dfx.showElement(dfx.getId('UserManagerScreen-infoSection'));
    };

    this.createNewUser = function() {
        dfx.hideElement(dfx.getId('UserManagerScreen-infoSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-createSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-createUser'));
    };

    this.createUser = function() {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');

        var userDetails = {
            username: GUI.getWidget('UserManagerScreen-newUser-username').getValue(),
            firstName: GUI.getWidget('UserManagerScreen-newUser-firstName').getValue(),
            lastName: GUI.getWidget('UserManagerScreen-newUser-lastName').getValue(),
            email: GUI.getWidget('UserManagerScreen-newUser-email').getValue(),
            password: GUI.getWidget('UserManagerScreen-newUser-password').getValue(),
            userGroups: dfx.jsonEncode(browserWidget.getValue())
        };

        GUI.sendRequest('User', 'createUser', userDetails, function(response) {
            if (response && response.result) {
                browserWidget.reload();
            }
        });

    };

    this.createNewUserGroup = function() {
        dfx.hideElement(dfx.getId('UserManagerScreen-infoSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-createSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-createGroup'));
    };

    this.createUserGroup = function() {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');

        var details = {
            name: GUI.getWidget('UserManagerScreen-newGroup-groupName').getValue(),
            parent: dfx.jsonEncode(browserWidget.getValue())
        };

        GUI.sendRequest('User', 'createUserGroup', details, function(response) {
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

    this.addExistingUserGroup = function() {
        _showAssetPicker(function(selection) {
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

    };

    this.addToGroups = function() {
        _showAssetPicker(function(userids) {
           if (userids.length <= 0) {
               return;
           }

           var groupsList = GUI.getWidget('UserManagerScreen-parentsList');
           if (!groupsList) {
               return;
           }

           var itemsData = {};
           dfx.foreach(userids, function(i) {
               itemsData[userids[i]] = userids[i];
           });

           groupsList.generateItems(itemsData);

        });

    },

    this.resetPassword = function() {
        var elem   = dfx.getId('UserManagerScreen-editExistingUser-resetPasswordBox');
        var button = dfx.getId('UserManagerScreen-editUser-resetPassword');
        dfx.remove(button);
        dfx.showElement(elem);
    };

    this.toggleUserStatus = function() {
        var status = GUI.getWidget('UserManagerScreen-editUser-toggleStatus').getValue();
        var header = dfx.getClass('UserManagerScreen-status-header')[0];
        if (status === true) {
            dfx.removeClass(header, 'Inactive');
        } else {
            dfx.addClass(header, 'Inactive');
        }

    };

    this.getValue = function() {
        console.info(111);
    };

    var _showAssetPicker = function(callback) {
        var options = {
            modal: true
        };

        // TODO: Get users folder id.
        var templateSettings = {
            rootNode: 1
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(callback);
        }, options);

    };

};
