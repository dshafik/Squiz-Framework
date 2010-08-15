<?php
/**
 * DALBaker.
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

require_once dirname(__FILE__).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/Channels/Channels.inc';
require_once dirname(dirname(__FILE__)).'/Channels/ChannelsBaker.inc';
require_once dirname(dirname(__FILE__)).'/Libs/FileSystem/FileSystem.inc';
require_once dirname(dirname(__FILE__)).'/Libs/XML/XML.inc';
require_once dirname(__FILE__).'/Exceptions/DALParserException.inc';
require_once dirname(__FILE__).'/Exceptions/DALBakerException.inc';
require_once dirname(__FILE__).'/Query.inc';

/**
 * DALbaker.
 *
 * Baker class for the Database Abstraction Layer, containing methods for
 * converting XML queries into PHP code.
 */
class DALBaker
{

    /**
     * List of where conditions and their signs.
     *
     * @var array $_whereConditions List of conditions that can be used in
     *                                where clause.
     */
    private static $_whereConditions = array(
                                        'equal'            => '=',
                                        'not-equal'        => '!=',
                                        'equal-or-greater' => '>=',
                                        'equal-or-less'    => '<=',
                                        'greater'          => '>',
                                        'less'             => '<',
                                        'like'             => 'LIKE',
                                        'not-like'         => 'NOT LIKE',
                                        'is-null'          => 'IS NULL',
                                        'not-null'         => 'NOT NULL',
                                        'similar-to'       => 'SIMILAR TO',
                                        'not-similar-to'   => 'NOT SIMILAR TO',
                                       );

    /**
     * The types of assertions that can be specified by systems.
     *
     * @var array $_assertionTypes Assertion Tag Names.
     */
    private static $_assertionTypes = array(
                                       'assert-true',
                                       'assert-false',
                                       'assert-empty',
                                       'assert-not-empty',
                                       'assert-null',
                                       'assert-not-null',
                                       'assert-equal',
                                       'assert-greater-than',
                                       'assert-less-than',
                                       'assert-array',
                                       'assert-not-array',
                                      );

    /**
     * Characters that cannot be used in the generation of variable names.
     *
     * @var array $_forbiddenVariablecharacters Characters that should be
     *                                            removed when generating
     *                                            variable names.
     */
    private static $_forbiddenVariablecharacters = array(
                                                    "'",
                                                    "\n",
                                                    "\t",
                                                    ' ',
                                                    '_',
                                                    '-',
                                                    '[',
                                                    ']',
                                                    '$',
                                                    '"',
                                                    ';',
                                                    '!',
                                                   );

    /**
     * A cache of dynamic table column type diff per system.
     *
     * @var array $_dynamicTableColDiff Array of column with different type.
     */
    private static $_dynamicTableColDiff = array();


    /**
     * DALBaker constructor.
     *
     * Private to avoid instantiating the object.
     * All DALBaker methods should be called statically.
     */
    private function __construct()
    {

    }//end __construct()


    /*
        New System Handling
    */


    /**
     * Returns the DomDocument for the schema.xml of the specfied system.
     *
     * Returns null if the system does not contain a system.xml file.
     *
     * @param string $systemName The name of the system to retrieve the schema
     *                           for.
     *
     * @return DomDocument The schema document.
     */
    public static function getSystemSchemaDocument($systemName)
    {
        // Get the system's schema data.
        $systemPath = Channels::getSystemsPath($systemName);
        $schemaXML  = $systemPath.'/DB/schema.xml';
        if (file_exists($schemaXML) === FALSE) {
            return NULL;
        }

        $doc = new DomDocument();
        $doc->load($schemaXML);

        return $doc;

    }//end getSystemSchemaDocument()


    /**
     * Adds and creates the schema for the supplied system.
     *
     * @param string  $systemName The system whose schema to add.
     * @param boolean $dropTable  Flag to drop existing table.
     *
     * @return boolean
     */
    public static function addSystemSchema($systemName, $dropTable=FALSE)
    {
        $schemaDoc = self::execSchemaQuery($systemName, $dropTable);
        if ($schemaDoc !== FALSE && $schemaDoc !== NULL) {
            $schemaOvenPath = DAL::getOvenPath($systemName);
            if (file_exists($schemaOvenPath) === FALSE) {
                mkdir($schemaOvenPath);
            }

            $fileName = $schemaOvenPath.'/'.$systemName.'.schema';
            file_put_contents($fileName, $schemaDoc->saveXML());

            return TRUE;
        }

        return FALSE;

    }//end addSystemSchema()


    /**
     * Executes all system DB schemas.
     *
     * Will find the <system_name>.schema file and execute it to create/update
     * database.
     *
     * @param string  $systemName The system whose schema to add.
     * @param boolean $dropTable  Flag to drop existing table.
     *
     * @return boolean
     */
    public static function execSchemaQuery($systemName, $dropTable=FALSE)
    {
        if (ChannelsBaker::systemExists($systemName) === FALSE) {
            return FALSE;
        }

        $schemaDoc = self::getSystemSchemaDocument($systemName);
        if ($schemaDoc === NULL) {
            return FALSE;
        }

        $schemaNode = $schemaDoc->getElementsByTagName('schema');
        if ($schemaNode->length === 0) {
            return FALSE;
        }

        $schemaNode  = $schemaNode->item(0);
        $schemaArray = self::parseSchema($schemaNode);
        $converter   = self::getConverter();
        if (array_key_exists('CREATE', $schemaArray) === TRUE) {
            foreach ($schemaArray['CREATE'] as $table) {
                $tableName = $table['table'];
                // Check if table exists.
                $exists = self::_dbTableExists($tableName, $converter);
                $sql    = $converter->convertCreateTable($table);
                if ($exists === TRUE && $dropTable === TRUE) {
                    self::_dbDropTable($tableName, $converter);
                    DAL::executeQueries($sql);
                } else if ($exists === FALSE) {
                    DAL::executeQueries($sql);
                }
            }
        }

        return $schemaDoc;

    }//end execSchemaQuery()


    /**
     * Drops the given table.
     *
     * Returns true if table was dropped.
     *
     * @param string       $tableName Name of the table to drop.
     * @param DALConverter $converter DAL Converter object.
     *
     * @return boolean
     */
    private static function _dbDropTable($tableName, DALConverter $converter=NULL)
    {
        if ($converter === NULL) {
            $converter = self::getConverter();
        }

        if (self::_dbTableExists($tableName, $converter) === TRUE) {
            $sql = $converter->convertDropTable($tableName);
            DAL::executeSql($sql);
            return TRUE;
        }

        return FALSE;

    }//end _dbDropTable()


    /**
     * Returns true if the given table exists in the database.
     *
     * @param string       $tableName Name of the table to drop.
     * @param DALConverter $converter DAL Converter object.
     *
     * @return boolean
     */
    private static function _dbTableExists($tableName, DALConverter $converter=NULL)
    {
        if ($converter === NULL) {
            $converter = self::getConverter();
        }

        $schema = '\''.DAL::getDbName().'\'';
        $name   = '\''.$tableName.'\'';
        $sql    = $converter->handleFunctionTableExists($name, $schema);
        $result = DAL::query($sql);
        $row    = $result->fetchAll();
        $count  = (int) current(current($row));
        if ($count === 1) {
            return TRUE;
        }

        return FALSE;

    }//end _dbTableExists()


    /**
     * Adds the queries from a system to their appropriate Oven locations.
     *
     * @param string $systemName The system to add the query to.
     *
     * @return boolean
     * @throws DALBakerException When an ID isn't specifued for a query.
     */
    public static function addSystemQueries($systemName)
    {
        if (ChannelsBaker::systemExists($systemName) === FALSE) {
            return FALSE;
        }

        $systemPath = Channels::getSystemsPath($systemName);
        $queryXML   = $systemPath.'/DB/queries.xml';
        if (file_exists($queryXML) === FALSE) {
            return FALSE;
        }

        $doc = new DomDocument();
        $doc->load($queryXML);
        $queryNode = $doc->getElementsByTagName('queries')->item(0);

        $currentDbType = DAL::getDbType();

        foreach ($queryNode->childNodes as $child) {
            if ($child->nodeType !== 1) {
                // Text Node. Probably just whitespace or a comment. We don't
                // want to deal with it.
                continue;
            }

            if ($child->tagName === 'query') {
                if ($child->hasAttribute('id') === FALSE) {
                    $error = 'ID not specified for query in '.$systemName;
                    throw new DALBakerException($error);
                }

                if ($child->hasAttribute('databases') === TRUE) {
                    $dbString = $child->getAttribute('databases');
                    $dbString = str_replace(' ', '', $dbString);
                    $dbs      = explode(',', $dbString);
                    if (in_array($currentDbType, $dbs) === FALSE) {
                        $id = $child->getAttribute('id');
                        echo 'Skipped Query: '.$id." (not for this DB type)\n";
                        continue;
                    }
                }

                if ($child->hasAttribute('hookid') === TRUE) {
                    $hookId = $child->getAttribute('hookid');
                    if (substr_count($hookId, '.') === 1) {
                        // We have a fragment.
                        self::addQueryFragment($systemName, $child);
                    } else {
                        // We have a sub query with a three part hookid (*.*.*).
                        self::addSubQuery($systemName, $child);
                    }
                } else {
                    // New Query.
                    self::addQuery($systemName, $child);
                }
            }//end if
        }//end foreach

        return TRUE;

    }//end addSystemQueries()


    /*
        Query Merging
    */


