<?php
/**
 * XML Lib.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Library for handling data in XML form.
 *
 * @since 4.0.0
 */
class XML
{


    /**
     * Converts a DomNodeList to an array containing its' elements.
     *
     * @param DomNodeList $nodeList List of nodes to convert.
     *
     * @since  4.0.0
     * @return array
     */
    public static function nodeListToArray(DomNodeList $nodeList)
    {
        $retval = array();
        foreach ($nodeList as $item) {
            $retval[] = $item;
        }

        return $retval;

    }//end nodeListToArray()


    /**
     * Searches under the passed element for elements of any of the passed type.
     *
     * @param array   $tagNames An array of tag names to search for.
     * @param DomNode $elem     The element to search under.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getElementsByTagNames(array $tagNames, DomNode $elem)
    {
        $retval = array();

        foreach ($tagNames as $tagName) {
            $elements = $elem->getElementsByTagName($tagName);
            foreach ($elements as $element) {
                $retval[] = $element;
            }
        }

        return $retval;

    }//end getElementsByTagNames()


    /**
     * Returns first level child elements that have the specified tag names.
     *
     * @param array   $tagNames An array of tag names to search for.
     * @param DomNode $elem     The element to search under.
     * @param integer $max      Maximum number of elements to return.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getChildrenByTagNames(
        array $tagNames,
        DomNode $elem,
        $max=NULL
    ) {
        $retval = array();
        $count  = 1;
        foreach ($elem->childNodes as $child) {
            if (self::isElementNode($child) === TRUE) {
                if (in_array($child->tagName, $tagNames) === TRUE) {
                    $retval[] = $child;

                    if ($max !== NULL && $max <= $count) {
                        break;
                    }
                }

                $count++;
            }
        }

        return $retval;

    }//end getChildrenByTagNames()


    /**
     * Searches for given attribute value under the given element.
     *
     * If recusrive set to false then only 1 level will be searched.
     *
     * @param DomDocument $doc       Xml document.
     * @param DomNode     $element   Parent element.
     * @param string      $attr      Name of the attribute.
     * @param string      $value     Attribute value.
     * @param boolean     $recursive Recurse the dom tree.
     *
     * @since  4.0.0
     * @return DOMNodeList
     */
    public static function getElementsByAttributeValue(
        DomDocument $doc,
        DomNode $element,
        $attr,
        $value,
        $recursive=TRUE
    ) {
        $nodes = array();
        if ($attr === '') {
            return $nodes;
        }

        $xp = new DOMXPath($doc);
        if ($recursive === FALSE) {
            $query = '*';
        } else {
            $query = '//*';
        }

        $query = $query.'[@'.$attr.' = "'.$value.'"]';
        $nodes = $xp->query($query, $element);

        return $nodes;

    }//end getElementsByAttributeValue()


    /**
     * Returns TRUE if the given node has element node.
     *
     * Unlike hasChildNodes, hasChildElements returns false if it cannot find a
     * XML_ELEMENT_NODE.
     *
     * @param DomNode $node Dom node.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function hasChildElements(DomNode $node)
    {
        if ($node->hasChildNodes() === TRUE) {
            foreach ($node->childNodes as $child) {
                if ($child->nodeType === XML_ELEMENT_NODE) {
                    return TRUE;
                }
            }
        }

        return FALSE;

    }//end hasChildElements()


    /**
     * Returns the first child element node of the given node.
     *
     * Unlike fisrtChild, getFirstChildElement returns
     * the first XML_ELEMENT_NODE. Returns NULL if none found.
     *
     * @param DomNode $node Dom node.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getFirstChildElement(DomNode $node)
    {
        foreach ($node->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE) {
                return $child;
            }
        }

        return NULL;

    }//end getFirstChildElement()


    /**
     * Returns the child element node of the given node at the given position.
     *
     * @param DomNode $node     Dom node.
     * @param integer $position Position.
     *
     * @since  4.0.0
     * @return object
     */
    public static function getChildAt(DomNode $node, $position)
    {
        $pos = 0;
        foreach ($node->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE) {
                if ($pos === $position) {
                    return $child;
                }

                $pos++;
            }
        }

