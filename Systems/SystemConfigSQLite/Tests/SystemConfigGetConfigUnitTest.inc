<?php
/**
 * Unit Tests for the SystemConfig::getConfig().
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
 * @package    CMS
 * @subpackage SystemConfigSQLite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * GetConfig UnitTest.
 *
 */
class SystemConfigGetConfigUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Test getting a system config.
     *
     * @return void
     */
    public function testGetConfig()
    {
        Channels::includeSystem('SystemConfig');
        $config = array(
                   'key1' => 'true',
                   'key2' => 'false',
                   'key3' => TRUE,
                   'key4' => FALSE,
                   'key5' => 'hello world',
                  );
        SystemConfig::setConfig('Dummy', $config);

        // Make sure 'true'/'false' gets converted to boolean.
        $ret      = SystemConfig::getConfig('Dummy');
        $expected = array(
                     'key1' => TRUE,
                     'key2' => FALSE,
                     'key3' => TRUE,
                     'key4' => FALSE,
                     'key5' => 'hello world',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

        // Modifying keys.
        SystemConfig::setConfig('Dummy', array('key2' => 'true'));
        SystemConfig::setConfig('Dummy', array('key3' => 'changed to string'));
        $ret      = SystemConfig::getConfig('Dummy');
        $expected = array(
                     'key1' => TRUE,
                     'key2' => TRUE,
                     'key3' => 'changed to string',
                     'key4' => FALSE,
                     'key5' => 'hello world',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetConfig()


}//end class

?>