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

    this.numberOfStaticButtons = settings.numberOfStaticButtons || 2;

    var elem = dfx.getId(this.id);

    this._staticBtnContainer     = dfx.getClass('modeList-1', elem)[0];
    this._mouseOver              = false;
    this._autoCollapseT          = null;
    this._collapseTimeout        = 1500;
    this._slideTime              = 300;
    this._dynamicBtnContainer    = dfx.getClass('modeList-2', elem)[0];
    this._sliderElement          = dfx.getClass('GUIModeSwitcher-overflow', elem)[0];
    this._lastVisibleButtonIndex = null;
    this._staticButtonsWidth     = 0;
    this._dynamicButtonsWidth    = 0;
    this._dynamicButtons         = null;
    this._staticButtons          = null;
    this._leftMargin             = 15;

    this._initButtonWidths();

    // Initialise the width of the switcher so that only specified amount of
    // buttons are shown.
    this.calculateSwitcherWidth(true);

    this.addExpandEvent();
    this.addModeClickEvent();

}

GUIModeSwitcher.prototype = {

    _initButtonWidths: function()
    {
        // Static buttons width.
        var staticButtons = this._getStaticButtons();
        var sln           = staticButtons.length;
        for (var i = 0; i < sln; i++) {
            this._staticButtonsWidth += dfx.getElementWidth(staticButtons[i]);
        }

        // The offset of the 2nd list is staticButton - 15px (due to margin on the left).
        this._staticButtonsWidth = this._staticButtonsWidth;

        // Dynamic buttons width.
        var dynamicButtons = this._getDynamicButtons();
        var dln            = dynamicButtons.length;
        for (var i = 0; i < dln; i++) {
            this._dynamicButtonsWidth += dfx.getElementWidth(dynamicButtons[i]);
        }

    },

    calculateSwitcherWidth: function(setWidth)
    {
        var elem     = dfx.getId(this.id);
        var minWidth = 0;
        var maxWidth = 0;

        var staticButtons  = this._getStaticButtons();
        var dynamicButtons = this._getDynamicButtons();

        // Get last button width.
        var lastButtonIndex = this.getLastVisibleButtonIndex();
        var lastButtonWidth = dfx.getElementWidth(dynamicButtons[lastButtonIndex]);

        if (setWidth === true) {
            dfx.setStyle(this._staticBtnContainer.parentNode.parentNode, 'width', this._staticButtonsWidth + lastButtonWidth + 'px');
            dfx.setStyle(this._dynamicBtnContainer, 'left', this._getSlideOffset() + 'px');
        }

    },

    _getStaticButtons: function()
    {
        if (!this._staticButtons) {
            this._staticButtons = dfx.getTag('a', this._staticBtnContainer);
        }

        return this._staticButtons;

    },

    _getDynamicButtons: function()
    {
        if (!this._dynamicButtons) {
            this._dynamicButtons = dfx.getTag('a', this._dynamicBtnContainer)
        }

        return this._dynamicButtons;

    },

    _isStaticButton: function(button)
    {
        var btns = this._getStaticButtons();
        var bln  = btns.length;

        for (var i = 0; i < bln; i++) {
            if (btns[i].parentNode === button) {
                return true;
            }
        }

        return false;

    },

    getLastVisibleButtonIndex: function()
    {
        // Check if a dynamic button is selected if it is then use that, else
        // use the first dynamic button.
        if (this._lastVisibleButtonIndex === null) {
            var dynamicButtons = this._getDynamicButtons();
            var index          = 0;
            dfx.foreach(dynamicButtons, function(i) {
                var btn = dynamicButtons[i].parentNode;
                if (dfx.hasClass(btn, 'selected') === true) {
                    index = i;
                    // Break out of foreach.
                    return false;
                }
            });

            this._lastVisibleButtonIndex = index;
        }

        return this._lastVisibleButtonIndex;

    },

    _getSlideOffset: function()
    {
        var dynamicButtons = this._getDynamicButtons();
        var lastIndex      = this.getLastVisibleButtonIndex();

        var slideOffset = this._staticButtonsWidth;
        for (var i = 0; i < lastIndex; i++) {
           slideOffset -= dfx.getElementWidth(dynamicButtons[i]);
        }

        return (slideOffset - this._leftMargin);

    },

    addExpandEvent: function()
    {
        var elem = dfx.getClass('GUIModeSwitcher-expander', dfx.getId(this.id))[0];
        var self = this;

        dfx.addEvent(elem, 'click', function() {
            self.toggle();
        });

        dfx.hover(dfx.getId(this.id), function(e) {
            clearTimeout(self._autoCollapseT);
            self._mouseOver = true;
        }, function(e) {
            self._mouseOver = false;
            if (dfx.attr(self._staticBtnContainer, 'state') === '1') {
                self.autoCollapse();
            }
        });

    },

    toggle: function()
    {
        var state = parseInt(dfx.attr(this._staticBtnContainer, 'state'));
        if (state === 1) {
            this.collapse();
        } else {
            this.expand();
        }

    },

    expand: function()
    {
        clearTimeout(this._autoCollapseT);

        dfx.attr(this._staticBtnContainer, 'state', 1);

        var self = this;
        dfx.attr(this._sliderElement, 'state', 1);
        dfx.animate(this._sliderElement, {
            width: (this._staticButtonsWidth + this._dynamicButtonsWidth)
        }, this._slideTime, function() {
            self.autoCollapse();
        });

        dfx.animate(this._dynamicBtnContainer, {
            left: (this._staticButtonsWidth - this._leftMargin)
        }, this._slideTime);

    },

    collapse: function()
    {
        clearTimeout(this._autoCollapseT);

        // Get last button width.
        var dynamicButtons  = this._getDynamicButtons();
        var lastButtonIndex = this.getLastVisibleButtonIndex();
        var lastButtonWidth = dfx.getElementWidth(dynamicButtons[lastButtonIndex]);

        dfx.attr(this._staticBtnContainer, 'state', 0);

        dfx.animate(this._sliderElement, {
            width: (this._staticButtonsWidth + lastButtonWidth)
        }, this._slideTime);

        dfx.animate(this._dynamicBtnContainer, {
            left: this._getSlideOffset()
        }, this._slideTime);

    },

    autoCollapse: function()
    {
        if (this._mouseOver === true) {
            return;
        }

        var self = this;
        clearTimeout(this._autoCollapseT);
        this._autoCollapseT = setTimeout(function() {
            if (self._mouseOver === true) {
                return;
            }

            self.collapse();
        }, this._collapseTimeout);

    },

    addModeClickEvent: function()
    {
        var self           = this;
        var buttons        = dfx.getTag('a', dfx.getId(this.id));
        var dynamicButtons = this._getDynamicButtons();
        dfx.addEvent(buttons, 'click', function(e) {
            var btn = dfx.getMouseEventTarget(e);
            btn     = btn.parentNode;
            dfx.removeClass('selected', dynamicButtons);
            dfx.addClass('selected', btn);

            if (self._isStaticButton(btn) === false) {
                var index = 0;
                for (var node = btn.previousSibling; node; node = node.previousSibling) {
                    if (dfx.isTag(node, 'li') === true) {
                        index++;
                    }
                }

                self._lastVisibleButtonIndex = index;
            }

            self.collapse();
        });

    }

};

dfx.noInclusionInherits('GUIModeSwitcher', 'GUIContentSwitcher', true);