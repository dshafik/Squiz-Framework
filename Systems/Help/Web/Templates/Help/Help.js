/**
 * JS Class for Help system.
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
 * @subpackage Help
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var Help = new function()
{
    var _apiURL        = null;
    var _searchURL     = null;
    var _systemInfos   = {};
    var _currentSystem = null;
    var _iframe        = null;
    var _loadPageOpts  = null;

    this.init = function() {
        var elem = dfx.getId('Help-dialog');
        if (!elem) {
            return;
        }

        var self = this;

        // Set the API URL.
        _searchURL = '/__api/raw/Help/searchDocs?_api_token=' + dfx.getId('__api_token').value + '&';
        _apiURL    = '/__api/raw/Help/getPage?_api_token=' + dfx.getId('__api_token').value + '&';

        var navHeight = parseInt(dfx.getElementHeight(dfx.getClass('Help-controls', elem)[0]));
        var iframe    = dfx.getId('Help-iframe');
        _iframe       = iframe;
        iframe.onload = function(e) {
            self.iframeLoaded();
        }

        var midElem    = dfx.getClass('GUIDialog-middle', elem)[0];
        var initHeight = parseInt(dfx.getElementHeight(midElem));

        dfx.setStyle(iframe, 'height', (initHeight - navHeight - 27) + 'px');

        GUI.getWidget('Help-dialog').addDialogResizedCallback(function(ui, contentSize) {
            dfx.setStyle(iframe, 'height', (contentSize.height - navHeight - 27) + 'px');
        });

        // Add search field event.
        var searchBox = GUI.getWidget('Help-search');
        searchBox.addKeyPressCallback(function(value, e) {
            if (e.keyCode === 13) {
                value = dfx.trim(value);
                if (value.length === 0) {
                    return;
                }

                self.search(value);
            }
        });
    }

    this.refresh = function() {
        this.loadIndexPage();
    };

    this.loadGlossaryPage = function(template) {
        template = template || GUI.getCurrentTemplate();

        // Construct the glossary page id for this template.
        var pageid = template.split(':').join('-') + '-glossary-index.html';
        this.loadPage(pageid);
    };

    this.loadIndexPage = function(template) {
        template   = template || GUI.getCurrentTemplate();
        var pageid = template.split(':').join('-') + '-index.html';
        this.loadPage(pageid);
    };

    this.loadGeneralPage = function() {
        this.loadPage('general.html');
    };

    this.loadPage = function(pageid, options) {
        var helpIframe = dfx.getId('Help-iframe');
        _loadPageOpts  = options;

        dfx.attr(helpIframe, 'src', _apiURL + 'pageid=' + escape(pageid));
    };

    this.iframeLoaded = function() {
        var self = this;
        dfx.removeEvent([document, dfx.getIframeDocument(_iframe)], 'click.Help-dialog');
        dfx.addEvent([document, dfx.getIframeDocument(_iframe)], 'click.Help-dialog', function(e) {
            var target     = dfx.getMouseEventTarget(e);
            var menuButton = dfx.getClass('Help-control-button menu', dfx.getId('Help-dialog'))[0];
            if (target !== menuButton) {
                self.closeMenu();
            }
        });

        _handlePageLoadOptions();
    };

    _handlePageLoadOptions = function() {
        if (!_loadPageOpts) {
            return;
        }

        var iframeDoc = dfx.getIframeDocument(_iframe);

        // Search.
        if (_loadPageOpts.searchedWords) {
            // Need to highlight all the searched words.
            var sln = _loadPageOpts.searchedWords.length;
            for (var i = 0; i < sln; i++) {
                var word      = _loadPageOpts.searchedWords[i];
                var textNodes = dfx.getTextNodes(iframeDoc.body);
                var tln       = textNodes.length;
                var regExp    = new RegExp(word, 'gi');
                for (var j = 0; j < tln; j++) {
                    textNodes[j].nodeValue = textNodes[j].nodeValue.replace(regExp, '__REPLACE_' + i + '__$&__');
                }
            }

            var c    = 1;
            var html = dfx.getHtml(iframeDoc.body);
            for (var i = 0; i < sln; i++) {
                if (c > 7) {
                    c = 1;
                }

                var word   = _loadPageOpts.searchedWords[i];
                var regExp = new RegExp('__REPLACE_' + i + '__(' + word + ')__', 'ig');
                html       = html.replace(regExp, '<span class="Help-searchTerm-' + c + '">$1</span>');
                c++;
            }

            dfx.setHtml(iframeDoc.body, html);
        }//end if

        _loadPageOpts = null;
    };

    this.search = function(value) {
        var helpIframe = dfx.getId('Help-iframe');
        dfx.attr(helpIframe, 'src', _searchURL + 'searchString=' + escape(value));
    };

    this.getSearchResults = function(searchString, startOffset) {
        var params = {
            searchString: searchString,
            startOffset: startOffset
        };

        GUI.sendRequest('Help', 'searchDocs', params, function(content) {
            // Append the content to the end of the current search results page.
            var wrapper = document.createElement('div');
            dfx.setHtml(wrapper, content);

            var iframeDoc       = dfx.getIframeDocument(_iframe);
            var currentMoreLink = dfx.getClass('Help-searchResults-moreLink', iframeDoc);

            dfx.insertAfter(currentMoreLink, wrapper);

            // Remove the old 'More' link.
            dfx.remove(currentMoreLink);
        }, 'raw');
    };

    this.back = function() {
        history.back(1);
    };

    this.forward = function() {
        history.forward(1);
    };

    this.home = function() {
        this.loadIndexPage();
    };

    this.toggleMenu = function() {
        var menuButton = dfx.getClass('Help-control-button menu', dfx.getId('Help-dialog'))[0];
        var menuElem   = dfx.getClass('Help-systemsMenu', dfx.getId('Help-dialog'))[0];
        dfx.toggleClass([menuElem, menuButton], 'active');
    };

    this.openMenu = function() {
        var menuElem   = dfx.getClass('Help-systemsMenu', dfx.getId('Help-dialog'))[0];
        var menuButton = dfx.getClass('Help-control-button menu', dfx.getId('Help-dialog'))[0];
        dfx.addClass([menuElem, menuButton], 'active');
    };

    this.closeMenu = function() {
        var menuElem   = dfx.getClass('Help-systemsMenu', dfx.getId('Help-dialog'))[0];
        var menuButton = dfx.getClass('Help-control-button menu', dfx.getId('Help-dialog'))[0];
        if (menuElem && menuButton) {
            dfx.removeClass([menuElem, menuButton], 'active');
        }
    };

    this.glossary = function() {
        this.loadGlossaryPage();
    };

    this.general = function() {
        this.loadGeneralPage();
    };

    this.setSystems = function(systemInfos) {
        _systemInfos = systemInfos;
    };

    this.setCurrentSystem = function(systemid) {
        if (_currentSystem !== systemid) {
            var dialogElem = dfx.getId('Help-dialog');
            dfx.addClass(dialogElem, 'activeItem-' + systemid);
            GUI.getWidget('Help-dialog').setSubTitle(_systemInfos[systemid].title);
            _currentSystem = systemid;

            var selectedItem = dfx.getId('Help-dialog-sysMenuItem-' + systemid);
            if (selectedItem) {
                var listItems = dfx.getTag('li', dfx.getClass('Help-systemsMenu', dialogElem));
                dfx.removeClass(listItems, 'selected');
                dfx.addClass(selectedItem, 'selected');
            }
        }
    };

    this.pointer = {
        pointer: null,
        pointerDim: {},
        container: null,

        pointTo: function(elemid, elemClass) {
            this.container = dfx.getId('Help-dialog');

            var elem = null;
            if (!elemid || elemid === '') {
                if (!elemClass || elemClass === '') {
                    return;
                }

                // Get the first element that has the elemClass.
                var celems = dfx.getClass(elemClass);
                var cln    = celems.length;
                for (var i = 0; i < cln; i++) {
                    if (dfx.getElementWidth(celems[i]) > 0) {
                        elem = celems[i];
                        break;
                    }
                }
            } else {
                // Get the elem element using the DOM element id.
                elem = dfx.getId(elemid);
            }

            // If the specified elem is not in the DOM then we cannot pint to it.
            if (!elem) {
                return;
            }

            // Do not point to elem if its hidden.
            if (dfx.getStyle(elem, 'visibility', 'hidden') === true) {
                return;
            }

            /*if (elem.scrollIntoView) {
                elem.scrollIntoView(false);
            }*/

            // Get element coords.
            var rect = dfx.getBoundingRectangle(elem);

            // If we cannot get the position then dont do anything,
            // most likely element is hidden.
            if (rect.x1 === 0
                && rect.x2 === 0
                || rect.x1 === rect.x2
                || rect.y1 === rect.y2
            ) {
                return;
            }

            // Determine where to show the arrow.
            var winDim = dfx.getWindowDimensions();

            var pointer = this.getPointer();

            dfx.setStyle(pointer, 'display', 'block');
            if (jQuery.support.opacity === true) {
                dfx.setOpacity(pointer, 1);
            }

            var pointerRect = dfx.getBoundingRectangle(pointer);
            var pointerH    = (pointerRect.y2 - pointerRect.y1);
            var pointerW    = (pointerRect.x2 - pointerRect.x1);

            this.pointerDim.height = pointerH;
            this.pointerDim.width  = pointerW;

            var bounceHeight = 20;
            var scroll       = dfx.getScrollCoords();

            // Try to position the pointer.
            if ((rect.y1 - pointerH + bounceHeight) > scroll.y) {
                // Arrow direction down.
                this.showPointer(elem, 'down');
            } else if ((rect.y2 + pointerH) < (winDim.height - scroll.y)) {
                // Up.
                this.showPointer(elem, 'up');
            } else if ((rect.y2 + pointerW) < winDim.width) {
                // Left.
                this.showPointer(elem, 'left');
            } else if ((rect.y1 - pointerW) > 0) {
                // Right.
                this.showPointer(elem, 'right');
            }
        },

        getPointer: function() {
            if (!this.pointer) {
                this.pointer = document.createElement('div');
                var c        = 'Help';
                dfx.addClass(this.pointer, c + '-pointer');
                dfx.addClass(this.pointer, c + '-pointer-hidden');
                document.body.appendChild(this.pointer);
            }

            return this.pointer;
        },

        showPointer: function(elem, direction) {
            var c = 'Help';

            this._removeDirectionClasses();
            dfx.addClass(this.pointer, c + '-pointer-' + direction);
            dfx.removeClass(this.pointer, c + '-pointer-hidden');

            var rect         = dfx.getBoundingRectangle(elem);
            var top          = 0;
            var left         = 0;
            var bounceHeight = 20;
            switch (direction) {
                case 'up':
                    bounceHeight = (-bounceHeight);
                    top          = rect.y2;
                    if ((rect.x2 - rect.x1) < 250) {
                        left = (this.getRectMidPnt(rect) - (this.pointerDim.width / 2));
                    } else {
                        left = rect.x1;
                    }
                break;

                case 'left':
                    left = rect.x2;
                    top  = this.getRectMidPnt(rect, true);
                break;

                case 'right':
                    left = (rect.x1 - this.pointerDim.width);
                    top  = this.getRectMidPnt(rect, true);
                break;

                case 'down':
                default:
                    top = (rect.y1 - this.pointerDim.height);
                    if ((rect.x2 - rect.x1) < 250) {
                        left = (this.getRectMidPnt(rect) - (this.pointerDim.width / 2));
                    } else {
                        left = rect.x1;
                    }
                break;
            }//end switch

            dfx.setStyle(this.pointer, 'top', top + 'px');
            dfx.setStyle(this.pointer, 'left', left + 'px');

            // Check if the help window is under the pointer then re-position it.
            if (dfx.isChildOf(elem, this.domElem) === true) {
                // Unless it is an element within the Help pop-up.
            } else {
                var coords    = dfx.getBoundingRectangle(this.container);
                rect          = dfx.getBoundingRectangle(this.pointer);
                var posOffset = 20;
                var newPos    = null;
                var midX      = (rect.x1 + ((rect.x2 - rect.x1) / 2));
                var midY      = (rect.y1 + ((rect.y2 - rect.y1) / 2));
                if (coords.x1 <= midX
                    && coords.x2 >= midX
                    && coords.y1 <= midY
                    && coords.y2 >= midY
                ) {
                    // It is on top of the pointer, so move it left or right.
                    var winDim = dfx.getWindowDimensions();
                    // Try to move it to the right first.
                    if (rect.x2 + posOffset + (coords.x2 - coords.x1) < winDim.width) {
                        newPos = (rect.x2 + posOffset);
                    } else {
                        newPos = (rect.x1 - posOffset - (coords.x2 - coords.x1));
                    }

                    dfx.setStyle(this.container, 'right', 'auto');
                    dfx.setStyle(this.container, 'left', newPos + 'px');
                }
            }//end if

            // Stop all pointer animations.
            dfx.stop(this.pointer);

            clearTimeout(this._fadeTimer);
            var self = this;
            dfx.bounce(this.pointer, 4, bounceHeight, function() {
                self._fadeTimer = setTimeout(function() {
                    if (jQuery.support.opacity === true) {
                        dfx.fadeOut(self.pointer, 600);
                    } else {
                        dfx.setStyle(self.pointer, 'display', 'none');
                    }
                }, 1000);
            });
        },

        hidePointer: function() {
            if (this.pointer) {
                // Stop all animations.
                dfx.stop(this.pointer);
                // Fade out.
                dfx.fadeOut(this.pointer, 200);
            }
        },

        getRectMidPnt: function(rect, height) {
            var midPnt = 0;
            if (height === true) {
                midPnt = (rect.y1 + ((rect.y2 - rect.y1) / 2));
            } else {
                midPnt = (rect.x1 + ((rect.x2 - rect.x1) / 2));
            }

            return midPnt;
        },

        _removeDirectionClasses: function() {
            var c = 'Help';
            var d = ['down', 'up', 'left', 'right'];
            var l = d.length;
            for (var i = 0; i < l; i++) {
                dfx.removeClass(this.pointer, c + '-pointer-' + d[i]);
            }
        },

        updateLinkIconState: function(pointerIconElem) {
            // Find out if the element the icon is pointing to exists on the screen.
            var elemid   = dfx.attr(pointerIconElem, 'elemid');
            var refElems = [];
            if (elemid) {
                var elem = dfx.getId(elemid);
                if (elem) {
                    refElems.push(elem);
                }
            } else {
                var elemClass = dfx.attr(pointerIconElem, 'elemClass');
                if (!elemClass) {
                    return;
                }

                refElems = dfx.getClass(elemClass);
            }

            if (refElems.length > 0) {
                // Element is not on the screen hide the link pointer icon.
                dfx.setStyle(pointerIconElem, 'display', 'inline');
            }
        }

    };

};
