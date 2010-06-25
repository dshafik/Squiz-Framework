<?php
/**
 * DALExceptParser.
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
 * DALExceptParser.
 *
 * EXCEPT/EXCEPT-ALL query parser.
 *
 * @since 4.0.0
 */
class DALExceptParser extends DALQueryParser
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
     * Constructs the EXCEPT/EXCEPT-ALL clause/query.
     *
     * Array(
     *  'EXCEPT' => array(
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
        $query = array();
        if (strtolower($xmlQuery->tagName) !== 'except') {
            $exceptTag = $xmlQuery->getElementsByTagName('except')->item(0);
        } else {
            $exceptTag = $xmlQuery;
        }

        // Find out which type of except clause this is.
        if ($exceptTag === NULL) {
            $exceptTag = $xmlQuery->getElementsByTagName('except-all')->item(0);
            if ($exceptTag === NULL) {
                return $query;
            }
        }

        // EXCEPT or EXCEPT-ALL.
        $tag         = strtoupper($exceptTag->tagName);
        $query[$tag] = array();

        // Get the alias if there is one.
        if ($exceptTag->getAttribute('alias') !== '') {
            $query[$tag]['alias'] = $exceptTag->getAttribute('alias');
        }

        $query[$tag]['SELECTS'] = array();

        // Get all select Tags inside except clause.
        foreach ($exceptTag->childNodes as $node) {
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
     * Validates EXCEPT/EXCEPT-ALL query.
     *
     * @param DomElement $query The query element.
     *
     * @since  4.0.0
     * @return void
     * @throws DALParserException If there are not exactly 2 select statement found.
     */
    public static function validate(DomElement $query)
    {
        // Get the select tag.
        $selectTags = $query->getElementsByTagName('select');
        if ($selectTags->length < 2) {
            $msg = 'Each Except query should have at least two select queries inside.';
            throw new DALParserException($msg);
        }//end if

        DALSelectParser::validate($query);

    }//end validate()


}//end class

?>