    /**
     * Merges the base query and all of its' fragments.
     *
     * Returns a string containing the PHP code for the query, and any possible
     * combinations of queries with the assertions present.
     *
     * @param string $systemName The system to which the query belongs.
     * @param string $queryName  The name of the query to generate.
     *
     * @return string
     * @throws DALBakerException When there is no primary query found.
     */
    public static function mergeQuery($systemName, $queryName)
    {
        $doc = self::getQueryXML($systemName, $queryName);

        /*
            We want to traverse through the document, and determine what
            fragments we have. If they have assertions, they must be added
            accordingly. If they don't have assertions, then they must be
            added to the base query, as they will always be executed regardless
            of the state of the system.
        */

        $completeCode = '';

        // This is our query with no fragments attached.
        $baseQuery   = $doc->getElementsByTagname('basequery')->item(0);
        $primaryNode = $baseQuery->getElementsByTagName('primary');
        if ($primaryNode->length === 0) {
            $msg = 'No primary query found for query.';
            throw new DALBakerException($msg);
        } else if ($primaryNode->length > 1) {
            $msg = 'Multiple primary queries found in query';
            throw new DALBakerException($msg);
        }

        // We have exactly 1 primary tag in our base query. This will be the
        // SQL executed if no alternate paths are executed.
        $primaryNode    = $primaryNode->item(0);
        $alternateNodes = $baseQuery->getElementsByTagName('alternate');
        $alternatives   = XML::nodeListToArray($alternateNodes);
        $alternatives[] = $primaryNode;
        $conditions     = array();
        $alternateCount = 1;
        foreach ($alternatives as $basePath) {
            // Make sure we get down to the actual base query, without
            // assertions surrounding it.
            $queryParent  = $basePath;
            $previousNode = $queryParent;
            $queryParent  = XML::getFirstChildElement($queryParent);
            while ($queryParent !== NULL) {
                if (in_array($queryParent->tagName, self::$_assertionTypes) === FALSE) {
                    $queryParent = $previousNode;
                    break;
                }

                $previousNode = $queryParent;
                $queryParent  = XML::getFirstChildElement($queryParent);
            }

            // Stores any fragments that have assertions wrapping them.
            $assertions = array();
            // Loop through each fragment defined for the query.
            $fragmentsNode = $doc->getElementsByTagName('fragments')->item(0);
            foreach ($fragmentsNode->childNodes as $fragment) {
                // Retrieve the assertions that exist.
                $types         = self::$_assertionTypes;
                $assertionList = XML::getElementsByTagNames($types, $fragment);
                if (count($assertionList) === 0) {
                    // No assertion found, so this should always be part of the base
                    // query.
                    $select   = $fragment->getElementsByTagName('select')->item(0);
                    $basePath = self::_combineFragment($basePath, $select);
                    $basePath = $doc->importNode($basePath, TRUE);
                } else {
                    // This assertion will be handled when the tree is constructed.
                    $assertions[] = $fragment;
                }
            }

            // Now we need to evaluate assertions.
            // So we need to get them into a tree structure.
            $assertTree = self::_recurseAddAssertion($assertions);

            // Print out each assertion in the assertion tree with each nodes'
            // SQL representation.
            $queryParent = $doc->importNode($queryParent, TRUE);
            $phpCode     = '';
            $queryBaseId = $systemName.'_'.$queryName;
            if ($queryParent->tagName !== 'primary') {
                $queryBaseId .= 'Alternate'.$alternateCount;
                $alternateCount++;
            }

            if (count($assertions) !== 0) {
                $phpCode = self::_recursePrintAssertions(
                    $assertTree,
                    $queryParent,
                    $queryBaseId
                );
            } else {
                // Just the base query to print out.
                $phpCode = self::_printNodeSql($queryParent, $queryBaseId);
            }

            $alternateCount++;

            // If it is primary with alternates, print else, otherwise print just
            // assign, otherwise it is an alternate, and we need to evaluate
            // assertions.
            if ($basePath->tagName === 'primary') {
                if (count($alternatives) > 1) {
                    $phpCode = "{\n".$phpCode."\n}\n";
                } else {
                    $phpCode .= "\n";
                }
            } else {
                $phpCode = self::_printAlternateAssertions($basePath, $phpCode);
            }

            $conditions[] = $phpCode;
        }//end foreach

        $completeCode = implode(' else ', $conditions);
        return $completeCode;

    }//end mergeQuery()


    /*
        Assertions
    */


    /**
     * Prints out a condition to control logic of an alternate query.
     *
     * @param DomElement $alternate The object containing the alternate query
     *                              and its assertions.
     * @param string     $contents  The PHP code to execute if the condition
     *                              is met.
     *
     * @return string
     */
    private static function _printAlternateAssertions(
        DomElement $alternate,
        $contents
    ) {
        $asserts    = XML::getElementsByTagNames(self::$_assertionTypes, $alternate);
        $assertVars = array();
        foreach ($asserts as $assertion) {
            $varName      = self::_getAssertionVariableName($assertion);
            $assertVars[] = '('.$varName.' === TRUE)';
        }

        $condition = 'if ('.implode(' && ', $assertVars).") {\n";
        return $condition.$contents."\n}";

    }//end _printAlternateAssertions()


    /**
     * Recursively add an assertion to the tree structure.
     *
     * Returns an array that contains structures for each assertion present in
     * the $assertions array (by recursively calling itself). The array is in
     * the form:
     *      array(
     *       'if_condition'   => array(
     *                            'current_assertion' => DomElement
     *                            'assertions'        => array(),
     *                            'children'          => array(),
     *                           ),
     *       'else_condition' => array(
     *                            'assertions' => array(),
     *                            'children'   => array(),
     *                           ),
     *      );
     *
     * @param array $assertions   An array of assertions that are yet to be
     *                            added to the tree structure.
     * @param array $currentStack An array of assertions that currently apply
     *                            to this branch of the tree.
     *
     * @return array
     */
    private static function _recurseAddAssertion(
        array $assertions,
        array $currentStack=array()
    ) {
        $retval = array();
        if (empty($assertions) === TRUE) {
            return $retval;
        }

        $currentAssertion = array_shift($assertions);
        // Setup the else condition first, as it doesn't have the assertion
        // on the current level. It should just pass through the current stack.
        $elseChildren  = self::_recurseAddAssertion($assertions, $currentStack);
        $elseCondition = array(
                          'assertions' => $currentStack,
                          'children'   => $elseChildren,
                         );

        // Add the current assertion for the IF condition.
        $currentStack[] = $currentAssertion;

        $ifChildren = self::_recurseAddAssertion($assertions, $currentStack);

        $ifCondition = array(
                        'current_assertion' => $currentAssertion,
                        'assertions'        => $currentStack,
                        'children'          => $ifChildren,
                       );

        $retval['if_condition']   = $ifCondition;
        $retval['else_condition'] = $elseCondition;

        return $retval;

    }//end _recurseAddAssertion()


    /**
     * Recursively prints out the PHP code for the tree of a query.
     *
     * Prints out the conditional statements that match the assertions and
     * fragments present in the query.
     *
     * @param array      $tree        The tree that has been built by
     *                                _recurseAddAssertion() via the calling
     *                                function.
     * @param DomElement $baseQuery   The Node of the base query, containing the
     *                                structure that exists regardless of
     *                                assertions.
     * @param string     $baseQueryId The base name of the query to print.
     * @param integer    $level       The level that the current assertion is at.
     *                                This is used to indent the code.
     *
     * @return string
     */
    private static function _recursePrintAssertions(
        array $tree,
        DomElement $baseQuery,
        $baseQueryId,
        $level=0
    ) {
        $contents = '';
        if (empty($tree) === TRUE) {
            return '';
        }

        $ifCondition   = $tree['if_condition'];
        $elseCondition = $tree['else_condition'];
        $currentAssert = $ifCondition['current_assertion'];

        // Find the assertion, and determine what variable to test for in the
        // if condition.
        $types            = self::$_assertionTypes;
        $assertionList    = XML::getElementsByTagNames($types, $currentAssert);
        $assertionNode    = array_shift($assertionList);
        $assertionChannel = $assertionNode->getAttribute('channel');
        $assertVariable   = self::_getAssertionVariableName($assertionNode);

        // Add the code to the string, for the if and else contents.
        $contents .= "\nif (".$assertVariable." === TRUE) {\n";
        if (empty($ifCondition['children']) === TRUE) {
            // Empty children, we need to print out the query.
            $fragments = $ifCondition['assertions'];
            $ifQuery   = self::_mergeAllAssertions($baseQuery, $fragments);
            $idParts   = array();
            foreach ($fragments as $queryPart) {
                $idParts[] = $queryPart->getAttribute('id');
            }

            $contents .= self::_printNodeSql($ifQuery, $baseQueryId, $idParts);
        } else {
            // It has children, we need to recurse again.
            $contents .= self::_recursePrintAssertions(
                $ifCondition['children'],
                $baseQuery,
                $baseQueryId,
                ($level + 1)
            );
        }

        $contents .= "\n} else {\n";
        if (empty($ifCondition['children']) === TRUE) {
            // Empty children, we need to print out the query.
            $fragments = $elseCondition['assertions'];
            $elseQuery = self::_mergeAllAssertions($baseQuery, $fragments);
            $idParts   = array();
            foreach ($fragments as $queryPart) {
                $idParts[] = $queryPart->getAttribute('id');
            }

            $contents .= self::_printNodeSql($elseQuery, $baseQueryId, $idParts);
        } else {
            // It has children, we need to recurse again.
            $contents .= self::_recursePrintAssertions(
                $elseCondition['children'],
                $baseQuery,
                $baseQueryId,
                ($level + 1)
            );
        }

        $indentSpace = '    ';
        $contents    = str_replace("\n", "\n".$indentSpace, $contents);
        $contents   .= "}\n";
        return $contents;

    }//end _recursePrintAssertions()


    /**
     * Combines the passed assertions with the base query.
     *
     * With all passed assertions, the main query is modified to add the
     * assertion's logic.
     *
     * @param DomElement $baseQuery  The parts of the query that are present
     *                               without any assertions.
     * @param array      $assertions An array of DomElements representing each
     *                               assertion that needs to be merged.
     *
     * @return DomElement
     */
    private static function _mergeAllAssertions(
        DomElement $baseQuery,
        array $assertions
    ) {
        $doc = new DomDocument();
        // We want a new copy of the base node, that we can add all assertions
        // to, without altering the original.
        $returnQuery = $doc->importNode($baseQuery, TRUE);
        foreach ($assertions as $fragment) {
            // Add each fragment to the query, making sure we only pass the
            // select part of the fragment to _combineFragment().
            $select      = $fragment->getElementsByTagName('select')->item(0);
            $returnQuery = self::_combineFragment($returnQuery, $select);
        }

        return $returnQuery;

    }//end _mergeAllAssertions()


    /**
     * Returns an array of assertion objects that are in the query.
     *
     * @param string $systemName The system where the query belongs.
     * @param string $queryName  The name of the query.
     *
     * @return array
     */
    private static function _getQueryAssertions($systemName, $queryName)
    {
        $doc = self::getQueryXML($systemName, $queryName);

        // Find a list of all assertions and return their array.
        $types      = self::$_assertionTypes;
        $assertions = XML::getElementsByTagNames($types, $doc);

        return $assertions;

    }//end _getQueryAssertions()


