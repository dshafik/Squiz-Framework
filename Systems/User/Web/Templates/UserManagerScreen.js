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
    var _usersFolderid = 0;
    var _screenData    = {};
    var _assetTypes    = {};
    var _reloading     = false;

    this.init = function(data) {
        _usersFolderid = data.usersFolderid;
        _screenData    = data.editTemplates;
        _assetTypes    = data.assetTypes;

        // Add the itemClicked event call back for the column browser.
        var browser = GUI.getWidget('userManager-assetBrowser');
        if (browser) {
            browser.addItemClickedCallback(function(itemid, childCount, itemElement) {
                // Get edit template contents for the item.
                _loadEditContents(itemid);
            });

            browser.selectItem(data.localUsersFolderid, 0);
        }
    };

    var _loadEditContents = function(assetid) {
        var params = {
            assetid: assetid
        };


        // Check if template was modified.
        if (_reloading === false && GUI.isTemplateModified() === true) {
            if (GUI.unloadTemplate(GUI.getCurrentTemplate()) === false) {
                return false;
            }
        }

        GUI.loadContent('User', 'loadUserManagerEditContents', dfx.getId('userManager-editPane'), params, function() {
            _reloading = false;
            var parentsList = GUI.getWidget('UserManagerScreen-parentsList');
            if (!parentsList) {
                return;
            }

            // When an item is removed from parent's list we need to check if its the last item.
            parentsList.addItemRemovedCallback(function(elemid, removedElem, deleteIcon) {
                var value = parentsList.getValue();
                if (dfx.isEmpty(value.items) === true) {
                    // Items is empty therefore this asset will not have any parents
                    // and it will be deleted, make sure user confirms this action.
                    var interventionId = 'UserManagerScreen-lastLinkIntervention';
                    if (UserUserManagerScreen.isUserType() === true) {
                        interventionId += '-user';
                    } else {
                        interventionId += '-userGroup';
                    }

                    GUI.getWidget(interventionId).show(deleteIcon, deleteIcon);
                }
            });
        });
    };

    this.cancelGroupDelete = function(id) {
        var interventionWidget = GUI.getWidget(id);
        if (!interventionWidget) {
            return;
        }

        var data = interventionWidget.getData();
        if (data) {
            // Data should be the delte icon of the list item.
            dfx.trigger(data, 'click');
        }

        interventionWidget.hide();
    };

    this.cancelCreate = function() {
        // Hide the create containers.
        dfx.hideElement(dfx.getClass('UserManagerScreen-createPanel'));
        dfx.showElement(dfx.getId('UserManagerScreen-infoSection'));
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
                _reloadBrowser();
            }
        });
    };

    this.showCreatePanel = function(panelName) {
        dfx.hideElement(dfx.getId('UserManagerScreen-infoSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-createSection'));
        dfx.showElement(dfx.getId('UserManagerScreen-' + panelName));
    };

    this.createUserGroup = function() {
        var browserWidget = GUI.getWidget('userManager-assetBrowser');

        var details = {
            name: GUI.getWidget('UserManagerScreen-newGroup-groupName').getValue(),
            parents: dfx.jsonEncode(browserWidget.getValue())
        };

        GUI.sendRequest('User', 'createUserGroup', details, function(response) {
            if (response && response.result) {
                _reloadBrowser();
            }
        });
    };

    this.addExistingAsset = function() {
        _showAssetPicker(function(selection) {
            if (selection.length <= 0) {
                return;
            }

            var browserWidget = GUI.getWidget('userManager-assetBrowser');
            var params        = {
                userids: dfx.jsonEncode(selection),
                userGroups: dfx.jsonEncode(browserWidget.getValue())
            };

            GUI.sendRequest('User', 'addUsersToGroups', params, function(response) {
                if (response && response.result) {
                    _reloadBrowser();
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
               itemsData['new_' + userids[i]] = userids[i];
           });

           groupsList.generateItems(itemsData);
        });
    };

    this.toggleMarkForDeletion = function() {
        var id    = 'UserManagerScreen-deleteOverlay';
        var delEl = dfx.getId(id);
        if (!delEl) {
            // Add delete overlay.
            var del = document.createElement('div');
            del.id  = id;

            dfx.getId('userManager-editPane').appendChild(del);
            dfx.addClass(dfx.getId('userManager-deleteButton'), 'deleted');
        } else {
            dfx.remove(delEl);
            dfx.removeClass(dfx.getId('userManager-deleteButton'), 'deleted');
        }

        GUI.setModified('User::UserManagerScreen', true);

    };

    this.resetPassword = function() {
        var elem   = dfx.getId('UserManagerScreen-editExistingUser-resetPasswordBox');
        var button = dfx.getId('UserManagerScreen-editUser-resetPassword');
        dfx.remove(button);
        dfx.showElement(elem);
    };

    this.toggleUserStatus = function() {
        var status = GUI.getWidget('UserManagerScreen-editUser-existingUser-toggleStatus').getValue();
        var header = dfx.getClass('UserManagerScreen-status-header')[0];
        if (status === true) {
            dfx.removeClass(header, 'Inactive');
        } else {
            dfx.addClass(header, 'Inactive');
        }
    };

    this.isUserType = function(assetType) {
        if (!assetType) {
            var editorPanel = dfx.getId('UserManagerScreen-editor');
            assetType       = dfx.attr(editorPanel, 'assetType');
        }

        if (_assetTypes.user.inArray(assetType) === true) {
            return true;
        }

        return false;
    };

    this.getValue = function() {
        // Depending on which asset type is selected get the widget values.
        var editorPanel = dfx.getId('UserManagerScreen-editor');
        if (!editorPanel) {
            return;
        }

        var idPrefix  = 'UserManagerScreen';
        var saveData  = {};
        var assetType = dfx.attr(editorPanel, 'assetType');
        var assetid   = GUI.getWidget('userManager-assetBrowser').getValue().pop();

        if (dfx.getId('UserManagerScreen-deleteOverlay')) {
            saveData.deleted = true;
        } else {
            switch (assetType) {
                case 'user':
                    idPrefix           += '-editUser-existingUser';
                    saveData.first_name = GUI.getWidget(idPrefix + '-firstName').getValue();
                    saveData.last_name  = GUI.getWidget(idPrefix + '-lastName').getValue();
                    saveData.email      = GUI.getWidget(idPrefix + '-email').getValue();
                    saveData.username   = GUI.getWidget(idPrefix + '-username').getValue();
                    saveData.status     = GUI.getWidget(idPrefix + '-toggleStatus').getValue();
                    saveData.parents    = GUI.getWidget('UserManagerScreen-parentsList').getValue();
                    saveData.password   = GUI.getWidget(idPrefix + '-password').getValue();
                break;

                case 'userGroup':
                    idPrefix        += '-editUserGroup';
                    saveData.name    = GUI.getWidget(idPrefix + '-groupName').getValue();
                    saveData.parents = GUI.getWidget('UserManagerScreen-parentsList').getValue();
                break;

                default:
                    // Check if there is a implementer for this type.
                    if (!_screenData[assetType]) {
                        GUI.message('developer', 'Failed to get asset type from editor panel', 'warning');
                        return null;
                    } else {
                        var objName = _screenData[assetType].jsObjectName;
                        saveData    = window[objName].getValue(assetType);
                    }
                break;
            }//end switch
        }//end if

        saveData.assetType = assetType;
        saveData.assetid   = assetid;

        return saveData;
    };

    this.saved = function() {
        _reloadBrowser();
    };

    var _showAssetPicker = function(callback) {
        var options = {
            modal: true
        };

        var templateSettings = {
            rootNode: _usersFolderid,
            getChildrenFunction: ['User', 'getChildren']
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(callback);
        }, options);
    };

    var _reloadBrowser = function() {
        var template = GUI.getCurrentTemplate();
        GUI.setModified(template, false);

        _reloading = true;
        GUI.getWidget('userManager-assetBrowser').reload();
    };

};
