<?php
/**
 * DALUpdateParser.
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
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALFromParser.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALUpdateParser.
 *
 * Update query parser. Parses Update query.
 */
class DALUpdateParser extends DALQueryParser
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