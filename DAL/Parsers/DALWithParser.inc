<?php
/**
 * DALWithParser.
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
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALUnionParser.inc';

/**
 * DALWithParser.
 *
 * WITH/CONNECT-BY query parser.
 */
class DALWithParser extends DALQueryParser
{


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the array form of the WITH query.
     *
     * Array(
     *  'WITH' => array(
     *             'FUNCTION'         => array(...),
     *             'UNION-ALL'        => array(...),
     *             'AFTER'            => array(...), // Outer SELECT statement.
     *             'CONNECT-BY-PRIOR' => array(...), // For Oracle DB.
     *            ),
     * );
     *
     * @param DomElement $xmlQuery Query element.
     *
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query   = array();
        $withTag = $xmlQuery->getElementsByTagName('with')->item(0);

        $query['WITH'] = self::parseSqlFunction($withTag);

        // Find the select.
        $query['WITH'] = array_merge(
            $query['WITH'],
            DALUnionParser::parse($withTag)
        );

        $after = $withTag->getElementsByTagName('after')->item(0);

        $query['WITH']['AFTER'] = DALSelectParser::parse($after);

        $connectBy = $withTag->getElementsByTagName('connect-by-prior')->item(0);

        $query['WITH']['CONNECT-BY-PRIOR'] = current(
            DALWhereParser::parse($connectBy->parentNode, 'connect-by-prior')
        );

        return $query;

    }//end parse()


    /**
     * Validate WITH query.
     *
     * @param DomElement $xmlQuery The WITH/CONNECT-BY-PRIOR query.
     *
     * @return void
     */
    public static function validate(DomElement $xmlQuery)
    {

    }//end validate()


    /**
     * Converts XML function element to PHP array.
     *
     * Array(
     *  'FUNCTION' => array(
     *                 'function' => string,
     *                 'ARGS'     => array(
     *                                0 => mixed,
     *                               ),
     *                ),
     * )
     *
     * @param DomElement $function Function XML node.
     *
     * @return array
     */
    protected static function parseSqlFunction(DomElement $function)
    {
        $query                         = array();
        $query['FUNCTION']             = array();
        $query['FUNCTION']['function'] = $function->getAttribute('function');
        $query['FUNCTION']['ARGS']     = array();

        if ($function->getAttribute('alias') !== '') {
            $query['FUNCTION']['alias'] = $function->getAttribute('alias');
        }

        $argsTag = $function->getElementsByTagName('args')->item(0);
        $argTags = $argsTag->getElementsByTagName('arg');

        foreach ($argTags as $arg) {
            $query['FUNCTION']['ARGS'][] = $arg->nodeValue;
        }

        return $query;

    }//end parseSqlFunction()


}//end class

?>