    /**
     * Generates a variable name that corresponds to the assertion node.
     *
     * Eg. If an assertion is passed that has a channel attribute
     *     'Users::isSystemAdmin', then '$usersIsSystemAdmin' will be returned.
     *
     *     If however a function call is specified in the assertion, then the
     *     variable name will be equal to the function name, and its' arguments.
     *
     * It is used when baking query methods, so that assertions can be
     * consistent and clashes or wrong references can be avoided.
     *
     * @param DomElement $assertion The channel to generate the variable for.
     *
     * @return string
     * @throws DALBakerException If an assetion doesn't have a funciton call.
     */
    private static function _getAssertionVariableName(DomElement $assertion)
    {
        $channelName  = '';
        $variableName = '';
        if ($assertion->hasAttribute('channel') === TRUE) {
            $channelName = $assertion->getAttribute('channel');
            if (trim($channelName) === '') {
                throw new DALBakerException('Channel name is empty');
            }

            list($system, $event) = explode('::', $channelName);
            $variableName         = '$'.strtolower($system).ucfirst($event);
        } else if ($assertion->hasAttribute('data') === TRUE) {
            $variableName  = '$'.$assertion->getAttribute('data').'ChannelData';
            $variableName .= str_replace('assert-', '', $assertion->tagName);
            $variableName  = str_replace('-', '', $variableName);
            // Shorten the value if need be.
            $value = strval($assertion->getAttribute('value'));
            if (strlen($value) > 5) {
                $value = substr($value, 0, 5);
            }

            $variableName .= $value;
        } else {
            $functionCall = $assertion->getElementsByTagName('function-call');
            if ($functionCall->length === 0) {
                $error = 'No function call specified for assertion.';
                throw new DALBakerException($error);
            }

            $functionCall = $functionCall->item(0);
            $argList      = $functionCall->getElementsByTagName('arg');
            $variableName = $functionCall->getAttribute('function');
            foreach ($argList as $argNode) {
                // Get the text and replace any forbidden characters.
                $argText = $argNode->nodeValue;
                $replace = self::$_forbiddenVariablecharacters;
                $argText = str_replace($replace, ' ', $argText);
                $argText = str_replace(' ', '', ucwords($argText));
                // Add the text to the variable name.
                $variableName .= ucfirst($argText);
            }

            $variableName = '$'.$variableName;
        }//end if

        return $variableName;

    }//end _getAssertionVariableName()


    /*
        XML Handling
    */


    /**
     * Generates a base file for adding XML data for the query specified.
     *
     * @param string $systemName The name of the system the query is from.
     * @param string $queryName  The name of the query to create the XML file
     *                           for.
     *
     * @return boolean
     */
    public static function generateQueryXML($systemName, $queryName)
    {
        $systemPath = DAL::getOvenPath().'/'.$systemName;
        $fileName   = $systemPath.'/'.$queryName.'.xml';

        if (file_exists($systemPath) === FALSE) {
            mkdir($systemPath);
        }

        $contents  = '<querychannel system="'.$systemName.'" query="';
        $contents .= $queryName.'">';
        $contents .= '<basequery />';
        $contents .= '<fragments />';
        $contents .= '<sub-queries />';
        $contents .= '</querychannel>';

        $res = file_put_contents($fileName, $contents);
        return $res;

    }//end generateQueryXML()


    /**
     * Returns TRUE if the XML file for a system exists.
     *
     * @param string $systemName The name of the system to check for.
     * @param string $queryName  The name of the query to check the XML file
     *                           for.
     *
     * @return boolean
     */
    public static function queryXMLExists($systemName, $queryName)
    {
        $systemPath = DAL::getOvenPath().'/'.$systemName;
        $fileName   = $systemPath.'/'.$queryName.'.xml';

        $exists = file_exists($fileName);
        return $exists;

    }//end queryXMLExists()


    /**
     * Retrieve the object representing the query's XML file.
     *
     * @param string $systemName The name of the system the query is in.
     * @param string $queryName  The name of the query to retrieve the XML file
     *                           for.
     *
     * @return DomDocument
     */
    public static function getQueryXML($systemName, $queryName)
    {
        $systemPath = DAL::getOvenPath().'/'.$systemName;
        $fileName   = $systemPath.'/'.$queryName.'.xml';

        if (self::queryXMLExists($systemName, $queryName) === FALSE) {
            self::generateQueryXML($systemName, $queryName);
        }

        $doc = new DomDocument();
        $doc->load($fileName);
        return $doc;

    }//end getQueryXML()


    /**
     * Save the passed representation of the query.
     *
     * @param string      $system    The name of the system the query is in.
     * @param string      $queryName The name of the query to save.
     * @param DomDocument $doc       The Object representing the new XML
     *                               contents.
     *
     * @return void
     */
    public static function saveQueryXML($system, $queryName, DomDocument $doc)
    {
        $systemPath = DAL::getOvenPath().'/'.$system;
        $fileName   = $systemPath.'/'.$queryName.'.xml';
        $doc->save($fileName);

    }//end saveQueryXML()


    /*
        Adding Query Fragments
    */


    /**
     * Adds a base query definition to the query's XML file.
     *
     * Extracts any hard-coded values, along with bindings and adds the data
     * to the query object. Then saves the new data to the XML file.
     *
     * @param string     $systemName The system to add the query to.
     * @param DomElement $query      The node containing the query to add.
     *
     * @return boolean
     */
    public static function addQuery($systemName, DomElement $query)
    {
        $queryId = $query->getAttribute('id');

        if (self::queryXMLExists($systemName, $queryId) === FALSE) {
            self::generateQueryXML($systemName, $queryId);
        }

        $doc       = self::getQueryXML($systemName, $queryId);
        $queryNode = $doc->getElementsByTagName('basequery')->item(0);

        if ($queryNode !== NULL) {
            if ($queryNode->getElementsByTagName('query')->length !== 0) {
                return FALSE;
            }
        }

        // Extract and replace any hard-coded news in the query, with place
        // holder variables, after setting the system for use by the
        // _prepareFragment() method.
        $query->setAttribute('system', $systemName);
        $importedNode = self::_prepareFragment($query);
        // Import the node to the document.
        $importedNode = $doc->importNode($importedNode, TRUE);

        $queryNode->appendChild($importedNode);
        self::saveQueryXML($systemName, $queryId, $doc);

        return TRUE;

    }//end addQuery()


    /**
     * Adds a base query fragment definition to the query's XML file.
     *
     * Extracts any hard-coded values, along with bindings and adds the data
     * to the query object. Then saves the new data to the XML file.
     *
     * @param string     $systemName The system the fragment has come from.
     * @param DomElement $fragment   The node containing the fragment to add.
     *
     * @return void
     */
    public static function addQueryFragment($systemName, DomElement $fragment)
    {
        // We need to add the originating system to the fragment, so that we
        // know where it comes from when it's out in the wild.
        $fragment->setAttribute('system', $systemName);

        // We need to parse this fragment, and convert all hard coded values to
        // placeholders.
        $fragment = self::_prepareFragment($fragment);

        $hookId                           = $fragment->getAttribute('hookid');
        list($targetSystem, $targetQuery) = explode('.', $hookId);

        if (self::queryXMLExists($targetSystem, $targetQuery) === FALSE) {
            self::generateQueryXML($targetSystem, $targetQuery);
        }

        $doc           = self::getQueryXML($targetSystem, $targetQuery);
        $fragmentsNode = $doc->getElementsByTagName('fragments')->item(0);

        /*
            Have to check if the query fragment exists. If it does, remove it.
        */

        // Import the node to the document.
        $importedNode = $doc->importNode($fragment, TRUE);
        $fragmentsNode->appendChild($importedNode);
        self::saveQueryXML($targetSystem, $targetQuery, $doc);

    }//end addQueryFragment()


    /**
     * Adds a sub-query that wishes to hook into a main query.
     *
     * Extracts any hard-coded values, along with bindings and adds the data
     * to the query object. Then saves the new data to the XML file.
     *
     * @param string     $systemName The system the fragment has come from.
     * @param DomElement $subQuery   The node containing the fragment to add.
     *
     * @return void
     */
    public static function addSubQuery($systemName, DomElement $subQuery)
    {
        // We need to add the originating system to the fragment, so that we
        // know where it comes from when it's out in the wild.
        $subQuery->setAttribute('system', $systemName);

        // We need to parse this sub-query, and convert all hard coded values to
        // placeholders.
        $subQuery = self::_prepareFragment($subQuery);

        $hookId                           = $subQuery->getAttribute('hookid');
        list($targetSystem, $targetQuery) = explode('.', $hookId);

        if (self::queryXMLExists($targetSystem, $targetQuery) === FALSE) {
            self::generateQueryXML($targetSystem, $targetQuery);
        }

        $doc            = self::getQueryXML($targetSystem, $targetQuery);
        $subQueriesNode = $doc->getElementsByTagName('sub-queries')->item(0);

        /*
            Have to check if the query fragment exists. If it does, remove it.
        */

        // Import the node to the document.
        $importedNode = $doc->importNode($subQuery, TRUE);
        $subQueriesNode->appendChild($importedNode);
        self::saveQueryXML($targetSystem, $targetQuery, $doc);

    }//end addSubQuery()


    /**
     * Integrates a fragment into the base query.
     *
     * Returns a DomElement representing the new, combined query. The combining
     * is performed by determining what elements are present in the fragment. If
     * they are not present in the query, the whole section is added. If however
     * the element is present in the query, then the fragment's element's child
     * nodes are traversed and added to the query's corresponding element.
     *
     * For example, if the fragment has a WHERE element, and the query doesn't,
     * the fragment's WHERE element is copied over. If the query does have a
     * WHERE element, then the child nodes of the fragment's WHERE element are
     * added as children of the query's WHERE element individually.
     *
     * @param DomElement $query    The base query.
     * @param DomElement $fragment The fragment to integrate. This should be
     *                             a representation of the fragment from the
     *                             opening select tag onwards.
     *
     * @return DomElement
     */
    private static function _combineFragment(DomElement $query, DomElement $fragment)
    {
        $doc = new DomDocument();
        // Import both nodes, so we are working in the same document.
        $query    = $doc->importNode($query, TRUE);
        $fragment = $doc->importNode($fragment, TRUE);

        foreach ($fragment->childNodes as $section) {
            if ($section->nodeType !== 1) {
                // Text node, disregard.
                continue;
            }

            // See if the existing query has the same section that we can add
            // our new data too.
            $currentNode = $query->getElementsByTagName($section->tagName);
            if ($currentNode->length === 0) {
                // We don't have this section in our base query yet, we can go
                // ahead and clone it over.
                $select = $query->getElementsByTagName('select')->item(0);
                $clone  = $section->cloneNode(TRUE);
                $select->appendChild($clone);
            } else {
                $currentNode = $currentNode->item(0);
                // Existing section, traverse the nodes of the fragment and
                // import them under the existing section.
                foreach ($section->childNodes as $condition) {
                    if ($condition->nodeType !== 1) {
                        continue;
                    }

                    // Clone the node and append it to the query's Element.
                    // Cloning is done, as appending the original element seems
                    // to stuff up the loop we are in.
                    $copy = $condition->cloneNode(TRUE);
                    $currentNode->appendChild($copy);
                }
            }//end if
        }//end foreach

        return $query;

    }//end _combineFragment()


