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
 * Unit tests for the validateQueries() method.
 *
 * validateQueries() Validates all the queries under the given queries node.
 */
class ValidateQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var string $_schema Schema xml.
     */
    private $_queries = '<queries system="system1">
                            <query id="test1"></query>
                            <query id="test2"></query>
                        </queries>';


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
     * Test that validateQueries throws exception when system attr is not found.
     *
     * @return void
     */
    public function testNoSystemAttribute()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);
        $queries->removeAttribute('system');
        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoSystemAttribute()


    /**
     * Tests validateQueries when there are duplicate query ids.
     *
     * An exception should be thrown by validateQueries in this situation.
     *
     * @return void
     */
    public function testUniqueQueryIds()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);
        // Set the second query id to same one as first query id.
        $queries->getElementsByTagName('query')->item(1)->setAttribute('id', 'test1');

        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testUniqueQueryIds()


    /**
     * Test validateQueries doesn't throw an exception when queries are valid.
     *
     * @return void
     */
    public function testValidQueries()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);

        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidQueries()


}//end class

?>
