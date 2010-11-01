/**
 * JS Class for the Bar Graph Widget.
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

function GUIGraphBar(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.init();
}

GUIGraphBar.prototype = {

    init: function()
    {
        var self = this;

        var graph = dfx.getId(this.id);
        var rows  = dfx.getClass('data-row', graph);

        dfx.foreach(rows, function(idx) {
            dfx.addEvent(rows[idx], 'mouseover', function(evt) {
                if (dfx.hasClass(this, 'hover') === false) {
                    dfx.addClass(this, 'hover');

                    // Position the row's tooltip correctly.
                    var tooltip = dfx.getClass('tooltip', rows[idx])[0];
                    var left = dfx.getElementCoords(tooltip).x;
                    var right = dfx.getWindowDimensions().width - left;

                    if (left < 10) {
                        // Allow 10px breathing space on the left.
                        dfx.removeClass(tooltip, 'arrowPosition-topMid');
                        dfx.addClass(tooltip, 'arrowPosition-topLeft');
                    } else if (right < 234) {
                        // 234px = 200px tooltip width + 16px of padding
                        //         + 8px of margin
                        //         + 10px for breathing space from the right edge.
                        dfx.removeClass(tooltip, 'arrowPosition-topMid');
                        dfx.addClass(tooltip, 'arrowPosition-topRight');
                    }
                }
            });
            dfx.addEvent(rows[idx], 'mouseout', function(evt) {
                dfx.removeClass(this, 'hover');
            });

            return true;
        });

    }

};
