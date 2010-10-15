/**
 * JS Class for the GUITabPane Widget.
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
  * The GUITabPane class.
  *
  * @param {string} id       Id of the widget.
  * @param {object} settings Settings of the widget.
  *
  * @constructor
  */
function GUITabPane(id, settings)
{
    this.id       = id;
    this.settings = settings;
    this.elem     = dfx.getId(this.id);

}

GUITabPane.prototype = {
    switchTo: function(tabid)
    {
        // Change the tab button styles.
        dfx.removeClass(dfx.getClass('GUITabPane-tab'), 'active');
        var tabButton = dfx.getClass('tabButton-' + tabid, this.elem)
        dfx.addClass(tabButton, 'active');

        // Change the tab content styles.
        dfx.removeClass(dfx.getClass('GUITabPane-tabContent'), 'active');
        var tabContent = dfx.getClass('tabContent-' + tabid, this.elem)
        dfx.addClass(tabContent, 'active');

    },

    getVisibleContentElement: function()
    {
        var visibleElement = dfx.getClass('GUITabPane-tabContent active', this.elem)[0];
        return visibleElement;

    }

};
