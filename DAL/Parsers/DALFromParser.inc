<?php
/**
 * DALFromParser.
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
require_once 'DAL/Parsers/DALUnionParser.inc';
require_once 'DAL/Parsers/DALSelectParser.inc';
require_once 'DAL/DALBaker.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALFromParser.
 *
 * From clause parser. Parses from SQL clause.
 *
 */
class DALFromParser extends DALQueryParser
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