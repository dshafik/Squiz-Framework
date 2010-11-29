/**
 * JS Class for the Project Invalid template.
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
 * @subpackage Project
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var ProjectProjectSelector =
{
    addProjectClicked: function()
    {
        // TODO: This function assumes that the initial Item in SystemConfig is the
        // project screen. Should be able to do this in another way.

        // Change the button highlighting.
        var buttons  = dfx.getTag('a', dfx.getId('mode-switcher'));
        var btnCount = buttons.length;
        for (var i = 0; i < btnCount; i++) {
            var btn = buttons[i];
            if (dfx.attr(btn, 'modeid') === 'SystemConfig') {
                dfx.trigger(btn, 'click');
                break;
            }
        }

    }
};
