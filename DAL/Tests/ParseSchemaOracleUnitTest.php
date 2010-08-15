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

require_once 'DAL/Tests/ParseSchemaUnitTest.php';

/**
 * Tests for the parsing of Oracle Schemas.
 *
 * Unit test for testing that the sql produced for Oracle is correct when
 * parsing the schema.xml.
 */
class ParseSchemaOracleUnitTest extends ParseSchemaUnitTest
{


    /**
     * Test for Oracle schema parsing.
     *
     * Tests to verify that the SQL produced from parsing a test schema is what
     * is expected for the Oracle database.
     *
     * @return void
     */
    public function testOracleSchemaParse()
    {
        if ($this->skipOnDbType('oracle') === TRUE) {
            return;
        }

        $result = $this->getSqlForTestSchema();

        PHPUnit_Framework_Assert::assertEquals($this->_getSql(), $result);

    }//end testOracleSchemaParse()


    /**
     * Returns the SQL expected for Oracle.
     *
     * We have to do this in a method because member vars don't allow concatination
     * of strings! arrg.
     *
     * @return string
     */
    private function _getSql()
    {
        $sql  = 'CREATE TABLE asset_type ('."\n";
        $sql .= 'type_code NVARCHAR2(100),'."\n";
        $sql .= 'version NVARCHAR2(20),'."\n";
        $sql .= 'name NVARCHAR2(100),'."\n";
        $sql .= 'description NVARCHAR2(255),'."\n";
        $sql .= 'CONSTRAINT asset_type_pk PRIMARY KEY (type_code),'."\n";
        $sql .= 'CONSTRAINT asset_type_fk1 FOREIGN KEY (type_code) REFERENCES asset_type(type_code) ON DELETE CASCADE);'."\n";
        $sql .= 'CREATE INDEX asset_type_type_code ON asset_type (type_code);'."\n";
        $sql .= 'CREATE INDEX asset_type_lvl ON asset_type (version);'."\n";

        return $sql;

    }//end _getSql()


}//end class

?>
