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
 * Unit tests for the validateTableIndexes method.
 *
 * validateTableIndexes method throws exception if there are any errors.
 *
 */
class ValidateTableIndexesUnitTest extends AbstractMySourceUnitTest
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
     * Tests validateValuesTag() with no name.
     *
     * Test that validateValuesTag throws exception when it has an index with no
     * name attribute.
     *
     * @return void
     */
    public function testNoIndexName()
    {
        $xml = '<table name="test"><indexes><index></index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has index with missing "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoIndexName()


    /**
     * Tests validateValuesTag() with no columns.
     *
     * Test that validateValuesTag throws exception when it has index with no
     * columns.
     *
     * @return void
     */
    public function testNoIndexColumns()
    {
        $xml = '<table name="test"><indexes>
                <index name="idx1"></index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has index with no columns.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoIndexColumns()


    /**
     * Tests validateValuesTag() with an undefined columns.
     *
     * Test that validateValuesTag throws exception when it has index that uses
     * undefined column.
     *
     * @return void
     */
    public function testInvalidColumn()
    {
        $xml = '<table name="test"><indexes>
                <index name="idx1">
                    <column>col1</column>
                </index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table does not have column "col1" but it was used in its index.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testInvalidColumn()


}//end class

?>
