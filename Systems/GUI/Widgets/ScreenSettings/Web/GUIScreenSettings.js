/**
 * JS Class for the Screen Settings Widget.
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

function GUIScreenSettings(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.className = 'GUIScreenSettings';

}

GUIScreenSettings.prototype = {

    getValue: function()
    {
        var items = dfx.getClass(this.className + '-item', dfx.getId(this.id));
        var value = {};
        var self  = this;
        dfx.foreach(items, function(i) {
            var item         = items[i];
            var widgetValues = self.getWidgetValues(item);
            dfx.foreach(widgetValues, function(widgetid) {
                value[widgetid] = widgetValues[widgetid];
            });
        });

        return value;

    },

    getWidgetValues: function(itemElem)
    {
        // Find all the elements with ids.
        var elems   = dfx.find(itemElem, '[id]');
        var ln      = elems.length;
        var values  = {};

        if (ln === 0) {
            return null;
        }

        for (var i = 0; i < ln; i++) {
            var elem = elems[i];
            var id   = elem.getAttribute('id');
            if (!id) {
                continue;
            }

            // Check if id is a valid widget id.
            var widget = GUI.getWidget(id);
            if (!widget) {
                continue;
            }

            // TODO: Should we only return top level widgets?
            values[id] = widget.getValue();
        }//end for

        return values;

    }

};
