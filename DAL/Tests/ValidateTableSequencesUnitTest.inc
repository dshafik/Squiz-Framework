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
 * Unit tests for the validateTableSequences method.
 *
 * validateTableSequences method throws exception if there are any errors.
 */
class ValidateTableSequencesUnitTest extends AbstractMySourceUnitTest
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
     * Tests empty sequences declaration.
     *
     * Test that validateTableSequences throws exception when it has a
     * sequences with no sequence tags.
     *
     * @return void
     */
    public function testNoSequnceTags()
    {
        $xml = '<table name="test"><sequences></sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has sequences tag with no sequence tags.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableSequences($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoSequnceTags()


    /**
     * Test sequence with missing name attribute.
     *
     * Test that validateTableSequences throws exception when it has a
     * sequence tag with no name attribute.
     *
     * @return void
     */
    public function testNoSequenceName()
    {
        $xml = '<table name="test"><sequences><sequence></sequence></sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has sequence with missing "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableSequences($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoSequenceName()


}//end class

?>
