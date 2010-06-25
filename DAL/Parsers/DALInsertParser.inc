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
require_once 'Libs/XML/XML.inc';

/**
 * DALInsertParser.
 *
 * INSERT query parser.
 *
 * @since 4.0.0
 */
class DALInsertParser extends DALQueryParser
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
     * Array(
     *  'into'   => string,
     *  'FIELDS' => array(
     *               0 => string,
     *              ),
     *  'VALUES' => array(
     *               'column' => mixed,
     *              ),
     * )
     *
     * @param DomElement $xmlQuery The query element.
     *
     * @since  4.0.0
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query     = array();
        $insertTag = $xmlQuery->getElementsByTagName('insert')->item(0);
        if ($insertTag !== NULL) {
            $query['INSERT'] = array();

            // Insert fields.
            $fieldsTag = $insertTag->getElementsByTagName('fields')->item(0);
            if ($fieldsTag !== NULL) {
                $query['INSERT']['into']   = $fieldsTag->getAttribute('table');
                $query['INSERT']['FIELDS'] = array();

                $fields = $fieldsTag->getElementsByTagName('field');
                foreach ($fields as $field) {
                    $query['INSERT']['FIELDS'][] = $field->nodeValue;
                }
            }

            // Check if we have select or values.
            $select = $insertTag->getElementsByTagName('select')->item(0);
            if ($select === NULL
                || ($select->parentNode->tagName !== 'insert'
                && $select->parentNode->tagName !== 'values')
            ) {
                $query['INSERT']['VALUES'] = array();
                // Must be using values.
                $valuesTag = $insertTag->getElementsByTagName('values')->item(0);
                foreach ($valuesTag->childNodes as $value) {
                    if ($value->nodeType === XML_ELEMENT_NODE) {
                        if ($value->tagName === 'value') {
                            $vCol = $value->getAttribute('column');
                            if (XML::hasChildElements($value) === TRUE) {
                                $colVal = self::parseSingleField(
                                    XML::getFirstChildElement($value)
                                );
                            } else {
                                $colVal = $value->nodeValue;
                            }

                            $query['INSERT']['VALUES'][$vCol] = $colVal;
                        }
                    }
                }
            } else {
                // Inner Select.
                 $query['INSERT']['VALUES'] = DALSelectParser::parse($select);
            }//end if
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
        $insertTag = $query->getElementsByTagName('insert')->item(0);

        if ($insertTag === NULL) {
            throw new DALParserException('insert tag not found.');
        } else {
            $fields = $insertTag->getElementsByTagName('fields')->item(0);

            if ($fields === NULL) {
                $msg = 'Insert query must have fields tag.';
                throw new DALParserException($msg);
            } else if ($fields->parentNode->tagName === 'insert') {
                if ($fields->getAttribute('table') === '') {
                    $msg = 'Fields tag must have table attribute.';
                    throw new DALParserException($msg);
                }

                // Must have select or values tag.
                $tags    = 0;
                $valsTag = $insertTag->getElementsByTagName('values')->item(0);
                if ($valsTag !== NULL) {
                    self::validateFieldsTag($fields);
                    if ($valsTag->parentNode->tagName === 'insert') {
                        $tags++;
                        // Values tag must have value tags.
                        self::validateValuesTag($valsTag);
                    }
                }

                $selTag = $insertTag->getElementsByTagName('select')->item(0);
                if ($selTag !== NULL) {
                    if ($selTag->parentNode->tagName === 'insert') {
                        $tags++;
                        // Validate the select query.
                        DALSelectParser::validate($insertTag);
                    }
                }

                if ($tags === 0) {
                    $msg = 'Insert query must have values or select tag.';
                    throw new DALParserException($msg);
                }
            } else {
                // This is not in insert tag.
                $msg = 'Inser query must have fields tag.';
                throw new DALParserException($msg);
            }//end if
        }//end if

    }//end validate()


}//end class

?>
