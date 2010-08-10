<?php
/**
 * DALSelectParser.
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
require_once 'DAL/Parsers/DALCaseParser.inc';
require_once 'DAL/Parsers/DALOrderByParser.inc';
require_once 'DAL/Parsers/DALGroupByParser.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALFromParser.inc';

/**
 * DALSelectParser.
 *
 * Select query parser. Parses a select query.
 *
 */
class DALSelectParser extends DALQueryParser
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
     * @param DomElement $xmlQuery Query element.
     *
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        if (strtolower($xmlQuery->tagName) !== 'select') {
            $selectTag = $xmlQuery->getElementsByTagName('select')->item(0);
        } else {
            $selectTag = $xmlQuery;
        }

        $query = self::_getSelectClause($selectTag);

        $query = array_merge($query, DALCaseParser::parse($selectTag));

        $query = array_merge($query, DALFromParser::parse($selectTag));

        $query = array_merge($query, DALWhereParser::parse($selectTag));

        $query = array_merge($query, DALWhereParser::parse($selectTag, 'having'));

        $query = array_merge(
            $query,
            DALWhereParser::parse($selectTag, 'start-with')
        );

        $query = array_merge(
            $query,
            DALWhereParser::parse($selectTag, 'connect-by')
        );

        // Get GROUP By clause.
        $query = array_merge($query, DALGroupByParser::parse($selectTag));

        // Get Order BY clause.
        $query = array_merge($query, DALOrderByParser::parse($selectTag));

        // Get the LIMIT clause.
        $query = array_merge($query, DALQueryParser::parseLimitClause($selectTag));

        $selectHooks = DALQueryParser::addHookIds($selectTag);
        if (empty($selectHooks) === FALSE) {
            $query['HOOKS'] = $selectHooks;
        }

        return $query;

    }//end parse()


    /**
     * Constructs the SQL's select clause from XML node.
     *
     * Returns the array for the SELECT clause.
     * array(
     *  'SELECT' => array(
     *               'FIELDS' => array(),
     *              ),
     * )
     *
     * @param DomElement $select Select Node of the XML.
     * @param integer    $level  Indentation level.
     *
     * @return array
     */
    private static function _getSelectClause(DomElement $select, $level=0)
    {
        $query['SELECT'] = array();

        if (strtolower($select->getAttribute('distinct')) === 'true') {
            $query['SELECT']['distinct'] = TRUE;
        }

        // Check if there is a distinct-on tag.
        foreach ($select->childNodes as $node) {
            if ($node->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            if ($node->tagName === 'distinct-on') {
                $query['SELECT']['DISTINCT-ON'] = self::parseChildFields($node);
            } else if ($node->tagName === 'fields') {
                $query['SELECT']['FIELDS'] = self::parseChildFields($node);
            }
        }

        return $query;

    }//end _getSelectClause()


    /**
     * Validates the given SELECT query.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     * @throws DALParserException If the SELECT tag is invalid for any reason.
     */
    public static function validate(DomElement $query)
    {
        // Get the select tag.
        $selectTag = $query->getElementsByTagName('select');

        // Check if we have select query.
        $numSelects = $selectTag->length;
        if ($numSelects === 0) {
            throw new DALParserException('No select tag found.');
        } else {
            $selectTag = $selectTag->item(0);

            // Select query must have 1 fields tag with 1 or more field tags.
            $fieldsTag = $selectTag->getElementsByTagName('fields');
            if ($fieldsTag->length < 1) {
                $msg = 'Each select query must have a fields tag.';
                throw new DALParserException($msg);
            }

            $fieldsTag = $fieldsTag->item(0);
            if ($fieldsTag === NULL) {
                throw new DALParserException('Select query must have fields tag');
            } else {
                // We have a fields tag, check if its fields are correct.
                $fields = $fieldsTag->getElementsByTagName('field');

                foreach ($fields as $field) {
                    self::validateField($field, TRUE);
                }
            }

            /*
                Check FROM tag. WE need to make sure there are equal number of
                select and from tags.
            */

            $fromTag = $selectTag->getElementsByTagName('from');
            if ($fromTag !== NULL) {
                $fromTag = $fromTag->item(0);
                if ($fromTag !== NULL) {
                    // Look for unions or tables.
                    $unionTag = self::getUnionTag($fromTag);
                    if ($unionTag === NULL) {
                        $tables = $fromTag->getElementsByTagName('table');
                        if ($tables->length === 0) {
                            // Check for function.
                            $functions = $fromTag->getElementsByTagName('function');
                            if ($functions->length === 0) {
                                $msg = 'No "table" tags found in "from" tag.';
                                throw new DALParserException($msg);
                            }
                        } else {
                            foreach ($tables as $table) {
                                if ($table->nodeValue === '') {
                                    $msg = 'table tag with empty value.';
                                    throw new DALParserException($msg);
                                }
                            }
                        }
                    } else {
                        // Get all select Tags inside union.
                        $foundSelect = FALSE;
                        foreach ($unionTag->childNodes as $node) {
                            if ($node->nodeType !== XML_ELEMENT_NODE) {
                                continue;
                            }

                            if ($node->tagName === 'select' || $node->tagName === 'hook') {
                                $foundSelect = TRUE;
                                break;
                            }
                        }

                        if ($foundSelect === FALSE) {
                            $msg  = strtoupper($unionTag->tagName);
                            $msg .= ' must have 1 or more SELECT queries or HOOKs.';
                            throw new DALParserException($msg);
                        }
                    }//end if
                }//end if
            }//end if

            // Check WHERE tag.
            $whereTag = $selectTag->getElementsByTagName('where')->item(0);
            if ($whereTag !== NULL) {
                DALWhereParser::validate($whereTag);
            }

            // Group By.
            DALGroupByParser::validate($selectTag);

            // Order By.
            DALOrderByParser::validate($selectTag);
        }//end if

    }//end validate()


}//end class

?>
