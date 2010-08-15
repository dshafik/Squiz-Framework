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
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the isDALQuery() method.
 */
class IsDALQueryUnitTest extends AbstractMySourceUnitTest
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
     * Tests checking a valid query will return TRUE.
     *
     * @return void
     */
    public function testIsDALQueryTrue()
    {
        $testArray = array(
                      'query' => array(),
                     );
        $query = new Query('', $testArray);

        $ret = DAL::isDALQuery($query);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testIsDALQueryTrue()


    /**
     * Tests that an empty array will throw an exception.
     *
     * @return void
     */
    public function testIsDALQueryEmptyQueryArray()
    {
        $caught = FALSE;
        try {
            $query = new Query();
        } catch (DALException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testIsDALQueryEmptyQueryArray()


    /**
     * Tests a query that is close but does not contain a DAL_QUERY element.
     *
     * @return void
     */
    public function testIsDALQueryFalse()
    {
        $testArray = array(
                      array(
                       'sql'      => '',
                       'bindings' => array(),
                      ),
                     );

        $ret = DAL::isDALQuery($testArray);
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testIsDALQueryFalse()


    /**
     * Test a query that is a normal array returns FALSE.
     *
     * @return void
     */
    public function testIsDALQueryRandom()
    {
        $testArray = array(
                      '1',
                      '2',
                      '3',
                      '4',
                     );

        $ret = DAL::isDALQuery($testArray);
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testIsDALQueryRandom()


}//end class

?>
