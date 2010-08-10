<?php
/**
 * DALGroupBy.
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
require_once 'Libs/XML/XML.inc';

/**
 * DALGroupBy.
 *
 * GROUP BY clause parser. Parses GROUP BY clause.
 *
 */
class DALGroupByParser extends DALQueryParser
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
     * Get the SQL's GROUP BY clause from XML node.
     *
     * Returns the array for the ORDER BY clause.
     * array(
     *  'GROUP-BY' => array(
     *                 0 => array(
     *                       'table'  => string,
     *                       'column' => string,
     *                      ),
     *                ),
     * )
     *
     * @param DomElement $parent Parent node.
     *
     * @return array
     */
    public static function parse(DomElement $parent)
    {
        $query         = array();
        $groupBy       = $parent->getElementsByTagName('group-by')->item(0);
        $selectClauses = $parent->getElementsByTagName('select');
        if ($parent->hasChildNodes() === TRUE) {
            foreach ($parent->childNodes as $childNode) {
                if ($childNode->nodeType !== XML_ELEMENT_NODE) {
                    continue;
                }//end if

                if ($childNode->tagName === 'group-by') {
                    $groupBy = $childNode;
                    if ($groupBy !== NULL) {
                        if ($groupBy->hasChildNodes() === TRUE) {
                            $query['GROUP-BY'] = self::parseChildFields($groupBy);
                        }//end if
                    }//end if
                }
            }//end foreach
        }//end if

        return $query;

    }//end parse()


    /**
     * Validates GROUP BY clause.
     *
     * GROUP BY clause must have at least one field or hook.
     *
     * @param DomElement $select Select query.
     *
     * @return void
     * @throws DALParserException When the group-by tag doesn't have any fields
     *                            or hooks.
     */
    public static function validate(DomElement $select)
    {
        // Check the first group-by (could be in a sub query).
        $groupBy = $select->getElementsByTagName('group-by')->item(0);
        if ($groupBy !== NULL) {
            $tagNames = array(
                         'field',
                         'hook',
                         'alias',
                        );

            $tags = XML::getElementsByTagNames($tagNames, $groupBy);
            if (count($tags) === 0) {
                $msg = 'group-by tag must have at least one field or hook.';
                throw new DALParserException($msg);
            } else {
                foreach ($tags as $tag) {
                    if ($tag->tagName === 'field') {
                        self::validateField($tag);
                    } else if ($tag->tagName === 'hook') {
                        self::validateHook($tag);
                    }
                }
            }
        }//end if

    }//end validate()


}//end class

?>
