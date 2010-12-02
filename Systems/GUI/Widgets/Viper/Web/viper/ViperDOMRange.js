/**
 * Represents an Abstract Range object. This object is an extension of the
 * W3C range object and expects that the extending objects only implement the
 * W3C IDL Definition as per
 * http =//www.w3.org/TR/DOM-Level-2-Traversal-Range/ranges.html.
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
 * @package    CMS
 * @subpackage Editing
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

function ViperDOMRange(rangeObj)
{
    this.rangeObj = rangeObj;

    /*
     * The container where the range starts.
     *
     * @type Node
     */
    this.startContainer = null;

    /*
     * The container where the range end.
     *
     * @type Node
     */
    this.endContainer = null;

    /*
     * The offset within the start container where the range starts.
     *
     * @type Integer (unsigned)
     */
    this.startOffset = 0;

    /*
     *  The offset within the end container where the range ends.
     *
     * @type Integer (unsigned)
     */
    this.endOffset = 0;

    /*
     * If TRUE the start and end points of the range are equal.
     *
     * @type boolean
     */
    this.collapsed = true;

    /*
     * The first parent element that is shared by the startContainer and
     * endContainer.
     *
     * @type Node
     */
    this.commonAncestorContainer = null;

    /*
     * If TRUE the anchor point will exist at the start of the range. Otherwise
     * it will exist at the end of the range.
     *
     * @type boolean
     */
    this.anchorToStart = 'undefined';

}

/*
 * Compares the start of a range with the start of another range.
 *
 * @type Integer
 * @see compareBoundaryPoints()
 */
ViperDOMRange.START_TO_START = 0;

/*
 * Compares the start of a range with the end of another range.
 *
 * @type Integer
 * @see compareBoundaryPoints()
 */
ViperDOMRange.START_TO_END = 1;

/*
 * Compares the end of a range with the end of another range.
 *
 * @type Integer
 * @see compareBoundaryPoints()
 */
ViperDOMRange.END_TO_END = 3;

/*
 * Compares the end of a range with the start of another range.
 *
 * @type Integer
 * @see compareBoundaryPoints()
 */
ViperDOMRange.END_TO_START = 4;

/*
 * Specifies the unit moved to be a chracter when used with moveStart() and moveEnd().
 */
ViperDOMRange.CHARACTER_UNIT = 'character';

/*
 * Specifies the unit moved to be a word when used as an argument to moveStart()
 * and moveEnd().
 */
ViperDOMRange.WORD_UNIT = 'word';

/*
 * Specifies the unit moved to be a line when used as an argument to moveStart()
 * and moveEnd().
 *
 * If the range ends in a text node, the range
 * will be moved to a point within the below line so that the end coordinates
 * are appropimately equal the start coordinates, unless the there is an insuffient
 * amount of text, in which case the end of the range will exist at the end
 * of the text below. If the range ends in a childless element, the end coords for
 * consequent selected lines will begin at approximately the same coordinates
 * as the left position of the element.
 *
 * @type Integer
 */
ViperDOMRange.LINE_UNIT = 'line';



