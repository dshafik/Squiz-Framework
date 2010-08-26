/**
 * JS Class for the GUIColumnBrowser Widget.
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

function GUIColumnBrowser(id, settings)
{
    this.id       = id;
    this.settings = settings;
    this.elem     = dfx.getId(this.id);

    this.init();

}

GUIColumnBrowser.prototype = {

    init: function()
    {
        var self = this;
        dfx.addEvent(this.elem, 'click', function(e) {
            self._handleClick(e);
        });

    },

    _handleClick: function(e)
    {
        var target = dfx.getMouseEventTarget(e);
        var elem   = null;
        if ((elem = this._hasClass(target, 'GUIColumnBrowser-item', this.elem, true)) !== null) {
            this.itemClicked(elem, e);
        }

    },

    _hasClass: function(elem, className, stopElem, returnElement)
    {
        if (dfx.hasClass(elem, className) === true) {
            if (returnElement === true) {
                return elem;
            } else {
                return true;
            }
        } else if (elem.parentNode && elem.parentNode !== stopElem) {
            return this._hasClass(elem.parentNode, className, stopElem, returnElement);
        }

        return null;

    },

    itemClicked: function(itemElement, e)
    {
        if (this.settings.enableMultiSelect !== true || (e.ctrlKey !== true && e.metaKey !== true)) {
            dfx.removeClass(dfx.getClass('selected', itemElement.parentNode.parentNode), 'selected');
        }

        dfx.addClass(itemElement, 'selected');

        // Show the children of this item in the next column.
        this.showChildren(itemElement);

    },

    showChildren: function(parentElement)
    {
        var parentid = dfx.attr(parentElement, 'itemid');

        // Get its level.
        var parentColumnElement = parentElement.parentNode.parentNode;
        var parentLevel         = parseInt(parentColumnElement.className.match(/level-(\d+)/)[1]);

        var lookupClass = 'GUIColumnBrowser-column level-' + (parentLevel + 1);

        var childColumnElement = dfx.getClass(lookupClass, this.elem);
        if (childColumnElement && childColumnElement.length > 0) {
            childColumnElement = childColumnElement[0];
            // Now find the card that needs to be shown.
            var cards = dfx.getClass('GUIColumnBrowser-column-card', childColumnElement);
            var cln   = cards.length;

            var card = null;
            for (var i = 0; i < cln; i++) {
                if (dfx.attr(cards[i], 'parentid') !== parentid) {
                    dfx.removeClass(cards[i], 'visible');
                } else {
                    card = cards[i];
                }
            }

            // Show the card and the column.
            dfx.addClass(card, 'visible');
            dfx.addClass(childColumnElement, 'visible');

            dfx.removeClass(dfx.getClass('GUIColumnBrowser-item selected', childColumnElement), 'selected');

            // The 'last' class represents the column that is shown last.
            dfx.removeClass(dfx.getClass('GUIColumnBrowser-column last', this.elem), 'last');
            dfx.addClass(childColumnElement, 'last');
        }//end if

        dfx.removeClass(dfx.getClass('GUIColumnBrowser-column active', this.elem), 'active');
        dfx.addClass(parentColumnElement, 'active');

        // Hide all other columns that shouldnt be visible.
        this._hideColumn((parentLevel + 2));

    },

    _hideColumn: function(colIndex)
    {
        var columns = dfx.getClass('GUIColumnBrowser-column');
        var cln     = columns.length;

        var width = 0;
        for (var i = 0; i < cln; i++) {
            if (i >= colIndex) {
                dfx.removeClass(columns[i], 'visible');
            } else {
                width += dfx.getElementWidth(columns[i]);
            }
        }

        // Set the parent width so that 'overflow: auto' works.
        dfx.setStyle(dfx.getClass('GUIColumnBrowser-columnWrapper', this.elem), 'width', width + 'px');

    }

};
