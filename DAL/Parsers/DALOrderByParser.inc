<?php
/**
 * DALOrderByParser.
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
 * DALOrderByParser.
 *
 * ORDER BY clause parser. Parses ORDER BY clause.
 *
 */
class DALOrderByParser extends DALQueryParser
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
     * Get the SQL's ORDER BY clause from XML node.
     *
     * Returns the array for the ORDER BY clause.
     * array(
     *  'ORDER-BY' => array(
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
        $query    = array();
        $orderBys = $parent->getElementsByTagName('order-by');
        for ($i = 0; $i < $orderBys->length; $i++) {
            $orderBy = $orderBys->item($i);
            if ($orderBy !== NULL && $orderBy->parentNode === $parent) {
                $query['ORDER-BY'][$i] = self::parseChildFields($orderBy);
                if ($orderBy->getAttribute('direction') !== '') {
                    $query['ORDER-BY'][$i]['direction'] = $orderBy->getAttribute('direction');
                }
            }
        }

        return $query;

    }//end parse()


    /**
     * Validates ORDER BY clause.
     *
     * ORDER BY clause must have at least one field or hook.
     *
     * @param DomElement $select Select query.
     *
     * @return void
     * @throws DALParserException If the order-by tag has no fields or hooks.
     */
    public static function validate(DomElement $select)
    {
        // Check the first order-by (could be in a sub query).
        $orderBy = $select->getElementsByTagName('order-by')->item(0);
        if ($orderBy !== NULL) {
            $tagNames = array(
                         'field',
                         'hook',
                         'alias',
                        );

            $tags = XML::getElementsByTagNames($tagNames, $orderBy);
            if (count($tags) === 0) {
                $msg = 'order-by tag must have at least one field or hook.';
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