<?php
/**
 * Array Attribute.
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

require_once 'Systems/DataSource/AttributeTypes/AttributeType.inc';

/**
 * ConfigurationDataSourceArray.
 */
class ConfigurationDataSourceArray implements AttributeType
{

    /**
     * Table postfix.
     *
     * @var string $_tablePostfix Table postfix.
     */
    private static $_tablePostfix = 'array';


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
        return unserialize($value);

    }//end getValue()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * Returns the number of affected rows.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param mixed   $value       Value of the attribute.
     *
     * @return boolean
     * @throws ChannelActionException When there is a problem setting the value.
     */
    public static function setValue($assetid, $attributeid, $value)
    {
        if (is_array($value) === TRUE) {
            Channels::modifyBasket('value', serialize($value));
        }

        $rows = 0;
        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'configuration_'.self::$_tablePostfix);
            $rows = DAL::executeQuery('ConfigurationDataSource', 'setAttributeValue');
            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelActionException('DBError:'.$e->getMessage());
        }

        return (bool) $rows;

    }//end setValue()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * Returns the number of affected rows.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param mixed   $value       Value of the attribute.
     * @param boolean $isDefault   TRUE if this is the default value.
     *
     * @return void
     * @throws ChannelActionException When the value is unable to be added.
     */
    public static function addAttributeValue($assetid, $attributeid, $value, $isDefault)
    {
        if (is_array($value) === TRUE) {
            Channels::modifyBasket('value', serialize($value));
        }

        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'configuration_'.self::$_tablePostfix);
            $rows = DAL::executeQuery('ConfigurationDataSource', 'addAttributeValue');
            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelActionException('DBError:'.$e->getMessage());
        }

    }//end addAttributeValue()


    /**
     * Finds all assets that have the specified value for the specified attribute.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $typeid      Id of the attribute type.
     * @param mixed  $value       Value of the attribute.
     *
     * @return void
     */
    public static function findAssetsByValue($attributeid, $typeid, $value)
    {

    }//end findAssetsByValue()


}//end class

?>