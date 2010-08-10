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
 * Unit tests for the validateSchemaUdts method.
 *
 * validateSchemaUdts() method throws exception if there is a problem with the
 * definition of a system's User-Defined types (UDT's).
 *
 */
class ValidateSchemaUdtsUnitTest extends AbstractMySourceUnitTest
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
     * Test that having a UDT name that is not uppercase throws an exception.
     *
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
