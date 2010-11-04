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
 * @subpackage Patching
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

        this._patchingSettingsDiv = dfx.getId('patchingSettings');

        this._notify = data.config.notify;

        this._notifyToggleBtn        = GUI.getWidget('PatchingScreen-notifyUpdates');
        this._asapCheckBtnWdgt       = GUI.getWidget('PatchingScreen-checkAsapBtn');
        this._notificationRecipients = data.config.recipient;
        this._deleteNotificationUser = {};
        this._addNotificationUser    = [];


        if (this._notify === false) {
            self.hideRecipients();
        }

        if (this._notifyToggleBtn) {
            this._notifyToggleBtn.addToggleOnCallback(function(e) {
                self.showRecipients();
            });

            this._notifyToggleBtn.addToggleOffCallback(function(e) {
                self.hideRecipients();
            });
        }

        if (this._notify === false && this._notificationRecipients.length > 0) {
            this.hideRecipients();
        }

        // Attach events for removing the user from the notification list.
        var delNotifyBtns = dfx.getClass('PatchingScreen-notifyDelete', this._patchingSettingsDiv);
        this.attachRemoveRecipientEvents(delNotifyBtns);

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

    };

    this.showRecipients = function() {
        var settingMid = dfx.getClass('GUIScreenSettings-mid', this._patchingSettingsDiv)[0]
        var childLen = settingMid.childNodes.length;
        var notifLen = this._notificationRecipients.length;
        for (var i = (childLen - 1); i > (childLen - 2 - notifLen); i--) {
            dfx.showElement(settingMid.childNodes[i]);
        }
    },

    this.attachRemoveRecipientEvents = function(btns) {
        // Attach events for removing the user from the notification list.
        var self = this;
        dfx.foreach(btns, function(idx) {
            dfx.addEvent(btns[idx], 'click', function(e) {
                var userid = btns[idx].getAttribute('userid');
                if (dfx.hasClass(btns[idx], 'deleted') === true) {
                    dfx.removeClass(btns[idx], 'deleted');
                    dfx.removeClass(btns[idx].parentNode.parentNode, 'deleted');
                    self._deleteNotificationUser[userid] = false;
                } else {
                    dfx.addClass(btns[idx], 'deleted');
                    dfx.addClass(btns[idx].parentNode.parentNode, 'deleted');
                    self._deleteNotificationUser[userid] = true;
                }
            });

            return true;
        });
    },

    this.hideRecipients = function() {
        var settingMid = dfx.getClass('GUIScreenSettings-mid', this._patchingSettingsDiv)[0];
        var childLen   = settingMid.childNodes.length;
        var notifLen   = this._notificationRecipients.length;
        for (var i = (childLen - 1); i > (childLen - notifLen - 2); i--) {
            dfx.hideElement(settingMid.childNodes[i]);
        }
    },

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
        GUI.sendRequest('Patching', 'checkUpdateASAP', params, function(data) {
            if (data.result === 'updated') {
                GUI.sendRequest('Patching', 'getNextUpdateCheckTime', params, function(data) {
                    var text = dfx.getClass(
                        'PatchingScreen-nextCheckString',
                        self._patchingSettingsDiv
                    )[0];

                    dfx.setHtml(text, data.result);
                    self._asapCheckBtnWdgt.disable();
                });
            }
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
                GUI.sendRequest('User', 'getUserById', {userid: userid}, function(data) {
                    var c = '<div class="PatchingScreen-notifyRow">';
                    c    += '    <div class="PatchingScreen-notifyUserIcon">&nbsp;</div>';
                    c    += '    <div class="PatchingScreen-notifyName">';
                    c    += dfx.ucFirst(data.result.first_name) + ' ' + dfx.ucFirst(data.result.last_name);
                    c    += '</div>';
                    c    += '    <div userid="' + userid + '" class="PatchingScreen-notifyDelete">&nbsp;</div>';
                    c    += '</div>';
                    self._addNotificationUser.push(userid);
                    var settingMid = dfx.getClass('GUIScreenSettings-mid', self._patchingSettingsDiv)[0]

                    var targetIdx = (settingMid.childNodes.length - 1);
                    GUI.getWidget('patchingSettings').addItem('new', c, false, targetIdx);

                    var delNotifyBtn = dfx.getClass('PatchingScreen-notifyDelete', settingMid.childNodes[targetIdx]);
                    self.attachRemoveRecipientEvents(delNotifyBtn);
                });
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
