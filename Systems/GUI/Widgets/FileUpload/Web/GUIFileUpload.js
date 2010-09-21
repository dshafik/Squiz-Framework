/**
 * JS Class for the Text Box Widget.
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
 * Construct GUIFileUpload JS object.
 *
 * @param {String} id ID of the widget.
 * @param {Object} settings Settings for the widget.
 * @return {Void}
 */
function GUIFileUpload(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.uploadForm     = dfx.getId(this.id + '-form');
    this.uploadIFrame   = dfx.getId(this.id + '-iframe');
    this.submitButton   = dfx.getId(this.id + '-upload');
    this.inputField     = dfx.getId(this.id + '-input');
    this.progressForm   = dfx.getId(this.id + '-progress-form');
    this.progressIFrame = dfx.getId(this.id + '-progress-iframe');
    this.completeWrap   = dfx.getClass('GUIFileUpload-complete-wrap', dfx.getId(this.id))[0];
    this.progressWrap   = dfx.getClass('GUIFileUpload-progress-wrap', dfx.getId(this.id))[0];

    this.uploadForm.action = sfapi.createURL('GUIFileUpload', 'processFileUpload', 'raw');

    this.uploadFinished = false;

    this.init();

}

GUIFileUpload.prototype = {
    init: function()
    {
        var self = this;

        this.uploadForm.onsubmit = function() {
            dfx.addClass(self.uploadForm, 'hidden');
            dfx.removeClass(self.progressWrap, 'hidden');
            self.uploadForm.submit();

            var ii = setInterval(function() {
                sfapi.post(
                    'GUIFileUpload',
                    'getFileUploadProgress',
                    {progKey: self.settings.widget.apc_key},
                    function(data) {
                        data = parseInt(data);
                        if (data === 100) {
                            clearInterval(ii);
                            self._uploadFinished();
                        } else {
                            self._updateProgress(data);
                        }
                    },
                    function(data) {
                        clearInterval(ii);
                    },
                    'raw'
                );
            }, 1000);
        };

    },

    _updateProgress: function(prog)
    {
        var progMsg = dfx.getClass('GUIFileUpload-progress-msg', dfx.getId(this.id))[0];
        dfx.setHtml(prog + '% has been completed');

    },

    _uploadFinished: function()
    {
        this.uploadFinished = true;
        dfx.addClass(this.progressWrap, 'hidden');

        var filename = dfx.getClass('GUIFileUpload-complete-filename', dfx.getId(this.id))[0];
        dfx.setHtml(filename, this.getValue());
        dfx.removeClass(this.completeWrap, 'hidden');

    },

    getValue: function()
    {
        if (this.uploadFinished === false) {
            return false;
        } else {
            return this.inputField.value;
        }

    },

    setValue: function(value)
    {
        return false;

    },

    focus: function()
    {
        this.inputField.focus();

    }

}