    /**
     * Performs additional operations on a fragment before it is added.
     *
     * Converts any hard-coded values present into Query Data Placeholders,
     * and adds a <placeholder> element with the value as a child of the main
     * query tag. Also calls _extractBindings() once these new QDPs are inserted
     * which adds <binding> tags for any (query or system defined) bindings to
     * the main query tag. The return DomElement therefore contains all of this
     * data, and is ready to be inserted into the XML file of the channel.
     *
     * @param DomElement $query The fragment or query to prepare.
     *
     * @return DomElement
     */
    private static function _prepareFragment(DomElement $query)
    {
        $doc = new DomDocument();

        // Import the node into our new doc.
        $returnQuery = $doc->importNode($query, TRUE);
        // Extract any hardcoded values that are in the query.
        $returnQuery = self::_extractPlaceHolders($returnQuery);
        $returnQuery = self::_extractKeywords($returnQuery);
        // Extract any bindings that are in the query.
        $returnQuery = self::_extractBindings($returnQuery);
        $returnQuery = $doc->importNode($returnQuery, TRUE);

        return $returnQuery;

    }//end _prepareFragment()


    /*
        PHP Code Handling
    */


    /**
     * Bakes out a system's queries and business logic into PHP files.
     *
     * Calls bakeQueryMethod() for each query that the system has defined.
     * For the query to be baked correctly it is required that its' XML files
     * have been created in the system's oven directory.
     *
     * @param string $systemName Name of the system to bake the code for.
     *
     * @return void
     * @throws DALBakerException If the system name supplied is empty.
     */
    public static function bakeQueriesFile($systemName)
    {
        if (trim($systemName) === '') {
            throw new DALBakerException('System name is empty');
        }

        // Write the file.
        $ovenDir = DAL::getOvenPath().'/'.$systemName;

        if (file_exists($ovenDir) === FALSE) {
            mkdir($ovenDir);
        }

        $fileName = $ovenDir.'/'.$systemName.'Queries.inc';

        $contents  = '<?'."php\n\n";
        $contents .= "require_once dirname(dirname(dirname(__FILE__))).'/DAL.inc';\n";
        $contents .= 'class '.$systemName."Queries\n{\n\n";
        // Determine what XML files are present in the oven directory for this
        // system, and print out the method for each.
        $types    = array('.xml');
        $xmlFiles = FileSystem::listDirectory($ovenDir, $types, TRUE, FALSE);
        sort($xmlFiles);
        foreach ($xmlFiles as $queryName) {
            $queryName = basename($queryName, '.xml');
            $contents .= self::bakeQueryMethod($systemName, $queryName);
        }

        $contents .= "\n\n}\n?>";

        file_put_contents($fileName, $contents);

    }//end bakeQueriesFile()


    /**
     * Bakes out a method for a single query.
     *
     * Reads in the XML files that have been generated for this query, gets all
     * fragments and their assertions, and bakes out an appropriate method that
     * will return an SQL statement that corresponds to the current system state
     * and the systems currently installed.
     *
     * @param string $systemName The name of the system to which the query
     *                           belongs.
     * @param string $queryName  The name of the query to bake.
     *
     * @return string
     */
    public static function bakeQueryMethod($systemName, $queryName)
    {
        // Output the method signature.
        $methodName = 'prepare'.ucwords($queryName).'Query';
        $contents   = 'public static function '.$methodName;
        $contents  .= "(array \$data, \$bind)\n{\n";
        // Print out the initialisation of hard coded values.
        $contents .= self::_printPlaceHolderVariables($systemName, $queryName);
        // Get the values required for our assertions.
        $contents .= "try {\n";
        // Loop through each assertion, and output a call that assigns its
        // value to a variable.
        $assertions = self::_getQueryAssertions($systemName, $queryName);
        // Print out the code for including the assertions systems and
        // initialising their values.
        $contents .= self::_printAssertionCalls($assertions);
        $contents .= "} catch (ChannelException \$e) {\n";
        $contents .= "    // Exception thrown in the called channel.\n";
        $contents .= "}\n";

        // Get all the conditions that represent all of the query's
        // combinations, and add it to the contents.
        $queryBody  = self::mergeQuery($systemName, $queryName);
        $contents  .= $queryBody;
        $subQueries = self::printSubQueries($systemName, $queryName);
        $contents  .= $subQueries;
        $contents  .= self::_printKeywords($systemName, $queryName);
        $contents  .= self::_printBindings($systemName, $queryName);
        $contents  .= "\n}//end ".$methodName."()\n";

        return $contents;

    }//end bakeQueryMethod()


    /**
     * Prints out the appropriate assignments for the passed assertions.
     *
     * The returned string contains PHP code that has on each line a variable
     * that is assigned to a channel that an assertion requires.
     *
     *  Eg: For assert-true:
     *          $fooSomeChannel = (Foo::SomeChannel() === TRUE);
     *
     *      For assert-false:
     *          $fooSomeChannel = (Foo::SomeChannel() === FALSE);
     *
     *      For assert-null:
     *          $fooSomeChannel = (Foo::SomeChannel() === NULL);
     *
     *      For assert-empty:
     *          $fooSomeChannel = (empty(Foo::SomeChannel()) === TRUE);
     *
     * The conditions contained in the remaining code then only need to check
     * that an assertion's variable is identical to TRUE as if it is, then
     * the assertion condition passed.
     *
     * The code will contain code to include the appropriate files from each
     * system that the assertions require to be evaluated.
     *
     * @param array $assertions The assertions to print calls for.
     *
     * @return string
     */
    private static function _printAssertionCalls(array $assertions)
    {
        // Will hold the return value.
        $contents = '';
        // Holds the assignment calls.
        $assignments = "\n// Initialise values for use in the conditions\n";
        // Holds the calls to include the appopriate system files for assertion
        // plug calls.
        $includes = '';
        // Keep a track of what files we have included, so we don't print the
        // call twice.
        $includedFiles = array();

        foreach ($assertions as $assertion) {
            $assignments .= self::_printAssertionInitialisation($assertion)."\n";
            if ($assertion->hasAttribute('channel') === TRUE) {
                // This has a file that needs to be included, and we have to
                // investigate the node and find the system it requires.
                $channel                   = $assertion->getAttribute('channel');
                list($system, $methodName) = explode('::', $channel);
                // Add the require for the system if it hasn't been done by a
                // previous assertion.
                if (in_array($system, $includedFiles) === FALSE) {
                    // Add the include for the system.
                    $includes .= "Channels::includeSystem('".$system."');\n";
                    // Actually include the file for our use in this method.
                    Channels::requireSystemActions($system);
                    $includedFiles[] = $system;
                }
            }
        }//end foreach

        $comment = "\n// Include required files for the assertion values.\n";
        if ($includes !== '') {
            $includes = $comment.$includes;
        }

        $contents = $includes.$assignments;
        return $contents;

    }//end _printAssertionCalls()


    /**
     * Prints out the variable initialisation for the assertion passed.
     *
     * Assigns the value of the assertion's channel or function call, then
     * assigns it to a variable name generated from the assertion's contents.
     * The returned string is in the form $variable = $method;
     *
     * @param DomElement $assertion The assertion to print the initialisation
     *                              for.
     *
     * @return string
     * @throws DALBakerException When the assertion contains no function call.
     */
    private static function _printAssertionInitialisation(DomElement $assertion)
    {
        $methodCall = '';
        if ($assertion->hasAttribute('channel') === TRUE) {
            // Standard assertion, with a call to a channel to exectute.
            $channel                   = $assertion->getAttribute('channel');
            list($system, $methodName) = explode('::', $channel);

            $actionsFilePath = Channels::getSystemsPath($system).'/'.$system.'Actions.inc';
            $ref             = new ReflectionClass($system.'Actions');
            $methods         = $ref->getMethods();
            $methodNames     = array();
            $params          = array();
            $paramIndexes    = array();
            foreach ($methods as $method) {
                if ($method->getName() === $methodName) {
                    $params = $method->getParameters();
                    break;
                }
            }

            foreach ($params as $param) {
                $paramIndexes[] = '$data[\''.$param->getName().'\']';
            }

            // Construct the method call for the channel.
            $paramString = '('.implode(', ', $paramIndexes).')';
            $methodCall  = $channel.$paramString;

            // Maybe query channel :-) Query object itself isn't very useful..
            $methodCall = 'DAL::executeDALQuery('.$methodCall.')';
        } else if ($assertion->hasAttribute('data') === TRUE) {
            // Has a channel data index specified.
            $methodCall = '$data[\''.$assertion->getAttribute('data').'\']';
        } else {
            // This is probably a function call (no channel specified) so the
            // method call has to be extracted.
            $functionCall = $assertion->getElementsByTagName('function-call');
            if ($functionCall->length === 0) {
                // No channel, and no function call. Nothing to assert.
                $error = 'No function call specified for assertion';
                throw new DALBakerException($error);
            }

            $functionCall = $functionCall->item(0);

            if ($functionCall->hasAttribute('function') === FALSE) {
                // No function specified for the function-call, which makes it
                // pretty tough to do anything.
                $error = 'No function call specified for assertion';
                throw new DALBakerException($error);
            }

            $functionName = $functionCall->getAttribute('function');
            $argList      = $functionCall->getElementsByTagName('arg');
            $args         = array();
            foreach ($argList as $argNode) {
                $args[] = self::_printAssertionArgument($argNode);
            }

            $argString  = implode(', ', $args);
            $methodCall = $functionName.'('.$argString.')';
        }//end if

        $initCode = '';

        // Add the assignment call to the contents.
        switch ($assertion->tagName) {
            case 'assert-true':
                $methodCall = '('.$methodCall.' === TRUE)';
            break;

            case 'assert-false':
                $methodCall = '('.$methodCall.' === FALSE)';
            break;

            case 'assert-null':
                $methodCall = '('.$methodCall.' === NULL)';
            break;

            case 'assert-not-null':
                $methodCall = '('.$methodCall.' !== NULL)';
            break;

            case 'assert-empty':
                $initCode   = '$empty = '.$methodCall.';'."\n";
                $methodCall = '(empty($empty) === TRUE)';
            break;

            case 'assert-not-empty':
                $initCode   = '$empty = '.$methodCall.';'."\n";
                $methodCall = '(empty($empty) === FALSE)';
            break;

            case 'assert-array':
                $initCode   = '$array = '.$methodCall.';'."\n";
                $methodCall = '(is_array($array) === TRUE)';
            break;

            case 'assert-not-array':
                $initCode   = '$array = '.$methodCall.';'."\n";
                $methodCall = '(is_array($array) === FALSE)';
            break;

            case 'assert-equal':
            case 'assert-greater-than':
            case 'assert-less-than':
                if ($assertion->hasAttribute('value') === FALSE) {
                    $msg = 'No value specified for comparison assertion.';
                    throw new DALBakerException($msg);
                }

                $assertValue = $assertion->getAttribute('value');
                if ($assertion->tagName === 'assert-equal') {
                    $operator = '===';
                } else if ($assertion->tagName === 'assert-greater-than') {
                    $operator = '>';
                } else {
                    $operator = '<';
                }

                $methodCall = '('.$methodCall.' '.$operator.' '.$assertValue.')';
            break;

            default:
                $error = 'Unknown Assertion Type: '.$assertion->tagName;
                throw new DALBakerException($error);
            break;
        }//end switch

        $variableName = self::_getAssertionVariableName($assertion);

        return $initCode.$variableName.' = '.$methodCall.';';

    }//end _printAssertionInitialisation()


