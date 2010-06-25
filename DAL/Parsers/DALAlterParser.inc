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
require_once 'DAL/Parsers/DALSchemaParser.inc';

/**
 * DALAlterParser.
 *
 * ALTER TABLE query parser.
 *
 * @since 4.0.0
 */
class DALAlterParser extends DALQueryParser
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
     * Parses the ALTER TABLE query.
     *
     * @param DomElement $xmlQuery The ALTER TABLE query.
     *
     * @since  4.0.0
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query    = array();
        $alterTag = $xmlQuery->getElementsByTagName('alter')->item(0);
        if ($alterTag !== NULL) {
            $query['ALTER'] = array();
            // Alter table.
            $query['ALTER']['table'] = $alterTag->getAttribute('table');

            // Get ALTER TABLE query info.
            foreach ($alterTag->childNodes as $alter) {
                if ($alter->nodeType !== XML_ELEMENT_NODE) {
                    continue;
                }

                $info = NULL;
                switch ($alter->tagName) {
                    case 'add-column':
                    case 'modify-column':
                        $info            = array();
                        $info['COLUMNS'] = DALSchemaParser::getTableColumns($alter);
                    break;

                    case 'drop-column':
                        $info = $alter->nodeValue;
                    break;

                    case 'rename-column':
                        $info        = array();
                        $info['OLD'] = $alter->getAttribute('old');
                        $info['NEW'] = $alter->nodeValue;
                    break;

                    case 'rename-table':
                        $info        = array();
                        $info['OLD'] = $alter->getAttribute('old');
                        $info['NEW'] = $alter->nodeValue;
                    break;

                    case 'add-constraint':
                        $info = DALSchemaParser::getConstraintsFromParent($alter);
                    break;

                    case 'drop-constraint':
                        $info = $alter->nodeValue;
                    break;

                    default:
                        // Invalid tag.
                    break;
                }//end switch

                $query['ALTER'][strtoupper($alter->tagName)] = $info;
            }//end foreach
        }//end if

        return $query;

    }//end parse()


    /**
     * Validates single ALTER TABLE query.
     *
     * Throws DALParserException if:
     * - Alter tag does not have table attribute or children.
     * - add-column and modify-column tag has invalid columns.
     * - rename-column tag does not have value or column attribute.
     * - add-constraint tag has invalid constraints.
     * - drop-constraint tag does not have a value
     *
     * @param DomElement $query Alter table element.
     *
     * @since  4.0.0
     * @return void
     * @throws DALParserException If the ALTER query is malformed.
     */
    public static function validate(DomElement $query)
    {
        // Alter tag must have table attribute.
        $alterTag = $query->getElementsByTagName('alter')->item(0);
        if ($alterTag === NULL) {
            throw new DALParserException('Alter tag not found.');
        }

        if ($alterTag->getAttribute('table') === '') {
            $msg = 'Alter tag must have table attribute.';
            throw new DALParserException($msg);
        }

        $emptyTag = TRUE;
        foreach ($alterTag->childNodes as $alter) {
            if ($alter->nodeType === XML_ELEMENT_NODE) {
                $emptyTag = FALSE;
                $info     = NULL;
                switch ($alter->tagName) {
                    case 'add-column':
                    case 'modify-column':
                        DALSchemaParser::validateTableColumns($alter);
                    break;

                    case 'drop-column':
                        if ($alter->nodeValue === '') {
                            $msg = 'drop-column tag must have a value.';
                            throw new DALParserException($msg);
                        }
                    break;

                    case 'rename-column':
                        if ($alter->nodeValue === '') {
                            $msg = 'rename-column tag must have a value.';
                            throw new DALParserException($msg);
                        }

                        if ($alter->getAttribute('old') === '') {
                            $msg = 'rename-column tag must have a old attr.';
                            throw new DALParserException($msg);
                        }
                    break;

                    case 'rename-table':
                        if ($alter->nodeValue === '') {
                            $msg = 'rename-table tag must have a value.';
                            throw new DALParserException($msg);
                        }

                        if ($alter->getAttribute('old') === '') {
                            $msg = 'rename-table tag must have a old attr.';
                            throw new DALParserException($msg);
                        }
                    break;

                    case 'add-constraint':
                        DALSchemaParser::validateConstraints($alter);
                    break;

                    case 'drop-constraint':
                        if ($alter->nodeValue === '') {
                            $msg = 'drop-constraint tag must have a value.';
                            throw new DALParserException($msg);
                        }
                    break;

                    default:
                        $msg = 'Invalid ALTER TABLE tag: '.$alter->tagName;
                        throw new DALParserException($msg);
                    break;
                }//end switch
            }//end if
        }//end foreach

        if ($emptyTag === TRUE) {
            $msg = 'Empty alter tag found.';
            throw new DALParserException($msg);
        }

    }//end validate()


}//end class

?>
