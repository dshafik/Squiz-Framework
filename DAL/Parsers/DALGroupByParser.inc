<?php
/**
 * DALGroupBy.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'Libs/XML/XML.inc';

/**
 * DALGroupBy.
 *
 * GROUP BY clause parser. Parses GROUP BY clause.
 *
 * @since 4.0.0
 */
class DALGroupByParser extends DALQueryParser
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
     * @since  4.0.0
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
     * @since  4.0.0
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
