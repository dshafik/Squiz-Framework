<?php
/**
 * DALSelectParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
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
 * @since 4.0.0
 */
class DALSelectParser extends DALQueryParser
{


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     * All DALBaker methods should be called statically.
     *
     * @since 4.0.0
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the array form of the SELECT query.
     *
     * @param DomElement $xmlQuery Query element.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
