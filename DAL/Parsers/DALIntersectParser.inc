<?php
/**
 * DALIntersectParser.
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
require_once 'DAL/DALBaker.inc';

/**
 * DALIntersectParser.
 *
 * INTERSECT query parser.
 *
 */
class DALIntersectParser extends DALQueryParser
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
     * Constructs the INTERSECT clause/query.
     *
     * Array(
     *  'INTERSECT' => array(
     *              'alias'   => string,
     *              'SELECTS' => array(
     *                            0 => array(),
     *             ),
     * )
     *
     * @param DomElement $xmlQuery Query element.
     *
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query = array();
        if (strtolower($xmlQuery->tagName) !== 'intersect') {
            $intersectTag = $xmlQuery->getElementsByTagName('intersect')->item(0);
        } else {
            $intersectTag = $xmlQuery;
        }

        if ($intersectTag === NULL) {
            return $query;
        }

        // INTERSECT.
        $tag         = strtoupper($intersectTag->tagName);
        $query[$tag] = array();

        // Get the alias if there is one.
        if ($intersectTag->getAttribute('alias') !== '') {
            $query[$tag]['alias'] = $intersectTag->getAttribute('alias');
        }

        $query[$tag]['SELECTS'] = array();

        // Get all select Tags inside intersect clause.
        foreach ($intersectTag->childNodes as $node) {
            if ($node->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            if ($node->tagName === 'select') {
                $query[$tag]['SELECTS'][] = DALSelectParser::parse($node);
            } else if ($node->tagName === 'hook') {
                $hookid = DALBaker::getHookPrefix().$node->getAttribute('id');
                $query[$tag]['SELECTS'][] = $hookid;
            }
        }

        return $query;

    }//end parse()


    /**
     * Validates INTERSECT query.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     * @throws DALParserException If there are not exactly 2 select statement found.
     */
    public static function validate(DomElement $query)
    {
        // Get the select tag.
        $selectTags = $query->getElementsByTagName('select');
        if ($selectTags->length < 2) {
            $msg = 'Each Intersect query should have at least two select queries inside.';
            throw new DALParserException($msg);
        }//end if

        DALSelectParser::validate($query);

    }//end validate()


}//end class

?>
