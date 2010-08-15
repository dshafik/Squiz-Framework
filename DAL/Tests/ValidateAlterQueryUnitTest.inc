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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the validateAlterQuery() method.
 *
 * validateAlterQuery() method validates specified alter query.
 */
class ValidateAlterQueryUnitTest extends AbstractMySourceUnitTest
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
     * Test that validateAlterQuery throws exception when there is no alter tag.
     *
     * @return void
     */
    public function testNoAlterTag()
    {
        $xml = '<query id="test">
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testNoAlterTag()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when alter tag has no
     * table attribute.
     *
     * @return void
     */
    public function testNoTableAttribute()
    {
        $xml = '<query id="test">
                    <alter>
                        <add-column>
                            <columns>
                                <column type="VARCHAR" size="255" allow-null="false" default="">name</column>
                                <column type="VARCHAR" size="255" allow-null="false" default="">name2</column>
                            </columns>
                        </add-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testNoTableAttribute()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when add-column tag has no
     * columns tag.
     *
     * @return void
     */
    public function testAddColumnNoColumnsTag()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-column></add-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testAddColumnNoColumnsTag()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when columns tag has no
     * column tags.
     *
     * @return void
     */
    public function testAddColumnNoColumns()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-column>
                        <columns></columns>
                        </add-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testAddColumnNoColumns()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when column tag has no
     * type attribute.
     *
     * @return void
     */
    public function testAddColumnNoColumnTypeAttr()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-column>
                            <columns>
                                <column size="255" allow-null="false" default="">name</column>
                            </columns>
                        </add-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testAddColumnNoColumnTypeAttr()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when modify-column tag has
     * no columns tag.
     *
     * @return void
     */
    public function testModifyColumnNoColumnsTag()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <modify-column></modify-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testModifyColumnNoColumnsTag()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when modify-column tag has
     * no column tags.
     *
     * @return void
     */
    public function testModifyColumnNoColumns()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <modify-column>
                        <columns></columns>
                        </modify-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testModifyColumnNoColumns()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when column tag has no
     * type attribute.
     *
     * @return void
     */
    public function testModifyColumnNoColumnTypeAttr()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <modify-column>
                            <columns>
                                <column size="255" allow-null="false" default="">name</column>
                            </columns>
                        </modify-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testModifyColumnNoColumnTypeAttr()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when there is an invalid
     * tag.
     *
     * @return void
     */
    public function testInvalidAlterTag()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <test></test>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testInvalidAlterTag()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when drop-column tag has
     * no value.
     *
     * @return void
     */
    public function testDropColumnNoValue()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <drop-column></drop-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testDropColumnNoValue()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when rename-column tag has
     * no value.
     *
     * @return void
     */
    public function testRenameColumnNoValue()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <rename-column></rename-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testRenameColumnNoValue()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when rename-column tag has
     * no 'old' attribute.
     *
     * @return void
     */
    public function testRenameColumnNoOldAttribute()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <rename-column>newName</rename-column>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testRenameColumnNoOldAttribute()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when rename-table tag has
     * no value.
     *
     * @return void
     */
    public function testRenameTableNoValue()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <rename-table></rename-table>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testRenameTableNoValue()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when rename-table tag has
     * no 'old' attribute.
     *
     * @return void
     */
    public function testRenameTableNoOldAttribute()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <rename-table>newName</rename-table>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testRenameTableNoOldAttribute()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when add-constraints has
     * invalid constraint.
     *
     * @return void
     */
    public function testAddConstraintInvalidConstraintPk()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-constraint><primary-key></primary-key></add-constraint>
                    </alter>
                </query>';

        $first = FALSE;
        $doc   = new DomDocument();
        $doc->loadXML($xml);
        $query = $doc->getElementsByTagName('query')->item(0);

        try {
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $first = TRUE;
        }

        $second = FALSE;
        try {
            $result = DALSchemaParser::validateConstraints($query);
        } catch (DALParserException $e) {
            $second = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($first, $second);

    }//end testAddConstraintInvalidConstraintPk()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when add-constraints has
     * invalid constraint.
     *
     * @return void
     */
    public function testAddConstraintInvalidConstraintFk()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-constraint><foreign-key></foreign-key></add-constraint>
                    </alter>
                </query>';

        $first = FALSE;
        $doc   = new DomDocument();
        $doc->loadXML($xml);
        $query = $doc->getElementsByTagName('query')->item(0);

        try {
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $first = TRUE;
        }

        $second = FALSE;
        try {
            $result = DALSchemaParser::validateConstraints($query);
        } catch (DALParserException $e) {
            $second = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($first, $second);

    }//end testAddConstraintInvalidConstraintFk()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when add-constraints has
     * invalid constraint.
     *
     * @return void
     */
    public function testAddConstraintInvalidConstraintUn()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-constraint><unique></unique></add-constraint>
                    </alter>
                </query>';

        $first = FALSE;
        $doc   = new DomDocument();
        $doc->loadXML($xml);
        $query = $doc->getElementsByTagName('query')->item(0);

        try {
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $first = TRUE;
        }

        $second = FALSE;
        try {
            $result = DALSchemaParser::validateConstraints($query);
        } catch (DALParserException $e) {
            $second = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($first, $second);

    }//end testAddConstraintInvalidConstraintUn()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when add-constraints has
     * invalid constraint.
     *
     * @return void
     */
    public function testAddConstraintValidConstraintPk()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <add-constraint>
                        <primary-key name="test_pk1">
                            <column>pkCol</column>
                        </primary-key></add-constraint>
                    </alter>
                </query>';

        $first = FALSE;
        $doc   = new DomDocument();
        $doc->loadXML($xml);
        $query = $doc->getElementsByTagName('query')->item(0);

        try {
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $first = TRUE;
        }

        $second = FALSE;
        try {
            $result = DALSchemaParser::validateConstraints($query);
        } catch (DALParserException $e) {
            $second = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($first, $second);

    }//end testAddConstraintValidConstraintPk()


    /**
     * Test for alter query.
     *
     * Test that validateAlterQuery throws exception when drop-constraint tag
     * has no value.
     *
     * @return void
     */
    public function testDropConstraintNoValue()
    {
        $xml = '<query id="test">
                    <alter table="test">
                        <drop-constraint></drop-constraint>
                    </alter>
                </query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $query  = $doc->getElementsByTagName('query')->item(0);
            $result = DALAlterParser::validate($query);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testDropConstraintNoValue()


}//end class

?>