    /**
     * Prints out the value of an argument according to its' data type.
     *
     * If a string is passed, its' value is printed with quotes, if it is an int
     * then the quotes will be omitted etc.
     *
     * @param DomElement $argNode The argument object to print.
     *
     * @return string
     */
    private static function _printAssertionArgument(DomElement $argNode)
    {
        $type    = $argNode->getAttribute('type');
        $data    = $argNode->nodeValue;
        $argText = '';

        switch ($type) {
            case 'string':
                $argText = "'".$data."'";
            break;

            default:
                $argText = $data;
            break;
        }

        return $argText;

    }//end _printAssertionArgument()


    /**
     * Returns an SQL string that corresponds to the passed DomElement.
     *
     * The returned string contains a full assigment to a variables $sql. This
     * allows the code to be inserted straight into the body of an assertion's
     * condition body.
     *
     * @param DomElement $query   The object containing the query's structure.
     * @param string     $baseId  The base ID to base the file on.
     * @param array      $idParts The parts of the ID to make up the hash of to
     *                            add to the end of the filename.
     *
     * @return string
     */
    private static function _printNodeSql(
        DomElement $query,
        $baseId='',
        array $idParts=array()
    ) {
        // Call the DAL to convert the XML to SQL.
        sort($idParts);
        $sqlArray = self::constructSql($query);
        $sql      = self::convertToSql($query, $sqlArray);
        $id       = $baseId.md5(implode('_', $idParts));
        self::_writeQueryObject($id, $sqlArray, $sql);
        $returnQuery = "\$query = DAL::getQueryObject('".$id."');\n";
        return $returnQuery;

    }//end _printNodeSql()


    /**
     * Creates and writes out a query object.
     *
     * @param string $id       The object containing the query's structure.
     * @param array  $sqlArray The array representation of the query.
     * @param string $sql      The SQL of the query.
     *
     * @return void
     * @throws DALException When the query object being written already exists.
     */
    private static function _writeQueryObject($id, array $sqlArray, $sql)
    {
        $queryObj = new Query($id, $sqlArray, $sql);
        $objData  = serialize($queryObj);
        $fileName = $id.'.qob';
        $path     = DAL::getQueryStorePath().'/'.$fileName;
        if (file_exists($path) === TRUE) {
            // If the file already exists, rewrite the file.
            unlink($path);
        }

        file_put_contents($path, $objData);

    }//end _writeQueryObject()


    /**
     * Returns PHP code for any placeholder elements that exist in the query.
     *
     * The code returned contains assignments that set each variable to the
     * hard-coded value that was defined in the original query. These variables
     * are then bound to the query later in the generated code.
     *
     * @param string $systemName The system to print the placeholders for.
     * @param string $queryName  The query to print the placeholders for.
     *
     * @return string
     */
    private static function _printPlaceHolderVariables($systemName, $queryName)
    {
        $doc = self::getQueryXML($systemName, $queryName);

        $contents = "\n// Values that were hard-coded in query definitions.\n";

        $placeHolders = $doc->getElementsByTagName('placeholder');
        foreach ($placeHolders as $element) {
            $varName   = substr($element->getAttribute('var_name'), 1);
            $varValue  = $element->getAttribute('value');
            $contents .= '$'.$varName." = '".$varValue."';\n";
        }

        $contents .= "\n";
        return $contents;

    }//end _printPlaceHolderVariables()


    /**
     * Prints out the data for searching and replacing sub-queries.
     *
     * @param string $systemName The system to print the sub-queries for.
     * @param string $queryName  The query to print the sub-queries for.
     *
     * @return string
     */
    public static function printSubQueries($systemName, $queryName)
    {
        $hookIds  = self::_generateHookArray($systemName, $queryName);
        $hookCode = self::_printHookCode($hookIds);
        return $hookCode;

    }//end printSubQueries()


    /**
     * Generates an array of data representing the sub-queries.
     *
     * @param string $systemName The system to print the sub-queries for.
     * @param string $queryName  The query to print the sub-queries for.
     *
     * @return array
     * @throws DALBakerException When a specified hook doesn't exist.
     */
    private static function _generateHookArray($systemName, $queryName)
    {
        $doc = self::getQueryXML($systemName, $queryName);

        $hookIds = array();
        $hooks   = $doc->getElementsByTagName('hook');
        foreach ($hooks as $hook) {
            $id                          = $hook->getAttribute('id');
            $hooksIds[$id]               = array();
            $parent                      = $hook->parentNode;
            $hookIds[$id]['parent-type'] = $parent->tagName;
            $hookIds[$id]['sub-queries'] = array();
        }

        $subNode = $doc->getElementsByTagName('sub-queries')->item(0);
        $queries = $subNode->getElementsByTagName('query');
        foreach ($queries as $subQuery) {
            $origHookId = $subQuery->getAttribute('hookid');
            $hookSys    = $subQuery->getAttribute('system');
            $hookId     = substr($origHookId, (strrpos($origHookId, '.') + 1));
            // Check this hook exists.
            if (array_key_exists($hookId, $hookIds) === FALSE) {
                throw new DALBakerException('Hook: '.$hookId.' does not exist');
            }

            // Setup an array to hold our sub-query data.
            $subData = array(
                        'assertion_var' => '',
                        'sub-query'     => NULL,
                       );

            // Get any assertions, along with the actual SQL query.
            $assertions = XML::getElementsByTagNames(
                self::$_assertionTypes,
                $subQuery
            );

            $numAssertions = count($assertions);
            for ($i = 0; $i < $numAssertions; $i++) {
                $select = $subQuery->getElementsByTagName('select')->item($i);
                if ($select === NULL) {
                    $select = $subQuery->getElementsByTagName('hookQuery')->item($i);
                    $select = XML::getFirstChildElement($select);
                }

                // Assertion present, so we only add it if the assertion var is
                // true.
                // Determine the assertion var to be used.
                $assertionVar = self::_getAssertionVariableName($assertions[$i]);
                // Add the data to our subData array.
                $subData['assertion_var'] = $assertionVar;

                // We need to add a temporary parent for our select, as convertToSql
                // expects the select to be the first child of the passed element.
                $tempDoc = new DomDocument();
                $newSub  = '<query hookid="'.$origHookId.'" system="'.$hookSys.'">';
                $newSub .= $doc->saveXML($select).'</query>';
                $tempDoc->loadXML($newSub);
                $queryNode = $tempDoc->getElementsByTagName('query')->item(0);
                // Add the newly created node, containing the sub-query, into the
                // array.
                $subData['sub-query'] = $queryNode;
                // Add the data for this sub-query to the array for the whole query
                // here.
                $hookIds[$hookId]['sub-queries'][] = $subData;
            }//end for
        }//end foreach

        return $hookIds;

    }//end _generateHookArray()


    /**
     * With an array of hook data, prints out the correct PHP code.
     *
     * The data provided must be in the form:
     *     array (
     *      [$hook_id] => array (
     *                     [parent-type] => '',
     *                     [sub-queries] => array (
     *                                       array (
     *                                        [assertion_var] => 'name_of_var',
     *                                        [sub-query] => DOMElement,
     *                                       ),
     *                                            ...
     *                                      ),
     *
     *                ),
     *
     *     );
     *
     * @param array $hookIds Data for the hooks to print.
     *
     * @return string
     */
    private static function _printHookCode(array $hookIds)
    {
        // Print out the contents.
        $contents = '';

        // For each hook id that we have, we need to determine the parent type
        // and how to seperate each SQL fragment. We also need to add each of
        // the sub-queries to the array that will be imploded with this
        // seperator.
        foreach ($hookIds as $id => $hookData) {
            $codeId    = self::getHookCodeId($id);
            $arrayId   = self::getHookArrayId($id);
            $contents .= "\n// Setting up hook data for ".$id.".\n\n";
            $contents .= $arrayId." = array();\n";

            $numSubQueries = count($hookData['sub-queries']);
            for ($i = 0; $i < $numSubQueries; $i++) {
                $subQuery = $hookData['sub-queries'][$i];

                // Store a query object for this sub query.
                $subQueryNode = $subQuery['sub-query'];
                $subQueryId   = $subQueryNode->getAttribute('id');
                $subQuerySys  = $subQueryNode->getAttribute('system');
                $subQueryHook = $subQueryNode->getAttribute('hookid');
                $subObjectId  = 'subquery_'.$id.'_'.$i.'_';
                $subObjectId .= md5($subQuerySys.$subQueryId.$subQueryHook);
                if ($subQuery['assertion_var'] !== '') {
                    $assertionVar = $subQuery['assertion_var'];
                    $contents    .= 'if ('.$assertionVar." === TRUE) {\n    ";
                }

                $sqlArray = self::constructSql($subQuery['sub-query']);
                $sql      = self::convertToSql($subQuery['sub-query'], $sqlArray);
                self::_writeQueryObject($subObjectId, $sqlArray, $sql);
                $contents .= $arrayId;
                $contents .= "[] = DAL::getQueryObject('".$subObjectId."');\n";

                if ($subQuery['assertion_var'] !== '') {
                    $contents .= "}\n";
                }
            }//end for

            // Implode the data and add seperators.
            $contents .= "\$query->subQueries('".$id."', ".$arrayId.");\n";
        }//end foreach

        return $contents;

    }//end _printHookCode()


