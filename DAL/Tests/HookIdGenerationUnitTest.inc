<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit Tests for the generation of HookIds.
 *
 * @since 4.0.0
 */
class HookIdGenerationUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test the getHookCodeId() method of DALBaker.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
