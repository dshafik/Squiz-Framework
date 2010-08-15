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
 * Unit tests for the validateHook method.
 *
 * validateHook() method throws exception if hook does not have id attribute.
 */
class ValidateHookUnitTest extends AbstractMySourceUnitTest
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
     * Test that validateHook throws exception when hook does not have id attr.
     *
     * @return void
     */
    public function testNoIdAttribute()
    {
        $xml = '<fields><hook /></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $hook   = $queryXml->getElementsByTagName('hook')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateHook($hook);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoIdAttribute()


    /**
     * Test that validateHook throws exception when hook does not have id attr.
     *
     * @return void
     */
    public function testValidHook()
    {
        $xml = '<fields><hook id="valid" /></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $hook   = $queryXml->getElementsByTagName('hook')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateHook($hook);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidHook()


}//end class

?>
