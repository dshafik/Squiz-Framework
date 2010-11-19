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
        var browser = dfx.browser();

        // Add the object tag if it's not IE, or it *is* IE but it's v.9 or later.
        // Older IEs do not support it natively (and plugin support is flaky), also
        // to hide any red X's that may appear due to lack of plugin at all.
        if ((browser.type !== 'msie') || (parseInt(browser.version, 10) >= 9)) {
            // Attach the API user token to the URL, and replace the tag with
            // one with the new URL. (Simply updating the 'data' tag does not work in
            // Webkit.)
            var container = dfx.getId(this.id + '-container');
            var tempUrl   = dfx.attr(container, 'tempurl');

            if ((typeof tempUrl === 'string') && (tempUrl !== '')) {
                var objectTag = document.createElement('object');
                dfx.attr(objectTag, 'id', this.id);
                dfx.attr(objectTag, 'type', 'image/svg+xml');
                dfx.setStyle(objectTag, 'width', this.settings.size + 'px');
                dfx.setStyle(objectTag, 'height', this.settings.size + 'px');

                var url = sfapi.attachTokenToURL(tempUrl);
                dfx.attr(objectTag, 'data', url);

                dfx.prepend(container, objectTag);
            }//end if
        }//end if
    }
};
