<?php
/**
 * Boolean Attribute.
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
 * @subpackage ConfigurationDataSource
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/ConfigurationDataSource/AttributeTypes/ConfigurationDataSourceArray.inc';

/**
 * ConfigurationDataSourceBoolean.
 */
class ConfigurationDataSourceBoolean extends ConfigurationDataSourceArray
{

    /**
     * Table postfix.
     *
     * @var string $_tablePostfix Table postfix.
     */
    private static $_tablePostfix = 'boolean';


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @return mixed
     */
    public static function getValue($assetid, $attributeid)
    {
        $value = DAL::executeOne('ConfigurationDataSource', 'getAttributeValue');
        return $value;

    }//end getValue()


}//end class

?>