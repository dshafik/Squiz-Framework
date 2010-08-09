<?php
/**
 * Datetime Attribute.
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
 * @subpackage AttributeDataSource
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/DataSource/AttributeTypes/DataSourceDatetime.inc';

/**
 * AttributeDataSourceDatetime.
 *
 */
class AttributeDataSourceDatetime extends DataSourceDatetime
{

    /**
     * Table postfix.
     *
     * @var   string $_tablePostfix Table postfix.
     */
    private static $_tablePostfix = 'datetime';


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @return integer
     */
    public static function getValue($assetid, $attributeid)
    {
        return DAL::executeOne('AttributeDataSource', 'getAttributeValue');

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
     * @throws ChannelException If there is an error updating the value
     *                                in the database.
     */
    public static function setValue($assetid, $attributeid, $value)
    {
        $rows = 0;
        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'attribute_'.self::$_tablePostfix);
            $rows = DAL::executeQuery('AttributeDataSource', 'setAttributeValue');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException('DBError:'.$e->getMessage());
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
     * @return boolean
     * @throws ChannelException If there is an error adding the value
     *                                to the database.
     */
    public static function addAttributeValue($assetid, $attributeid, $value, $isDefault)
    {
        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'attribute_'.self::$_tablePostfix);
            $rows = DAL::executeQuery('AttributeDataSource', 'addAttributeValue');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException('DBError:'.$e->getMessage());
        }

        return (bool) $rows;

    }//end addAttributeValue()


}//end class

?>