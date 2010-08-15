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
 * Unit tests for the getTableSequences method.
 *
 * getTableSequences method returns correct values.
 */
class GetTableSequencesUnitTest extends AbstractMySourceUnitTest
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
     * Test for getTableSequences() with no sequences.
     *
     * Test that getTableSequences returns empty array when table has no
     * sequences.
     *
     * @return void
     */
    public function testNoSequences()
    {
        $xml = '<table name="test"></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array();
        $retVal   = DALSchemaParser::getTableSequences($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testNoSequences()


    /**
     * Test for getTableSequences() for valid sequences.
     *
     * Test that getTableSequences returns non empty array when table has
     * sequences.
     *
     * @return void
     */
    public function testSequences()
    {
        $xml = '<table name="test">
                <sequences>
                    <sequence name="assetid_seq" />
                    <sequence name="assetid_seq2" />
                </sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array (
                     0 => 'assetid_seq',
                     1 => 'assetid_seq2',
                    );
        $retVal = DALSchemaParser::getTableSequences($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testSequences()


}//end class

?>
