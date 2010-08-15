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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the getDbType method.
 */
class GetDbTypeUnitTest extends AbstractMySourceUnitTest
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
     * Tests that getDbType throws exception if there is no type specified.
     *
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
