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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';

/**
 * Unit tests for the getQueryType() method.
 *
 * getQueryType() returns the given queries type.
 *
 */
class GetQueryTypeUnitTest extends AbstractMySourceUnitTest
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
     * Test that getQueryType returns valid string for non empty query tag.
     *
     * @return void
     */
    public function testNonEmptyQueryTag()
    {
        $xml = '<query id="test"><select></select></query>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::getQueryType($query);

        PHPUnit_Framework_Assert::assertEquals('select', $result);

    }//end testNonEmptyQueryTag()


    /**
     * Test that getQueryType returns NULL when query tag is empty.
     *
     * @return void
     */
    public function testEmptyQueryTag()
    {
        $xml = '<query id="test"></query>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::getQueryType($query);

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testEmptyQueryTag()


}//end class

?>
