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
    var _patchingSettingsDiv = null;
    var _notifyToggleBtn     = null;

    this.initScreen = function(data) {
        var self = this;

        _patchingSettingsDiv = dfx.getId('patchingSettings');
        _notifyToggleBtn     = GUI.getWidget('PatchingScreen-notifyUpdates');

        GUI.setModified(this, true);
    };

    this.toggleActivation = function() {
        var activWrap  = dfx.getClass('PatchingScreen-activateWrap', _patchingSettingsDiv)[0];
        var cancelWrap = dfx.getClass('PatchingScreen-idTextFieldWrap', _patchingSettingsDiv)[0];
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

}
