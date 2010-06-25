<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the getDatabaseType() method of DALBaker.
 *
 * @since 4.0.0
 */
class GetDatabaseTypeUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test the database type for Postgres DSN.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
