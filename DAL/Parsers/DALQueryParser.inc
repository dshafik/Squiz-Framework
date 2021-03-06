<?php
/**
 * Main class for the Parsing of Queries into arrays.
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

require_once 'DAL/Exceptions/DALParserException.inc';
require_once 'DAL/Parsers/DALSelectParser.inc';
require_once 'DAL/Parsers/DALCaseParser.inc';
require_once 'DAL/Parsers/DALFromParser.inc';
require_once 'DAL/Parsers/DALAlterParser.inc';
require_once 'DAL/Parsers/DALSchemaParser.inc';
require_once 'DAL/Parsers/DALWithParser.inc';
require_once 'DAL/Parsers/DALDeleteParser.inc';
require_once 'DAL/Parsers/DALDropParser.inc';
require_once 'DAL/Parsers/DALInsertParser.inc';
require_once 'DAL/Parsers/DALUpdateParser.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALGroupByParser.inc';
require_once 'DAL/Parsers/DALOrderByParser.inc';
require_once 'DAL/Parsers/DALUnionParser.inc';
require_once 'DAL/Parsers/DALExceptParser.inc';
require_once 'DAL/Parsers/DALIntersectParser.inc';
require_once 'DAL/DALBaker.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALQueryParser.
 *
 * Base query parser. Contains common parsing functions like field parser etc.
 */
class DALQueryParser
{


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     * All DALBaker methods should be called statically.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Constructs an SQL query from DomElement.
     *
     * This is the main function where the SQL array is constructed from XML.
     * It will combine queries from different parts of the system.
     *
     * @param DomElement $query DomElement that represents a single query.
     *
     * @return array
     * @throws DALParserException If the element passed is not a query or is of
     *                            an invalid type.
     */
    public static function parse(DomElement $query)
    {
        $firstChild = $query->firstChild;
        if ($firstChild === NULL) {
            throw new DALParserException('Invalid query element.');
        }

        foreach ($query->childNodes as $childNode) {
            if ($childNode->nodeType === XML_ELEMENT_NODE) {
                $type = $childNode->tagName;
                break;
            }
        }

        $sqlArray = array();

        switch ($type) {
            case 'select':
                DALSelectParser::validate($query);
                $sql = DALSelectParser::parse($query);
            break;

            case 'insert':
                DALInsertParser::validate($query);
                $sql = DALInsertParser::parse($query);
            break;

            case 'update':
                DALUpdateParser::validate($query);
                $sql = DALUpdateParser::parse($query);
            break;

            case 'delete':
                DALDeleteParser::validate($query);
                $sql = DALDeleteParser::parse($query);
            break;

            case 'union':
            case 'union-all':
                DALUnionParser::validate($query);
                $sql = DALUnionParser::parse($query);
            break;

            case 'except':
            case 'except-all':
                DALExceptParser::validate($query);
                $sql = DALExceptParser::parse($query);
            break;

            case 'intersect':
                DALIntersectParser::validate($query);
                $sql = DALIntersectParser::parse($query);
            break;

            case 'alter':
                DALAlterParser::validate($query);
                $sql = DALAlterParser::parse($query);
            break;

            case 'drop':
                DALDropParser::validate($query);
                $sql = DALDropParser::parse($query);
            break;

            case 'function':
                $function = $query->getElementsByTagName('function')->item(0);
                self::validateFunctionTag($function);
                $sql = self::parseSqlFunction($function);
            break;

            case 'with':
                DALWithParser::validate($query);
                $sql = DALWithParser::parse($query);
            break;

            case 'call':
                $sql = self::parseCallQuery($query);
            break;

            case 'where':
                $sql = DALWhereParser::parse($query);
                $sql = current($sql);
            break;

            case 'from':
                $sql = DALFromParser::parse($query);
            break;

            default:
                throw new DALParserException('Invalid query type:'.$type);
            break;
        }//end switch

        // Add Query type.
        if (is_array($sql) === TRUE) {
            $sqlArray['query'] = $sql;
            $aliases           = array();
            self::_convertAliases($sqlArray['query'], $aliases);
            $sqlArray['query_type'] = $type;
        }

        return $sqlArray;

    }//end parse()


