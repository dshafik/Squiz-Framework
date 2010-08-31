/**
 * JS Class for the GUILineage Widget.
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

function GUILineage(id, settings)
{
    this.id       = id;
    this.settings = settings;
    this.elem     = dfx.getId(this.id);
    this.listElem = dfx.getTag('ul', this.elem)[0];

    GUI.addWidgetEvent(this, 'itemClicked');

    this.init();

}

GUILineage.prototype = {

    init: function()
    {
        var self = this;
        dfx.addEvent(this.listElem, 'click', function(e) {
            var target = dfx.getMouseEventTarget(e);
            if (dfx.isTag(target, 'a') === true) {
                 // Get the index of the clicked item.
                 var index = 0;
                 for (var node = target.parentNode.previousSibling; node; node = node.previousSibling) {
                     if (dfx.isTag(node, 'li') === true) {
                         index++;
                     }
                 }

                 // Handle list item click event.
                 self.fireItemClickedCallbacks(dfx.attr(target.parentNode, 'itemid'), index, target.parentNode);
                 return;
            }
        });

    },

    /**
     * Replaces the lineage with the specified list of lineage items.
     *
     * @param object lineage Object structure: {itemid: title}.
     *
     * @return void
     */
    setLineage: function(lineage)
    {
        var itemTemplate = this.settings._templates.itemTemplate;
        var elements     = [];
        dfx.foreach(lineage, function(itemid) {
            var tmp     = document.createElement('div');
            var itemtpl = itemTemplate;

            itemtpl = itemtpl.replace('%itemid%', itemid);
            itemtpl = itemtpl.replace('%title%', lineage[itemid]);

            dfx.setHtml(tmp, itemtpl);

            elements.push(tmp.firstChild);
        });

        dfx.empty(this.listElem);
        var eln = elements.length;
        for (var i = 0; i < eln; i++) {
            this.listElem.appendChild(elements[i]);
        }

    }

};
