<?php
/**
 * Unit Tests for the SystemConfig::deleteConfig().
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
 * DeleteConfig UnitTest.
 *
 */
class SystemConfigDeleteConfigUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Test deleting a system config.
     *
     * @return void
     */
    public function testDeleteConfig()
    {
        Channels::includeSystem('SystemConfig');
        SystemConfig::setConfig('testDeleteConfig', array('Hello world!'));

        $ret = SystemConfig::getConfig('testDeleteConfig');
        PHPUnit_Framework_Assert::assertEquals(array('Hello world!'), $ret);

        SystemConfig::deleteConfig('testDeleteConfig');
        $ret = SystemConfig::getConfig('testDeleteConfig');
        PHPUnit_Framework_Assert::assertEquals(array(), $ret);

    }//end testDeleteConfig()


}//end class

?>