/**
 * JS Class for the Pie Graph Widget.
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

function GUIGraphPie(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.init();
}

GUIGraphPie.prototype = {
    init: function() {
        // Attach the API user token to the URL, and replace the tag with
        // one with the new URL. (Simply updating the 'data' tag does not work in
        // Webkit.)
        var container = dfx.getId(this.id + '-container');

        var objectTag = document.createElement('object');
        dfx.attr(objectTag, 'id', this.id);
        dfx.attr(objectTag, 'type', 'image/svg+xml');
        dfx.setStyle(objectTag, 'width', this.settings.size + 'px');
        dfx.setStyle(objectTag, 'height', this.settings.size + 'px');

        var url = sfapi.attachTokenToURL(dfx.attr(container, 'tempurl'));
        dfx.attr(objectTag, 'data', url);

        dfx.prepend(container, objectTag);

    }
};
