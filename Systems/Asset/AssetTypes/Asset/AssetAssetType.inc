<?php
/**
 * Base Asset Type.
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
 * @subpackage Asset
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Asset Asset Type Class.
 *
 */
class AssetAssetType
{


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     *
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the contents of this asset.
     *
     * By default, a blank string is returned.
     *
     * @return string
     */
    public static function getContents()
    {
        return '';

    }//end getContents()


    /**
     * Validates that the name for a page is valid.
     *
     * @param string $name The name for the newly created page.
     *
     * @return mixed
     */
    public static function validateName($name)
    {
        if (trim($name) === '') {
            return 'You must supply a name.';
        }

    }//end validateName()


    /**
     * Returns the system name the asset belongs to.
     *
     * @return string
     */
    public static function getSystemName()
    {
        $systemPath = dirname(dirname(dirname(__FILE__)));
        return basename($systemPath);

    }//end getSystemName()


}//end class

?>