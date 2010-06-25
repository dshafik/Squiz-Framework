<?php
/**
 * DALDropParser.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(__FILE__).'/DALQueryParser.inc';

/**
 * DALDropParser.
 *
 * DROP query parser.
 *
 * @since 4.0.0
 */
class DALDropParser extends DALQueryParser
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
     * Converts drop tag to PHP array.
     *
     * @param DomElement $xmlQuery Single DROP query.
     *
     * @since  4.0.0
     * @return array
     */
    public static function parse(DomElement $xmlQuery)
    {
        $query   = array();
        $dropTag = $xmlQuery->getElementsByTagName('drop')->item(0);
        if ($dropTag !== NULL) {
            if ($dropTag->getAttribute('table') !== '') {
                $query['DROP']['TABLE'] = $dropTag->getAttribute('table');
            } else if ($dropTag->getAttribute('sequence') !== '') {
                $query['DROP']['SEQUENCE'] = $dropTag->getAttribute('sequence');
            }
        }

        return $query;

    }//end parse()


    /**
     * Validates the drop tag.
     *
     * Each drop tag must have table attribute.
     *
     * @param DomElement $query Single DROP query tag.
     *
     * @since  4.0.0
     * @return void
     * @throws DALParserException If the DROP tag is malformed.
     */
    public static function validate(DomElement $query)
    {
        $dropTag = $query->getElementsByTagName('drop')->item(0);
        if ($dropTag === NULL) {
            throw new DALParserException('DROP tag not found.');
        }

        if (($dropTag->getAttribute('table') === '')
            && ($dropTag->getAttribute('sequence') === '')
        ) {
            throw new DALParserException('No DROP query type specified.');
        }

    }//end validate()


}//end class

?>
