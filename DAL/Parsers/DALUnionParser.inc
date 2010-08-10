<?php
/**
 * DALUnionParser.
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
 * DALUnionParser.
 *
 * UNION/UNION-ALL query parser.
 *
 */
class DALUnionParser extends DALQueryParser
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
     * Constructs the UNION/UNION-ALL clause/query.
     *
     * Array(
     *  'UNION' => array(
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
        // Find out which type of union this is.
        $query    = array();
        $unionTag = $xmlQuery->getElementsByTagName('union')->item(0);
        if ($unionTag === NULL) {
            $unionTag = $xmlQuery->getElementsByTagName('union-all')->item(0);
            if ($unionTag === NULL) {
                return $query;
            }
        }

        // UNION or UNION-ALL.
        $tag         = strtoupper($unionTag->tagName);
        $query[$tag] = array();

        // Get the alias if there is one.
        if ($unionTag->getAttribute('alias') !== '') {
            $query[$tag]['alias'] = $unionTag->getAttribute('alias');
        }

        $query[$tag]['SELECTS'] = array();

        // Get all select Tags inside union.
        foreach ($unionTag->childNodes as $node) {
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
     * Validates UNION/UNION-ALL query.
     *
     * @param DomElement $query The query element.
     *
     * @return void
     */
    public static function validate(DomElement $query)
    {

    }//end validate()


}//end class

?>