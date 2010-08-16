/**
 * JS Class for the Mode Switcher Widget.
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

function GUIModeSwitcher(id, settings)
{
    GUIContentSwitcher.call(this, id, settings);
    this.className = 'GUIModeSwitcher';

    this.numberOfVisibleButtons = settings.numberOfButtons || 3;

    this._maxWidth        = 0;
    this._minWidth        = 0;
    this._listContainer   = null;
    this._mouseOver       = false;
    this._autoCollapseT   = null;
    this._collapseTimeout = 1500;
    this._slideTime       = 500;

    // Initialise the width of the switcher so that only specified amount of
    // buttons are shown.
    this.calculateSwitcherWidth(true);

    this.addExpandEvent();
    this.addModeClickEvent();

}

GUIModeSwitcher.prototype = {

    calculateSwitcherWidth: function(setWidth)
    {
        var elem     = dfx.getId(this.id);
        var minWidth = 0;
        var maxWidth = 0;
        var buttons  = dfx.getTag('a', elem);
        var bln      = buttons.length;
        var visBtns  = this.numberOfVisibleButtons;

        if (visBtns > bln) {
            visBtns = bln;
        }

        for (var i = 0; i < bln; i++) {
            var elemWidth = dfx.getElementWidth(buttons[i]);
            maxWidth     += elemWidth;

            if (visBtns > 0 && dfx.getStyle(buttons[i].parentNode, 'display') !== 'none') {
                visBtns--;
                minWidth += elemWidth;
            }
        }

        if (setWidth === true) {
            this._maxWidth = maxWidth;
        }

        this._minWidth = minWidth;

        if (setWidth === true) {
            this._listContainer = dfx.getClass('GUIModeSwitcher-overflow', elem)[0];
            dfx.setStyle(this._listContainer, 'width', minWidth + 'px');
        }

    },

    addExpandEvent: function()
    {
        var elem = dfx.getClass('GUIModeSwitcher-expander', dfx.getId(this.id))[0];

        var self     = this;
        var listCont = this._listContainer;

        dfx.addEvent(elem, 'click', function() {
            self.toggle();
        });

        dfx.hover(this._listContainer, function(e) {
            clearTimeout(self._autoCollapseT);
            self._mouseOver = true;
        }, function(e) {
            self._mouseOver = false;
            self.autoCollapse();
        });

    },

    toggle: function()
    {
        var state = parseInt(dfx.attr(this._listContainer, 'state'));
        if (state === 1) {
            this.collapse();
        } else {
            this.expand();
        }
    },

    expand: function()
    {
        clearTimeout(this._autoCollapseT);

        var self = this;
        dfx.attr(this._listContainer, 'state', 1);
        dfx.setStyle(dfx.getTag('li', dfx.getId(this.id)), 'display', 'inline');
        dfx.animate(this._listContainer, {
            width: this._maxWidth
        }, this._slideTime, function() {
            self.autoCollapse();
        });

    },

    collapse: function()
    {
        dfx.attr(this._listContainer, 'state', 0);
        dfx.animate(this._listContainer, {
            width: this._minWidth
        }, this._slideTime);

    },

    autoCollapse: function()
    {
        if (this._mouseOver === true) {
            return;
        }

        var self = this;
        this._autoCollapseT = setTimeout(function() {
            if (self._mouseOver === true) {
                return;
            }

            self.collapse();
        }, this._collapseTimeout);

    },

    addModeClickEvent: function()
    {
        var self     = this;
        var buttons  = dfx.getTag('a', dfx.getId(this.id));
        dfx.addEvent(buttons, 'click', function(e) {
            var btn = dfx.getMouseEventTarget(e);
            btn     = btn.parentNode;
            self._hidePrevButtons(btn);
        });

    },

    _hidePrevButtons: function(button)
    {
        var buttons  = dfx.getTag('a', dfx.getId(this.id));
        var bln      = buttons.length;
        var index    = this._getModeIndex(button);
        for (var i = (this.numberOfVisibleButtons - 1); i < index; i++) {
            dfx.setStyle(buttons[i].parentNode, 'display', 'none');
        }

        this.calculateSwitcherWidth(false);
        this.collapse();

    },

    _getModeIndex: function(button)
    {
        var count = 0;
        var elem  = button.previousSibling;
        while (elem) {
            if (dfx.isTag(elem, 'li') === true) {
                count++;
            }

            elem = elem.previousSibling;
        }

        return count;

    },


};

dfx.noInclusionInherits('GUIModeSwitcher', 'GUIContentSwitcher', true);