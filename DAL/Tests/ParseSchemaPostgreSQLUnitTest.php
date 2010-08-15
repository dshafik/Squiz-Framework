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
 * Tests for the parsing of Postgres schemas.
 *
 * Unit test for testing that the sql produced for PostgreSQL is correct when
 * parsing the schema.xml.
 */
class ParseSchemaPostgreSQLUnitTest extends ParseSchemaUnitTest
{


    /**
     * Test for Postgres schema parsing.
     *
     * Tests to verify that the SQL produced from parsing a test schema is what
     * is expected for the PostgreSQL database.
     *
     * @return void
     */
    public function testPostgreSQLSchemaParse()
    {
        if ($this->skipOnDbType('pgsql') === TRUE) {
            return;
        }

        $result = $this->getSqlForTestSchema();
        PHPUnit_Framework_Assert::assertEquals($this->_getSql(), $result);

    }//end testPostgreSQLSchemaParse()


    /**
     * Returns the SQL expected for PostgreSQL.
     *
     * We have to do this in a method because member vars don't allow concatination
     * of strings! arrg.
     *
     * @return string
     */
    private function _getSql()
    {
        $sql  = 'CREATE TABLE asset_type ('."\n";
        $sql .= 'type_code VARCHAR(100),'."\n";
        $sql .= 'version VARCHAR(20),'."\n";
        $sql .= 'name VARCHAR(100),'."\n";
        $sql .= 'description VARCHAR(255),'."\n";
        $sql .= 'CONSTRAINT asset_type_pk PRIMARY KEY (type_code),'."\n";
        $sql .= 'CONSTRAINT asset_type_fk1 FOREIGN KEY (type_code) REFERENCES asset_type(type_code) ON DELETE CASCADE);'."\n";
        $sql .= 'CREATE INDEX asset_type_type_code ON asset_type (type_code);'."\n";
        $sql .= 'CREATE INDEX asset_type_lvl ON asset_type (version);'."\n";

        return $sql;

    }//end _getSql()


}//end class

?>
