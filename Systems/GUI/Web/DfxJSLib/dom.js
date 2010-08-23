/**
 * Collection of DOM related JS functions.
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

if (!window.dfx) {
    window.dfx = function() {};
}

/**
 * General DOM operations that are used in javascript.
 */

function dom()
{
    /*
     * Setup any general data that needs to be stored for DOM operations.
     */
     this.scrollBarWidth = null;

}


/*
 * Keeps the track of all included scripts' URLs.
 */
dfx.scriptsIncluded = [];

/*
 * Keeps the track of all CSS files included.
 */
dfx.cssIncluded = [];

/*
 * Node constants.
 */
dfx.ELEMENT_NODE                = 1;
dfx.ATTRIBUTE_NODE              = 2;
dfx.TEXT_NODE                   = 3;
dfx.CDATA_SECTION_NODE          = 4;
dfx.ENTITY_REFERENCE_NODE       = 5;
dfx.ENTITY_NODE                 = 6;
dfx.PROCESSING_INSTRUCTION_NODE = 7;
dfx.COMMENT_NODE                = 8;
dfx.DOCUMENT_NODE               = 9;
dfx.DOCUMENT_TYPE_NODE          = 10;
dfx.DOCUMENT_FRAGMENT_NODE      = 11;
dfx.NOTATION_NODE               = 12;

/**
 * Retrieves all elements underneath the parent element that contain the class.
 *
 * @param {String}     className    Space seperated string of classes to search
 *                                  on.
 * @param {DomElement} startElement The element to start from.
 *                                  If null, will search the whole document.
 * @param {String}     tagName      The elements to restrict the search to.
 *                                  Eg. 'div span'.
 *
 * @return An array of elements that have the passed class applied.
 * @type   Array(DomElement)
 */
dfx.getClass = function(className, startElement, tagName)
{
    if (!startElement) {
        startElement = document.body;
    }

    className = '.' + className.split(' ').join('.');

    if (tagName) {
        className = tagName + className;
    }

    return jQuery.makeArray(jQuery(startElement).find(className));

};

/**
 * Returns the object with the passed ID.
 *
 * @param {String}     id           The ID of the element to find.
 * @param {DomElement} startElement The element to search under (if omitted
 *                                  defaults to document).
 *
 * @return DomElement
 */
dfx.getId = dom.getId = function(id, startElement)
{
    if (!startElement) {
        startElement = document;
    }

    element = startElement.getElementById(id);
    return element;

};

/**
 * Returns objects with the passed tag name, beneath the optional start element.
 *
 * @param {String}     tagName      The type of tag to find.
 * @param {DomElement} startElement The element to search under (if omitted
 *                                  defaults to document).
 *
 * @return Array(DomElement).
 */
dfx.getTag = function(tagName, startElement)
{
    if (!startElement) {
        startElement = document;
    }

    return jQuery.makeArray(jQuery(startElement).find(tagName));

};


/**
 * Retrieves all elements that match the string passed.
 *
 * These can be '.className', '#id', 'tagname', or 'tagname.classname'.
 *
 * @param {String} searchString The string of elements to search for.
 * @param object startElement   The element to start the search from. If omitted
 *                              the whole document will be searched.
 *
 * @return array(DomElement)
 */
