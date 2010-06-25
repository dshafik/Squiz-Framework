<?php
/**
 * DALWithParser.
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
require_once 'DAL/Parsers/DALWhereParser.inc';
require_once 'DAL/Parsers/DALUnionParser.inc';

/**
 * DALWithParser.
 *
 * WITH/CONNECT-BY query parser.
 *
 * @since 4.0.0
 */
class DALWithParser extends DALQueryParser
{


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     *
     * @since 4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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