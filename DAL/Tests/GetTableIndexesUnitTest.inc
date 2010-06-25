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
 * Unit tests for the getTableIndexes() method.
 *
 * getTableIndexes() converts XML table indexes to php array.
 *
 * @since 4.0.0
 */
class GetTableIndexesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var   string $_schema Schema xml.
     * @since 4.0.0
     */
    private $_schema = '<schema system="asset">
                         <tables>
                          <table name="asset">
                           <columns></columns>
                           <indexes>
                               <index name="asset_type_code">
                                   <column>type_code</column>
                                   <column>assetid</column>
                               </index>
                               <index name="asset_type_code2">
                                   <column>type_code</column>
                               </index>
                           </indexes></table></tables></schema>';


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
     * Test that getTableIndexes returns empty array when no indexes defined.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoIndexes()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table   = $doc->getElementsByTagName('table')->item(0);
        $indexes = $doc->getElementsByTagName('indexes')->item(0);
        $table->removeChild($indexes);
        $result = DALSchemaParser::getTableIndexes($table);

        PHPUnit_Framework_Assert::assertTrue(empty($result));

    }//end testNoIndexes()


    /**
     * Test for getTableIndexes() with valid indexes.
     *
     * Test that getTableIndexes returns correct array result when there are
     * valid indexes defined.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidatedIndexes()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::getTableIndexes($table);

        $expected = array(
                     0 => array(
                           'name'    => 'asset_type_code',
                           'COLUMNS' => array(
                                         'type_code',
                                         'assetid',
                                        ),
                          ),
                     1 => array(
                           'name'    => 'asset_type_code2',
                           'COLUMNS' => array(
                                         'type_code',
                                        ),
                          ),
                    );

        PHPUnit_Framework_Assert::assertEquals($result, $expected);

    }//end testValidatedIndexes()


}//end class

?>
