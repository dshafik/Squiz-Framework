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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the getTableColumnTypes() method.
 *
 * getTableColumnTypes() gets the column types of a specific table in a system.
 *
 * @since 4.0.0
 */
class GetTableColumnTypesUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
               );

    }//end getRequiredSystems()


    /**
     * Test that getTableColumnTypes return NULL when system does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentSystem()
    {
        $result = DALBaker::getTableColumnTypes('InvalidSystem', 'table');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSystem()


    /**
     * Test that getTableColumnTypes return NULL when table does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentTable()
    {
        $result = DALBaker::getTableColumnTypes('TestSys1', 'invalidTable');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentTable()


    /**
     * Test that getTableColumnTypes return correct array values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testArrayOfColumns()
    {
        $result   = DALBaker::getTableColumnTypes('TestSys1', 'table1');
        $expected = array(
                     'assetid'               => 'INTEGER',
                     'type_code'             => 'VARCHAR',
                     'version'               => 'VARCHAR',
                     'name'                  => 'VARCHAR',
                     'status'                => 'SMALLINT',
                     'status_changed_userid' => 'INTEGER',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testArrayOfColumns()


    /**
     * Test that getTableColumnTypes with column specified return correct value.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSingleColumn()
    {
        $result   = DALBaker::getTableColumnTypes('TestSys1', 'table1', 'version');
        $expected = 'VARCHAR';

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testSingleColumn()


    /**
     * Test getTableColumnTypes for a non-existent column.
     *
     * Test that getTableColumnTypes with non existent column specified return
     * NULL.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentSingleColumn()
    {
        $result = DALBaker::getTableColumnTypes('TestSys1', 'table1', 'asdfdf');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSingleColumn()


}//end class

?>