dfx.getElements = function(searchString, startElement)
{
    if (!startElement) {
        startElement = document;
    }

    var searchElems   = searchString.split(' ');
    var matches       = [];
    var searchResults = [];
    var schLen        = searchElems.length;
    for (var i = 0; i < schLen; i++) {
        searchResults = [];
        if (searchElems[i].match(/.+\./)) {
            // A class query.
            var classPieces = searchElems[i].split('.');
            if (classPieces.length === 2) {
                searchResults = dfx.getClass(classPieces[1], startElement, classPieces[0]);
            } else {
                searchResults = dfx.getClass(classPieces[0], startElement);
            }
        } else if (searchElems[i].match(/^#/)) {
            // An ID query.
            var idString = searchElems[i].substring(1);
            searchResults.push(dfx.getId(idString));
        } else {
            // Just a tag query.
            searchResults = dfx.getTag(searchElems[i], startElement);
        }

        matches.mergeCollection(searchResults);
    }//end for

    return matches;

};//end dom.getElements()


/**
 * Retrieves the coordinate of the element relative to the page.
 *
 * @param {DomElement} el  The element which we want the coordinates for.
 *
 * @return Object
 */
dfx.getElementCoords = function(element, relative)
{
    var offset = jQuery(element).offset();
    return {
        x: offset.left,
        y: offset.top
    };

};

dfx.setCoords = function(element, x, y)
{
    dfx.setStyle(element, 'top', y + 'px');
    dfx.setStyle(element, 'left', x + 'px');

};


/**
 * Returns the current scroll position of the screen.
 *
 * @return Object
 * @type   Object
 */
dfx.getScrollCoords = function()
{
    var scrollX = 0;
    var scrollY = 0;
    if (window.pageYOffset) {
        // Mozilla, Firefox, Safari and Opera will fall into here.
        scrollX = window.pageXOffset;
        scrollY = window.pageYOffset;
    } else if (document.body && (document.body.scrollLeft || document.body.scrollTop)) {
        // This is the DOM compliant method of retrieving the scroll position.
        // Safari and OmniWeb supply this, but report wrongly when the window
        // is not scrolled. They are caught by the first condition however, so
        // this is not a problem.
        scrollX = document.body.scrollLeft;
        scrollY = document.body.scrollTop;
    } else {
        // Internet Explorer will get into here when in strict mode.
        scrollX = document.documentElement.scrollLeft;
        scrollY = document.documentElement.scrollTop;
    }

    var coords = {
        x: scrollX,
        y: scrollY
    };
    return coords;

};//end dom.getScrollCoords()


/**
 * Returns the width of the element including any padding and borders.
 *
 * @param {DomElement} element The element which we want the width for.
 *
 * @return integer
 */
dfx.getElementWidth = function(element)
{
    return element.offsetWidth;

};//end dom.getElementWidth()


/**
 * Returns the height of the element including any padding and borders.
 *
 * @param {DomElement} element The element which we want the height for.
 *
 * @return integer
 */
dfx.getElementHeight = function(element)
{
    return element.offsetHeight;

};//end dom.getElementHeight()


/**
 * Returns the absolute dimensions of the passed element.
 *
 * @param {DomElement} element The element which we want the dimensions for.
 *
 * @return Object
 */
dfx.getElementDimensions = function(element)
{
    var result = {
        'width'  : dfx.getElementWidth(element),
        'height' : dfx.getElementHeight(element)
    };

    return result;

};//end dom.getElementDimensions()


/**
 * Returns the coordinates of a rectangle that will cover the element.
 *
 * Returns the coordinates of the top left corner (x1, y1) as well as the
 * bottom-right corner (x2, y2).
 *
 * @param {DomElement} element The element which we want the dimensions for.
 *
 * @return The 2 x and 2 y coordinates of the element's bounding rectangle.
 * @type   Object
 */
dfx.getBoundingRectangle = function(element)
{
    // Retrieve the coordinates and dimensions of the element.
    var coords     = dfx.getElementCoords(element);
    var dimensions = dfx.getElementDimensions(element);
    // Create an array by using the elements dimensions.
    var result = {
        'x1' : coords.x,
        'y1' : coords.y,
        'x2' : coords.x + dimensions.width,
        'y2' : coords.y + dimensions.height
    };
    return result;

};//end dom.getBoundingRectangle()


/**
 * Returns the dimensions of the viewable window.
 *
 * @return The 2 x and 2 y coordinates of the viewable window.
 * @type   Object
 */
dfx.getWindowDimensions = function()
{
    var windowWidth  = 0;
    var windowHeight = 0;
    if (window.innerWidth) {
        // Will work on Mozilla, Opera and Safari etc.
        windowWidth  = window.innerWidth;
        windowHeight = window.innerHeight;
        // If the scrollbar is showing (it is always showing in IE) then its'
        // width needs to be subtracted from the height and/or width.
        var scrollWidth = dfx.getScrollbarWidth();
        // The documentElement.scrollHeight.
        if (document.documentElement.scrollHeight > windowHeight) {
            // Scrollbar is shown.
            if (typeof scrollWidth === 'number') {
                windowWidth -= scrollWidth;
            }
        }

        if (document.body.scrollWidth > windowWidth) {
            // Scrollbar is shown.
            if (typeof scrollWidth === 'number') {
                windowHeight -= scrollWidth;
            }
        }
    } else if (document.documentElement && (document.documentElement.clientWidth || document.documentElement.clientHeight)) {
        // Internet Explorer.
        windowWidth  = document.documentElement.clientWidth;
        windowHeight = document.documentElement.clientHeight;
    } else if (document.body && (document.body.clientWidth || document.body.clientHeight)) {
        // Browsers that are in quirks mode or weird examples fall through here.
        windowWidth  = document.body.clientWidth;
        windowHeight = document.body.clientHeight;
    }//end if

    var result = {
        'width'  : windowWidth,
        'height' : windowHeight
    };
    return result;

};//end dfx.getWindowDimensions()


/**
 * Returns the width of the scrollbar programmatically.
 *
 * This is necessary to determine the viewable browser window, as Firefox and
 * Opera only display the scrollbars when necessary and report their window
 * dimensions without the scrollbar. This method will create a div out of the
 * viewable range, and measure it with and without a scrollbar. The difference
 * is returned.
 * NB: This will only be run once, as the value is stored after execution.
 *
 * @return integer
 */
dfx.getScrollbarWidth = function()
{
    if (dom.scrollBarWidth) {
        return dom.scrollBarWidth;
    }

    var wrapDiv            = null;
    var childDiv           = null;
    var widthNoScrollBar   = 0;
    var widthWithScrollBar = 0;
    // Outer scrolling div.
    wrapDiv                = document.createElement('div');
    wrapDiv.style.position = 'absolute';
    wrapDiv.style.top      = '-1000px';
    wrapDiv.style.left     = '-1000px';
    wrapDiv.style.width    = '100px';
    wrapDiv.style.height   = '50px';
    // Start with no scrollbar.
    wrapDiv.style.overflow = 'hidden';

    // Inner content div.
    childDiv              = document.createElement('div');
    childDiv.style.width  = '100%';
    childDiv.style.height = '200px';

    // Put the inner div in the scrolling div.
    wrapDiv.appendChild(childDiv);
    // Append the scrolling div to the doc.
    document.body.appendChild(wrapDiv);

    // Width of the inner div sans scrollbar.
    widthNoScrollBar = childDiv.offsetWidth;
    // Add the scrollbar.
    wrapDiv.style.overflow = 'auto';
    // Width of the inner div width scrollbar.
    widthWithScrollBar = childDiv.offsetWidth;

    // Remove the scrolling div from the doc.
    document.body.removeChild(document.body.lastChild);

    // Pixel width of the scroller.
    var scrollBarWidth = (widthNoScrollBar - widthWithScrollBar);
    // Set the DOM variable so we don't have to run this again.
    dom.scrollBarWidth = scrollBarWidth;
    return scrollBarWidth;

};//end dom.getScrollbarWidth()


/**
 * Completely removes all content that is contained within the passed element.
 *
 * If the element was the following: <p id="mypara"><span>Content</span></p>,
 * when called with dom.empty(dfx.getId('mypara')) the paragraph would become
 * <p id="mypara"></p>.
 *
 * @param {DomElement} element The element to empty.
 *
 * @return TRUE if the element is successfully emptied.
 * @type   boolean
 */
dfx.empty = function(element)
{
    if (element) {
        return jQuery(element).empty();
    }

};//end dom.empty()


/**
 * Removes the element from the document.
 *
 * The element is removed by telling the element's parent node to remove the
 * element we want.
 *
 * @param {DomElement} element The element to remove from the document.
 *
 * @return TRUE if the element is removed successfully.
 * @type   bool
 */
dfx.remove = function(element)
{
    if (element) {
        return jQuery(element).remove();
    }

};//end dom.remove()


/**
 * Insert the passed element as the first child element of the parent.
 *
 * @param {DomElement} parent The element to insert underneath.
 * @param {DomElement} elem   The element to insert underneath the parent.
 *
 * @return TRUE if the element was inserted correctly.
 * @type   bool
 */
dfx.prepend = function(parent, elem)
{
    jQuery(parent).prepend(elem);

};//end dfx.prepend()


/**
 * Inserts an element as the last child of the parent element.
 *
 * @param {DomElement} parent The parent to insert under.
 * @param {DomElement} elem   The element to insert underneath the parent.
 *
 * @return TRUE if the element is successfully inserted.
 * @type   bool
 */
dfx.append = function(parent, elem)
{
    jQuery(parent).append(elem);

};//end dfx.insertLast()


/**
 * Inserts a new element directly before another element in the DOM tree.
 *
 * @param {DomElement} before The element that is to be inserted before.
 * @param {DomElement} elem   The element to insert.
 *
 * @return TRUE if the element is inserted correctly.
 * @type   bool
 */
dfx.insertBefore = function(before, elem)
{
    jQuery(before).before(elem);

};//end dfx.insertBefore()


/**
 * Inserts an element in the DOM tree before another element.
 *
 * @param {DomElement} after The element to insert after.
 * @param {DomElement} elem  The element to insert.
 *
 * @return TRUE If the element is inserted correctly.
 * @type   bool
 */
dfx.insertAfter = function(after, elem)
{
    jQuery(after).after(elem);

};//end dom.insertAfter()


/**
 * Get the content of an element.
 *
 * If the element is a textbox or another element that doesn't have HTML then
 * its' value will be returned, which allows this method to be used to retrieve
 * the content of any element.
 *
 * @param {DomElement} element The element to retrieve the content from.
 *
 * @return The content of the element.
 * @type   String
 */
dfx.getHtml = function(element)
{
    return jQuery(element).html();

};//end dfx.getHtml()


/**
 * Sets the content of an element.
 *
 * If the element doesn't have an innerHTML property, then the value or other
 * equivalent property will be set instead, so this method can and should be
 * used for all elements.
 *
 * @param {DomElement} element The element to set content for.
 * @param {String}     content The content to apply to the element.
 *
 * @return TRUE if the content is correctly set.
 * @type   bool
 */
dfx.setHtml = function(element, content)
{
    if (element) {
        jQuery(element).html(content);
    }

};//end dfx.setHtml()


/**
 * Adds the passed string to the end of the element's current content.
 *
 * @param {DomElement} element The element to add the content to.
 * @param {string}     content The content to add to the element.
 *
 * @return TRUE if the content was able to be added to the element.
 * @type   bool
 */
dfx.appendHtml = function(element, content)
{
    jQuery(element).html(dfx.getHtml() + content);

};//end dfx.appendHtml()


/**
 * Adds the passed content to the beginning of the element's content.
 *
 * @param {DomElement} element The element to add the content to.
 * @param {string}     content The content to add to the start of the element.
 *
 * @return TRUE if the content was successfully added.
 * @type   bool
 */
dfx.prependHtml = function(element, content)
{
   jQuery(element).html(content + dfx.getHtml());

};//end dfx.prependHtml()


dfx.getParents = function(elements, filter, stopEl)
{
    var res = jQuery(elements).parents(filter);
    var ln  = res.length;
    var ar  = [];
    for (var i = 0; i < ln; i++) {
        if (res[i] === stopEl) {
            break;
        }

        ar.push(res[i]);
    }

    return ar;

};

/**
 * Returns the siblings of the element.
 *
 * @param DomNode element          The element.
 * @param string  dir              Direction of the siblings. (values: prev, next).
 * @param boolean elementNodesOnly If true then only the ELEMENT_NODEs will be returned.
 *                                 Other nodes like TEXT_NODE will be ignored.
 * @param DomNode stopElem         If specified any sibling from stopElem will not be returned.
 */
dfx.getSiblings = function(element, dir, elementNodesOnly, stopElem)
{
    if (elementNodesOnly === true) {
        if (dir === 'prev') {
            return jQuery(element).prevAll();
        } else {
            return jQuery(element).nextAll();
        }
    } else {
        var elems = [];
        if (dir === 'prev') {
            while (element.previousSibling) {
                element = element.previousSibling;
                if (element === stopElem) {
                    break;
                }

                elems.push(element);
            }
        } else {
            while (element.nextSibling) {
                element = element.nextSibling;
                if (element === stopElem) {
                    break;
                }

                elems.push(element);
            }
        }

        return elems;
    }//end if

};

/**
 * Normalizes the given element (ignores block elements).
 * It will look at the element's siblings and if they are the same tag
 * then they will be merged.
 */
dfx.normalize = function(element)
{
    if (dfx.isBlockElement(element) === true) {
        return false;
    }

    while (element.nextSibling) {
        var next = element.nextSibling;
        if (element.nodeType === dfx.TEXT_NODE) {
            if (next.nodeType === dfx.TEXT_NODE) {
                dfx.remove(next);
                element.nodeValue += next.nodeValue;
            } else {
                break;
            }
        } else if (element.tagName === next.tagName) {
            dfx.remove(next);
            var childLen = next.childNodes.length;
            while (childLen > 0) {
                element.appendChild(next.firstChild);
                childLen = next.childNodes.length;
            }
        } else {
            break;
        }
    }

    // Join the element to previous siblings.
    while (element.previousSibling) {
        var prev = element.previousSibling;
        if (element.nodeType === dfx.TEXT_NODE) {
            if (prev.nodeType === dfx.TEXT_NODE) {
                dfx.remove(element);
                prev.nodeValue += element.nodeValue;
                element = prev;
            } else {
                break;
            }
        } else if (element.tagName === prev.tagName) {
            dfx.remove(element);
            var childLen = element.childNodes.length;
            while (childLen > 0) {
                prev.appendChild(element.firstChild);
                childLen = element.childNodes.length;
            }

            element = prev;
        } else {
            break;
        }
    }//end while

};

dfx.normalizeChildren = function(parent)
{
    if (parent.nodeType !== dfx.ELEMENT_NODE) {
        return false;
    }

    var nodeLen = parent.childNodes.length;
    for (var i = 0; i < nodeLen; i++) {
        var child = parent.childNodes[i];
        if (child) {
            if (child.nodeType !== dfx.TEXT_NODE) {
                dfx.normalizeChildren(child);
            }

            dfx.normalize(child);
        }
    }

};


/**
 * Dynamically includes a JavaScript source file.
 *
 * @param {String} url The URL of the file to include.
 *
 * @return void
 * @type void
 */
dfx.includeScript = function(url, callback)
{
    if (url.indexOf('http://') === -1 && url.indexOf('https://') === -1) {
        // Ignore this URL.
        return;
    }

    // Note: If url returns 404 then jQuery wont call callBack.
    jQuery.getScript(url, callback);

};//end dfx.includeScript()


dfx.includeScripts = function(urls, callback)
{
    if (!urls || urls.length === 0) {
        callback.call(this);
    } else {
        var url = urls.shift();
        dfx.includeScript(url, function() {
           dfx.includeScripts(urls, callback);
        });
    }

};


/**
 * Dynamically includes a Css source file.
 *
 * @param {String} url The URL of the file to include.
 *
 * @return void
 * @type void
 */
dfx._includeCss = function(url)
{
    var head   = document.getElementsByTagName('head').item(0);
    var links  = head.getElementsByTagName('link');
    var lnkLen = links.length;
    for (var i = 0; i < lnkLen; i++) {
        if (links[i].href === url) {
            return;
        }
    }

    var link = document.createElement('link');
    jQuery(link).attr({
        href: url,
        media: 'screen',
        type: 'text/css',
        rel: 'stylesheet'
    }).appendTo('head');

};//end dfx.includeCss()


dfx.includeCss = function(url, callback)
{
    var self = this;
    if (Widget.CSS_MINIFIED === true) {
        if (callback) {
            callback.call(self);
        }
    } else {
        if (dfx.cssIncluded[url] !== true) {
            var loadCss = function(cb) {
                var loadData = {
                    css: true,
                    action: 'loadCss',
                    url: url
                };

                dfx.post(window.location.href, loadData, function(cssContent) {
                    dfx.addCssToStyle(cssContent);
                    dfx.cssIncluded[url] = true;
                    if (cb) {
                        cb.call(self);
                    }
                });
            };

            loadCss(callback);
        } else {
            if (callback) {
                callback.call(self);
            }
        }//end if
    }//end if

};//end dfx.includeCss()

dfx.addCssToStyle = function(cssContent, id)
{
    var styleid = 'backendStyle';
    if (id) {
        styleid = id;
    }

    var head     = document.getElementsByTagName("head").item(0);
    var styleTag = document.getElementById(styleid);
    if (styleTag) {
        // Updte the main one.
        if (styleTag.styleSheet) {
            styleTag.styleSheet.cssText = styleTag.styleSheet.cssText + cssContent;
        } else {
            dfx.setNodeTextContent(styleTag, dfx.getNodeTextContent(styleTag) + cssContent);
        }
    } else {
        styleTag = document.createElement('style');
        styleTag.setAttribute('type', 'text/css');
        styleTag.setAttribute('id', styleid);
        if (styleTag.styleSheet) {
            styleTag.styleSheet.cssText = cssContent;
        } else {
            dfx.setNodeTextContent(styleTag, cssContent);
        }

        head.appendChild(styleTag);
    }

};

/**
 * Returns the textual information within a text node.
 *
 * @param {Text} The node to reteive the text for.
 *
 * @return string
 * @type void
 */
dfx.getNodeTextContent = function(node)
{
   return jQuery(node).text();

};//end dfx.getNodeTextContent()

/**
 * Set the textual information within a text node.
 *
 * @param {Text} The node to manipulate.
 * @param {String} Text value of the node.
 *
 * @return void
 * @type void
 */
dfx.setNodeTextContent = function(node, txt)
{
   return jQuery(node).text(txt);

};//end dfx.setNodeTextContent()


/**
 * Returns the document element for the specified iframe.
 *
 * @param {IFrameElement} iframe The iframe to retreive the document for.
 *
 * @return {DocumentElement}
 */
dfx.getIframeDocument = function(iframe)
{
    var doc = null;
    if (iframe.contentDocument) {
        doc = iframe.contentDocument;
    } else if (iframe.contentWindow) {
        doc = iframe.contentWindow.document;
    } else if (iframe.document) {
        doc = iframe.document;
    }

    return doc;

};//end dfx.getIframeDocument()

/**
 * Retrurns TRUE if the specified element is a block element.
 *
 * @param {DOMElement} element The element to check.
 *
 * @return {boolean}
 */
dfx.isBlockElement = function(element)
{
    switch (element.nodeName.toLowerCase()) {
        case 'p':
        case 'div':
        case 'pre':
        case 'ul':
        case 'ol':
        case 'li':
        case 'table':
        case 'tbody':
        case 'td':
        case 'th':
        case 'fieldset':
        case 'form':
        case 'blockquote':
        case 'dl':
        case 'dir':
        case 'center':
        case 'address':
        case 'h1':
        case 'h2':
        case 'h3':
        case 'h4':
        case 'h5':
        case 'h6':
        case 'img':
            return true;
        break;

        default:
            return false;
        break;
    }//end switch

    return false;

};//end dfx.isBlockElement()

/**
 * Returns true if the specified element does not have any children.
 *
 * @param {DOMElement} element The element to check.
 *
 * @return {boolean}
 */
dfx.isStubElement = function(element)
{
    if (element) {
        switch (element.nodeName.toLowerCase()) {
            case 'img':
            case 'br':
            case 'hr':
                return true;
            break;

            default:
                return false;
            break;
        }
    }

    return false;

};//end dfx.isStubElement()

/**
 * Returns true if the specified element is a child of parent.
 */
dfx.isChildOf = function(el, parent)
{
    try {
        while (el && el.parentNode) {
            if (el.parentNode === parent) {
                return true;
            }

            el = el.parentNode;
        }
    } catch (e) {
        // Sometimes elements have "Invalid argument" as parentNode in IE
        // which causes exception..
    }

    return false;

};


/**
 * Return any form data under the element.
 */
dfx.getFormData = function(element)
{
    var data = {}, parent = null;
    if (element) {
        parent = element;
    } else {
        parent = document;
    }

    $(':input', parent).each(function() {
        var type = this.type;
        var tag  = this.tagName.toLowerCase();
        if (type === 'text' || type === 'password' || tag === 'textarea') {
            data[this.name] = this.value;
        }

        if (type === 'checkbox' || type === 'radio') {
            data[this.name] = this.checked;
        }

        if (tag === 'select') {
            data[this.name] = this.selectedIndex;
        }
    });

    return data;

};//end getFormData()


/**
 * Clear any form data under the element.
 */
dfx.clearFormData = function(element)
{
    var parent = null;
    if (element) {
        parent = element;
    } else {
        parent = document;
    }

    $(':input', parent).each(function() {
        var type = this.type;
        var tag  = this.tagName.toLowerCase();
        if (type === 'text' || type === 'password' || tag === 'textarea') {
            this.value = "";
        }

        if (type === 'checkbox' || type === 'radio') {
            this.checked = false;
        }

        if (tag === 'select') {
            this.selectedIndex = -1;
        }
    });

};//end getFormData()


dfx.cloneNode = function(elems)
{
    return jQuery(elems).clone(true);

};

/**
 * Sets/Gets the attribute for specified element(s).
 *
 * @param mixed element Element(s) to modify.
 * @param mixed key     Key can the the attribute name or key/value object.
 * @param mixed value   Value can be string, number, function.
 *                      Ignored when key param is an object.
 */
dfx.attr = function(elements, key, val)
{
    return jQuery(elements).attr(key, val);

};

dfx.removeAttr = function(elements, name)
{
    jQuery(elements).removeAttr(name);

};


/**
 * Returns elements between fromElem and toElem.
 * Example:
 * <div>text
 *    <span id="x">SPAN</span>
 *    <br />
 *    <p>
 *          <strong>text</strong>
 *    </p>
 * </div>
 *  <div>
 *    <div id="a">AAAA</div>
 *    <div id="y">xxxxx</div>
 *  </div>
 * Calling getElementsFromTo(x, y);
 * Result: [br, p, div(id=a)]
 */
dfx.getElementsBetween = function(fromElem, toElem)
{
    var elements = [];

    if (fromElem === toElem) {
        return elements;
    }

    // The toElem could be a child of fromElem.
    if (dfx.isChildOf(toElem, fromElem) === true) {
        var fElemLen = fromElem.childNodes.length;
        for (var i = 0; i < fElemLen; i++) {
            if (fromElem.childNodes[i] === toElem) {
                break;
            } else if (dfx.isChildOf(toElem, fromElem.childNodes[i]) === true) {
                return dfx.arrayMerge(elements, dfx.getElementsBetween(fromElem.childNodes[i], toElem));
            } else {
                elements.push(fromElem.childNodes[i]);
            }
        }

        return elements;
    }

    // Get the next siblings of the fromElem.
    var startEl = fromElem.nextSibling;
    while (startEl) {
        if (dfx.isChildOf(toElem, startEl) === true) {
            // If the toElem is a child of this element then
            // we have to get the elements from this node to target node.
            elements = dfx.arrayMerge(elements, dfx.getElementsBetween(startEl, toElem));
            return elements;
        } else if (startEl === toElem) {
            return elements;
        } else {
            elements.push(startEl);
            startEl = startEl.nextSibling;
        }
    }

    var fromParents = dfx.getParents(fromElem);
    var toParents   = dfx.getParents(toElem);

    // Find the parents of fromElem that are not parent of toElem.
    var parentElems = dfx.arrayDiff(fromParents, toParents, true);
    var pElemLen    = parentElems.length;
    for (var j = 0; j < (pElemLen - 1); j++) {
        elements = dfx.arrayMerge(elements, dfx.getSiblings(parentElems[j], 'next'));
    }

    var lastParent = parentElems[(parentElems.length - 1)];
    elements       = dfx.arrayMerge(elements, dfx.getElementsBetween(lastParent, toElem));

    return elements;

};

dfx.getCommonAncestor = function(a, b)
{
    var node = a;
    while (node) {
        if (dfx.isChildOf(b, node) === true) {
            return node;
        }

        node = node.parentNode;
    }

    return null;

};


dfx.getNextNode = function(node)
{
    if (node.nextSibling) {
        return node.nextSibling;
    } else if (node.parentNode) {
        return dfx.getFirstChild(node.parentNode);
    }

    return null;

};

dfx.getPrevNode = function(node)
{
    if (node.previousSibling) {
        return node.previousSibling;
    } else if (node.parentNode) {
        return dfx.getLastChild(node.parentNode);
    }

    return null;

};

dfx.getFirstChild = function(node)
{
    if (node.firstChild) {
        if (node.firstChild.nodeType === dfx.ELEMENT_NODE) {
            return dfx.getFirstChild(node.firstChild);
        } else {
            return node.firstChild;
        }
    }

    return node;

};

dfx.getLastChild = function(node)
{
    if (node.lastChild) {
        if (node.lastChild.nodeType === dfx.ELEMENT_NODE) {
            return dfx.getLastChild(node.lastChild);
        } else {
            return node.lastChild;
        }
    }

    return node;

};

dfx.removeEmptyNodes = function(parent, callback)
{
    var elems = jQuery(parent).find(':empty');
    var i     = elems.length;
    while (i > 0) {
        i--;
        if (dfx.isStubElement(elems[i]) === false) {
            if (!callback || callback.call(this, elems[i]) !== false) {
                dfx.remove(elems[i]);
            }
        }
    }

};

dfx.find = function(parent, exp)
{
    // Note: For valid selectors for exp go to http://docs.jquery.com/Selectors.
    return jQuery(parent).find(exp);

};

dfx.getTextNodes = function(parent, removeEmpty)
{
    var nodes = [];

    if (parent && parent.childNodes) {
        var ln = parent.childNodes.length;
        for (var i = 0; i < ln; i++) {
            var child = parent.childNodes[i];
            if (child.nodeType === dfx.TEXT_NODE) {
                if (removeEmpty === true && /^\s*$/.test(child.data) === true) {
                    dfx.remove(child);
                } else {
                    nodes.push(child);
                }
            } else if (child.childNodes && child.childNodes.length > 0) {
                nodes = nodes.concat(dfx.getTextNodes(child));
            }
        }
    }

    return nodes;

};

dfx.isTag = function(node, tag)
{
    if (node && node.tagName && node.tagName.toLowerCase() === tag.toLowerCase()) {
        return true;
    }

    return false;

};

dfx.getTagName = function(node)
{
    if (node && node.tagName) {
        return node.tagName.toLowerCase();
    }

    return null;

};

dfx.getFirstBlockParent = function(elem, stopEl)
{
    while (elem.parentNode) {
        elem = elem.parentNode;
        if (stopEl && elem === stopEl) {
            return null;
        }

        if (dfx.isBlockElement(elem) === true) {
            return elem;
        }
    }

    return null;

};

dfx.walk = function(elem, callback, lvl)
{
    if (!elem) {
        return;
    }

    if (!lvl) {
        lvl = 0;
    }

    var retVal = callback.call(this, elem, lvl);
    if (retVal === false) {
        // Stop.
        return;
    }

    if (elem.childNodes && elem.childNodes.length > 0) {
        dfx.walk(elem.firstChild, callback, (lvl + 1));
    } else if (elem.nextSibling) {
        dfx.walk(elem.nextSibling, callback, lvl);
    } else if (elem.parentNode && elem.parentNode.nextSibling) {
        dfx.walk(elem.parentNode.nextSibling, callback, (lvl - 1));
    }

};

dfx.revWalk = function(elem, callback)
{
    if (!elem) {
        return;
    }

    var retVal = callback.call(this, elem);
    if (retVal === false) {
        // Stop.
        return;
    }

    if (elem.childNodes && elem.childNodes.length > 0) {
        dfx.walk(elem.lastChild, callback);
    } else if (elem.previousSibling) {
        dfx.walk(elem.previousSibling, callback);
    } else if (elem.parentNode && elem.parentNode.previousSibling) {
        dfx.walk(elem.parentNode.previousSibling, callback);
    }

};

/**
 * Disables the text selection on specified element.
 * If text is already selected and the specified element is clicked then
 * the selection will not change. Also changes the cursor to 'default'
 *
 * @param {DomElement} elem       A DOM Element.
 * @param {boolean}    selectable If TRUE then selection will be disabled and
 *                                cursor will be set to 'default'.
 */
dfx.setUnselectable = function(elem, selectable)
{
    if (elem) {
        if (selectable === true) {
            elem.unselectable = 'on';
            dfx.setStyle(elem, '-moz-user-select', 'none');
            dfx.setStyle(elem, 'cursor', 'default');
        } else {
            elem.unselectable = 'off';
            dfx.setStyle(elem, '-moz-user-select', 'normal');
            dfx.setStyle(elem, 'cursor', 'auto');
        }
    }

};

/**
 * Google Analytics code uses document.write() method, which is a fail in
 * the backend since HTML contents is printed after page load, and therefore
 * document.write replaces the whole HTML. So, document.write( is replaced with
 * dfx.noDocWriteAllowed. True story.
 */
dfx.noDocWriteAllowed = function() {};
