<?php
/**
 * DALDropParser.
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

require_once dirname(__FILE__).'/DALQueryParser.inc';

/**
 * DALDropParser.
 *
 * DROP query parser.
 *
 */
class DALDropParser extends DALQueryParser
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
     * Converts drop tag to PHP array.
     *
     * @param DomElement $xmlQuery Single DROP query.
     *
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
