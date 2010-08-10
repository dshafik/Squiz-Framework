<?php
/**
 * DALInsertParser.
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
require_once 'DAL/DALBaker.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';

/**
 * DALInsertParser.
 *
 * INSERT query parser.
 *
 */
class DALDeleteParser extends DALQueryParser
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
     * Construct the DELETE query from XML element.
     *
     * Array(
     *  'DELETE' => array(
     *               'from'  => string,
     *               'WHERE' => array(),
     *              ),
     * )
     *
     * @param DomElement $xmlQuery The query element.
     *
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query     = array();
        $deleteTag = $xmlQuery->getElementsByTagName('delete')->item(0);

        if ($deleteTag !== NULL) {
            $query['DELETE'] = array();
            $deleteFrom      = '';

            // There must be a where tag and should have been checked before.
            $whereTag = $deleteTag->getElementsByTagName('where')->item(0);

            $i      = 0;
            $length = $whereTag->childNodes->length;
            // Get the first field with table attr to retrieve the table name.
            while ($i < $length) {
                $cond = $whereTag->childNodes->item($i);
                $i++;

                if ($cond->nodeType !== XML_ELEMENT_NODE) {
                    continue;
                }

                if (DALBaker::getComparisonOperators($cond->tagName) !== ''
                    || $cond->tagName === 'in' || $cond->tagName === 'not-in'
                ) {
                    $deleteFrom = $cond->getAttribute('table');
                    break;
                } else {
                    $whereTag = $cond;
                    $length   = $whereTag->childNodes->length;
                    $i        = 0;
                }
            }//end while

            if (empty($deleteFrom) === TRUE
                && $deleteTag->hasAttribute('from') === TRUE
            ) {
                $deleteFrom = $deleteTag->getAttribute('from');
            }

            $query['DELETE']['from'] = $deleteFrom;

            // Where condition.
            $whereCond       = DALWhereParser::parse($deleteTag);
            $query['DELETE'] = array_merge($query['DELETE'], $whereCond);
        }//end if

        return $query;

    }//end parse()


    /**
     * Validates DELETE query.
     *
     * Throws DALParserException. Note: This validation checks required XML
     * elements and attributes. It does not check if tables, columns etc
     * exists in the system.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     * @throws DALParserException If the DELETE query is malformed.
     */
    public static function validate(DomElement $query)
    {
        $deleteTag = $query->getElementsByTagName('delete')->item(0);
        if ($deleteTag === NULL) {
            throw new DALParserException('Delete tag not found.');
        }

        // Must have where tag.
        $whereTag = $deleteTag->getElementsByTagName('where')->item(0);
        if ($whereTag === NULL) {
            $msg = 'Delete query must have a where tag.';
            throw new DALParserException($msg);
        } else {
            DALWhereParser::validate($whereTag);
        }

    }//end validate()


}//end class

?>