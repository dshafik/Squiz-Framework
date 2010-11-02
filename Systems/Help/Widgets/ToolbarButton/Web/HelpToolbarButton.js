/**
 * JS Class for the Help Toolbar Button Widget.
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
 * @subpackage Help
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * HelpToolbarButton.
 *
 * @param {string} id       The id of the widget.
 * @param {Array}  settings Settings of the widget.
 *
 * @return void
 */
function HelpToolbarButton(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = settings.widget.type;

    this._loaded = false;

    this.init();

}

HelpToolbarButton.prototype = {
    init: function()
    {
        var self = this;
        dfx.addEvent(dfx.getId(this.id), 'click', function() {
            self.toggleHelp();
        });

    },

    _loadHelp: function(showAfterLoad, pageid)
    {
        var self = this;
        var options = {
            modal: false
        };

        GUI.loadTemplate('Help', 'Help.tpl', null, function() {
            self._loaded = true;
            self.showHelp(pageid);
        }, options);

    },

    toggleHelp: function()
    {
        var dialog = dfx.getId('Help-dialog');
        if (!dialog || dfx.getElementHeight(dialog) === 0) {
            this._loadHelp(true);
        } else {
            dfx.removeClass(dfx.getId(this.id), 'active');
            GUI.getWidget('Help-dialog').close();
        }

    },

    showHelp: function(pageid)
    {
        if (this._loaded === false) {
            this._loadHelp(true, pageid);
            return;
        }

        var elem = dfx.getId(this.id);
        GUI.getWidget('Help-dialog').addDialogClosedCallback(function() {
            dfx.removeClass(elem, 'active');
        });

        dfx.addClass(dfx.getId(this.id), 'active');

        // Initialise the main Help widget.
        Help.init();

        Help.refresh(pageid);
    }

};
