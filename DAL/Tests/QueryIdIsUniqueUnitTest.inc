<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the queryIdIsUnique() method.
 *
 * queryIdIsUnique() returns FALSE if given query id already used by another
 * query.
 */
class QueryIdIsUniqueUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test that queryIdIsUnique returns false if query id is not unique.
     *
     * @return void
     */
    public function testNonUniqueId()
    {
        $xml = '<queries><query id="test"></query><query id="test"></query></queries>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::queryIdIsUnique($doc, $query);

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testNonUniqueId()


    /**
     * Test that queryIdIsUnique returns true if query id is unique.
     *
     * @return void
     */
    public function testUniqueId()
    {
        $xml = '<queries><query id="test"></query><query id="test2"></query></queries>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::queryIdIsUnique($doc, $query);

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testUniqueId()


}//end class

?>
