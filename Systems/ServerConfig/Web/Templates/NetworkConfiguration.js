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

var NetworkConfiguration = new function()
{
    var _deletedProducts    = {};
    var _ipTableDOM         = null;
    var _savingMsgDOM       = null;
    var _savingProgDOM      = null;
    var _completeMsgSuccess = null;
    var _completeMsgFailed  = null;
    var _confirmBtnDOM      = null;

    this.initScreen = function(data) {
        var self = this;

        this._ipTableDOM         = dfx.getId('ipTable');
        this._savingMsgDOM       = dfx.getId('savingMessage');
        this._savingProgDOM      = dfx.getId('savingProgress');
        this._completeMsgSuccess = dfx.getId('completeMessage-success');
        this._completeMsgFailed  = dfx.getId('completeMessage-failed');
        this._confirmBtnDOM      = dfx.getId('confirm');

        var confirmBtn = GUI.getWidget('confirm');
        confirmBtn.addClickCallback(function(e) {
            var templateid = 'ServerConfig:NetworkConfiguration';
            var params     = {
                action: 'saveNetworkConfig',
                values: GUI.getTemplateWidgetValues(templateid)
            };

            GUI.sendRequest('ServerConfig', 'saveNetworkConfiguration', params, function(data) {
                if (data.result === 'success') {
                    self.showProgress();

                    var iid = setInterval(function() {
                        params = {action: 'checkNetworkConfigUpdate'};
                        GUI.sendRequest('ServerConfig', 'saveNetworkConfiguration', params, function(res) {
                            if (res.result === 'true') {
                                self.showCompleteMessage('success');
                                clearInterval(iid);
                            } else if (res === 'false') {
                                self.showCompleteMessage('failed');
                                clearInterval(iid);
                            }
                        });
                    }, 10000);
                } else if (data.result === 'failed') {
                    alert('The new network settings caused the system to stop responding. The previous settings have been restored.');
                } else if (data.result === 'invalid') {
                    alert('Invalid IP address has been entered.');
                }//end if
            });
        });

        GUI.setModified(this, true);
    };

    this.showIPTable = function() {
        dfx.showElement(this._ipTableDOM);
        dfx.hideElement(this._savingMsgDOM);
        dfx.hideElement(this._savingProgDOM);
        dfx.hideElement(this._completeMsgSuccess);
        dfx.hideElement(this._completeMsgFailed);
        dfx.showElement(this._confirmBtnDOM);
    };

    this.showProgress = function() {
        dfx.hideElement(this._ipTableDOM);
        dfx.showElement(this._savingMsgDOM);
        dfx.showElement(this._savingProgDOM);
        dfx.hideElement(this._completeMsgSuccess);
        dfx.hideElement(this._completeMsgFailed);
        dfx.hideElement(this._confirmBtnDOM);
    };

    this.showCompleteMessage = function(type) {
        dfx.hideElement(this._ipTableDOM);
        dfx.hideElement(this._savingMsgDOM);
        dfx.hideElement(this._savingProgDOM);
        if (type === 'success') {
            dfx.showElement(this._completeMsgSuccess);
            dfx.removeClass(this._completeMsgSuccess, 'hidden')

            dfx.hideElement(this._completeMsgFailed);
        } else if (type === 'failed') {
            dfx.showElement(this._completeMsgFailed);
            dfx.removeClass(this._completeMsgFailed, 'hidden')

            dfx.hideElement(this._completeMsgSuccess);
        }

        dfx.hideElement(this._confirmBtnDOM);
    };

    this.getValue = function() {
        var data = {};
        return data;
    };

    this.saved = function(retval) {
        GUI.reloadTemplate('SquizSuite:SquizSuiteScreen');
    };

}
