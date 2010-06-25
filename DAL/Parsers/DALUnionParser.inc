<?php
/**
 * DALUnionParser.
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
require_once 'DAL/DALBaker.inc';

/**
 * DALUnionParser.
 *
 * UNION/UNION-ALL query parser.
 *
 * @since 4.0.0
 */
class DALUnionParser extends DALQueryParser
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
     * @since  4.0.0
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
     * @since  4.0.0
     * @return void
     */
    public static function validate(DomElement $query)
    {

    }//end validate()


}//end class

?>