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
 * Unit tests for the validateValuesTag and validateValueTag method.
 *
 * These methods throws exception if they do not have required children etc.
 *
 */
class ValidateValueTagUnitTest extends AbstractMySourceUnitTest
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
     * Test validateValuesTag throws exception when it has no value children.
     *
     * @return void
     */
    public function testNoValuesTags()
    {
        $xml = '<insert><values></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $values = $queryXml->getElementsByTagName('values')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateField($values);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoValuesTags()


    /**
     * Test that validateValueTag throws exception when it has no column attr.
     *
     * @return void
     */
    public function testNoValueTags()
    {
        $xml = '<insert><values><value></value></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $value  = $queryXml->getElementsByTagName('value')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateField($value);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoValueTags()


    /**
     * Test valid values tag.
     *
     * Test that validateValuesTag does not throw exception when it has value
     * children and they are valid.
     *
     * @return void
     */
    public function testValidValuesTags()
    {
        $xml = '<insert><values><value column="ts">a</value></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $values = $queryXml->getElementsByTagName('values')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateValuesTag($values);
        } catch (DALParserException $e) {
            var_dump($e->getMessage());
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidValuesTags()


}//end class

?>