ViperDOMRange.prototype = {

    /**
     * Moves the start point of the range to the offset within the specified node.
     *
     * @param Node    node   The node where the range should start.
     * @param Integer offset The offset within the node where the range should start.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setStart: function(node, offset) {},

    /**
     * Moves the end point of the range to the offset within the specified node.
     *
     * @param Node    node   The node where the range should end.
     * @param Integer offset The offset within the node where the range should end.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setEnd: function(node, offset) {},

    /**
     * Moves the start point of the range to before the specified node.
     *
     * @param Node node The node where the range should start before.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setStartBefore: function(node) {},

    /**
     * Moves the start point of the range to after the specified node.
     *
     * @param Node node The node where the range should start after.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setStartAfter: function(node) {},

    /**
     * Moves the end point of the range to before the specified node.
     *
     * @param Node node The node where the range should end before.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setEndBefore: function(node) {},

    /**
     * Moves the end point of the range to after the specified node.
     *
     * @param Node node The node where the range should end after.
     *
     * @return void
     * @throws RangeException, DomException
     */
    setEndAfter: function(node) {},

    /**
     * Selects the node, including its element.
     *
     * @param Node node The node to be selected.
     *
     * @return void
     * @throws RangeException, DomException
     */
    selectNode: function(node) {},

    /**
     * Selects the node, excluding its element.
     *
     * @param Node node The node who's contents should be selected.
     *
     * @return void
     * @throws RangeException, DomException
     */
    selectNodeContents: function(node) {},

    /**
     * Selects the node, excluding its element.
     *
     * @param Node node The node who's contents should be selected.
     *
     * @return void
     * @throws RangeException, DomException
     */
    surroundContents: function(node) {},

    /**
     * Collapses the range to the start or end of the current boundary points.
     *
     * @param boolean toStart If TRUE the range will be collapsed the the start of
     *                        the range, otherwise it will be collapsed to the end
     *                        of the range.
     *
     * @return void
     * @throws DOMException
     */
    collapse: function(toStart) {},

    // Range Comparisons.
    /**
     * Compare the boundary-points of two Ranges in a document and returns
     * -1, 0 or 1 depending on whether the corresponding boundary-point of the
     * Range is respectively before, equal to, or after the corresponding
     * boundary-point of sourceRange.
     *
     * @param integer  how         A flag as to how to compare the boundary points
     *                             of the range, which should be one of
     *                             START_TO_START, END_TO_END, START_TO_END and
     *                             END_TO_START.
     * @param W3CRange sourceRange The source range to compare to this range.
     *
     * @return integer
     * @see W3CRange.START_TO_START
     * @see W3CRange.START_TO_END
     * @see W3CRange.END_TO_END
     * @see W3CRange.END_TO_START
     * @thows DOMException
     */
    compareBoundaryPoints: function(how, sourceRange) {},

    // Extract Content.
    /**
     * Deletes the contents within the current range.
     *
     * @return void
     * @throws DOMException
     */
    deleteContents: function() {},

    /**
     * Extracts the contents from the current document, returning the contents in
     * a document fragment.
     *
     * @return DocumentFragment
     * @throws DOMException
     */
    extractContents: function() {},

    /**
     * Clones the contents of the range, returning the content in a DocumentFragment
     * without modifying the current Viper.document.
     *
     * @return DocumentFragment.
     * @throws DOMException
     */
    cloneContents: function() {},

    // Inserting.
    /**
     * Inserts a node into the Document or DocumentFragment at the start of the Range.
     * If the container is a Text node, this will be split at the start of the Range
     * (as if the Text node's splitText method was performed at the insertion point)
     * and the insertion will occur between the two resulting Text nodes. Adjacent Text
     * nodes will not be automatically merged. If the node to be inserted is a
     * DocumentFragment node, the children will be inserted rather than the
     * DocumentFragment node itself.
     *
     * @param Node node The node to be inserted.
     *
     * @return void
     * @throws DOMException, RangeException
     */
    insertNode: function(node) {},

    // Misc.
    /**
     * Clones this range object and returns an exact copy.
     *
     * @return W3CRange.
     */
    cloneRange: function() {},

    /**
     * Returns the string contents of the selected text within the range. The contents
     * will not contain any markup.
     *
     * @return string
     */
    toString: function() {},

    /**
     * Detaches the range from the document releasing any resources used.
     *
     * @return void
     * @throws DOMException
     */
    detach: function() {},

    /*
     * Extensions the W3C Range, including some of the Internet Explorer Range
     * methods.
     */

    /**
     * Returns the DOMElement that is common between the start and end positions
     * of the range.
     *
     * @return DOMElement
     */
    getCommonElement: function () {},

    /**
     * Moves the start of the range using the specified unitType, by the specified
     * number of units. Defaults to ViperDOMRange.CHARACTER_UNIT and units of 1.
     *
     * @param int unitType The unitType to move, which should be one of
     *                     ViperDOMRange.CHARACTER_UNIT, ViperDOMRange.WORD_UNIT or ViperDOMRange.LINE_UNIT.
     * @param int units    The number of units to move. If positive the range will
     *                     be moved RIGHT for LTR languages, or LEFT or RTL languages.
     *
     * @return void
     */
    moveStart: function(unitType, units) {},

    /**
     * Moves the end of the range using the specified unitType, by the specified
     * number of units. Defaults to ViperDOMRange.CHARACTER_UNIT and units of 1.
     *
     * @param int unitType The unitType to move, which should be one of
     *                     ViperDOMRange.CHARACTER_UNIT, ViperDOMRange.WORD_UNIT or ViperDOMRange.LINE_UNIT.
     * @param int units    The number of units to move. If positive the range will
     *                     be moved RIGHT for LTR languages, or LEFT or RTL languages.
     *
     * @return void
     */
    moveEnd: function(unitType, units) {},

    /**
     * Sets the anchor point of the range to the start or end of the range. Once
     * the anchor point is set, the focus point becomes the other end of the range.
     *
     * @param boolean toStart If TRUE the anchor point will be set to the start
     *                        of the range, other the focus will be set the the
     *                        end.
     *
     * @see setFocus()
     * @see moveFocus()
     */
    setAnchor: function(toStart) {},

    /**
     * Sets the focus point to exist in the node at the specified offset. The
     * anchor point should be set using setAnchor before calling this method.
     *
     * @param DomNode node   The node where the focus should exist.
     * @param integer offset The offset within the node where the offset should
     *                       exist.
     *
     * @see setAnchor()
     * @see moveFocus()
     * @throws RangeException If no anchor point is set.
     */
    setFocus: function(node, offset) {},

    /**
     * Moves the focus point by the specified unitType by the number of specified
     * units. Defaults to ViperDOMRange.CHARACTER_UNIT and units of 1.
     *
     * @param string unitType  The unitType to move, which should be one of
     *                         ViperDOMRange.CHARACTER_UNIT, ViperDOMRange.WORD_UNIT
     *                         or ViperDOMRange.LINE_UNIT.
     * @param integer units    The number of units to move. If positive the range will
     *                         be moved RIGHT for LTR languages, or LEFT or RTL languages.
     *
     * @see setAnchor()
     * @see moveFocus()
     * @throws RangeException If no anchor point is set.
     */
    moveFocus: function(unitType, units) {},

    /**
     * Returns the coordinates where the range starts.
     *
     * If true, the start coodinates of the range will be return, otherwise the
     * end coordinates will be returned.
     *
     * @return Object[x, y]
     */
    getRangeCoords: function(toStart) {},

    /**
     * Returns the bounding rectangle for the range.
     *
     * @return Object[left, top, right, bottom]
     */
    getBoundingClientRect: function() {},

    /**
     * Returns the deepest previous container that the range can be extended to.
     * For example, if the previous container is an element that contains text nodes,
     * the the container's lastChild is returned.
     *
     * @param {DomNode} container        The current container.
     * @param {array}   skippedBlockElem Skipped block elements.
     *
     * @return The text container that range can extend to.
     * @type   {TextNode}
     */
    getPreviousContainer: function(container, skippedBlockElem)
    {
        if (!container) {
            return null;
        }

        while (container.previousSibling) {
            container = container.previousSibling;
            if (container.nodeType !== dfx.TEXT_NODE) {
                if (dfx.isStubElement(container) === true) {
                    return container;
                } else {
                    var child = this._getLastSelectableChild(container);
                    if (child !== null) {
                        return child;
                    }
                }
            } else if (this._isSelectable(container) === true) {
                return container;
            }
        }

        // Look at parents next sibling.
        while (container && !container.previousSibling) {
            container = container.parentNode;
        }

        if (!container) {
            return null;
        }

        container = container.previousSibling;
        if (this._isSelectable(container) === true) {
            return container;
        } else if (skippedBlockElem && dfx.isBlockElement(container) === true) {
            skippedBlockElem.push(container);
        }

        var selChild = this._getLastSelectableChild(container);
        if (selChild !== null) {
            return selChild;
        }

        return this.getPreviousContainer(container, skippedBlockElem);

    },

    _isSelectable: function(container)
    {
        if (container && container.nodeType === dfx.TEXT_NODE && container.data.length !== 0) {
            return true;
        }

        return false;

    },

    /**
     * Returns the deepest next container that the range can be extended to.
     * For example, if the next container is an element that contains text nodes,
     * the the container's firstChild is returned.
     *
     * @param {DomNode} container        The current container.
     * @param {array}   skippedBlockElem Skipped block elements.
     *
     * @return The text container that range can extend to.
     * @type   {TextNode}
     */
    getNextContainer: function(container, skippedBlockElem)
    {
        if (!container) {
            return null;
        }

        while (container.nextSibling) {
            container = container.nextSibling;
            if (container.nodeType !== dfx.TEXT_NODE) {
                var child = this._getFirstSelectableChild(container);
                if (child !== null) {
                    return child;
                }
            } else if (this._isSelectable(container) === true) {
                return container;
            }
        }

        // Look at parents next sibling.
        while (container && !container.nextSibling) {
            container = container.parentNode;
        }

        if (!container) {
            return null;
        }

        container = container.nextSibling;
        if (this._isSelectable(container) === true) {
            return container;
        } else if (skippedBlockElem && dfx.isBlockElement(container) === true) {
            skippedBlockElem.push(container);
        }

        var selChild = this._getFirstSelectableChild(container);
        if (selChild !== null) {
            return selChild;
        }

        return this.getNextContainer(container, skippedBlockElem);

    },

    _getFirstSelectableChild: function(element)
    {
        if (element) {
            if (element.nodeType !== dfx.TEXT_NODE) {
                var child = element.firstChild;
                while (child) {
                    if (this._isSelectable(child) === true) {
                        return child;
                    } else if (child.firstChild) {
                        // This node does have child nodes.
                        var res = this._getFirstSelectableChild(child);
                        if (res !== null) {
                            return res;
                        } else {
                            child = child.nextSibling;
                        }
                    } else {
                        child = child.nextSibling;
                    }
                }
            } else {
                // Given element is a text node so return it.
                return element;
            }//end if
        }//end if

        return null;

    },

    _getLastSelectableChild: function(element)
    {
        if (element) {
            if (element.nodeType !== dfx.TEXT_NODE) {
                var child = element.lastChild;
                while (child) {
                    if (this._isSelectable(child) === true) {
                        return child;
                    } else if (child.lastChild) {
                        // This node does have child nodes.
                        var res = this._getLastSelectableChild(child);
                        if (res !== null) {
                            return res;
                        } else {
                            child = child.previousSibling;
                        }
                    } else {
                        child = child.previousSibling;
                    }
                }
            } else {
                // Given element is a text node so return it.
                return element;
            }//end if
        }//end if

        return null;

    },

    _normalizeNode: function(node)
    {
        // Joins all sibling text elements.
        if (node.nodeType === dfx.ELEMENT_NODE) {
            var c      = node.childNodes.length;
            var str    = '';
            var mChild = null;
            for (var i = 0; i < c; i++) {
                var child = node.childNodes[i];
                if (child.nodeType === dfx.TEXT_NODE) {
                    str += child.data;
                    if (mChild === null) {
                        mChild = child;
                    } else {
                        // Remove this node.
                        dfx.remove(child);
                    }
                } else if (mChild !== null) {
                    mChild.data = str;
                    mCHild      = null;
                }
            }

            if (mChild !== null) {
                mChild.nodeValue = str;
            }
        } else if (node.nodeType === dfx.TEXT_NODE) {
            this._normalizeNode(node.parentNode);
        }//end if

    },

    getNodeIndex: function(node)
    {
        if (!node || !node.parentNode) {
            return;
        }

        var index = 0;
        var prev  = node.previousSibling;

        while (prev) {
            prev = prev.previousSibling;
            index++;
        }

        return index;

    },

    getStartNode: function()
    {
        if (!this.startContainer) {
            return null;
        }

        if (this.startContainer.nodeType === dfx.ELEMENT_NODE) {
            return this.startContainer.childNodes[this.startOffset];
        }

        return this.startContainer;

    },

    getEndNode: function()
    {
        if (!this.endContainer) {
            return null;
        }

        if (this.endContainer.nodeType === dfx.ELEMENT_NODE) {
            return this.endContainer.childNodes[this.endOffset];
        }

        return this.endContainer;

    }

};
