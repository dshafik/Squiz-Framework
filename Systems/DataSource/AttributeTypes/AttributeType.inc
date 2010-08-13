<?php
/**
 * Base Attribute Type.
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
 * @subpackage DataSource
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * AttributeType.
 *
 * Defines methods that each attribute type must implement.
 */
interface AttributeType
{


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @return mixed
     */
    public static function getValue($assetid, $attributeid);


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param mixed   $value       Value of the attribute.
     *
     * @return boolean
     */
    public static function setValue($assetid, $attributeid, $value);


    /**
     * Finds all assets that have the specified value for the specified attribute.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $typeid      Id of the attribute type.
     * @param mixed  $value       Value of the attribute.
     *
     * @return boolean
     */
    public static function findAssetsByValue($attributeid, $typeid, $value);


}//end interface

?>