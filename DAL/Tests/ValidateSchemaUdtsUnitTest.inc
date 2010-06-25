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
 * Unit tests for the validateSchemaUdts method.
 *
 * validateSchemaUdts() method throws exception if there is a problem with the
 * definition of a system's User-Defined types (UDT's).
 *
 * @since 4.0.0
 */
class ValidateSchemaUdtsUnitTest extends AbstractMySourceUnitTest
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
     * Test that having a UDT name that is not uppercase throws an exception.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLowerCaseUdtName()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="VARCHAR" size="100">typecode</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'Lower Case UDTs being passed as valid';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testLowerCaseUdtName()


    /**
     * Test that having a UDT name that is uppercase is passed as valid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUpperCaseUdtName()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="VARCHAR" size="100">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'Upper case UDTs being passed as invalid';
        PHPUnit_Framework_Assert::AssertFalse($caughtException, $msg);

    }//end testUpperCaseUdtName()


    /**
     * Test UDT without a type specified.
     *
     * Test that having a UDT with a type that needs a size throws an exception
     * if it doesn't have it specified.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtWithoutSize()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="VARCHAR">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'VARCHAR UDT passed as valid with no size attribute specified';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testUdtWithoutSize()


    /**
     * Test UDT without a size specified.
     *
     * Test that having a UDT with a type that needs a size throws an exception
     * if it doesn't have it specified.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtWithSize()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="VARCHAR" size="100">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'VARCHAR UDT passed as invalid with size attribute specified';
        PHPUnit_Framework_Assert::AssertFalse($caughtException, $msg);

    }//end testUdtWithSize()


    /**
     * Tests that UDTs with no types throw exceptions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtNoType()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt size="100">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT with no type did not throw exception';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testUdtNoType()


    /**
     * Tests that UDTs with types specified don't throw exceptions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtWithType()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="VARCHAR" size="100">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT with type specified threw an exception';
        PHPUnit_Framework_Assert::AssertFalse($caughtException, $msg);

    }//end testUdtWithType()


    /**
     * Tests that NUMERIC UDTs without a scale attribute throw exceptions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNumericUdtWithNoScale()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="NUMERIC" size="100">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT with NUMERIC type and no scale didn\'t throw an exception';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testNumericUdtWithNoScale()


    /**
     * Tests that NUMERIC UDTs with a scale attribute don't throw exceptions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNumericUdtWithScale()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="NUMERIC" size="100" scale="8">TYPECODE</udt>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT with NUMERIC type and no scale didn\'t throw an exception';
        PHPUnit_Framework_Assert::AssertFalse($caughtException, $msg);

    }//end testNumericUdtWithScale()


    /**
     * Tests that udt-imports without a system attribute throw an eception.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtImportNoSystem()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="NUMERIC" size="100" scale="8">TYPECODE</udt>
                         <udt-import>TYPECODE</udt-import>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT-Import with no system specified did not cause an error.';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testUdtImportNoSystem()


    /**
     * Tests that udt-imports with a system attribute do not throw an eception.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtImportWithSystem()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="NUMERIC" size="100" scale="8">TYPECODE</udt>
                         <udt-import system="assets">FOREIGNUDT</udt-import>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT-Import with a system specified caused an error.';
        PHPUnit_Framework_Assert::AssertFalse($caughtException, $msg);

    }//end testUdtImportWithSystem()


    /**
     * Tests that udt-imports with an invalid UDT name throw an exception.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUdtImportWithInvalidName()
    {
        $schemaSrc = '<schema system="asset_type">
                        <udts>
                         <udt type="NUMERIC" size="100" scale="8">TYPECODE</udt>
                         <udt-import system="assets">foreignUDT</udt-import>
                        </udts>
                        <tables>
                         <table name="asset_type">
                          <columns>
                           <column type="TYPECODE" allow-null="true">type_code</column>
                           <column type="VARCHAR" length="20" allow-null="true">version</column>
                          </columns>
                         </table>
                        </tables>
                      </schema>';

        $doc = new DomDocument();
        $doc->loadXML($schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);

        try {
            DALSchemaParser::validateSchemaUdts($schemaNode);
            $caughtException = FALSE;
        } catch (DALParserException $e) {
            $caughtException = TRUE;
        }

        $msg = 'UDT-Import with an invalid type name didn\'t throw an error.';
        PHPUnit_Framework_Assert::AssertTrue($caughtException, $msg);

    }//end testUdtImportWithInvalidName()


}//end class

?>
