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
 * Unit tests for the validateConstraints() method.
 *
 * validateConstraints() returns array of constraints from a parent node.
 */
class ValidateConstraintsUnitTest extends AbstractMySourceUnitTest
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
     * Test validateConstraints for valid arguments.
     *
     * Test that validateConstraints does not throw exception for valid
     * constraints.
     *
     * @return void
     */
    public function testValidConstraints()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
                    <foreign-key foreign-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                        <column references="type_code">parent_type</column>
                    </foreign-key>
                    <unique name="unique">
                        <column>col2</column>
                    </unique>
                </test></query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $parent = $doc->getElementsByTagName('test')->item(0);
            $result = DALSchemaParser::validateConstraints($parent);
        } catch (DALParserException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        PHPUnit_Framework_Assert::assertFalse($exception);

    }//end testValidConstraints()


    /**
     * Test that validateConstraints throws exception for invalid constraints.
     *
     * @return void
     */
    public function testInValidConstraints()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key>
                        <column>type_code</column>
                    </primary-key>
                </test></query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $parent = $doc->getElementsByTagName('test')->item(0);
            $result = DALSchemaParser::validateConstraints($parent);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testInValidConstraints()


}//end class

?>
