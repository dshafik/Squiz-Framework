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
 * Unit tests for the tableHasColumn() method.
 *
 * tableHasColumn() returns true if table has the specified column listed in
 * its columns tag.
 */
class TableHasColumnUnitTest extends AbstractMySourceUnitTest
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
     * Tests tableHasColumn for invalid column.
     *
     * Test that tableHasColumn returns false for a column that is not listed in
     * columns tag.
     *
     * @return void
     */
    public function testNonExistenColumn()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::tableHasColumn($table, 'NoColumn');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testNonExistenColumn()


    /**
     * Tests tableHasColumn for a valid column.
     *
     * Test that tableHasColumn returns true for a column that is listed in
     * columns tag.
     *
     * @return void
     */
    public function testColumn()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::tableHasColumn($table, 'assetid');

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testColumn()


    /**
     * Tests tableHasColumn for a an invalid column.
     *
     * @return void
     */
    public function testInvalidArgument()
    {
        $file  = dirname(dirname(dirname(__FILE__)));
        $file .= '/Channels/Tests/TestSystems/TestSys1/DB/schema.xml';

        $doc = new DomDocument();
        $doc->load($file);
        $table = $doc->getElementsByTagName('column')->item(0);
        try {
            $result = DALSchemaParser::tableHasColumn($table, 'assetid');
        } catch (DALParserException $e) {
            return;
        }

        $msg = 'tableHasColumn() should have thrown DALParserException exception';
        PHPUnit_Framework_Assert::fail($msg);

    }//end testInvalidArgument()


}//end class

?>