    /**
     * Adds bind variables to the query array.
     *
     * Loc element references the location of the bind variable in the main query.
     *
     * @param array &$sqlArray The main query array.
     * @param array &$aliases  Array of bind variables.
     *
     * @return void
     */
    private static function _convertAliases(array &$sqlArray, array &$aliases)
    {
        $vars = array();
        foreach ($sqlArray as $key => $value) {
            if (is_array($value) === FALSE) {
                continue;
            }

            if (isset($value['alias']) === TRUE) {
                $clonedArray = array();
                $clonedArray = array_merge($clonedArray, $sqlArray);
                unset($clonedArray[$key]['alias']);
                $aliases[$value['alias']] = $clonedArray;
            }

            if (array_key_exists('isalias', $value) === TRUE) {
                $sqlArray[$key] = $aliases[$value['name']];
            }

            if (is_array($value) === TRUE) {
                self::_convertAliases($sqlArray[$key], $aliases);
            }
        }

    }//end _convertAliases()


    /**
     * Constructs the SQL's LIMIT clause from XML node.
     *
     * Returns the array for the LIMIT clause.
     * array(
     *  'LIMIT' = array(
     *             'rows'   => int,
     *             'offset' => int,
     *            ),
     * );
     *
     * @param DomElement $select Select Node of the XML.
     *
     * @return array
     */
    protected static function parseLimitClause(DomElement $select)
    {
        $query = array();
        foreach ($select->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE
                && $child->tagName === 'limit'
            ) {
                $query['LIMIT']           = array();
                $query['LIMIT']['rows']   = $child->nodeValue;
                $query['LIMIT']['offset'] = $child->getAttribute('offset');

                // Only one limit per query.
                break;
            }
        }

