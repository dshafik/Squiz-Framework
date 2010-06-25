<?php
/**
 * DALFromParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/Parsers/DALUnionParser.inc';
require_once 'DAL/Parsers/DALSelectParser.inc';
require_once 'DAL/DALBaker.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALFromParser.
 *
 * From clause parser. Parses from SQL clause.
 *
 * @since 4.0.0
 */
class DALFromParser extends DALQueryParser
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
     * Constructs the SQL's FROM clause from XML node.
     *
     * Returns the array for the FROM clause.
     * array(
     *  'FROM' => array(
     *             0 => string,
     *            ),
     * )
     *
     * @param DomElement $fromTag Select Node of the XML.
     *
     * @since  4.0.0
     * @return array
     */
    public static function parse(DomElement $fromTag)
    {
        if ($fromTag->tagName !== 'from') {
            foreach ($fromTag->childNodes as $childNode) {
                if ($childNode->nodeType === XML_ELEMENT_NODE
                    && $childNode->tagName === 'from'
                ) {
                    $fromTag = $childNode;
                    break;
                }
            }
        }

        $from = array();
        if ($fromTag !== NULL) {
            $from['FROM'] = array();

            // Get union/union-all if there is one, else get tables.
            $unionTag = self::getUnionTag($fromTag);
            if ($unionTag !== NULL) {
                $from['FROM'] = DALUnionParser::parse($fromTag);
            } else {
                $tables = XML::getElementsByTagNames(
                    array(
                     'table',
                     'hook',
                    ),
                    $fromTag
                );

                foreach ($fromTag->childNodes as $table) {
                    if ($table->nodeType !== XML_ELEMENT_NODE) {
                        continue;
                    }

                    // Make sure this is the first level.
                    if ($table->tagName === 'hook') {
                        $hook = DALBaker::getHookPrefix().$table->getAttribute('id');
                        // Add the hook to the 'FROM' array.
                        $from['FROM'][] = $hook;
                    } else if ($table->tagName === 'table') {
                        // Could have a select statement.
                        $tableInfo = array();
                        $tblSelect = $table->getElementsByTagName('select')->item(0);
                        if ($tblSelect !== NULL) {
                            $tableInfo['name'] = DALSelectParser::parse($tblSelect);
                        } else {
                            $tableInfo['name'] = $table->nodeValue;
                        }

                        // Check if we have alias.
                        if ($table->getAttribute('alias') !== '') {
                            $tableInfo['alias'] = $table->getAttribute('alias');
                        }

                        if (isset($tableInfo['alias']) === TRUE) {
                            $from['FROM'][] = $tableInfo;
                        } else {
                            $from['FROM'][] = $tableInfo['name'];
                        }
                    } else if ($table->tagName === 'function') {
                        self::validateFunctionTag($table);
                        $from['FROM'][] = self::parseSqlFunction($table);
                    }//end if
                }//end foreach
            }//end if
        }//end if

        if (isset($from['FROM']) === TRUE && empty($from['FROM']) === TRUE) {
            $from = array();
        }

        return $from;

    }//end parse()


}//end class

?>