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

        // Add event callbacks for screen and mode changes so a message can be displayed.
        var timeout = null;
        GUI.addTemplateAddedCallback(function() {
            clearTimeout(timeout);
            timeout = setTimeout(function() {
                var pageType = _getCurrentPageType();
                if (pageType === 'templateIndexPage' || pageType === 'pageNotFound') {
                    self.showMessage('screenChanged');
                }
            }, 100);
        });

        GUI.addTemplateRemovedCallback(function() {
            clearTimeout(timeout);
            timeout = setTimeout(function() {
                var pageType = _getCurrentPageType();
                if (pageType === 'templateIndexPage' || pageType === 'pageNotFound') {
                    self.showMessage('screenChanged');
                }
            }, 100);
        });
    };

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
        if (this.isPickerActive() === true) {
            return false;
        }

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
        self.hideMessages();
    };

    /**
     * Returns the current page's type (e.g. index page).
     *
     * Uses the className that is applied to the body tag in iframe.
     *
     * @return {string} The page's type.
     */
    _getCurrentPageType = function() {
        var iframeDoc = dfx.getIframeDocument(_iframe);
        if (!iframeDoc || !iframeDoc.body) {
            return null;
        }

        var classNames = iframeDoc.body.className.split(' ');
        if (classNames.length < 2 || classNames[0] !== 'Help-iframe') {
            return;
        }

        return classNames[1];
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
                    if (dfx.isTag(textNodes[j].parentNode, 'script') === true) {
                        continue;
                    }

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

            var iframeDoc         = dfx.getIframeDocument(_iframe);
            var searchResultsList = dfx.getClass('Help-searchResults-list', iframeDoc)[0];

            var currentMoreLink = dfx.getClass('Help-searchResults-moreLink', iframeDoc)[0];

            dfx.insertAfter(searchResultsList.lastChild, wrapper.firstChild.childNodes);

            if (wrapper.childNodes[1]) {
                dfx.insertAfter(currentMoreLink, wrapper.childNodes[1]);
            }

            // Remove the old 'More' link.
            dfx.remove(currentMoreLink);
        }, 'raw');
    };

    this.back = function() {
        if (this.isPickerActive() === true) {
            return false;
        }

        history.back(1);
    };

    this.forward = function() {
        if (this.isPickerActive() === true) {
            return false;
        }

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

    this.picker = function(showMessage) {
        if (showMessage !== false) {
            this.showMessage('pointer');
        }

        var self = this;
        dfx.addEvent(document, 'keypress.Help_finder', function(e) {
            if (e.keyCode === 27) {
                // If ESC key then cancel find.
                dfx.removeEvent(document, 'keypress.Help_finder');
                dfx.removeEvent(document, 'mousedown.Help_finder');
                self.hideMessages();
            }
        });

        dfx.addEvent(document, 'mousedown.Help_finder', function(e) {
            var target = dfx.getMouseEventTarget(e);
            dfx.removeEvent(document, 'keypress.Help_finder');

            /*
                Note: We need to prevent all events on the target element
                and its parents (due to event bubbling) firing so that when
                the element is clicked event functions are not called.

                To do this we need to access JQuery's internal data :(
                First we remove the events from the elements and then
                add them back on mouseup event.
            */

            var target       = dfx.getMouseEventTarget(e);
            var parents      = dfx.getParents(target);
            var parentEvents = _removeEvents(parents);
            var targetEvents = _removeEvents([target]);

            dfx.addEvent(document, 'mouseup.Help_finder', function() {
                setTimeout(function() {
                    _addElemEvents(parentEvents);
                    _addElemEvents(targetEvents);
                }, 150);

                dfx.removeEvent(document, 'mouseup.Help_finder');
            });

            dfx.removeEvent(document, 'mousedown.Help_finder');

            // Ignore if the clicked element is the "find" button.
            if (dfx.hasClass(target, 'Help-message-close') === true) {
                return true;
            }

            var elemInfos = _getIds(parents);
            if (target.id) {
                elemInfos.ids.unshift(target.id);
            }

            if (target.className) {
                elemInfos.classNames.unshift(target.className);
            }

            // Send the ids of target and parents to Help system to find
            // an article.
            var params = {
                ids: dfx.jsonEncode(elemInfos.ids),
                classNames: dfx.jsonEncode(elemInfos.classNames)
            };

            GUI.sendRequest('Help', 'findArticleForElement', params, function(result) {
                var resultParts = result.split('|');
                if (resultParts.length !== 3) {
                    // No results.
                    self.showMessage('pointer-noInfo');
                    self.picker(false);
                } else {
                    var pageid = resultParts[0];
                    Help.pointer.pointTo(resultParts[1], resultParts[2]);
                    Help.loadPage(pageid);
                }
            }, 'raw');

            dfx.preventDefault(e);
            dfx.stopPropagation(e);
            return false;
        });
    };

    this.isPickerActive = function() {
        var msgElem = dfx.getId('Help-dialog-msg-pointer-noInfo');
        if (msgElem && dfx.hasClass(msgElem, 'active') === true) {
            return true;
        }

        msgElem = dfx.getId('Help-dialog-msg-pointer');
        if (msgElem && dfx.hasClass(msgElem, 'active') === true) {
            return true;
        }

        return false;
    };

    /**
     * Removes mouse events from specified elements.
     *
     * When the Help picker is used the events on clicked element and its parents
     * will need to be disabled. To do this we need to reset the onclick, etc.
     * attributes and store the original values in an array. Same must be done for
     * events added via DfxJSLib (jQuery).
     *
     * @param {Array} elements Array of DOMElement's.
     *
     * @return {Array} Array of elements and their events.
     */
    var _removeEvents = function(elements) {
        var events = {
            elements: [],
            dfxEvents: [],
            browserEvents: []
        };

        var eventTypes      = ['onmousedown', 'onmouseup', 'onclick'];
        var eventTypesCount = eventTypes.length;

        var ln = elements.length;
        for (var i = 0; i < ln; i++) {
            var element = elements[i];
            if (element.tagName.toLowerCase() === 'body') {
                break;
            }

            var es = null;
            if (element.tagName.toLowerCase() === 'label') {
                // Special case: label tag, remove event on actual input.
                var elem = dfx.getId(element.htmlFor);
                if (elem) {
                    es = jQuery.data(dfx.getId(element.htmlFor), 'events');
                    if (es) {
                        events.elements.push(dfx.getId(element.htmlFor));
                        events.dfxEvents.push(es);
                        jQuery.data(dfx.getId(element.htmlFor), 'events', {});
                    }
                }
            } else {
                var jqueryEvents = jQuery.data(element, 'events');
                if (jqueryEvents) {
                    // Reset the jQuery events for this element.
                    jQuery.data(element, 'events', {});
                }

                // Also remove browser based events (e.g. elem.onclick).
                var browserEvents = {};
                for (var j = 0; j < eventTypesCount; j++) {
                    var eventType = eventTypes[j];
                    var eventStr  = element[eventType];
                    if (eventStr) {
                        browserEvents[eventType] = eventStr;

                        // Set the attribute to nothing.
                        element[eventType] = '';
                    }
                }

                events.elements.push(element);
                events.dfxEvents.push(jqueryEvents);
                events.browserEvents.push(browserEvents);
            }//end if
        }//end for

        return events;
    };

    var _addElemEvents = function(events) {
        var ln = events.elements.length;
        for (var i = 0; i < ln; i++) {
            if (events.dfxEvents[i]) {
                // Add the jQuery events.
                jQuery.data(events.elements[i], 'events', events.dfxEvents[i]);
            }

            // Add the browser based events.
            dfx.foreach(events.browserEvents[i], function(eventType) {
                events.elements[i][eventType] = events.browserEvents[i][eventType];
            });
        }
    };

    var _getIds = function(elements) {
        var elemInfos = {
            ids: [],
            classNames: []
        };

        var ln = elements.length;
        for (var i = 0; i < ln; i++) {
            elemInfos.ids.push(elements[i].id);
            elemInfos.classNames.push(elements[i].className);
        }

        return elemInfos;
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

    this.showMessage = function(msgid) {
        var elemid  = 'Help-dialog-msg-' + msgid;
        var msgElem = dfx.getId(elemid);

        if (msgElem) {
            if (dfx.attr(dfx.getClass('Help-message', msgElem)[0], 'iniframe') === 'true') {
                var iframeDoc = dfx.getIframeDocument(_iframe);
                if (!iframeDoc.body
                    || dfx.getId(elemid, iframeDoc)
                ) {
                    return;
                }

                var tmpDiv = iframeDoc.createElement('div');
                tmpDiv.id  = msgElem.id;
                dfx.addClass(tmpDiv, 'Help-iframe-msg');
                dfx.setHtml(tmpDiv, dfx.trim(dfx.getHtml(msgElem)));
                dfx.insertBefore(iframeDoc.body.firstChild, tmpDiv);
                _fadeInMessage(tmpDiv);
            } else {
                if (dfx.hasClass(msgElem, 'visible') === true) {
                    jQuery(msgElem).effect("bounce", {distance: 10, times: 3}, 300);
                } else {
                    // Hide all other messages.
                    this.hideMessages();
                    dfx.addClass(msgElem, 'visible');
                }

                dfx.addClass(dfx.getId('Help-overlay'), 'visible');
            }//end if
        }//end if
    };

    this.hideMessages = function(inIframe) {
        if (inIframe === true) {
            var iframeDoc = dfx.getIframeDocument(_iframe);
            var msgElem   = dfx.getClass('Help-iframe-msg', iframeDoc.body)[0];
            dfx.removeClass(msgElem, 'visible');
            self._fadeOutMessage(msgElem, function() {
                dfx.remove(msgElem);
            });
        } else {
            dfx.removeClass(dfx.getClass('Help-dialog-msg', dfx.getId('Help-iframeWrapper')), 'visible');
            dfx.removeClass(dfx.getId('Help-overlay'), 'visible');
        }
    };

    _fadeInMessage = function(msgElem) {
        // Fade in and move down.
        dfx.setStyle(msgElem, 'opacity', '0');
        dfx.setStyle(msgElem, 'display', 'block');
        dfx.setStyle(msgElem, 'margin-top', -dfx.getElementHeight(msgElem));
        dfx.animate(msgElem, {marginTop: 0, opacity: 1}, 1000, function() {
            dfx.setStyle(msgElem, 'display', 'block');
            dfx.setStyle(msgElem, 'margin-top', '10px');
            dfx.setStyle(msgElem, 'opacity', 1);
        });
    };

    _fadeOutMessage = function(msgElem, callback) {
        // Fade out and move up.
        dfx.animate(
            msgElem,
            {
                marginTop: (-dfx.getElementHeight(msgElem)),
                opacity: 0
            },
            500,
            function() {
                dfx.setStyle(msgElem, 'display', 'none');
                dfx.setStyle(msgElem, 'margin-top', '0px');
                dfx.setStyle(msgElem, 'opacity', 1);
                if (callback) {
                    callback.call(this, msgElem);
                }
            }
        );
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

            // Scroll in to view if not visible.
            if (elem.scrollIntoView && (rect.y1 < scroll.y || rect.y1 > scroll.y + winDim.height)) {
                elem.scrollIntoView(false);
            }

            // Try to position the pointer.
            if ((rect.y1 - pointerH - bounceHeight) > scroll.y) {
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
            // Unless it is an element within the Help pop-up.
            if (dfx.isChildOf(elem, dfx.getId('Help-dialog')) !== true) {
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

                    dfx.setStyle(this.container, 'margin-left', '0');
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
