/**
 * ViperKeyboardEditorPlugin. Handles auxillery content editing via keyboard
 * commands. For example, inserting p tags when the ENTER key is pressed.
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

function ViperKeyboardEditorPlugin(viper)
{
    ViperPlugin.call(this, viper);
    ViperPluginManager.addKeyPressListener('SHIFT+ENTER', this, 'handleSoftEnter');
    ViperPluginManager.addKeyPressListener('ENTER', this, 'handleEnter');

}

ViperKeyboardEditorPlugin.prototype = {
    start: function()
    {
        var self = this;

        // Note: Should be a format change since it will be used in the whole
        // container.
        ViperChangeTracker.addChangeType('splitContainer', 'Insert', 'format');
        ViperChangeTracker.setDescriptionCallback('splitContainer', function(node) {
            return self._getChangeDescription(node, 'splitContainer');
        });
        ViperChangeTracker.setApproveCallback('splitContainer', function(clone, node) {
            ViperChangeTracker.removeTrackChanges(node);
        });
        ViperChangeTracker.setRejectCallback('splitContainer', function(clone, node) {
            // Get previous sibling.
            var prev = node.previousSibling;
            if (!prev) {
                return;
            }

            while (node.firstChild) {
                prev.appendChild(node.firstChild);
            }

            dfx.remove(node);
        });

        ViperChangeTracker.addChangeType('createContainer', 'Insert', 'insert');
        ViperChangeTracker.setDescriptionCallback('createContainer', function(node) {
            return self._getChangeDescription(node);
        });
        ViperChangeTracker.setApproveCallback('createContainer', function(clone, node) {
            ViperChangeTracker.removeTrackChanges(node);
        });
        ViperChangeTracker.setRejectCallback('createContainer', function(clone, node) {
            dfx.remove(node);
        });

    },

    _getChangeDescription: function(node, changeType)
    {
        var pImgURL = this.viper.getStylesURL() + '/icon-p_tag.png';
        var pImg    = Viper.document.createElement('img');
        dfx.attr(pImg, 'src', pImgURL);
        dfx.attr(pImg, 'title', 'Paragraph Break');
        var desc = pImg;

        if (changeType !== 'splitContainer') {
            for (var child = node.firstChild; child; child = child.nextSibling) {
                if (child.nodeType === dfx.TEXT_NODE && dfx.trim(child.nodeValue).length === 0) {
                    continue;
                } else if (ViperChangeTracker.isTrackingNode(child) === true) {
                    var ctnType = ViperChangeTracker.getCTNTypeFromNode(child);
                    if (ViperChangeTracker.isInsertType(ctnType) === true) {
                        var extraDesc = ViperChangeTracker.getDescriptionForNode(child);
                        if (dfx.isObj(extraDesc) === false) {
                            extraDesc = Viper.document.createTextNode(extraDesc);
                        }

                        desc = [desc, extraDesc];
                    }
                }//end if

                break;
            }//end for
        }

        return desc;

    },

    _isKeyword: function()
    {
        var keywordPlugin = ViperPluginManager.getPlugin('ViperKeywordPlugin');
        if (!keywordPlugin) {
            return false;
        }

        var range = this.viper.getCurrentRange();
        if (keywordPlugin._isKeyword(range.startContainer) === false && keywordPlugin._isKeyword(range.startContainer) === false) {
            return false;
        }

        return true;

    },

    handleTab: function()
    {
        if (this._isKeyword() === true) {
            return true;
        }

        var numSpaces = 4;
        // Insert.
        var sp = String.fromCharCode(160);
        var c  = '';
        while (numSpaces-- > 0) {
            c += sp;
        }

        this.viper.insertNodeAtCaret(c);

        this.viper.fireNodesChanged('ViperKeyboardEditorPlugin:tab');

        return true;

    },

    handleEnter: function(returnFirstBlock)
    {
        if (this.viper.inlineMode === true) {
            return this.handleSoftEnter();
        }

        var range = this.viper.getCurrentRange();

        // If the range is not collapsed then remove the contents of the selection.
        if (range.collapsed !== true) {
            this.viper.deleteContents();
        }

        if (range.startContainer.nodeType === dfx.TEXT_NODE) {
            // Find the first parent block element.
            var parent = range.startContainer.parentNode;
            while (parent) {
                if (parent.tagName.toLowerCase() === 'li') {
                    // Lists are special they are handled by the ViperListPlugin.
                    var listPlugin = ViperPluginManager.getPlugin('ViperListPlugin');
                    if (listPlugin && listPlugin.handleEnter(parent) === false) {
                        return true;
                    }

                    break;
                } else if (dfx.isBlockElement(parent) === true) {
                    break;
                }

                if (parent.parentNode && parent.parentNode === this.viper.element) {
                    break;
                }

                parent = parent.parentNode;
            }
        } else {
            parent = range.startContainer;
        }//end if

        // Create a new element and a document fragment with the contents of
        // the selection.
        var tag = parent.tagName.toLowerCase();

        // If the parent is not part of the editable element then we need to
        // create two new P tags.
        if (dfx.isChildOf(parent, this.viper.element) === false) {
            // Find the next non block sibling.
            var node = range.endContainer;
            while (dfx.isset(node.nextSibling) === true) {
                if (dfx.isBlockElement(node.nextSibling) === true) {
                    break;
                }

                node = node.nextSibling;
            }

            range.setEndAfter(node);

            var elem    = Viper.document.createElement('p');
            var docFrag = range.extractContents('p');

            this.viper.deleteContents();
            elem.appendChild(docFrag);
            dfx.insertAfter(range.startContainer, elem);
            range.collapse(true);

            // Find the previous non block sibling.
            node = range.startContainer;
            while (dfx.isset(node.previousSibling) === true) {
                if (dfx.isBlockElement(node.previousSibling) === true) {
                    break;
                }

                node = node.previousSibling;
            }

            range.setStartBefore(node);

            var felem = Viper.document.createElement('p');
            docFrag   = range.extractContents('p');
            felem.appendChild(docFrag);
            dfx.insertBefore(elem, felem);

            range.setStart(elem.firstChild, 0);
            range.collapse(true);
            return;
        } else if (tag === 'pre') {
            // If the text is in a PRE tag then we need to insert a br element.
            this.handleSoftEnter();
            return;
        } else if (tag === 'td' || tag === 'th') {
            // Cannot create a new TD tag so need the move td contents in to a P tag.
            var bookmark = this.viper.createBookmark(range);
            var p        = Viper.document.createElement('P');
            while (parent.firstChild) {
                p.appendChild(parent.firstChild);
            }

            // Add the new P tag as TD's child node.
            parent.appendChild(p);

            // Update tag name and parent element.
            tag    = 'p';
            parent = p;

            // Update range.
            this.viper.selectBookmark(bookmark);
        }//end if

        // If the selection is at the end of text node and has no next sibling
        // then move the range out of its parent node to prevent empty tags being
        // created by range.extractContents().
        if (range.startContainer.nodeType === dfx.TEXT_NODE
            && range.startOffset === range.startContainer.data.length
        ) {
            if (!range.startContainer.nextSibling) {
                var newTextNode = Viper.document.createTextNode('');
                dfx.insertAfter(range.startContainer.parentNode, newTextNode);
                range.setStart(newTextNode, 0);
                range.collapse(true);
            }
        }

        try {
            // Select everything from the current position to the end of the parent.
            range.setEndAfter(parent.lastChild);
        } catch (e) {

        }

        ViperSelection.addRange(range);

        // Need to clone the node so that its attributes are also copied.
        // This may cause ID conflicts.
        var elem    = parent.cloneNode(false);
        var docFrag = range.extractContents(tag);

        elem.appendChild(docFrag);

        // Remove DEL tags before getting the text content.
        var elemClone = elem.cloneNode(true);
        dfx.remove(dfx.getTag('del', elemClone));

        if (dfx.isBlank(dfx.getNodeTextContent(elemClone)) === true) {
            // Do not need this empty element.
            elem = null;
        }

        if (elem === null || (elem.tagName && elem.tagName.toLowerCase() !== 'li' && dfx.isBlockElement(elem) === false)) {
            // If the newly created element is not a block element then
            // create a P tag and make it the elem's parent.
            var newTag = 'p';

            // If element is in a list block then create a new list item instead of a paragraph.
            if (tag === 'li') {
                newTag = tag;
            }

            var pelem = Viper.document.createElement(newTag);
            if (elem !== null) {
                pelem.appendChild(elem);
            } else {
                dfx.setHtml(pelem, '&nbsp;');
            }

            elem = pelem;
            ViperChangeTracker.addChange('createContainer', [elem]);
        } else {
            ViperChangeTracker.removeTrackChanges(elem, true);
            ViperChangeTracker.addChange('splitContainer', [elem]);
        }//end if

        if (this.viper.elementIsEmpty(parent) === true) {
            dfx.setHtml(parent, '&nbsp;');
        }

        // Insert the new element after the current parent.
        dfx.insertAfter(parent, elem);

        range.setStart(elem, 0);
        range.setStart(elem, 0);
        try {
            range.moveStart('character', 1);
            range.moveStart('character', -1);
        } catch (e) {
            // Handle empty node..
        }

        range.collapse(true);
        ViperSelection.addRange(range);

        // Check the parent element contents.
        // If the parent element is now empty and its a block element
        // then add a space to it.
        if (dfx.isBlockElement(parent) === true && dfx.trim(dfx.getHtml(parent)) === '') {
            dfx.setHtml(parent, '&nbsp;');
        }

        this.viper.fireNodesChanged('ViperKeyboardEditorPlugin:enter');

        if (returnFirstBlock === true) {
            return parent;
        }

        return true;

    },

    handleSoftEnter: function(e)
    {
        if (this._isKeyword() === true) {
            return true;
        }

        var range = this.viper.getCurrentRange();
        if (e) {
            var startNode = range.getStartNode();
            if (startNode && dfx.isTag(startNode.parentNode, 'pre') === true) {
                // Break out from PRE tag.
                var p = Viper.document.createElement('p');
                dfx.setHtml(p, '&nbsp;');
                dfx.insertAfter(startNode.parentNode, p);
                range.setStart(p.firstChild, 0);
                range.collapse(true);
                ViperSelection.addRange(range);
                this.viper.fireNodesChanged('ViperKeyboardEditorPlugin:softEnter');
                return;
            }
        }

        var node = Viper.document.createElement('br');
        this.viper.insertNodeAtCaret(node);
        range = this.viper.getCurrentRange();

        if (dfx.isTag(node.previousSibling, 'br') === true) {
            // Insert a text node in between these br tags.
            var text = Viper.document.createTextNode(String.fromCharCode(160));
            dfx.insertAfter(node.previousSibling, text);
        }

        if (!node.nextSibling || node.nextSibling.nodeType !== dfx.TEXT_NODE) {
            var text = Viper.document.createTextNode(String.fromCharCode(160));
            dfx.insertAfter(node, text);
        }

        range.setStart(node.nextSibling, 0);
        range.collapse(true);
        this.viper.fireNodesChanged('ViperKeyboardEditorPlugin:softEnter');

        return true;

    },

    _changeInfo: function()
    {
        var info = {
            before: dfx.getHtml(this.viper.element),
            path: XPath.getPath(this.viper.element)
        };

        return info;

    }

};

dfx.noInclusionInherits('ViperKeyboardEditorPlugin', 'ViperPlugin', true);
