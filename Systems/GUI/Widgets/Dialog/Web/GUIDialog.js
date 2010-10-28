/**
 * JS Class for the Dialog Widget.
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

function GUIDialog(id, settings)
{
    this.id       = id;
    this.settings = settings;

    this.elem = dfx.getId(this.id);

    var midElem = dfx.getClass('GUIDialog-middle', this.elem)[0];

    // Events.
    GUI.addWidgetEvent(this, 'dialogResized');
    GUI.addWidgetEvent(this, 'dialogClosed');

    var self = this;
    this.update();

    if (this.settings.resizable === true) {
        jQuery(this.elem).resizable({
            handles: 'se',
            ghost: true,
            stop: function(e, ui) {
                // Need to adjust the content element size.
                var widthDiff  = (ui.size.width - ui.originalSize.width);
                var heightDiff = (ui.size.height - ui.originalSize.height);
                var newWidth   = parseInt(dfx.getStyle(midElem, 'width')) + widthDiff;
                var newHeight  = parseInt(dfx.getStyle(midElem, 'height')) + heightDiff;
                dfx.setStyle(midElem, 'width', newWidth + 'px');
                dfx.setStyle(midElem, 'height', newHeight + 'px');
                self.fireDialogResizedCallbacks({
                        width: ui.size.width,
                        height: ui.size.height
                    }, {
                        width: newWidth,
                        height: newHeight
                    }
                );
            },
            maxWidth: this.settings.maxWidth,
            minWidth: this.settings.minWidth,
            maxHeight: this.settings.maxHeight,
            minHeight: this.settings.minHeight
        });
    }//end if

    if (this.settings.draggable === true) {
        jQuery(this.elem).draggable({
            handle: dfx.getClass('GUIDialog-top', this.elem)[0]
        });
    }

    // Enable toggling between fullscreen and default width using double click on title.
    // Note that it has no effect when the default size is already set to full screen.
    var fullScreen = this.settings.fullScreen || false;
    dfx.addEvent(dfx.getClass('GUIDialog-top', this.elem), 'safedblclick', function() {}, function() {
        if (fullScreen === true) {
            self.update();
            fullScreen = false;
        } else {
            self.update(true);
            fullScreen = true;
        }
    });

}

GUIDialog.prototype = {
    update: function(forceFullScreen)
    {
        if (!this.elem) {
            return;
        }

        // If element is hidden then use visibility: hidden while calculating size and pos.
        var resetVis = false;
        if (dfx.getElementHeight(this.elem) === 0
            || dfx.getElementHeight(this.elem) === 0
        ) {
            resetVis = true;
            dfx.setStyle(this.elem, 'visibility', 'hidden');
            dfx.setStyle(this.elem, 'display', 'block');
        }

        // Set the width/height of main element to auto.
        dfx.setStyle(this.elem, 'height', 'auto');
        dfx.setStyle(this.elem, 'width', 'auto');
        dfx.setStyle(this.elem, 'left', '50%');
        dfx.setStyle(this.elem, 'top', '50%');

        var contentDiv    = dfx.getClass('GUIDialog-middle', this.elem)[0];
        var topElement    = dfx.getClass('GUIDialog-top', this.elem)[0];
        var bottomElement = dfx.getClass('GUIDialog-bottom', this.elem)[0];
        var contentWidth  = this.settings.width;
        var contentHeight = null;

        if (forceFullScreen === true || this.settings.fullScreen === true) {
            // Use fullscreen mode.
            var winDim    = dfx.getWindowDimensions();
            contentWidth  = (winDim.width - 100);
            contentHeight = (winDim.height - 250);
        } else {
            contentHeight = (this.settings.height - parseInt(dfx.getElementHeight(topElement)) - parseInt(dfx.getElementHeight(bottomElement)));
        }

        dfx.setStyle(contentDiv, 'width', contentWidth + 'px');
        dfx.setStyle(contentDiv, 'height', contentHeight + 'px');

        // Calculate the position of the main element.
        var mainWidth  = dfx.getElementWidth(this.elem);
        var mainHeight = dfx.getElementHeight(this.elem);
        var marginLeft = (parseInt(mainWidth / 2) * -1);
        var marginTop  = (parseInt(mainHeight / 2) * -1);
        dfx.setStyle(this.elem, 'margin-left', marginLeft + 'px');
        dfx.setStyle(this.elem, 'margin-top', marginTop + 'px');

        if (resetVis === true) {
            // Reset visibility.
            dfx.setStyle(this.elem, 'visibility', 'visible');
        }

        this.fireDialogResizedCallbacks({
                width: mainWidth,
                height: mainHeight
            }, {
                width: contentWidth,
                height: contentHeight
            }
        );

    },

    setSubTitle: function(title)
    {
        var subTitleEl = dfx.getClass('GUIDialog-subTitle', this.elem)[0];
        dfx.setHtml(subTitleEl, title);

    },

    setContent: function(content)
    {
        if (this.elem === null) {
            var main = document.createElement('div');
            var c    = 'GUIDialog';
            var id   = this.id;
            dfx.addClass(main, c);
            dfx.attr(main, 'id', id);

            var html = '';

            // Top section.
            html += '<div class="' + c + '-top">';
            html += '<div class="' + c + '-title">' + this.settings.title + '</div>';
            html += '<div class="' + c + '-subTitle">' + this.settings.subTitle + '</div>';
            html += '<div class="' + c + '-closeIcon"></div>';
            html += '</div>';

            // Middle section.
            html += '<div class="' + c + '-middle"></div>';

            // Bottom section.
            html += '<div class="' + c + '-bottom">';
            html += '</div>';

            dfx.setHtml(main, html);
            this.elem = main;
        }//end if

        var container = dfx.getClass(c + '-middle', this.elem)[0];

        if (typeof content === 'string') {
            dfx.setHtml(container, content);
        } else {
            container.appendChild(content);
        }

    },

    open: function()
    {
        document.body.appendChild(this.elem);

    },

    close: function(clickedElement)
    {
        dfx.remove(this.elem);
        this.fireDialogClosedCallbacks(this.id);

    },

    getSelection: function()
    {
        var activeCol     = dfx.getClass('GUIColumnBrowser-column active', this.elem);
        var selectedElems = dfx.getClass('selected', activeCol);
        var sln           = selectedElems.length;

        var selection = [];
        for (var i = 0; i < sln; i++) {
            selection.push(dfx.attr(selectedElems[i], 'itemid'));
        }

        return selection;

    }


};
