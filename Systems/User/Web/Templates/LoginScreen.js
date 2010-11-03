/**
 * JS Class for the User Login Screen.
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

var UserLoginScreen = new function()
{
    this.hashEnabled = true;

    var _loadingAnim = null;
    if (document.images) {
        // Preload loading animation.
        _loadingAnim     = new Image(16,16);
        _loadingAnim.src = '/__web/Systems/User/Web/Templates/save_inact.gif'
    }

    this.displayError = function(msg) {
        dfx.setHtml(dfx.getId('LoginError'), msg);
    };

    this.submit = function() {
        var url = (new String(document.location)).valueOf();
        url     = url.replace('#', '');

        // Clear error message.
        dfx.setHtml(dfx.getId('LoginError'), '&nbsp;');

        var username = GUI.getWidget('loginName').getValue();
        var password = GUI.getWidget('loginPassword').getValue();
        if (!username || !password) {
            return false;
        }

        var params         = {};
        params.auth_action = 'login';

        var self = this;
        var _processLoginResponse = function(data) {
            data       = dfx.jsonDecode(data.result);
            var status = data.login;
            if (status === 0) {
                self.hideLoadingAnimation();
                // Display Errors here.
                self.displayError(data.error);
                return false;
            } else {
                window.location = url;
                return true;
            }
        };

        self.showLoadingAnimation();
        if (this.hashEnabled === true) {
            params.step     = 'req_chal';
            params.username = username;
            GUI.sendRequest('User', 'processAuthentication', params, function(data) {
                // Username exists and get the challenge string back.
                data = dfx.jsonDecode(data.result);
                var challenge = data.challenge;
                if (challenge !== 0) {
                    var hashedPassword = hex_md5(hex_hmac_sha1(challenge, hex_sha1(password)));
                    var browserInfo    = dfx.browser();
                    if (challenge === -1) {
                        // Plain text for ldap user.
                        // TODO: add SSL check.
                        hashedPassword = password;
                    }

                    params.step     = 'req_auth';
                    params.password = hashedPassword;
                    params.bt       = browserInfo.type;
                    params.bv       = browserInfo.version;
                    GUI.sendRequest('User', 'processAuthentication', params, _processLoginResponse);
                } else {
                    // Error occurred.
                    data         = dfx.jsonDecode(data.result);
                    var errorMsg = data.error;
                    self.displayError(errorMsg);
                    return false;
                }//end if
            });
        } else {
            params.step     = 'req_auth';
            params.username = username;
            params.password = password;
            GUI.sendRequest('User', 'processAuthentication', params, _processLoginResponse);
        }//end if

        return false;
    };

    this.showLoadingAnimation = function() {
        var button = dfx.getId('LoginSubmitButton');
        dfx.attr(button, 'disabled', true);
        button.blur();

        var buttonSpan = dfx.getTag('span', button)[0];
        dfx.setStyle(buttonSpan, 'visibility', 'hidden');
        var spinner = _loadingAnim;
        dfx.setStyle(spinner, 'top', '-15px');
        dfx.setStyle(spinner, 'position', 'relative');
        button.appendChild(spinner);
    }

    this.hideLoadingAnimation = function() {
        var button     = dfx.getId('LoginSubmitButton');
        var buttonSpan = dfx.getTag('span', button)[0];
        var spinner    = dfx.getTag('img', button)[0];
        dfx.remove(spinner);
        dfx.setStyle(buttonSpan, 'visibility', 'visible');
        dfx.attr(button, 'disabled', false);
    };

    this.setHashing = function(enabled) {
        this.hashEnabled = enabled;
    };

};