        return NULL;

    }//end getChildAt()


    /**
     * Returns the number of child nodes for specified node.
     *
     * @param DomNode $node Dom node.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function childCount(DomNode $node)
    {
        $count = 0;
        foreach ($node->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE) {
                $count++;
            }
        }

        return $count;

    }//end childCount()


    /**
     * Traverse DOM tree from given start node.
     *
     * Returns details about each element it comes across and these elements'
     * children.
     *
     * @param DomElement $element Node to start from.
     *
     * @since  4.0.0
     * @return array
     */
    public static function traverseDom(DomElement $element)
    {
        $nodeInfo            = array();
        $nodeInfo['tagName'] = $element->tagName;

        if ($element->hasAttributes() === TRUE) {
            foreach ($element->attributes as $attribute) {
                $nodeInfo['attrs'][$attribute->name] = $attribute->value;
            }
        }

        if ($element->hasChildNodes() === TRUE) {
            foreach ($element->childNodes as $childNode) {
                if ($childNode->nodeType === XML_COMMENT_NODE) {
                    continue;
                } else if ($childNode->nodeType === XML_TEXT_NODE) {
                    $nodeInfo['value'] = trim($childNode->nodeValue);
                } else if ($childNode->tagName !== NULL) {
                    $nodeInfo['children'][] = self::traverseDOM($childNode);
                }
            }
        }

        return $nodeInfo;

    }//end traverseDom()


    /**
     * Allows multiple attributes of a DomElement to be set simultaneously.
     *
     * @param DomElement $element    The element to set the attributes for.
     * @param array      $attributes An associate array of the form:
     *                                 array(
     *                                  $attrname => $attrValue,
     *                                  ...,
     *                                 ).
     *
     * @since  4.0.0
     * @return DomElement
     */
    public static function setAttributes(DomElement $element, array $attributes)
    {
        foreach ($attributes as $attrName => $attrValue) {
            $element->setAttribute($attrName, $attrValue);
        }

        return $element;

    }//end setAttributes()


    /**
     * Returns true if the given element is an XML_ELEMENT_NODE.
     *
     * @param DomNode $node The element test.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isElementNode(DomNode $node)
    {
        if ($node !== NULL && $node->nodeType === XML_ELEMENT_NODE) {
            return TRUE;
        }

        return FALSE;

    }//end isElementNode()


    /**
     * Converts specified DomNode to array.
     *
     * @param DomNode $node             XML node to convert.
     * @param boolean $includeTextNodes Include text nodes.
     *
     * @since  4.0.0
     * @return array
     */
    public static function nodeToArray(DomNode $node, $includeTextNodes=FALSE)
    {
        $elArray = NULL;
        if (self::isElementNode($node) === TRUE) {
            $elArray               = array();
            $elArray['tag']        = $node->tagName;
            $elArray['attributes'] = array();
            $elArray['elements']   = array();
            $elArray['contents']   = '';

            // Add Attributes.
            if ($node->hasAttributes() === TRUE) {
                $attributes = $node->attributes;
                if ($attributes !== NULL) {
                    foreach ($attributes as $attr) {
                        $elArray['attributes'][$attr->name] = $attr->value;
                    }
                }
            }

            // Add Child Nodes.
            if (self::hasChildElements($node) === TRUE) {
                foreach ($node->childNodes as $childNode) {
                    $childArray = self::nodeToArray($childNode);
                    if ($childArray !== NULL) {
                        $elArray['elements'][] = $childArray;
                    }
                }
            } else {
                $elArray['contents'] = $node->nodeValue;
            }
        } else if ($includeTextNodes === TRUE) {
            $elArray = $node->nodeValue;
        }//end if

        return $elArray;

    }//end nodeToArray()


    /**
     * Converts a SimpleXML object into an array.
     *
     * Note: This method does not parse attributes.
     * <?xml version="1.0" encoding="utf-8"?>
     *  <persons>
     *    <developer>
     *      <name>rong</name>
     *      <extension>229</extension>
     *      <skill>Bubble Hearth</skill>
     *    </developer>
     *    <developer>
     *      <name>lwright</name>
     *      <extension>225</extension>
     *      <skill>Poker</skill>
     *    </developer>
     *  </persons>
     * into
     * "developer" => array (
     *                 0 => array (
     *                       "name" => "rong",
     *                       "extension" => "229",
     *                       "skill" => "Bubble Hearth"
     *                 ),
     *                 1 => array (
     *                       "name" => "lwright",
     *                       "extension" => "225",
     *                       "skill" => "Poker"
     *                 ),
     * ).
     *
     * @param object $root SimpleXML object to convert.
     *
     * @since  4.0.0
     * @return array
     */
    public static function simplexml2array($root)
    {
        $result = array();
        foreach ($root->children() as $elemName => $elemNode) {
            $first = TRUE;
            $name  = (string) $elemName;
            $text  = trim((string) $elemNode);
            if (strlen($text) > 0) {
                $result[$name] = $text;
            } else {
                $children = self::simplexml2array($elemNode);
                if (empty($children) === FALSE) {
                    if (empty($result[$name]) === FALSE) {
                        // More than 1 kid with same name, make an array.
                        if ($first === TRUE) {
                            $first           = FALSE;
                            $elem            = $result[$name];
                            $result[$name]   = array();
                            $result[$name][] = $elem;
                        }

                        $result[$name][] = $children;
                    } else {
                        $result[$name] = $children;
                    }
                }
            }
        }//end foreach

        return $result;

    }//end simplexml2array()


    /**
     * Converts empty XML tags to valid XHTML tags.
     *
     * For example: <div /> to <div></div>.
     *
     * @param string $xml The XML string to convert.
     *
     * @since  4.0.0
     * @return string
     */
    public static function xmlToXHTML($xml)
    {
        $pattern  = '#<(\w+)([^>]*)\s*/>#s';
        $callback = array(
                     'XML',
                     '_xmlToXHTMLCallback',
                    );
        $xml      = preg_replace_callback($pattern, $callback, $xml);

        return $xml;

    }//end xmlToXHTML()


    /**
     * Callback function used by XML::xmlToXHTML().
     *
     * @param array $matches Regex matches.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _xmlToXHTMLCallback(array $matches)
    {
        $validTags = '|br|hr|input|img|link|param|frame|col|area|base|basefont|';

        if (strpos($validTags, '|'.$matches[1].'|') !== FALSE) {
            $str = '<'.$matches[1].$matches[2].' />';
        } else {
            $str = '<'.$matches[1].$matches[2].'></'.$matches[1].'>';
        }

        return $str;

    }//end _xmlToXHTMLCallback()


    /**
     * Returns flat array of all the attribute/element values by sequential order.
     *
     * Each result element includes the information
     * where the values are used like tag or attr.
     *
     * @param string  $html       Content to process.
     * @param boolean $embedToDoc If true, the content is wrapped with
     *                            in XHTML structure.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getFlatValueListFromHTMLDom($html, $embedToDoc=TRUE)
    {
        if ($embedToDoc === TRUE) {
            $xhtml  = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" ';
            $xhtml .= '"http://www.w3.org/TR/REC-html40/loose.dtd">';
            $xhtml .= '<html><head></head><body>'.$html.'</body></html>';
        } else {
            $xhtml = $html;
        }

        $dom = new DomDocument();
        @$dom->loadHTML($xhtml);

        $body = $dom->getElementsByTagName('body');
        $body = $body->item(0);

        $flatList = array();
        self::_getFlatValueListFromHTMLDom($body, $flatList);
        return $flatList;

    }//end getFlatValueListFromHTMLDom()


    /**
     * Private helper function for getFlatValueListFromHTMLDom.
     *
     * @param DomNode $node      DomNode to process.
     * @param array   &$flatList The result array.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _getFlatValueListFromHTMLDom(DomNode $node, array &$flatList)
    {
        $childrenLen = $node->childNodes->length;
        for ($i = 0; $i < $childrenLen; $i++) {
            $child = $node->childNodes->item($i);
            switch ($child->nodeType)
            {
                case XML_ELEMENT_NODE:
                    foreach ($child->attributes as $attrName => $attrNode) {
                        $attrValue = trim($attrNode->value);
                        if (empty($attrValue) === FALSE) {
                            $flatList[] = array(
                                           'tag'    => $child->nodeName,
                                           'attr'   => $attrName,
                                           'value'  => $attrNode->value,
                                           'length' => strlen($attrNode->value),
                                          );
                        }
                    }

                    self::_getFlatValueListFromHTMLDom($child, $flatList);
                break;

                case XML_TEXT_NODE:
                    $text = trim($child->wholeText);
                    if (empty($text) === FALSE) {
                        $flatList[] = array(
                                       'tag'    => $child->parentNode->nodeName,
                                       'attr'   => '',
                                       'value'  => $text,
                                       'length' => strlen($text),
                                      );
                    }
                break;

                default:
                    // Do nothing.
                break;
            }//end switch
        }//end for

    }//end _getFlatValueListFromHTMLDom()


}//end class

?>
