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
 * Unit tests for the validateSchema method.
 *
 * ValidateSchema() method throws exception if there is an error in the schema
 * definition.
 *
 * @since 4.0.0
 */
class ValidateSchemaUnitTest extends AbstractMySourceUnitTest
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
     * Test validateSchema when a required tag or attr is missing.
     *
     * It should also throw exception when node or attr values
     * are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateTable()
    {
        require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
        $fileName = dirname(__FILE__).'/Files/ValidateSchemaUnitTest.xml';
        $schema   = new DOMDocument();
        $schema->load($fileName);
        $schema = $schema->getElementsByTagName('schema')->item(0);
        $tables = $schema->getElementsByTagName('table');
        $msg    = 'validateTable() should have thrown an exception for table ';
        $tc     = 1;

        foreach ($tables as $table) {
            $caught = FALSE;
            try {
                DALSchemaParser::validateTable($table);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateTable()


}//end class

?>
