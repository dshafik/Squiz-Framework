/**
 * JS Class for the GUIPassword Widget.
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
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

 /**
  * Password widget.
  *
  * @param {string} id       The id of the widget.
  * @param {object} settings Settings for this widget.
  *
  * @constructor
  */
function GUIPassword(id, settings)
{
    this.id       = id;
    this.settings = settings;

    var elem          = dfx.getId(id);
    var text          = GUI.getWidget(id + '-confirmText');
    var passwordField = GUI.getWidget(id + '-main');

    this._barElem          = dfx.getClass('GUIPassword-strengthBar', elem)[0];
    this._strengthTextElem = dfx.getClass('GUIPassword-strengthText', elem)[0];
    this._matchStr         = dfx.getClass('GUIPassword-matchText', elem)[0];
    this._confirmInput     = GUI.getWidget(id + '-confirm');;
    this._passwordInput    = passwordField;

    text.addFocusCallback(function() {
        dfx.addClass(dfx.getClass('GUIPassword-confirmWrapper', elem), 'typing');
        dfx.getId(id + '-confirm-input').focus();
    });

    var self = this;
    passwordField.addKeyUpCallback(function(passwd) {
         var rating    = self.getPasswdRating(passwd);
         var className = self.getRatingClassName(rating);

         self.checkPasswordMatch();

         var barElem = dfx.getClass('GUIPassword-strengthBar', elem)[0];
         self.initRatingBar();
         dfx.addClass(barElem, className);
         self.setBarStrengthText(rating);
    });

    this._confirmInput.addKeyUpCallback(function(passwd) {
        self.checkPasswordMatch();
    });

}

GUIPassword.prototype = {
    getValue: function()
    {
        var value = '';
        if (this._passwordInput.getValue() === this._confirmInput.getValue()) {
            value = this._passwordInput.getValue();
        }

        return value;

    },

    getPasswdRating: function(val)
    {
        var score = 0;

        // Longer than 6 characters.
        if (val.length >= 6) {
            score++;
        }

        // Lower/upper case and numbers.
        if (val.match(/[a-z]+/) !== null && val.match(/[A-Z]+/) !== null && val.match(/[0-9]+/) !== null) {
            score++;
        }

        // Space.
        if (val.match(/[ ]+/) !== null) {
            score++;
        }

        // Special Char.
        if (val.match(/[!@#$%\^&*\?_~\-\(\)]+/) !== null) {
            score++;
        }

        // Longer than 12.
        if (val.length >= 12) {
            score++;
        }

        return score;

    },

    getRatingClassName: function(rating)
    {
        var classN = '';
        switch(rating) {
            case 0:
                classN = 'veryWeak';
            break;

            case 1:
                classN = 'weak';
            break;

            case 2:
                classN = 'better';
            break;

            case 3:
                classN = 'medium';
            break;

            case 4:
                classN = 'strong';
            break;

            case 5:
                classN = 'veryStrong';
            break;

            default:
                // No rating.
            break;
        }//end switch

        return classN;

    },

    initRatingBar: function()
    {
        this.setBarStrengthText(0);
        var classNames = ['veryWeak', 'weak', 'better', 'medium', 'strong', 'veryStrong'];
        var len        = classNames.length;
        for (var i = 0; i < len; i++) {
            dfx.removeClass(this._barElem, classNames[i]);
        }

    },

    setBarStrengthText: function(rating)
    {
        dfx.setHtml(this._strengthTextElem, this.settings._ratings[rating]);

    },

    checkPasswordMatch: function()
    {
        var index   = 0;
        var matched = false;
        if (this._passwordInput.getValue() === this._confirmInput.getValue()) {
            matched = true;
            index   = 1;
        }

        dfx.setHtml(this._matchStr, this.settings._matchStrs[index]);

        return matched;

    }

};