    /**
     * Returns a string like HOOKID:hook_id corresponding to the hook id passed.
     *
     * This ID is used for replacements in PHP code.
     *
     * @param string $hookId The hook id to retireve the code ID for.
     *
     * @return string
     */
    public static function getHookCodeId($hookId)
    {
        return 'HOOKID:'.$hookId;

    }//end getHookCodeId()


    /**
     * Returns a string like $hook_iddArray corresponding to the hook id passed.
     *
     * This ID is used for the variable name of the array storing all fragments
     * that wish to reference the hook.
     *
     * @param string $hookId The hook id to retrieve the array ID for.
     *
     * @return string
     */
    public static function getHookArrayId($hookId)
    {
        $str  = ucwords(str_replace('_', ' ', $hookId));
        $code = '$'.str_replace(' ', '', $str).'Array';
        return $code;

    }//end getHookArrayId()


    /**
     * Gets the text for a seperator from the tag type passed.
     *
     * @param string $tagName The tag name to get the seperator for.
     *
     * @return string
     * @throws DALParserException Unidentified separator passed.
     */
    public static function getSeperator($tagName)
    {
        // All tags will use a comma, unless specified in the switch below.
        $seperator = ', ';

        switch (strtolower($tagName)) {
            case 'union-all':
                $seperator = '\nUNION-ALL\n';
            break;

            case 'and':
                $seperator = '\nAND\n';
            break;

            case 'or':
                $seperator = '\nOR\n';
            break;

            default:
                throw new DALParserException('Unidentified separator passed.');
            break;
        }

        return $seperator;

    }//end getSeperator()


    /**
     * Prints code to replace keywords inside the SQL string.
     *
     * Each keyword defined in the XML file as [keyword]. Where keyword is part
     * of the channel event basket. If keyword does not exists in the event
     * basket then sql will not be modified and most likely cause SQL error.
     *
     * @param string $systemName The system to print the bindings for.
     * @param string $queryName  The query to print the bindings for.
     *
     * @return string
     */
    private static function _printKeywords($systemName, $queryName)
    {
        $doc       = self::getQueryXML($systemName, $queryName);
        $keywords  = $doc->getElementsByTagName('keyword');
        $processed = array();
        $arrayInit = FALSE;
        $suffix    = '';
        $contents  = '';
        foreach ($keywords as $keyword) {
            if (in_array($keyword->nodeValue, $processed) === FALSE) {
                if ($arrayInit === FALSE) {
                    // Initialise array.
                    $contents  = "\$queryKeywordsArray = array();\n";
                    $suffix    = "\$query->keywords(\$queryKeywordArray);\n";
                    $arrayInit = TRUE;
                }

                $processed[] = $keyword->nodeValue;
                $contents   .= '$queryKeywordArray[\''.$keyword->nodeValue;
                $contents   .= '\'] = $data[\''.$keyword->nodeValue."'];\n";
            }
        }

        // If a keyword is printed.
        $contents .= $suffix;

        return $contents;

    }//end _printKeywords()


    /**
     * Prints code to prepare the query and bind the placeholders to data.
     *
     * This returns the code that is executed after the $sql variable is
     * obtained from the assertion's conditions. The $dbh variable is
     * initialised by getting the database connection from the DAL. Using the
     * connection, the obtained $sql is prepared. All bindings are then obtained
     * from the XML for the query. If they are a placeholder for a hard-coded
     * value, then the placeholder is bound to the initialised variable,
     * otherwise, the binding is done for an index into the passed $data array.
     *
     * @param string  $systemName    The system to print the bindings for.
     * @param string  $queryName     The query to print the bindings for.
     * @param boolean $printBindings If TRUE, will print bindings, otherwise
     *                               will print entries into the query array.
     *
     * @return string
     */
    private static function _printBindings(
        $systemName,
        $queryName,
        $printBindings=TRUE
    ) {
        $doc      = self::getQueryXML($systemName, $queryName);
        $contents = '';

        // For dynamic table col type [tableName].
        $dynColType = array();
        $printed    = FALSE;

        $bindingList = $doc->getElementsByTagName('binding');
        // Create an array containing our place holder names, so we can easily
        // find if a variable name has been generated by the system.
        $placeHolders     = array();
        $placeHoldersList = $doc->getElementsByTagName('placeholder');
        foreach ($placeHoldersList as $element) {
            $varName                = $element->getAttribute('var_name');
            $varValue               = $element->getAttribute('value');
            $placeHolders[$varName] = $varValue;
        }

        $usedNames = array();
        foreach ($bindingList as $binding) {
            $bindName = $binding->getAttribute('name');

            // We need to determine what data type the column is.
            $columnType = $binding->getAttribute('column_type');
            if ($columnType !== '') {
                $dataType = self::_getPdoDataType(
                    $system,
                    $table,
                    $column,
                    $columnType
                );
            } else {
                $column   = $binding->getAttribute('column');
                $table    = $binding->getAttribute('table');
                $system   = $binding->getAttribute('system');
                $dataType = self::_getPdoDataType($system, $table, $column);
            }

            // Dynamic table name, get an dynamic table col types.
            if ($table === '[tableName]' && $printed === FALSE) {
                $dynColType = self::_getDynamicTableColumnTypes($system);
                $contents  .= '$dynColType = '.var_export($dynColType, TRUE).';';
                $printed    = TRUE;
            }

            $varName = '';
            $index   = substr($bindName, 1);
            // Check if the binding is a hard coded value. If it is, set its'
            // variable to be the same as defined in _printPlaceHoldersVariables
            // otherwise print an index into the $data array.
            if (isset($placeHolders[$bindName]) === TRUE) {
                // This is a place holder for a hard-coded value.
                $varName = '$'.substr($bindName, 1);
            } else {
                // This is not hard-coded, so should be referenced in the $data
                // array.
                $varName = "\$data['".$index."']";
            }

            if ($printBindings === TRUE) {
                // Add check for array values.
                $bindCall = '';
                if (isset($placeHolders[$bindName]) === FALSE) {
                    $bindCall = "\nif (array_key_exists('".$index."',\$data) === TRUE) {\n";
                }

                $colName = $binding->getAttribute('column');
                if (isset($dynColType[$colName]) === TRUE) {
                    // This column has dynamic types. So we get the correct,
                    // PDO type based on the table name at run time.
                    $bindCall .= "\$tableName = strtolower(\$data['tableName']);\n";
                    $bindCall .= "\$query->bind('".$bindName."', ".$varName.", \$dynColType['".$colName."'][\$tableName]);\n";
                } else {
                    $bindCall .= '$query->bind(\''.$bindName.'\', '.$varName.', '.$dataType.');'."\n";
                }

                if (isset($placeHolders[$bindName]) === FALSE) {
                    $bindCall .= "}\n\n";
                }

                // Add this binding to the contents.
                $contents .= $bindCall;
            }//end if
        }//end foreach

        $contents .= "return \$query;\n";

        return $contents;

    }//end _printBindings()


    /**
     * Extracts the :value bindings from a fragment of a query.
     *
     * The return value contains the original query, along with new binding
     * elements attached.
     * To find these bindings, all elements need to be searched. This is because
     * with bindings, they can appear in many different areas of an XML query
     * definition. To ensure all elements are only searched for fragments that
     * actually have atleast one binding in them, the XML source of the fragment
     * is searched using a regular expression. If there is a binding found, all
     * elemets of the fragment are obtained, and iterated over. Elements with
     * exactly 1 child are the only ones examined for bindings. If a binding is
     * found, it's parent element(s) are examined to find details about the
     * value ie. the column it applies to, the table etc. These details are
     * added to the newly created <binding /> element, and the element is
     * appended to the <bindings /> element of the fragments <query> element.
     *
     * @param DomElement $fragment The object representing the fragment to
     *                             search in.
     *
     * @return DomElement
     */
    private static function _extractBindings(DomElement $fragment)
    {
        $doc   = new DomDocument();
        $query = $doc->importNode($fragment, TRUE);
        // Add the bindings element to the the base query element.
        $bindingsParent = $doc->createElement('bindings');
        $query->appendChild($bindingsParent);
        $systemName = $fragment->getAttribute('system');
        // Get the XML contents, and determine if there are any bindings used.
        $xmlString = $doc->saveXML($query);
        $matches   = array();
        preg_match_all('/:[a-zA-Z\d_]+/i', $xmlString, $matches);
        $bindings      = $matches[0];
        $bindingsCount = count($bindings);
        if ($bindingsCount === 0) {
            // None present.
            return $query;
        }

        // There are some bindings needed in the query, we need to determine
        // where.
        $allElements = $query->getElementsByTagName('*');
        foreach ($allElements as $element) {
            if ($element->childNodes->length === 1) {
                // One child node, see if the nodeValue is a place-holder.
                $nodeValue   = trim($element->nodeValue);
                $bindMatches = array();
                preg_match('/:[a-zA-Z\d_]+/i', $nodeValue, $bindMatches);
                if (empty($bindMatches) === FALSE) {
                    // Check if this is really just a text node with a binding,
                    // or it could still get here if this is a <value></value>
                    // tag. If it is a value tag, we will get it on the next
                    // iteration. Using the firstChild rather than nodeValue
                    // is used here, as if nodeValue will fail this test always.
                    $fullTagValue = $doc->saveXML($element->firstChild);
                    if (substr(trim($fullTagValue), 0, 1) !== ':') {
                        continue;
                    }

                    // Setup variables from the attributes, and create a new
                    // binding element representing this data.
                    $bindColumn     = '';
                    $bindColumnType = '';
                    $bindTable      = '';
                    $bindSystem     = $systemName;
                    $bindName       = $bindMatches[0];

                    $targetNode = NULL;
                    // If this node is a field or another element that doesn't
                    // have a table specified, we need to determine where it
                    // is targeted, so that we can set its' data type.
                    if ($element->hasAttribute('table') === FALSE) {
                        if ($element->parentNode->hasAttribute('table') === TRUE) {
                            $targetNode = $element->parentNode;
                        } else if ($element->tagName === 'value') {
                            // Values only have column specified, so we need to
                            // find the fields node, underneath our parent.
                            $parent    = $element->parentNode->parentNode;
                            $fieldNode = $parent->getElementsByTagName('fields');
                            if ($fieldNode->length !== 0) {
                                $targetNode = $fieldNode->item(0);
                            }
                        }
                    } else {
                        // Element has its' table attribute specified.
                        $targetNode = $element;
                    }

                    $bindColumnType = $element->getAttribute('column_type');

                    if ($targetNode !== NULL) {
                        $bindColumn = $targetNode->getAttribute('column');
                        if ($bindColumn === '') {
                            $bindColumn = $element->getAttribute('column');
                        }

                        $bindTable = $targetNode->getAttribute('table');
                    }

                    // Create the binding element, set its' attributes and
                    // append it to the bindings tag of the fragment.
                    $newBinding = $doc->createElement('binding');
                    $newBinding->setAttribute('table', $bindTable);
                    $newBinding->setAttribute('column', $bindColumn);
                    $newBinding->setAttribute('column_type', $bindColumnType);

                    $newBinding->setAttribute('system', $bindSystem);
                    $newBinding->setAttribute('name', $bindName);
                    $bindingsParent->appendChild($newBinding);
                }//end if
            }//end if
        }//end foreach

        return $query;

    }//end _extractBindings()


