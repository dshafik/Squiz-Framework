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
 * Unit tests for the tableHasColumn() method.
 *
 * tableHasColumn() returns true if table has the specified column listed in
 * its columns tag.
 *
 * @since 4.0.0
 */
class TableHasColumnUnitTest extends AbstractMySourceUnitTest
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
     * Tests tableHasColumn for invalid column.
     *
     * Test that tableHasColumn returns false for a column that is not listed in
     * columns tag.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistenColumn()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::tableHasColumn($table, 'NoColumn');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testNonExistenColumn()


    /**
     * Tests tableHasColumn for a valid column.
     *
     * Test that tableHasColumn returns true for a column that is listed in
     * columns tag.
     *
     * @since  4.0.0
     * @return void
     */
    public function testColumn()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::tableHasColumn($table, 'assetid');

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testColumn()


    /**
     * Tests tableHasColumn for a an invalid column.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidArgument()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table = $doc->getElementsByTagName('column')->item(0);
        try {
            $result = DALSchemaParser::tableHasColumn($table, 'assetid');
        } catch (DALParserException $e) {
            return;
        }

        $msg = 'tableHasColumn() should have thrown DALParserException exception';
        PHPUnit_Framework_Assert::fail($msg);

    }//end testInvalidArgument()


}//end class

?>
