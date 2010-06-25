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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the getDbType method.
 *
 * @since 4.0.0
 */
class GetDbTypeUnitTest extends AbstractMySourceUnitTest
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
     * Tests that getDbType throws exception if there is no type specified.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoType()
    {
        $path = dirname(dirname(__FILE__)).'/testingDALConf.inc';
        // Get current conf file contents.
        $originalConf = file_get_contents($path);

        $exception = FALSE;
        try {
            $newConf = "<?php
                    \$conf = array(
                     'DSN' => 'pgsql:dbname=;host=;user=;password=',
                    );
                    return \$conf;
                    ?>";
            file_put_contents($path, $newConf);
            DAL::getDbType();
        } catch (DALException $e) {
            $exception = TRUE;
        }

        file_put_contents($path, $originalConf);
        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testNoType()


    /**
     * Tests that getDbType throws exception if specified type is invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidType()
    {
        $path = dirname(dirname(__FILE__)).'/testingDALConf.inc';
        // Get current conf file contents.
        $originalConf = file_get_contents($path);

        $exception = FALSE;
        try {
            $newConf = "<?php
                    \$conf = array(
                     'DSN' => 'pgsql:dbname=;host=;user=;password=',
                     'type' => 'sdfsd',
                    );
                    return \$conf;
                    ?>";
            file_put_contents($path, $newConf);
            DAL::getDbType();
        } catch (DALException $e) {
            $exception = TRUE;
        }

        file_put_contents($path, $originalConf);
        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testInvalidType()


    /**
     * Tests that getDbType returns correct value for a valid type.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidType()
    {
        $path = dirname(dirname(__FILE__)).'/testingDALConf.inc';
        // Get current conf file contents.
        $originalConf = file_get_contents($path);

        $exception = FALSE;
        try {
            $newConf = "<?php
                    \$conf = array(
                     'DSN' => 'pgsql:dbname=;host=;user=;password=',
                     'type' => 'mysql',
                    );
                    return \$conf;
                    ?>";
            file_put_contents($path, $newConf);
            $type = DAL::getDbType();
            file_put_contents($path, $originalConf);
            PHPUnit_Framework_Assert::assertEquals('mysql', $type);
        } catch (DALException $e) {
            $exception = FALSE;
        }

        file_put_contents($path, $originalConf);
        PHPUnit_Framework_Assert::assertFalse($exception);

    }//end testValidType()


    /**
     * Tests that getDbType returns correct readable value for a valid type.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidTypeReadable()
    {
        $path = dirname(dirname(__FILE__)).'/testingDALConf.inc';
        // Get current conf file contents.
        $originalConf = file_get_contents($path);

        $exception = FALSE;
        try {
            $newConf = "<?php
                    \$conf = array(
                     'DSN' => 'pgsql:dbname=;host=;user=;password=',
                     'type' => 'pgsql',
                    );
                    return \$conf;
                    ?>";
            file_put_contents($path, $newConf);
            $type = DAL::getDbType(TRUE);
            file_put_contents($path, $originalConf);
            PHPUnit_Framework_Assert::assertEquals('PostgreSQL', $type);
        } catch (DALException $e) {
            $exception = FALSE;
        }

        file_put_contents($path, $originalConf);
        PHPUnit_Framework_Assert::assertFalse($exception);

    }//end testValidTypeReadable()


}//end class

?>
