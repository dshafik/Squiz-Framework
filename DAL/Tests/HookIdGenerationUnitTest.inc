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
 * Unit Tests for the generation of HookIds.
 */
class HookIdGenerationUnitTest extends AbstractMySourceUnitTest
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
     * Test the getHookCodeId() method of DALBaker.
     *
     * @return void
     */
    public function testGetHookCodeId()
    {
        $expected   = 'HOOKID:test_hook_id';
        $testHookId = 'test_hook_id';
        $ret        = DALBaker::getHookCodeId($testHookId);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetHookCodeId()


    /**
     * Test the getHookArrayId() method of DALBaker.
     *
     * @return void
     */
    public function testGetHookArrayId()
    {
        $expected   = '$TestHookIdArray';
        $testHookId = 'Test_hook_id';
        $ret        = DALBaker::getHookArrayId($testHookId);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetHookArrayId()


}//end class

?>
