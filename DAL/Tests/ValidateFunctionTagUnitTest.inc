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
 * Unit tests for validateFunctionTag method.
 *
 */
class ValidateFunctionTagUnitTest extends AbstractMySourceUnitTest
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
     * Test that a function without a function attribute causes an error.
     *
     * Test that validateFunctionTag throws exception if tag does not have
     * function attribute.
     *
     * @return void
     */
    public function testNoFunctionAttribute()
    {
        $xml = '<fields><function><arg>test</arg></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $expected = 'function tag must have \'function\' attribute.';

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expected, $msg);

    }//end testNoFunctionAttribute()


    /**
     * Test that a funciton without arguments is still valid.
     *
     * Test that validateFunctionTag does not throws exception if tag does not
     * have arg attributes.
     *
     * Function might not have any arguments.
     *
     * @return void
     */
    public function testNoFunctionArgs()
    {
        $xml = '<fields><function function="CONCAT"></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = 'There should be no exception, but got: '.$e->getMessage();
            PHPUnit_Framework_Assert::fail($msg);
        }

    }//end testNoFunctionArgs()


    /**
     * Tests funtion with empty arguments.
     *
     * Test that validateFunctionTag does not throws exception if it has arg
     * attributes with no values.
     *
     * Function might have empty strings arguments.
     *
     * @return void
     */
    public function testNoFunctionArgValue()
    {
        $xml = '<fields><function function="CONCAT"><arg></arg></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = 'There should be no exception, but got: '.$e->getMessage();
            PHPUnit_Framework_Assert::fail($msg);
        }

    }//end testNoFunctionArgValue()


}//end class

?>
