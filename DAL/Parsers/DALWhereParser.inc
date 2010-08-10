<?php
/**
 * DALWhereParser.
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
 * @subpackage DAL
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/Parsers/DALSelectParser.inc';
require_once 'DAL/Parsers/DALExceptParser.inc';
require_once 'DAL/Parsers/DALIntersectParser.inc';
require_once 'DAL/DALBaker.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALWhereParser.
 *
 * Where clause parser. Parses a where clause.
 *
 */
class DALWhereParser extends DALQueryParser
{


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     * All DALBaker methods should be called statically.
     *
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the array form of the SELECT query.
     *
     * @param DomElement $parent Query element.
     * @param string     $type   Type of the clause, where or having.
     *
     * @return array
     */
    public static function parse(DomElement $parent, $type='where')
    {
        $where    = array();
        $whereTag = NULL;

        foreach ($parent->childNodes as $tag) {
            if ($tag->nodeType !== XML_ELEMENT_NODE) {
                continue;
            } else if ($tag->tagName !== $type) {
                continue;
            } else {
                $whereTag = $tag;
            }
        }

        if ($whereTag !== NULL) {
            $where[strtoupper($type)] = self::getWhereConditions($whereTag);
        }

        $joins = self::_praseJoinsForSelect($parent);
        if (empty($joins) === FALSE) {
            $where = array_merge($where, $joins);
        }

        return $where;

    }//end parse()


