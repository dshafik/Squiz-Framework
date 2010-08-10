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
 * Unit tests for the getDatabaseType() method of DALBaker.
 *
 */
class GetDatabaseTypeUnitTest extends AbstractMySourceUnitTest
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
     * Test the database type for Postgres DSN.
     *
     * @return void
     */
    public function testPostgresDsn()
    {
        $dsn      = 'pgsql:dbname=test_db;host=db;user=test_user;password=';
        $expected = 'postgres';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testPostgresDsn()


    /**
     * Test the database type for MSSQL DSN.
     *
     * @return void
     */
    public function testMssqlDsn()
    {
        $dsn      = 'dblib:dbname=test_db;host=db;user=test_user;password=';
        $expected = 'mssql';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testMssqlDsn()


    /**
     * Test the database type for MySQL DSN.
     *
     * @return void
     */
    public function testMysqlDsn()
    {
        $dsn      = 'mysql:dbname=test_db;host=db;user=test_user;password=';
        $expected = 'mysql';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testMysqlDsn()


    /**
     * Test the database type for Oracle DSN.
     *
     * @return void
     */
    public function testOracleDsn()
    {
        $dsn      = 'oci:dbname=test_db;host=db;user=test_user;password=';
        $expected = 'oracle';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testOracleDsn()


    /**
     * Test the database type for DB/2 DSN.
     *
     * @return void
     */
    public function testDb2Dsn()
    {
        $dsn      = 'odbc:dbname=test_db;host=db;user=test_user;password=';
        $expected = 'db2';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testDb2Dsn()


    /**
     * Test the database type for empty DSN.
     *
     * @return void
     */
    public function testEmptyDsn()
    {
        $dsn      = '';
        $expected = '';
        $ret      = DALBaker::getDatabaseType($dsn);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testEmptyDsn()


}//end class

?>
