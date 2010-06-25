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
 * Unit Tests for the getArray() method of DAL.
 *
 * @since 4.0.0
 */
class ExecuteArrayUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Entries to use as test inserts into the database.
     *
     * @var   $_testEntries The array of test entries.
     * @since 4.0.0
     */
    private $_testEntries = array(
                             array(
                              'id'   => '1',
                              'name' => 'EntryA',
                             ),
                             array(
                              'id'   => '2',
                              'name' => 'EntryB',
                             ),
                             array(
                              'id'   => '3',
                              'name' => 'EntryC',
                             ),
                             array(
                              'id'   => '4',
                              'name' => 'EntryD',
                             ),
                             array(
                              'id'   => '5',
                              'name' => 'EntryE',
                             ),
                             array(
                              'id'   => '6',
                              'name' => 'EntryF',
                             ),
                            );


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(__FILE__).'/TestSystems/QueryTestSystem',
               );

    }//end getRequiredSystems()


    /**
     * This test messes around with the ovens, so we need to bake separately.
     *
     * @since  4.0.0
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Test the array is executed correctly.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExecuteQueryArray()
    {
        // Install test entries.
        foreach ($this->_testEntries as $data) {
            Channels::createEventBasket();
            Channels::addToBasket('id', $data['id']);
            Channels::addToBasket('name', $data['name']);
            DAL::executeQuery('QueryTestSystem', 'insertTestNames');
            Channels::removeEventBasket();
        }

        $query    = DAL::getDALQuery('QueryTestSystem', 'getTestNames', 'assoc');
        $ret      = DAL::ExecuteDALQuery($query);
        $expected = array(
                     array(
                      'name' => 'EntryA',
                     ),
                     array(
                      'name' => 'EntryB',
                     ),
                     array(
                      'name' => 'EntryC',
                     ),
                     array(
                      'name' => 'EntryD',
                     ),
                     array(
                      'name' => 'EntryE',
                     ),
                     array(
                      'name' => 'EntryF',
                     ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testExecuteQueryArray()


}//end class
