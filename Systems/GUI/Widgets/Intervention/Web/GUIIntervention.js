/**
 * JS Class for the GUIIntervention Widget.
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
 * GUIIntervention widget.
 *
 * @param {string} id       The id of the widget.
 * @param {object} settings The settings of the widget.
 *
 * @constructor
 */
function GUIIntervention(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.elem = dfx.getId(id);

    this._data = null;

    this._orientationOrder = [
        'top.center',
        'top.left',
        'top.right',
        'bottom.center',
        'bottom.left',
        'bottom.right'
        /*'left.middle',
        'left.top',
        'left.bottom',
        'right.middle',
        'right.top',
        'right.bottom'*/
    ];

    this.init();

}

GUIIntervention.prototype = {
    init: function()
    {
        if (this.elem) {
            var elem = this.elem;
            dfx.addLoadEvent(function() {
                document.body.appendChild(elem);
            });
        }

        if (this.settings.persistent !== true) {
            // If the persistent settings is not true then hide when ever a
            // template is unloaded, most likely due to screen switch.
            var self = this;
            GUI.addTemplateRemovedCallback(function() {
                self.hide(null, true);
            });
        }

    },

    hide: function(info, dontFireEvents)
    {
        dfx.removeClass(this.elem, 'visible');

        if (this.settings.onclose && dontFireEvents !== true) {
            eval(this.settings.onclose + '.call(this, info, this._data);');
        }

    },

    show: function(targetElement, data)
    {
        dfx.removeClass(this.elem, 'visible');
        dfx.addClass(this.elem, 'calc');

        var classNames = ['top', 'left', 'right', 'bottom', 'middle', 'center'];
        var elem       = this.elem;
        dfx.foreach(classNames, function(i) {
            dfx.removeClass(elem, classNames[i]);
        });

        if (!targetElement && this.settings.targetElementid) {
            targetElement = dfx.getId(this.settings.targetElementid);
        }

        if (targetElement) {
            this.determinePosition(this.elem, targetElement, this.getOrientationOrder());
        }

        dfx.addClass(this.elem, 'visible');
        dfx.removeClass(this.elem, 'calc');

        this._data = data;

    },

    getData: function()
    {
        return this._data;

    },

    determinePosition: function(element, targetElement, orientationOrder)
    {
        // Get target elements position.
        var targetRect = dfx.getBoundingRectangle(targetElement);

        // Get the rectangle of the element that will be moved.
        var elemRect = dfx.getBoundingRectangle(element);
        var elemH    = (elemRect.y2 - elemRect.y1);
        var elemW    = (elemRect.x2 - elemRect.x1);

        // Get window dimensions.
        var winDim = dfx.getWindowDimensions();

        // Find target element's middle positions.
        var targetMidX = targetRect.x1 + ((targetRect.x2 - targetRect.x1) / 2);
        var targetMidY = targetRect.y1 + ((targetRect.y2 - targetRect.y1) / 2);

        var posX = 0;
        var posY = 0;

        // Using the default position top left (of the intervention box) determine
        // the correct position.
        var oln = orientationOrder.length;
        for (var o = 0; o < oln; o++) {
            var classNames = orientationOrder[o];

            var parts = classNames.split('.');

            switch (parts[0]) {
                case 'top':
                    posY = targetMidY;
                break;

                case 'bottom':
                    posY = (targetMidY - elemH);
                break;

                default:
                    // Unknown type.
                break;
            }

            switch (parts[1]) {
                case 'left':
                    posX = targetMidX;
                break;

                case 'center':
                    posX = (targetMidX - (elemW / 2));
                break;

                case 'right':
                    posX = (targetMidX - elemW);
                break;

                default:
                    // Unknown type.
                break;
            }

            if (this.settings.fixedPositioning === true) {
                var scrollCoords = dfx.getScrollCoords();
                posX -= scrollCoords['x'];
                posY -= scrollCoords['y'];
            }

            dfx.addClass(element, parts[0]);
            dfx.addClass(element, parts[1]);
            dfx.setStyle(element, 'left', posX + 'px');
            dfx.setStyle(element, 'top', posY + 'px');

            var newElemRect = dfx.getBoundingRectangle(element);

            // Check if the element is on the screen.
            if ((newElemRect.x1 > 0)
                && (winDim.width > newElemRect.x2)
                && (newElemRect.y1 > 0)
                && (winDim.height > newElemRect.y2)
            ) {
                // Its on the screen so set styles and stop looping.
                return;
            } else {
                dfx.removeClass(element, parts[0]);
                dfx.removeClass(element, parts[1]);
            }
        }//end for

    },

    getOrientationOrder: function()
    {
        var order = [];

        if (this.settings.orientationOrder) {
            order = this.settings.orientationOrder;
        }

        return order.concat(this._orientationOrder);

    }

};