    /**
     * Constructs the SQL's conditions from XML node.
     *
     * @param DomElement $parent Select Node of the XML.
     * @param string     $type   Type of the where condition.
     * @param integer    $level  Indentation level.
     *
     * @return array
     */
    public static function getWhereConditions(
        DomElement $parent,
        $type='AND',
        $level=1
    ) {
        $conditions = array();
        foreach ($parent->childNodes as $cond) {
            if ($cond->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            if (DALBaker::getComparisonOperators($cond->tagName) !== '') {
                $currentCond            = array();
                $currentCond['compare'] = array();
                $currentCond['type']    = $cond->tagName;

                $offset = 0;
                if ($cond->getAttribute('table') !== '') {
                    // Field compare.
                    $currentCond['compare']['table']  = $cond->getAttribute('table');
                    $currentCond['compare']['column'] = $cond->getAttribute('column');
                } else if (XML::hasChildElements($cond) === TRUE) {
                    $offset = 1;
                    $value  = self::parseSingleField(
                        XML::getFirstChildElement($cond)
                    );

                    $currentCond['compare'] = $value;
                }

                $value = '';
                if (XML::hasChildElements($cond) === TRUE) {
                    $child = XML::getChildAt($cond, $offset);
                    if ($child !== NULL) {
                        $value = self::parseSingleField($child);
                    }
                } else {
                    $value = $cond->nodeValue;
                }

                $currentCond['to']   = $value;
                $conditions[$type][] = $currentCond;
            } else if (($cond->tagName === 'in') || ($cond->tagName === 'not-in')) {
                // This is an IN/NOT-IN condition.
                $conditions[$type][] = self::parseInClause($cond, $level);
            } else if (($cond->tagName === 'exists')
                || ($cond->tagName === 'not-exists')
            ) {
                $conditions[$type][] = self::parseExistsClause($cond, $level);
            } else if ($cond->tagName === 'or') {
                $conditions[$type][] = self::getWhereConditions($cond, 'OR');
            } else if ($cond->tagName === 'and') {
                $conditions[$type][] = self::getWhereConditions($cond, 'AND');
            } else if ($cond->tagName === 'not') {
                $conditions[$type][] = self::getWhereConditions($cond, 'NOT');
            } else if ($cond->tagName === 'hook') {
                $conditions[$type][] = DALBaker::getHookPrefix().$cond->getAttribute('id');
            } else if ($cond->tagName === 'keyword') {
                $conditions[$type][] = array('KEYWORD' => $cond->nodeValue);
            }//end if
        }//end foreach

        return $conditions;

    }//end getWhereConditions()


    /**
     * Constructs the SQL's joins from XML node.
     *
     * Returns the part of where clause.
     * array(
     *  'JOIN' => array(
     *             0 => array(
     *                   'ARGS' => array( // must have 2 elements.
     *                              0 => array(
     *                                    'table'  => string,
     *                                    'column' => string,
     *                                   ),
     *                              1 => array(
     *                                    'table'  => string,
     *                                    'column' => string,
     *                                   ),
     *                             ),
     *                   'TYPE' => string,
     *                  ),
     *            ),
     * )
     *
     * @param DomElement $select Select Node of the XML.
     *
     * @return array
     */
    private static function _praseJoinsForSelect(DomElement $select)
    {
        $joinList = array();
        $joinsTag = NULL;
        foreach ($select->childNodes as $childNode) {
            if ($childNode->nodeType === XML_ELEMENT_NODE
                && $childNode->tagName === 'joins'
            ) {
                $joinsTag = $childNode;
                break;
            }
        }

        if ($joinsTag !== NULL) {
            $joinList['JOIN'] = array();

            $joins = $joinsTag->getElementsByTagName('join');
            foreach ($joins as $join) {
                $condition          = array();
                $condition['ARGS']  = self::parseChildFields($join);
                $condition['type']  = $join->getAttribute('type');
                $joinList['JOIN'][] = $condition;
            }
        }

        return $joinList;

    }//end _praseJoinsForSelect()


    /**
     * Validates where clause.
     *
     * Throws DALParserException.
     *
     * @param DomElement $parent The query element.
     *
     * @return void
     * @throws DALParserException If validation fails.
     */
    public static function validate(DomElement $parent)
    {
        $condSigns = DALBaker::getComparisonOperators();

        // Add in and not-in to conditions.
        $condSigns['in']     = '';
        $condSigns['not-in'] = '';

        // Add exists and not-exists to conditions.
        $condSigns['exists']     = '';
        $condSigns['not-exists'] = '';

        // Loop through each childNode and check their contents.
        foreach ($parent->childNodes as $cond) {
            if ($cond->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            $tagName = $cond->tagName;

            if ($tagName === 'keyword') {
                continue;
            }

            if (isset($condSigns[$tagName]) === TRUE) {
                if ($tagName === 'is-null' || $tagName === 'not-null') {
                    if (XML::hasChildElements($cond) === FALSE) {
                        $msg = $tagName.' tag must have child.';
                        throw new DALParserException($msg);
                    }

                    continue;
                }

                if ((Xml::childCount($cond) !== 2) && ($tagName !== 'exists')
                    && ($tagName !== 'not-exists')
                ) {
                    if ($cond->getAttribute('table') === '') {
                        $msg = 'WHERE condition with no table attribute.';
                        throw new DALParserException($msg);
                    }
                }

                if (($tagName === 'in') || ($tagName === 'not-in')) {
                    // We need additional checks for in and not-in.
                    $value     = $cond->getElementsByTagName('value')->item(0);
                    $select    = $cond->getElementsByTagName('select')->item(0);
                    $except    = $cond->getElementsByTagName('except')->item(0);
                    $intersect = $cond->getElementsByTagName('intersect')->item(0);
                    if (($value === NULL) && ($select === NULL)
                        && ($except === NULL) && ($intersect === NULL)
                    ) {
                        if ($cond->nodeValue === '') {
                            $msg = $tagName.' must have value or select tag.';
                            throw new DALParserException($msg);
                        }
                    }

                    if ($except !== NULL) {
                        // Validate inner EXCEPT query.
                        DALExceptParser::validate($cond);
                    } else if ($intersect !== NULL) {
                        // Validate inner INTERSECT query.
                        DALIntersectParser::validate($cond);
                    } else if ($select !== NULL) {
                        // Validate inner SELECT query.
                        DALSelectParser::validate($cond);
                    }
                }//end if

                if (($tagName === 'exists') || ($tagName === 'not-exists')) {
                    // We need additional checks for exists and not-exists.
                    $select = $cond->getElementsByTagName('select')->item(0);
                    if ($select === NULL) {
                        $msg = $tagName.' must have select tag.';
                        throw new DALParserException($msg);
                    }

                    DALSelectParser::validate($cond);
                }//end if
            } else if (($tagName === 'or') || ($tagName === 'and') || ($tagName === 'not')) {
                self::validate($cond);
            } else if ($tagName !== 'hook') {
                $msg = 'Found invalid WHERE comparison type "'.$tagName.'".';
                throw new DALParserException($msg);
            }//end if
        }//end foreach

    }//end validate()


}//end class

?>