    /**
     * Extracts the keywords ([keyword]) from a fragment of a query.
     *
     * Each keyword defined in the XML file as [keyword]. Where keyword can be
     * any string with characters a-Z, A-Z and 0-9. The return value contains
     * the original query, along with new keyword elements attached.
     *
     * @param DomElement $fragment The object representing the fragment to
     *                             search in.
     *
     * @return string
     */
    private static function _extractKeywords(DomElement $fragment)
    {
        $doc   = new DomDocument();
        $query = $doc->importNode($fragment, TRUE);

        // Add the placeholder element to the the base query element.
        $keywordParent = $doc->createElement('keywords');
        $query->appendChild($keywordParent);
        $xmlString = $doc->saveXML($query);
        $matches   = array();

        preg_match_all('/\[([a-zA-Z0-9]+)\]/m', $xmlString, $matches);

        if (isset($matches[1]) === TRUE && empty($matches[1]) === FALSE) {
            foreach ($matches[1] as $keyword) {
                $newKeyword            = $doc->createElement('keyword');
                $newKeyword->nodeValue = $keyword;
                $keywordParent->appendChild($newKeyword);
            }
        }

        return $query;

    }//end _extractKeywords()


    /**
     * Extracts hard-coded values from a query and converts them to bindings.
     *
     * Finds only hard-coded values that exist inside <value> tags, and creates
     * a <placeholder> element under the parent <placeholders> element. This new
     * element contains the value, along with a variable name that is composed
     * by combining the table of the value and its column. This variable name is
     * subsituted into the query in place of the original value. This allows
     * the baked out code for the query, to bind the value to the query
     * at run time.
     *
     * @param DomElement $fragment The object representing the fragment to
     *                             search in.
     *
     * @return DomElement
     */
    private static function _extractPlaceHolders(DomElement $fragment)
    {
        $doc   = new DomDocument();
        $query = $doc->importNode($fragment, TRUE);
        // Add the placeholder element to the the base query element.
        $placeParent = $doc->createElement('placeholders');
        $query->appendChild($placeParent);
        $systemName = $fragment->getAttribute('system');
        $queryId    = $fragment->getAttribute('id');

        $allElements = $query->getElementsByTagName('value');
        foreach ($allElements as $element) {
            if ($element->childNodes->length === 1) {
                // One child node, see if the nodeValue is a place-holder.
                $nodeValue = trim($element->nodeValue);
                if ($nodeValue === '') {
                    continue;
                }

                if ($element->tagName === 'table') {
                    continue;
                }

                if (substr($nodeValue, 0, 1) === ':') {
                    // This is already a binding.
                    continue;
                }

                if (substr($nodeValue, 0, 1) === '[') {
                    // This is a keyword.
                    continue;
                }

                // Setup variables from the attributes, and create a new
                // binding element representing this data.
                $placeColumn = '';
                $placeTable  = '';
                $targetNode  = NULL;
                // If this node is a field or another element that doesn't
                // have a table specified, we need to determine where it
                // is targeted, so that we can set its' data type.
                if ($element->hasAttribute('table') === FALSE) {
                    if ($element->parentNode->hasAttribute('table') === TRUE) {
                        $targetNode = $element->parentNode;
                    } else if ($element->tagName === 'value') {
                        // Values only have column specified, so we need to
                        // find the fields node, underneath our parent.
                        $parent    = $element->parentNode->parentNode;
                        $fieldNode = $parent->getElementsByTagName('fields');
                        if ($fieldNode->length !== 0) {
                            $targetNode = $fieldNode->item(0);
                        }
                    } else if ($element->tagName === 'table') {
                        $placeTable = $nodeValue;
                        $targetNode = $element;
                    }
                } else {
                    // Element has its' table attribute specified.
                    $targetNode = $element;
                }

                if ($targetNode === NULL) {
                    continue;
                }

                if ($placeColumn === '') {
                    if ($targetNode->hasAttribute('column') === TRUE) {
                        $placeColumn = $targetNode->getAttribute('column');
                    } else {
                        // Column not specified, use the query name instead.
                        $placeColumn = $queryId;
                    }
                }

                if ($placeTable === '') {
                    if ($targetNode->hasAttribute('table') === TRUE) {
                        $placeTable = $targetNode->getAttribute('table');
                    } else {
                        // Table not specified, use the system name instead.
                        $placeTable = $systemName;
                    }
                }

                // Create the binding element, set its' attributes and
                // append it to the bindings tag of the fragment.
                $newPlaceHolder = $doc->createElement('placeholder');
                $varName        = $placeTable.' '.$placeColumn;
                $varName        = ucwords(str_replace('_', ' ', $varName));
                $varName        = ':'.str_replace(' ', '', $varName);
                $newPlaceHolder->setAttribute('value', $element->nodeValue);
                $newPlaceHolder->setAttribute('var_name', $varName);
                $placeParent->appendChild($newPlaceHolder);
                // Now replace the original node with the new var name.
                $element->nodeValue = $varName;
            }//end if
        }//end foreach

        return $query;

    }//end _extractPlaceHolders()


    /**
     * Retrieves the PDO data type for a column.
     *
     * Used when printing bindings for queries.
     *
     * @param string $systemId System the column is from.
     * @param string $table    Table the column is in.
     * @param string $column   The column name.
     * @param string $dataType The datatype to bind this value with. Will
     *                         override any other value.
     *
     * @return string
     */
    private static function _getPdoDataType(
        $systemId,
        $table,
        $column,
        $dataType=NULL
    ) {
        if ($dataType === NULL) {
            $dataType = self::getTableColumnTypes($systemId, $table, $column);
        }

        switch (strtoupper($dataType)) {
            case 'NUMERIC':
            case 'DECIMAL':
            case 'INTEGER':
            case 'SMALLINT':
            case 'REAL':
            case 'DOUBLE PRECISION':
            case 'FLOAT':
                $pdoType = 'PDO::PARAM_INT';
            break;

            case 'BOOLEAN':
                $pdoType = 'PDO::PARAM_BOOL';
            break;

            default:
                $pdoType = 'PDO::PARAM_STR';
            break;
        }

        return $pdoType;

    }//end _getPdoDataType()


    /**
     * Converts XML schema to php array.
     *
     * @param DomElement $schema Schema node of a schema.
     *
     * @return array
     */
    public static function parseSchema(DomElement $schema)
    {
        include_once 'DAL/Parsers/DALSchemaParser.inc';
        $parseRes = DALSchemaParser::parse($schema);

        return $parseRes;

    }//end parseSchema()


    /*
        VALIDATE SQL QUERY
    */


    /**
     * Validates all the queries under the given queries node.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $queries The queries node of the queries XML file.
     *
     * @return void
     * @throws DALParserException When a query tag doesn't have a system
     *                            attribute.
     */
    public static function validateQueries(DomElement $queries)
    {
        // Queries tag must have system attribute.
        if ($queries->getAttribute('system') === '') {
            throw new DALParserException('queries tag must have system attr.');
        }

        // Check that each query in this sytem have unique id.
        $ids = array();
        $qs  = $queries->getElementsByTagName('query');
        foreach ($qs as $query) {
            $id = $query->getAttribute('id');
            if (in_array($id, $ids) === TRUE) {
                throw new DALParserException('Query must have a unique id.');
            }

            $ids[] = $id;
        }

    }//end validateQueries()


    /**
     * Returns FALSE if given query id already used by another query.
     *
     * @param DomDocument $doc   DomDocument object.
     * @param DomElement  $query Query element.
     *
     * @return boolean
     */
    public static function queryIdIsUnique(DomDocument $doc, DomElement $query)
    {
        $xp    = new DOMXPath($doc);
        $qry   = '//query[@id = "'.$query->getAttribute('id').'"]';
        $nodes = $xp->query($qry);

        foreach ($nodes as $node) {
            if ($node !== $query) {
                return FALSE;
            }
        }

        return TRUE;

    }//end queryIdIsUnique()


    /**
     * Validates the given query.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     * @throws DALParserException When a query is missing an ID, its' ID is
     *                            not unique, or the query is invalidly
     *                            formatted.
     */
    public static function validateQuery(DomElement $query)
    {
        $doc     = $query->ownerDocument;
        $queryId = $query->getAttribute('id');
        // Query must have id attribute.
        if ($queryId === '') {
            throw new DALParserException('Query must have id attribute.');
        } else if (self::queryIdIsUnique($doc, $query) === FALSE) {
            $msg  = 'Query must have a unique id. Id "'.$queryId;
            $msg .= '" already used by another query.';
            throw new DALParserException($msg);
        }

        $type = self::getQueryType($query);
        $fn   = 'validate'.ucwords($type).'Query';
        if (method_exists('DALBaker', $fn) === FALSE) {
            throw new DALParserException('Invalid validate query. Type:'.$type);
        } else {
            self::$fn($query);
        }

    }//end validateQuery()


