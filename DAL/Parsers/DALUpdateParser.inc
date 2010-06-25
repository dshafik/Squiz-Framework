<?php
/**
 * DALUpdateParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALFromParser.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALUpdateParser.
 *
 * Update query parser. Parses Update query.
 *
 * @since 4.0.0
 */
class DALUpdateParser extends DALQueryParser
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
     * @param DomElement $xmlQuery Select Node of the XML.
     *
     * @since  4.0.0
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query = array();

        $updateTag = $xmlQuery->getElementsByTagName('update')->item(0);
        if ($updateTag !== NULL) {
            $query['UPDATE'] = array();

            // Insert fields.
            $fieldsTag = $updateTag->getElementsByTagName('fields')->item(0);
            if ($fieldsTag !== NULL) {
                $query['UPDATE']['table'] = $fieldsTag->getAttribute('table');

                $qfields = array();
                $fields  = $fieldsTag->getElementsByTagName('field');
                foreach ($fields as $field) {
                    $qfields[] = $field->nodeValue;
                }
            }

            $query['UPDATE']['FIELDS'] = $qfields;

            // Get fields.
            $valuesTag = $updateTag->getElementsByTagName('values')->item(0);

            // Add values.
            foreach ($valuesTag->childNodes as $value) {
                if ($value->nodeType === XML_ELEMENT_NODE) {
                    $vCol = $value->getAttribute('column');
                    if (XML::hasChildElements($value) === TRUE) {
                        $qvals[$vCol] = self::parseSingleField(
                            XML::getFirstChildElement($value)
                        );
                    } else {
                        $qvals[$vCol] = $value->nodeValue;
                    }
                }
            }

            $query['UPDATE']['VALUES'] = $qvals;

            // Get from clause if there is any.
            $from            = DALFromParser::parse($updateTag);
            $query['UPDATE'] = array_merge($query['UPDATE'], $from);

            // Get where clause if there is any.
            $where           = DALWhereParser::parse($updateTag);
            $query['UPDATE'] = array_merge($query['UPDATE'], $where);
        }//end if

        return $query;

    }//end parse()


    /**
     * Validates UPDATE query.
     *
     * Throws DALParserException. Note: This validation checks required XML
     * elements and attributes. It does not check if tables, columns etc
     * exists in the system.
     *
     * @param DomElement $query The query element.
     *
     * @since  4.0.0
     * @return void
     * @throws DALParserException If the UPDATE tag doesn't exist, contains no
     *                            no fields, or has no values.
     */
    public static function validate(DomElement $query)
    {
        $updateTag = $query->getElementsByTagName('update')->item(0);
        if ($updateTag === NULL) {
            throw new DALParserException('Update tag not found.');
        }

        // Update tag must have fields tag.
        $fieldsTag = $updateTag->getElementsByTagName('fields');
        if ($fieldsTag->length !== 0) {
            $fieldsTag     = $fieldsTag->item(0);
            $parentTagName = $fieldsTag->parentNode->tagName;
        } else {
            $fieldsTag = NULL;
        }

        if (($fieldsTag === NULL) || ($parentTagName !== 'update')) {
            $msg = 'Update query must have fields tag.';
            throw new DALParserException($msg);
        }

        self::validateFieldsTag($fieldsTag);

        $valuesTag = $updateTag->getElementsByTagName('values')->item(0);
        if ($valuesTag === NULL) {
            $msg = 'Update query must have values tag.';
            throw new DALParserException($msg);
        }

        self::validateValuesTag($valuesTag);

        $whereTag = $updateTag->getElementsByTagName('where')->item(0);
        if ($whereTag !== NULL) {
            DALWhereParser::validate($whereTag);
        }

    }//end validate()


}//end class

?>