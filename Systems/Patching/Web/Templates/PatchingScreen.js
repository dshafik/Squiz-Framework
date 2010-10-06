/**
 * JS Class for the SquizSuite Screen.
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
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var PatchingPatchingScreen = new function()
{
    var _patchingSettingsWdgt = null;
    var _patchingSettingsDiv  = null;
    var _notifyToggleBtn      = null;
    var _newUpdatesTableDiv   = null;
    var _scheduleUpdateWdgt   = null;
    var _updateBoxDiv         = null;
    var _asapCheckBtnWdgt     = null;
    var _config = {};

    this.initScreen = function(data) {
        var self     = this;
        this._config = data.config;

        this._patchingSettingsDiv = GUI.getWidget('patchingSettings');
        this._patchingSettingsDiv = dfx.getId('patchingSettings');
        this._notifyToggleBtn     = GUI.getWidget('PatchingScreen-notifyUpdates');

        this._deleteNotificationUser = {};
        this._addNotificationUser    = [];

        // Remove the user from the notification list.
        var delNotifyBtns = dfx.getClass('PatchingScreen-notifyDelete', this._patchingSettingsDiv);
        dfx.foreach(delNotifyBtns, function(idx) {
            dfx.addEvent(delNotifyBtns[idx], 'click', function(e) {
                var userid = delNotifyBtns[idx].getAttribute('userid');
                if (dfx.hasClass(delNotifyBtns[idx], 'deleted') === true) {
                    dfx.removeClass(delNotifyBtns[idx], 'deleted');
                    dfx.removeClass(delNotifyBtns[idx].parentNode.parentNode, 'deleted');
                    self._deleteNotificationUser[userid] = false;
                } else {
                    dfx.addClass(delNotifyBtns[idx], 'deleted');
                    dfx.addClass(delNotifyBtns[idx].parentNode.parentNode, 'deleted');
                    self._deleteNotificationUser[userid] = true;
                }
            });

            return true;
        });

        // Attach expander events.
        this._updateBoxDiv = dfx.getId('updatesBox');
        var expanders      = dfx.getClass('PatchingScreen-expander', this._updateBoxDiv);
        dfx.foreach(expanders, function(idx) {
            dfx.addEvent(expanders[idx], 'click', function(e) {
                dfx.removeClass(
                    dfx.getClass('hidden', expanders[idx].parentNode.parentNode)[0],
                    'hidden'
                );
                dfx.addClass(expanders[idx].parentNode, 'hidden');
            });

            return true;
        });

        // Schedule new update toggle button.
        this._newUpdatesTableDiv = dfx.getId('newUpdates-table');
        this._scheduleUpdateWdgt = GUI.getWidget('PatchingScreen-schedulePatch');
        if (this._scheduleUpdateWdgt) {
            var datePickerWrap = dfx.getClass('PatchingScreen-scheduleDatePicker', this._newUpdatesTableDiv)[0];
            this._scheduleUpdateWdgt.addToggleOnCallback(function() {
                dfx.removeClass(datePickerWrap, 'hidden');
            });

            this._scheduleUpdateWdgt.addToggleOffCallback(function() {
                dfx.addClass(datePickerWrap, 'hidden');
            });
        }

        GUI.setModified(this, true);
    };

    this.toggleActivation = function() {
        var activWrap  = dfx.getClass('PatchingScreen-activateWrap', this._patchingSettingsDiv)[0];
        var cancelWrap = dfx.getClass('PatchingScreen-idTextFieldWrap', this._patchingSettingsDiv)[0];
        if (dfx.hasClass(activWrap, 'hidden') === true) {
            dfx.removeClass(activWrap, 'hidden');
            dfx.addClass(cancelWrap, 'hidden');
        } else {
            dfx.removeClass(cancelWrap, 'hidden');
            dfx.addClass(activWrap, 'hidden');
        }
    };

    this.changeActivationID = function() {
        dfx.addClass(dfx.getId('PatchingScreen-systemidWrap'), 'hidden');
        dfx.removeClass(dfx.getId('PatchingScreen-newActivationTextWrap'), 'hidden');

        dfx.addClass(dfx.getId('PatchingScreen-changeIdBtnWrap'), 'hidden');
        dfx.removeClass(dfx.getId('PatchingScreen-cancel-changeIdBtnWrap'), 'hidden');
    };

    this.cancelNewActivationID = function() {
        dfx.removeClass(dfx.getId('PatchingScreen-systemidWrap'), 'hidden');
        dfx.addClass(dfx.getId('PatchingScreen-newActivationTextWrap'), 'hidden');

        dfx.removeClass(dfx.getId('PatchingScreen-changeIdBtnWrap'), 'hidden');
        dfx.addClass(dfx.getId('PatchingScreen-cancel-changeIdBtnWrap'), 'hidden');
    };

    this.checkUpdateASAP = function() {
        var self   = this;
        var params = {};
        GUI.sendRequest('Patching', 'getNextUpdateCheckTime', params, function(data) {
            var text = dfx.getClass(
                'PatchingScreen-nextCheckString',
                self._patchingSettingsDiv
            )[0];

            dfx.setHtml(text, data.result);
        });
    };

    this.addNotificationUser = function() {
        var options = {
            modal: true
        };

        // TODO: Use project asset etc..
        var templateSettings = {
            rootNode: 2
        };

        var self = this;
        GUI.loadTemplate('GUIAssetPicker', 'GUIAssetPicker.tpl', templateSettings, function() {
            GUI.getWidget('AssetPicker').setSelectAssetsCallback(function(selection) {
                self.addSelectedUsers(selection);
            });
        }, options);
    };

    this.addSelectedUsers = function(users) {
        var self = this;
        dfx.foreach(users, function(idx) {
            var userid = parseInt(users[idx]);
            if (dfx.inArray(userid, self._config.recipient) === false
                && dfx.inArray(userid, self._addNotificationUser) === false
            ) {
                alert('New user added. Insert a new row to the notificationlist.');
                self._addNotificationUser.push(userid);
            }

            return true;
        });
    };

    this.getValue = function() {
        var data = {
            'deleteNotification' : this._deleteNotificationUser,
            'addNotification' : this._addNotificationUser
        };

        return data;
    };

    this.saved = function(retval) {
        GUI.reloadTemplate('Patching:PatchingScreen');
    };

}
