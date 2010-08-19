<?php
/**
 * DALAlterParser.
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
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALCaseParser.
 *
 * INSERT query parser.
 */
class DALCaseParser extends DALQueryParser
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
     * Construct the CASE query from XML element.
     *
     * @param DomElement $xmlQuery The query element.
     * @param boolean    $noParent If the query element is the start of the query.
     *
     * @return array
     * @throws Exception When parsing error occurs.
     */
    public static function parse(DomElement $xmlQuery, $noParent=FALSE)
    {
        $query   = array();
        $caseTag = NULL;
        if ($noParent === TRUE) {
            $caseTag = $xmlQuery;
        } else {
            $cases = $xmlQuery->getElementsByTagName('case');
            for ($i=0; $i<$cases->length; $i++) {
                if ($cases->item($i)->parentNode->isSameNode($xmlQuery) === TRUE) {
                    // We are looking for the first case directly underneath this query.
                    $caseTag = $cases->item($i);
                    break;
                }
            }
        }

        if ($caseTag !== NULL) {
            $alias = $xmlQuery->getAttribute('alias');
            $query['CASE']  = array(
                               'CONDITIONS' => array(),
                               'ALIAS'      => $xmlQuery->getAttribute('alias'),
                              );

            $index      = 0;
            $expectWhen = TRUE;
            $expectThen = FALSE;
            for ($i = 0; $i < $caseTag->childNodes->length; $i++) {
                $node = $caseTag->childNodes->item($i);
                if ($node !== NULL && $node->nodeType === XML_ELEMENT_NODE) {
                    if ($expectThen === TRUE && $node->nodeName !== 'then') {
                        throw new Exception('Expecting then');
                    }

                    if ($expectWhen === TRUE
                        && ($node->nodeName !== 'when' && $node->nodeName !== 'else')
                    ) {
                        throw new Exception('Expecting when or else');
                    }

                    $innerNode = NULL;
                    foreach ($node->childNodes as $child) {
                        if ($child->nodeType !== XML_ELEMENT_NODE) {
                            continue;
                        } else {
                            $innerNode = $child;
                            break;
                        }
                    }

                    if ($node->nodeName === 'when') {
                        $expectThen = TRUE;
                        $expectWhen = FALSE;
                        $query['CASE']['CONDITIONS'][] = array(
                                                          'WHEN' => array(),
                                                          'THEN' => array(),
                                                         );

                        if ($innerNode->nodeName === 'select') {
                            $query['CASE']['CONDITIONS'][$index]['WHEN'] = DALSelectParser::parse($innerNode);
                        } else if ($innerNode->nodeName === 'condition') {
                            $query['CASE']['CONDITIONS'][$index]['WHEN'] = DALWhereParser::getWhereConditions($innerNode);
                        } else {
                            $query['CASE']['CONDITIONS'][$index]['WHEN'] = DALWhereParser::getWhereConditions($innerNode->parentNode);
                        }
                    } else if ($node->nodeName === 'then') {
                        $expectThen = FALSE;
                        $expectWhen = TRUE;
                        $query['CASE']['CONDITIONS'][$index]['THEN'] = array();

                        if ($innerNode->nodeName === 'select') {
                            $query['CASE']['CONDITIONS'][$index]['THEN'] = DALSelectParser::parse($innerNode);
                        } else {
                            $query['CASE']['CONDITIONS'][$index]['THEN'] = DALQueryParser::parseSingleField($innerNode);
                        }

                        $index++;
                    } else if ($node->nodeName === 'else') {
                        $query['CASE']['ELSE'] = array();
                        if ($innerNode->nodeName === 'select') {
                            $query['CASE']['ELSE'] = DALSelectParser::parse($innerNode);
                        } else {
                            $query['CASE']['ELSE'] = DALQueryParser::parseSingleField($innerNode);
                        }
                    }//end if
                }//end if
            }//end for
        }//end if

        return $query;

    }//end parse()


    /**
     * Validates INSERT query.
     *
     * Throws DALParserException. Note: This validation checks required XML
     * elements and attributes. It does not check if tables, columns etc
     * exists in the system.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     * @throws DALParserException If the INSERT tag is malformed.
     */
    public static function validate(DomElement $query)
    {

    }//end validate()


}//end class

?>
