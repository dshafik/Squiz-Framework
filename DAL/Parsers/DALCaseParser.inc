<?php
/**
 * DALAlterParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/Parsers/DALSelectParser.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALCaseParser.
 *
 * INSERT query parser.
 *
 * @since 4.0.0
 */
class DALCaseParser extends DALQueryParser
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
     * Construct the INSERT query from XML element.
     *
     * @param DomElement $xmlQuery The query element.
     *
     * @since  4.0.0
     * @return array
     * @throws Exception When parsing error occurs.
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query   = array();
        $caseTag = $xmlQuery->getElementsByTagName('case')->item(0);

        if ($caseTag !== NULL) {
            $alias = $xmlQuery->getAttribute('alias');
            $query['ALIAS'] = $xmlQuery->getAttribute('alias');
            $query['CASE']  = array(
                               'CONDITIONS' => array(),
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
                            $query['CASE']['CONDITIONS'][$index]['THEN'] = DALWhereParser::getWhereConditions($innerNode->parentNode);
                        }

                        $index++;
                    } else if ($node->nodeName === 'else') {
                        $query['ELSE'] = array();
                        if ($innerNode->nodeName === 'select') {
                            $query['ELSE'] = DALSelectParser::parse($innerNode);
                        } else {
                            $query['ELSE'] = DALWhereParser::getWhereConditions(
                                $innerNode->parentNode
                            );
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
     * @since  4.0.0
     * @return void
     * @throws DALParserException If the INSERT tag is malformed.
     */
    public static function validate(DomElement $query)
    {

    }//end validate()


}//end class

?>
