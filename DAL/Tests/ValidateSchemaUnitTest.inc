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
 * Unit tests for the validateSchema method.
 *
 * ValidateSchema() method throws exception if there is an error in the schema
 * definition.
 */
class ValidateSchemaUnitTest extends AbstractMySourceUnitTest
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
     * Test validateSchema when a required tag or attr is missing.
     *
     * It should also throw exception when node or attr values
     * are invalid.
     *
     * @return void
     */
    public function testValidateTable()
    {
        require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
        $fileName = dirname(__FILE__).'/Files/ValidateSchemaUnitTest.xml';
        $schema   = new DOMDocument();
        $schema->load($fileName);
        $schema = $schema->getElementsByTagName('schema')->item(0);
        $tables = $schema->getElementsByTagName('table');
        $msg    = 'validateTable() should have thrown an exception for table ';
        $tc     = 1;

        foreach ($tables as $table) {
            $caught = FALSE;
            try {
                DALSchemaParser::validateTable($table);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateTable()


}//end class

?>
