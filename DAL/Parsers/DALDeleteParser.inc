<?php
/**
 * DALInsertParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/DALBaker.inc';
require_once 'DAL/Parsers/DALWhereParser.inc';

/**
 * DALInsertParser.
 *
 * INSERT query parser.
 *
 * @since 4.0.0
 */
class DALDeleteParser extends DALQueryParser
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
     * @since  4.0.0
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
     * @since  4.0.0
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