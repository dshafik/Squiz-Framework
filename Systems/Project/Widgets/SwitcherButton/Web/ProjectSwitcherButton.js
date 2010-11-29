/**
 * JS Class for the Project Switcher Toolbar Button Widget.
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
 * @package    Analytics
 * @subpackage
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * ProjectSwitcherButton.
 *
 * @param {string} id       The id of the widget.
 * @param {Array}  settings Settings of the widget.
 *
 * @return void
 */
function ProjectSwitcherButton(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = settings.widget.type;

    this._loaded = false;

    this.init();

}

ProjectSwitcherButton.prototype = {
    init: function()
    {
        var self = this;
        dfx.addEvent(dfx.getId(this.id), 'click', function() {
            GUI.dequeueOverlay('normal');
            self.toggleProjectSwitcher();
        });

    },

    _loadProjectSwitcher: function(showAfterLoad)
    {
        var self    = this;
        var options = {
            modal: true
        };

        GUI.loadTemplate('Project', 'ProjectSwitcher.tpl', null, function() {
            self._loaded = true;
            self.showProjectSwitcher();

            dfx.addClass(dfx.getId(self.id), 'active');
            GUI.getWidget('ProjectSwitcher-dialog').addDialogClosedCallback(function() {
                GUI.unloadTemplate(':ProjectSwitcher');
                GUI.dequeueOverlay('normal');
                dfx.removeClass(dfx.getId(self.id), 'active');
            });
        }, options);

    },

    toggleProjectSwitcher: function()
    {
        this.showProjectSwitcher();

    },

    showProjectSwitcher: function()
    {
        if (this._loaded === false) {
            this._loadProjectSwitcher(true);
            return;
        }

        this._loaded = false;

    }

};