    /**
     * Returns the given queries type.
     *
     * @param DomElement $query The query element.
     *
     * @return string
     */
    public static function getQueryType(DomElement $query)
    {
        foreach ($query->childNodes as $child) {
            if ($child->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            return $child->tagName;
        }

    }//end getQueryType()


    /**
     * Returns true if the fragment exists.
     *
     * @param string $system  Id of the system.
     * @param string $queryid Id of the main query.
     * @param string $hookid  Id of the fragment.
     *
     * @return boolean
     */
    public static function fragmentExists($system, $queryid, $hookid)
    {
        // Get hooks XML.
        list($hsysid, $hqid) = explode('.', $hookid);

        $doc = self::loadBakedQuery($hsysid, $hqid);
        if ($doc === NULL) {
            return FALSE;
        }

        $queries = $doc->getElementsByTagName('query');

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === $queryid) {
                if ($query->getAttribute('system') === $system) {
                    return TRUE;
                }
            }
        }

        return FALSE;

    }//end fragmentExists()


    /**
     * Converts given XML query to its SQL format.
     *
     * @param DomElement $xmlQuery Single XML query.
     * @param array      $sqlArray The Array representation of the SQL, if it
     *                             has already been generated.
     *
     * @return string
     */
    public static function convertToSql(
        DomElement $xmlQuery,
        array $sqlArray=array()
    ) {
        if (empty($sqlArray) === TRUE) {
            $sqlArray = self::constructSql($xmlQuery);
        }

        $converter     = self::getConverter();
        $convertResult = $converter->convertToSql($sqlArray['query']);

        return $convertResult;

    }//end convertToSql()


    /**
     * Returns the DAL converter class for a given db type.
     *
     * If no dbType is given then the db type specified in DALConf will be used.
     *
     * @param string $dbType Database Type.
     *
     * @return object
     */
    public static function getConverter($dbType=NULL)
    {
        $converter = DAL::getConverter($dbType);
        return $converter;

    }//end getConverter()


    /**
     * Gets the column types of a specific table in a system.
     *
     * If column argument is NULL then array of all column types will be
     * returned. If column argument is not NULL then a single type will be
     * returned. Returns NULL if table/column not found.
     *
     * @param string $systemid Id of the system.
     * @param string $table    Name of the table.
     * @param string $column   Name of a single column.
     *
     * @return (array|string)
     */
    public static function getTableColumnTypes($systemid, $table, $column=NULL)
    {
        if ($column === NULL) {
            $types = array();
        } else {
            $column = strtolower($column);
            $types  = NULL;
        }

        $doc = self::loadSystemSchemaXML($systemid);
        if ($doc === NULL) {
            return NULL;
        }

        // Find the table that we are looking for.
        $tables = $doc->getElementsByTagName('table');
        foreach ($tables as $currTable) {
            if ($currTable->getAttribute('name') === $table) {
                // Get table columns tag.
                $colsTag = $currTable->getElementsByTagName('columns')->item(0);
                $cols    = $colsTag->getElementsByTagName('column');
                foreach ($cols as $col) {
                    if ($column === strtolower($col->nodeValue)) {
                        return $col->getAttribute('type');
                    } else if ($column === NULL) {
                        $types[strtolower($col->nodeValue)] = $col->getAttribute('type');
                    }
                }

                return $types;
            }
        }//end foreach

        return NULL;

    }//end getTableColumnTypes()


    /**
     * Gets the different column types (if any) from all tables in a system.
     *
     * Only used when table name is dynamic, i.e. [tableName]. Returns empty
     * array if bind types for all table column are of the same type.
     * For example, the ConfigurationDataSource system will get
     *              array (
     *               'value' => array (
     *                           'configuration_array'   => PDO::PARAM_STR,
     *                           'configuration_boolean' => PDO::PARAM_BOOL,
     *                          ),
     *              );
     *
     * @param string $systemid Id of the system.
     *
     * @return array
     */
    private static function _getDynamicTableColumnTypes($systemid)
    {
        if (isset(self::$_dynamicTableColDiff[$systemid]) === TRUE) {
            return self::$_dynamicTableColDiff[$systemid];
        }

        $result = array();
        $doc    = self::loadSystemSchemaXML($systemid);
        if ($doc === NULL) {
            return $result;
        }

        // Work out all column types for every table in this sytem.
        $tables = $doc->getElementsByTagName('table');
        foreach ($tables as $table) {
            $name    = strtolower($table->getAttribute('name'));
            $colsTag = $table->getElementsByTagName('columns')->item(0);
            $cols    = $colsTag->getElementsByTagName('column');
            foreach ($cols as $col) {
                $colName   = strtolower($col->nodeValue);
                $colType   = $col->getAttribute('type');
                $paramType = self::_getPdoDataType('', '', '', $colType);
                eval('$type = '.$paramType.';');
                $result[$colName][$name] = $type;
            }
        }

        // Filter out columns with consistent PDO type.
        foreach ($result as $col => $info) {
            $count = count(array_unique($info));
            if ($count === 1) {
                unset($result[$col]);
            }
        }

        // Cache the result for this system.
        self::$_dynamicTableColDiff[$systemid] = $result;
        return $result;

    }//end _getDynamicTableColumnTypes()


    /**
     * Returns query in its XML format.
     *
     * DomElement of the query in queries.xml file is returned. Null if query
     * was not found.
     *
     * @param string $systemid System name that the query belongs to.
     * @param string $queryid  Id of the query.
     *
     * @return object
     */
    public static function getQuerySource($systemid, $queryid)
    {
        $queriesXML = self::loadQueriesXML($systemid);

        if ($queriesXML !== NULL) {
            // Get all query tags and check their id attr.
            $queries = $queriesXML->getElementsByTagName('query');
            foreach ($queries as $query) {
                if ($query->getAttribute('id') === $queryid) {
                    return $query;
                }
            }
        }

        return NULL;

    }//end getQuerySource()


    /**
     * Creates systems DAL oven directory.
     *
     * Returns true on success, false if directory already exists, and throws
     * ChannelExeption if it cannot create the directory.
     *
     * @param string $system System name.
     *
     * @return boolean
     * @throws ChannelException When the path cannot be created.
     */
    public static function createSystemOvenPath($system)
    {
        $ovenPath = DAL::getOvenPath($system);
        if (file_exists($ovenPath) === TRUE) {
            return FALSE;
        } else {
            if (mkdir($ovenPath, 0775, TRUE) === FALSE) {
                $msg = 'Could not create directory: '.$ovenPath;
                throw new ChannelException($msg);
            }
        }

        return TRUE;

    }//end createSystemOvenPath()


    /**
     * Returns the DomDocument object for the system's queries XML.
     *
     * Returns NULL if queries.xml cannot be loaded.
     *
     * @param string $systemid System name.
     *
     * @return object
     */
    public static function loadQueriesXML($systemid)
    {
        $fileName = Channels::getSystemsPath($systemid).'/DB/queries.xml';
        if (file_exists($fileName) === FALSE) {
            return NULL;
        }

        $doc = new DOMDocument();
        $doc->load($fileName);

        return $doc;

    }//end loadQueriesXML()


    /**
     * Returns the file name of the baked query file.
     *
     * @param string $systemid Id of the system.
     * @param string $queryid  Id of the query.
     *
     * @return string
     */
    public static function getBakedQueryFileName($systemid, $queryid)
    {
        $fileName = DAL::getOvenPath($systemid).'/'.$queryid.'.xml';
        return $fileName;

    }//end getBakedQueryFileName()


    /**
     * Returns the DomElement of the baked query file.
     *
     * @param string $systemid Id of the system.
     * @param string $queryid  Id of the query.
     *
     * @return object
     */
    public static function loadBakedQuery($systemid, $queryid)
    {
        $fileName = self::getBakedQueryFileName($systemid, $queryid);
        if (file_exists($fileName) === FALSE) {
            return NULL;
        }

        $doc = new DOMDocument();
        $doc->load($fileName);

        return $doc;

    }//end loadBakedQuery()


    /**
     * Loads the System's schema XML file.
     *
     * Returns NULL if schema file could not be loaded.
     *
     * @param string $systemid Id of the system.
     *
     * @return object
     */
    public static function loadSystemSchemaXML($systemid)
    {
        $fileName = Channels::getSystemsPath($systemid).'/DB/schema.xml';
        if (file_exists($fileName) === FALSE) {
            return NULL;
        }

        $doc = new DOMDocument();
        $doc->load($fileName);

        return $doc;

    }//end loadSystemSchemaXML()


    /*
        XQD to PHP Query Conversions
    */


    /**
     * Constructs an SQL query from DomElement.
     *
     * This is the main function where the SQL array is constructed from XML.
     * It will combine queries from different parts of the system.
     *
     * @param DomElement $query DomElement that represents a single query.
     *
     * @return array
     */
    public static function constructSql(DomElement $query)
    {
        $sql = array();

        // Require parser..
        include_once dirname(__FILE__).'/Parsers/DALQueryParser.inc';

        // Parse.
        $sql = DALQueryParser::parse($query);

        return $sql;

    }//end constructSql()


    /**
     * Returns a prefix for the hooks.
     *
     * Baker will later on look for this prefix in SQL query string and replace
     * them with other SQL queries.
     *
     * @return string
     */
    public static function getHookPrefix()
    {
        $prefix = 'HOOKID:';
        return $prefix;

    }//end getHookPrefix()


    /**
     * Returns list of allowed where clause conditions.
     *
     * @param string $type Type of the condition. It will return its sign.
     *
     * @return array
     */
    public static function getComparisonOperators($type=NULL)
    {
        if ($type !== NULL) {
            if (isset(self::$_whereConditions[$type]) === TRUE) {
                return self::$_whereConditions[$type];
            } else {
                return '';
            }
        }

        return self::$_whereConditions;

    }//end getComparisonOperators()


    /**
     * Parses the connection string and returns the database type.
     *
     * @param string $dsn The string to parse. If NULL, the DSN setting of the
     *                    system will be used.
     *
     * @return string
     */
    public static function getDatabaseType($dsn=NULL)
    {
        if ($dsn === NULL) {
            $dsn        = DAL::getDsn();
            $connectStr = $dsn['DSN'];
        } else {
            $connectStr = $dsn;
        }

        $dbType = substr($connectStr, 0, strpos($connectStr, ':'));
        switch ($dbType) {
            case 'pgsql':
                $fullType = 'postgres';
            break;

            case 'oci':
                $fullType = 'oracle';
            break;

            case 'odbc':
                $fullType = 'db2';
            break;

            case 'dblib':
                $fullType = 'mssql';
            break;

            case 'mysql':
                $fullType = $dbType;
            break;

            default:
                $fullType = '';
            break;
        }//end switch

        return $fullType;

    }//end getDatabaseType()


}//end class

?>
