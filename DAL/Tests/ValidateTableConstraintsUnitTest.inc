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
 * Unit tests for the validateTableConstraints method.
 *
 * validateTableConstraints method throws exception if there are any errors.
 */
class ValidateTableConstraintsUnitTest extends AbstractMySourceUnitTest
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
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has no
     * constraints.
     *
     * @return void
     */
    public function testNoConstraints()
    {
        $xml = '<table name="test"></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table does not have "constraints".';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoConstraints()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has no
     * primary-key constraint.
     *
     * @return void
     */
    public function testNoPrimaryKeyConstraint()
    {
        $xml = '<table name="test"><constraints></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table must have a primary-key.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoPrimaryKeyConstraint()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * primary-key constraint with no name attribute.
     *
     * @return void
     */
    public function testPrimaryKeyWithNoName()
    {
        $xml = '<table name="test"><constraints>
                <primary-key></primary-key></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table primary-key does not have "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testPrimaryKeyWithNoName()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * primary-key constraint with no columns.
     *
     * @return void
     */
    public function testPrimaryKeyWithNoColumns()
    {
        $xml = '<table name="test"><constraints>
                <primary-key name="pk"></primary-key></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has no columns defined for its primary-key.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testPrimaryKeyWithNoColumns()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * primary-key constraint with columns that are not defined.
     *
     * @return void
     */
    public function testPrimaryKeyWithInvalidColumn()
    {
        $xml = '<table name="test"><constraints>
                <primary-key name="pk">
                <column>noWay</column>
                </primary-key></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table does not have column "noWay", but it was used in its primary-key.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testPrimaryKeyWithInvalidColumn()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with no name attribute.
     *
     * @return void
     */
    public function testForeignKeyWithNoName()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key></foreign-key></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 does not have "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyWithNoName()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with no foreign-table attribute.
     *
     * @return void
     */
    public function testForeignKeyWithNoForeignTableAttr()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1"></foreign-key></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 does not have "foreign-table" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyWithNoForeignTableAttr()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with invalid on-delete attribute value.
     *
     * @return void
     */
    public function testForeignKeyWithInvalidDeleteAttr()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1" foreign-table="test2" on-delete="INVALID"></foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 has invalid on-delete attribute value.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyWithInvalidDeleteAttr()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with no columns.
     *
     * @return void
     */
    public function testForeignKeyWithNoColumns()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1" foreign-table="test2" on-delete="CASCADE">
                </foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 does not have columns.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyWithNoColumns()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with a column that has no references attribute.
     *
     * @return void
     */
    public function testForeignKeyMissingReferenceAttr()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1" foreign-table="test2" on-delete="CASCADE">
                    <column>col1</column>
                </foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 has column (#1) with missing references attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyMissingReferenceAttr()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with a column that has no value.
     *
     * @return void
     */
    public function testForeignKeyColumnMissingValue()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1" foreign-table="test2" on-delete="CASCADE">
                    <column references="type_code"></column>
                </foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 has column (#1) with missing content.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyColumnMissingValue()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * foreign-key constraint with a column that was not defined.
     *
     * @return void
     */
    public function testForeignKeyUnDefinedColumn()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                <foreign-key name="fk1" foreign-table="test2" on-delete="CASCADE">
                    <column references="type_code">col2</column>
                </foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table foreign-key #1 has column (#1) with column name that was NOT defined in table columns. Name: col2';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testForeignKeyUnDefinedColumn()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * unique constraint with no name.
     *
     * @return void
     */
    public function testUniqueNoName()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                    <unique>
                        <column>col1</column>
                    </unique>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table unique constraint #1 does not have "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testUniqueNoName()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * unique constraint with no columns.
     *
     * @return void
     */
    public function testUniqueNoColumns()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                    <unique name="unique">
                    </unique>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s tableunique constraint #1 does now have columns.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testUniqueNoColumns()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints throws exception when table has
     * unique constraint with no columns.
     *
     * @return void
     */
    public function testUniqueUndefinedColumn()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                <column>col1</column>
                </primary-key>
                    <unique name="unique">
                        <column>col2</column>
                    </unique>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table does not have column "col2", but it was used in its unique constraint.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testUniqueUndefinedColumn()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints does not throws exception when
     * constraint is valid.
     *
     * @return void
     */
    public function testValidPrimaryKey()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                    <column>col1</column>
                </primary-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $caught = FALSE;

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidPrimaryKey()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints does not throws exception when
     * constraint is valid.
     *
     * @return void
     */
    public function testValidForeignKey()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                    <column>col1</column>
                </primary-key>
                <foreign-key name="asset_fk1" foreign-table="asset_type" on-delete="CASCADE">
                    <column references="type_code">col1</column>
                </foreign-key>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $caught = FALSE;

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidForeignKey()


    /**
     * Test for validateTableConstrants().
     *
     * Test that validateTableConstraints does not throws exception when
     * constraint is valid.
     *
     * @return void
     */
    public function testValidUnique()
    {
        $xml = '<table name="test">
                <columns><column>col1</column></columns>
                <constraints>
                <primary-key name="pk">
                    <column>col1</column>
                </primary-key>
                <unique name="unique_constraint">
                    <column>col1</column>
                </unique>
                </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $caught = FALSE;

        try {
            DALSchemaParser::validateTableConstraints($table);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidUnique()


}//end class

?>
