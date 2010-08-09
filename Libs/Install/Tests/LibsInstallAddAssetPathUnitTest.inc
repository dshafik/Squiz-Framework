<?php
/**
 * Install Lib Tests.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Libs/Install/Install.inc';

/**
 * Unit tests for Install::addAssetPath().
 *
 * NB: Most methods in the Install Lib are used to install these tests so are
 * tested constantly. Some tests require user input, so are not tested here.
 * Methods not tested:
 *      - getInstallOptions()
 *      - getRealSystems() - Not able to determine which systems will exist.
 *      - installQueries()
 *      - installSystems()
 *      - listNestedDirectories()
 *      - runSystemInstallMethods()
 *
 */
class LibsInstallAddAssetPathUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that addAssetPath stores the appropriate path.
     *
     * @return void
     */
    public function testAddAssetPath()
    {
        Channels::includeSystem('AssetType');
        Install::addAssetPath('test1', dirname(__FILE__));
        $ret = AssetType::getTypePath('test1');
        PHPUnit_Framework_Assert::assertEquals($ret, dirname(__FILE__));

    }//end testAddAssetPath()


}//end class

?>