        return $query;

    }//end parseLimitClause()


    /**
     * Constructs the IN condition from XML node.
     *
     * Array(
     *  'IN' => array(
     *           'table'      => string,
     *           'column'     => string,
     *           'CONDITIONS' => array(
     *                            0 => string,
     *                            ---- or ----
     *                            'SELECT' => array(),
     *                           ),
     *          ),
     * )
     *
     * @param DomElement $inCond Select Node of the XML.
     * @param integer    $level  Indentation level.
     *
     * @return array
     */
    protected static function parseInClause(DomElement $inCond, $level=0)
    {
        $conditions = array();
        if ($inCond !== NULL) {
            $type = strtoupper($inCond->tagName);

            $conditions[$type] = array();

            // In clause can be complex, check for left and right tags.
            if ($inCond->getElementsByTagName('left')->length !== 0
                && $inCond->getElementsByTagName('right')->length !== 0
            ) {
                $nodeTypes = array(
                              'left'  => 'LEFT',
                              'right' => 'RIGHT',
                             );

                foreach ($nodeTypes as $nodeType => $uNodeType) {
                    $node = $inCond->getElementsByTagName($nodeType)->item(0);
                    $conditions[$type][$uNodeType] = array();
                    foreach ($node->childNodes as $child) {
                        if ($child->nodeType !== XML_ELEMENT_NODE) {
                            continue;
                        }

                        $conditions[$type][$uNodeType][] = self::parseSingleField($child);
                    }
                }

                return $conditions;
            }//end if

            $conditions[$type]['table']      = $inCond->getAttribute('table');
            $conditions[$type]['column']     = $inCond->getAttribute('column');
            $conditions[$type]['CONDITIONS'] = array();

            // Check if this IN statement is using inner select.
            $select    = $inCond->getElementsByTagName('select')->item(0);
            $except    = $inCond->getElementsByTagName('except')->item(0);
            $intersect = $inCond->getElementsByTagName('intersect')->item(0);

            if ($except !== NULL) {
                $conditions[$type]['CONDITIONS'] = DALExceptParser::parse($except);
            } else if ($intersect !== NULL) {
                $conditions[$type]['CONDITIONS'] = DALIntersectParser::parse($intersect);
            } else if ($select === NULL) {
                // Must be using values.
                // There could be a single value with no value tag.
                if (XML::hasChildElements($inCond) === FALSE) {
                    $conditions[$type]['CONDITIONS'][] = $inCond->nodeValue;
                } else {
                    foreach ($inCond->childNodes as $child) {
                        if ($child->nodeType === XML_ELEMENT_NODE) {
                            $val = self::parseSingleField($child);
                            // Add field value to the list.
                            $conditions[$type]['CONDITIONS'][] = $val;
                        }
                    }
                }
            } else if ($select !== NULL) {
                // Inner Select.
                $conditions[$type]['CONDITIONS'] = DALSelectParser::parse($select);
            }//end if
        }//end if

        return $conditions;

    }//end parseInClause()


    /**
     * Constructs the EXISTS condition from XML node.
     *
     * Array(
     *  'EXISTS' => array(
     *               'CONDITIONS' => array(
     *                                'SELECT' => array(),
     *                               ),
     *              ),
     * )
     *
     * @param DomElement $existsCond Select Node of the XML.
     * @param integer    $level      Indentation level.
     *
     * @return array
     */
    protected static function parseExistsClause(DomElement $existsCond, $level=0)
    {
        $conditions = array();
        if ($existsCond !== NULL) {
            $type   = strtoupper($existsCond->tagName);
            $select = $existsCond->getElementsByTagName('select')->item(0);

            if ($select !== NULL) {
                // Parse the inner select for this exists clause...
                $conditions[$type] = array(
                                      'CONDITIONS' => DALSelectParser::parse($select),
                                     );
            }
        }

        return $conditions;

    }//end parseExistsClause()


    /**
     * Parses the calling of a method within a query.
     *
     * @param DomElement $call The tag defining the call.
     *
     * @return array
     */
    public static function parseCallQuery(DomElement $call)
    {
        $query         = array();
        $query['CALL'] = array();

        // Get the function tag.
        $function      = $call->getElementsByTagName('function')->item(0);
        $query['CALL'] = self::parseSqlFunction($function);

        return $query;

    }//end parseCallQuery()


    /**
     * Returns list of fields separated with commas.
     *
     * Function will look for field tag under parent tag.
     *
     * @param DomElement $parent Fields Node of the XML.
     *
     * @return array
     */
    protected static function parseChildFields(DomElement $parent)
    {
        $fields = array();
        foreach ($parent->childNodes as $field) {
            if ($field->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            $newField = self::parseSingleField($field);
            if ($newField !== FALSE) {
                $fields[] = $newField;
            }
        }//end foreach

        return $fields;

    }//end parseChildFields()


    /**
     * Returns field array.
     *
     * This function will first determine which type of field is given. If non
     * found then it will return FALSE.
     *
     * @param DomElement $fieldXML Singel field element.
     *
     * @return (array|boolean)
     */
    protected static function parseSingleField(DomElement $fieldXML)
    {
        $field = FALSE;
        if ($fieldXML->tagName === 'field') {
            $field           = array();
            $field['table']  = $fieldXML->getAttribute('table');
            $field['column'] = $fieldXML->getAttribute('column');

            $alias = $fieldXML->getAttribute('alias');
            if (empty($alias) === FALSE) {
                $field['alias'] = $alias;
            }

            if ($fieldXML->nodeValue !== '') {
                $field['alias'] = $fieldXML->nodeValue;
            }
        } else if ($fieldXML->tagName === 'value') {
            $nodeValue = $fieldXML->nodeValue;
            $caseList  = DALCaseParser::parse($fieldXML);
            if (empty($caseList) === FALSE) {
                $nodeValue = $caseList;
            }

            if ($fieldXML->getAttribute('type') === 'integer') {
                $nodeValue = (int) $nodeValue;
            }//end if

            if ($fieldXML->getAttribute('alias') === '') {
                $field = $nodeValue;
            } else {
                $field          = array();
                $field['alias'] = $fieldXML->getAttribute('alias');
                $field['value'] = $nodeValue;
            }
        } else if ($fieldXML->tagName === 'alias') {
            $field            = array();
            $field['name']    = $fieldXML->getAttribute('name');
            $field['isalias'] = TRUE;
        } else if ($fieldXML->tagName === 'literal') {
            $field = $fieldXML->nodeValue;
        } else if ($fieldXML->tagName === 'function') {
            $field = self::parseSqlFunction($fieldXML);
        } else if ($fieldXML->tagName === 'case') {
            $field = DALCaseParser::parse($fieldXML, TRUE);
        } else if ($fieldXML->tagName === 'hook') {
            $field = DALBaker::getHookPrefix().$fieldXML->getAttribute('id');
        } else if ($fieldXML->tagName === 'math-op') {
            $field = self::parseMathOperation($fieldXML);
        } else if ($fieldXML->tagName === 'select') {
            $field = DALSelectParser::parse($fieldXML);
        }//end if

        return $field;

    }//end parseSingleField()


    /**
     * Returns the array for a math operation block.
     *
     * @param DomElement $parent The math-op element.
     * @param string     $op     Math operator description (i.e. ADD).
     *
     * @return array
     */
    protected static function parseMathOperation(DomElement $parent, $op='MATH-OP')
    {
        $query      = array();
        $query[$op] = array();
        if (($op === 'MATH-OP') && ($parent->getAttribute('alias') !== '')) {
            $query[$op]['alias'] = $parent->getAttribute('alias');
        }

        foreach ($parent->childNodes as $node) {
            if ($node->nodeType === XML_ELEMENT_NODE) {
                switch ($node->tagName) {
                    case 'add':
                    case 'divide':
                    case 'subtract':
                    case 'multiply':
                        $query[$op][] = self::parseMathOperation(
                            $node,
                            strtoupper($node->tagName)
                        );
                    break;

                    default:
                        if ($node->tagName === 'select') {
                            $query[$op][] = DALSelectParser::parse($node);
                        } else {
                            $query[$op][] = self::parseSingleField($node);
                        }
                    break;
                }
            }//end if
        }//end foreach

        return $query;

    }//end parseMathOperation()


    /**
     * Converts XML function element to PHP array.
     *
     * Array(
     *  'FUNCTION' => array(
     *                 'function' => string,
     *                 'ARGS'     => array(
     *                                0 => mixed,
     *                               ),
     *                ),
     * )
     *
     * @param DomElement $function Function XML node.
     *
     * @return array
     */
    protected static function parseSqlFunction(DomElement $function)
    {
        $query = array();
        if ($function !== NULL) {
            $query['FUNCTION']             = array();
            $query['FUNCTION']['function'] = $function->getAttribute('function');
            $query['FUNCTION']['ARGS']     = array();

            if ($function->getAttribute('alias') !== '') {
                $query['FUNCTION']['alias'] = $function->getAttribute('alias');
            }

            foreach ($function->childNodes as $arg) {
                if ($arg->nodeType === XML_ELEMENT_NODE) {
                    $field = XML::getFirstChildElement($arg);
                    if ($field !== NULL) {
                        switch ($field->tagName) {
                            case 'select':
                                    $query['FUNCTION']['ARGS'][] = DALSelectParser::parse($field);
                            break;

                            default:
                                $fields = self::parseChildFields($arg);
                                $query['FUNCTION']['ARGS'][] = array_shift($fields);
                            break;
                        }
                    } else {
                        $query['FUNCTION']['ARGS'][] = $arg->nodeValue;
                    }
                }
            }
        }//end if

        return $query;

    }//end parseSqlFunction()


    /**
     * Validate Field tag.
     *
     * Field tag must have column and table attribute, and tag node value is
     * only allowed in SELECT clause (for field alias).
     *
     * @param DomElement $field      Single field tag.
     * @param boolean    $allowAlias Allow field alias.
     *
     * @return void
     * @throws DALParserException If a field doesn't have a table or a column
     *                            specified, or if an alias is used incorrectly.
     */
    public static function validateField(DomElement $field, $allowAlias=FALSE)
    {
        // Field must have table and column attribute.
        if ($field->hasAttribute('table') === FALSE) {
            throw new DALParserException('field tag must have table attr.');
        } else if ($field->getAttribute('column') === '') {
            throw new DALParserException('field tag must have column attr.');
        } else if (($allowAlias === FALSE) && ($field->nodeValue !== '')) {
            $msg = 'alias for field is not allowed here.';
            throw new DALParserException($msg);
        }

    }//end validateField()


    /**
     * Validates INSERT and UDPATE Fields tag.
     *
     * Throws DALParserException.
     *
     * @param DomElement $fields The query element.
     *
     * @return void
     * @throws DALParserException If the fields tag is invalid for any reason.
     */
    protected static function validateFieldsTag(DomElement $fields)
    {
        // Fields tag must have table attribute.
        if ($fields->getAttribute('table') === FALSE) {
            $msg = 'Fields tag must have table attribute.';
            throw new DALParserException($msg);
        }

        // Make sure fields tag have field tags with values.
        $fields = $fields->getElementsByTagName('field');
        if ($fields->length === 0) {
            $msg = 'Fields tag with no field tags.';
            throw new DALParserException($msg);
        }

        // Field tags must have values.
        foreach ($fields as $field) {
            if ($field->nodeValue === '') {
                $msg = 'Field with no value.';
                throw new DALParserException($msg);
            }
        }

    }//end validateFieldsTag()


    /**
     * Validates the values tag.
     *
     * Values tag must have one or more value tags.
     *
     * @param DomElement $valuesTag The values tag.
     *
     * @return void
     * @throws DALParserException If the values tag contains no value or select
     *                            tags.
     */
    public static function validateValuesTag(DomElement $valuesTag)
    {
        $foundValue = FALSE;
        foreach ($valuesTag->childNodes as $value) {
            if ($value->nodeType === XML_ELEMENT_NODE) {
                if ($value->tagName === 'value') {
                    $foundValue = TRUE;
                    self::validateValueTag($value);
                } else if ($value->tagName === 'select') {
                    $foundValue = TRUE;
                    DALSelectParser::validate($valuesTag);
                }
            }
        }

        if ($foundValue === FALSE) {
            $msg = 'Values tag with no value/select tags.';
            throw new DALParserException($msg);
        }

    }//end validateValuesTag()


    /**
     * Validates a single value tag for update statement.
     *
     * Value tag must have a column attribute and may contain a math-op tag.
     *
     * @param DomElement $value The value tag.
     *
     * @return void
     * @throws DALParserException If a value tag doesn't have a column
     *                            specified.
     */
    protected static function validateValueTag(DomElement $value)
    {
        if ($value->getAttribute('column') === '') {
            $msg = 'Update query value tags must have column attribute.';
            throw new DALParserException($msg);
        }

    }//end validateValueTag()


    /**
     * Returns the union or union-all tag if there is one else returns NULL.
     *
     * @param DomElement $parentNode Parent node.
     *
     * @return object
     */
    protected static function getUnionTag(DomElement $parentNode)
    {
        $unionTag = $parentNode->getElementsByTagName('union')->item(0);
        if ($unionTag === NULL) {
            $unionTag = $parentNode->getElementsByTagName('union-all')->item(0);
        }

        return $unionTag;

    }//end getUnionTag()


    /**
     * Gets all the hooks in this query.
     *
     * @param DomElement $xmlQuery DomElement that represents a single query.
     *
     * @return array
     */
    protected static function addHookIds(DomElement $xmlQuery)
    {
        $list = array();
        foreach ($xmlQuery->childNodes as $child) {
            if ($child->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            if ($child->tagName === 'hook') {
                $list[] = $child->getAttribute('id');
            }
        }

        return $list;

    }//end addHookIds()


    /**
     * Validates a single function tag.
     *
     * @param DomElement $function Single function tag.
     *
     * @return void
     * @throws DALParserException If a function tag doesn't have a function
     *                            specified.
     */
    public static function validateFunctionTag(DomElement $function)
    {
        if ($function->getAttribute('function') === '') {
            $msg = 'function tag must have \'function\' attribute.';
            throw new DALParserException($msg);
        }

    }//end validateFunctionTag()


    /**
     * Validate Hook tag.
     *
     * Hook tag must have id attribute.
     *
     * @param DomElement $hook Single hook tag.
     *
     * @return void
     * @throws DALParserException If a hook doesn't have an ID specified.
     */
    public static function validateHook(DomElement $hook)
    {
        // Hook must have an id attribute.
        if ($hook->getAttribute('id') === '') {
            throw new DALParserException('hook tag must have an id attribute.');
        }

    }//end validateHook()


}//end class

?>
