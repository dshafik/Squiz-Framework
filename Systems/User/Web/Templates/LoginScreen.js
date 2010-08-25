var LoginScreen = new function()
{
    this.hashEnabled = true;

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

        if (this.hashEnabled === true) {
            params.step     = 'req_chal';
            params.username = username;
            self.showLoadingAnimation();
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
            var browserInfo = dfx.browser();
            params.step     = 'req_auth';
            params.username = username;
            params.password = password;
            params.bt       = browserInfo.type;
            params.bv       = browserInfo.version;
            dfx.post(window.location.href, params, _processLoginResponse);
        }//end if

        return false;
    };

    this.showLoadingAnimation = function() {
        var button = dfx.getId('LoginSubmitButton');
        dfx.attr(button, 'disabled', true);
        button.blur();

        var buttonSpan = dfx.getTag('span', button)[0];
        dfx.setStyle(buttonSpan, 'visibility', 'hidden');
        var spinner = document.createElement('img');
        spinner.src = '/__web/Systems/User/Web/Templates/save_inact.gif';
        dfx.setStyle(spinner, 'margin-top', '-16px');
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

};
