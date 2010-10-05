<?php
/**
 * Actions for the ConfigurationDataSource System.
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

require_once 'Systems/DataSource/DataSourceActions.inc';

/**
 * ConfigurationDataSourceActions.
 *
 * Implementation of the abstract DataSource System. Uses
 * the local database for storage of configuration values.
 */
class ConfigurationDataSourceActions implements DataSourceActions
{


    /**
     * Retrieves the value if a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    Type of the attribute.
     *
     * @return mixed
     */
    public static function getAttributeValue($assetid, $attributeid, $attrType)
    {
        $className = self::_getAttributeTypeClassName($attrType);
        Channels::addToBasket('tableName', 'configuration_'.$attrType);
        $path = self::_getAttributeTypePath($className);
        include_once $path;

        $value = call_user_func(array($className, 'getValue'), $assetid, $attributeid);
        return $value;

    }//end getAttributeValue()


    /**
     * Retrieves the value of one ore more attributes from one or more assets.
     *
     * @param array|Query $assetids   List of asset ids.
     * @param array       $attributes List of attribute ids.
     * @param array       $attrTypes  An array of all attribute types being retrieved.
     *
     * @return mixed
     */
    public static function batchGetAttributeValue($assetids, array $attributes, array $attrTypes)
    {
        $dalQuery = ConfigurationDataSource::batchGetAttributeValueQc($assetids, $attributes, $attrTypes);
        $values   = DAL::executeDALQuery($dalQuery, 'Grouped');
        return $values;

    }//end batchGetAttributeValue()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * Value will be converted for storage by the data source. Returns true if
     * action was successful.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    Type of the attribute.
     * @param mixed   $value       Value of the attribute for the specified asset.
     *
     * @see    DataSourceActions::addAttributeValue()
     * @return boolean
     */
    public static function setAttributeValue($assetid, $attributeid, $attrType, $value)
    {
        $className = self::_getAttributeTypeClassName($attrType);
        Channels::addToBasket('tableName', 'configuration_'.$attrType);
        $path = self::_getAttributeTypePath($className);
        include_once $path;

        return call_user_func(array($className, 'setValue'), $assetid, $attributeid, $value);

    }//end setAttributeValue()


    /**
     * Sets the values of one or more attributes for one or more assets.
     *
     * Value will be converted for storage by the data source. Returns true if
     * action was successful.
     *
     * @param array|object $assetids List of asset ids.
     * @param array        $attrVals Array of attribute ids with their values.
     *                               array(attributeid => value).
     *
     * @return void
     * @throws ChannelException When an attribute type is unable to be retrieved or
     *                          there is an error setting one of the attributes.
     */
    public static function batchSetAttributeValue($assetids, array $attrVals)
    {
        try {
            DAL::beginTransaction();
            foreach ($assetids as $assetid) {
                foreach ($attrVals as $attrid => $value) {
                    // Get attribute's type.
                    if (isset($attrTypes[$attrid]) === FALSE) {
                        Channels::includeSystem('Attribute');
                        $attrInfo = Attribute::getAssetAttributeInfo($assetid, $attrid);
                        if (empty($attrInfo) === FALSE) {
                            $attrTypes[$attrid] = $attrInfo['attribute_type'];
                        } else {
                            $msg = 'Failed to get attribute type.';
                            throw new ChannelException($msg);
                        }
                    }

                    ConfigurationDataSource::setAttributeValue($assetid, $attrid, $attrTypes[$attrid], $value);
                }
            }

            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }//end try

    }//end batchSetAttributeValue()


    /**
     * Adds an attribute value when there exists no value for the attribute.
     *
     * This method is called when its determined that there is currently no value
     * for the specified attribute, which is determined by a call to
     * attributeValueExists().
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    Type of the attribute.
     * @param mixed   $value       Value of the attribute for the specified asset.
     * @param boolean $isDefault   TRUE if it is the default value.
     *
     * @see    DataSourceActions::setAttributeValue()
     * @see    DataSourceActions::attributeValueExists()
     * @return boolean
     */
    public static function addAttributeValue($assetid, $attributeid, $attrType, $value, $isDefault)
    {
        $className = self::_getAttributeTypeClassName($attrType);
        Channels::addToBasket('tableName', 'configuration_'.$attrType);
        $path = self::_getAttributeTypePath($className);
        include_once $path;

        $rows = call_user_func(array($className, 'addAttributeValue'), $assetid, $attributeid, $value, $isDefault);
        return $rows;

    }//end addAttributeValue()


    /**
     * Checks if specified attribute type is valid for the data source.
     *
     * @param string $attributeType Attribute type to validate.
     *
     * @return boolean
     */
    public static function isValidAttributeType($attributeType)
    {
        // Check if the attribute type class exists for this DS.
        $className = self::_getAttributeTypeClassName($attributeType);
        $path      = self::_getAttributeTypePath($className);
        if (file_exists($path) === FALSE) {
            return FALSE;
        }

        // Check if the database table exists for this attribute type.
        Channels::addToBasket('tableName', 'configuration_'.$attributeType);
        $result = DAL::executeOne('ConfigurationDataSource', 'tableExists');
        if ($result !== '1') {
            return FALSE;
        }

        return TRUE;

    }//end isValidAttributeType()


    /**
     * Retrieves all values set for an attribute.
     *
     * @param string $assetType   Asset type the attribute is set for.
     * @param string $attributeid The ID of the attribute.
     * @param string $attrType    The attribute type.
     *
     * @return array
     * @throws ChannelException If there is an error retrieving the values.
     */
    public static function getAllAttributeValues($assetType, $attributeid, $attrType)
    {
        $ret = array();
        try {
            $ret = DAL::executeAssoc('ConfigurationDataSource', 'getAllAttributeValues');
        } catch (PDOException $e) {
            throw new ChannelException('DB Error: '.$e->getMessage());
        }

        return $ret;

    }//end getAllAttributeValues()


    /**
     * Removes all values set for an attribute.
     *
     * @param string $attributeid   The ID of the attribute.
     * @param string $assetType     Asset type the attribute is set for.
     * @param string $attributeType The attribute type.
     *
     * @return array
     */
    public static function removeAttributeValues($attributeid, $assetType, $attributeType)
    {
        // Remove the old values.
        return DAL::executeQuery('ConfigurationDataSource', 'removeAttributeValues');

    }//end removeAttributeValues()


    /**
     * Returns TRUE if the asset contains a value for the specified attribute.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    The type of attribute.
     * @param string  $value       The value of the attribute.
     *
     * @return boolean
     */
    public static function attributeValueExists($assetid, $attributeid, $attrType, $value=NULL)
    {
        $result = DAL::executeAll('ConfigurationDataSource', 'attributeValueExists');
        if (empty($result) === TRUE) {
            return FALSE;
        } else {
            return TRUE;
        }

    }//end attributeValueExists()


    /**
     * Retrieves assets based off the value of the specified attribute.
     *
     * @param string $assetType   Asset type the attribute is set for.
     * @param string $attributeid The ID of the attribute.
     * @param string $attrType    The attribute type.
     * @param mixed  $value       Value of attribute used to match assets.
     *
     * @return array
     * @throws ChannelException If there is an error retrieving the assets.
     */
    public static function getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        try {
            $ret = DAL::executeGrouped('ConfigurationDataSource', 'getAssetsByAttributeValue');
        } catch (PDOException $e) {
            throw new ChannelException('DB Error: '.$e->getMessage());
        }

        return array_keys($ret);

    }//end getAssetsByAttributeValue()


    /**
     * Checks to see if the specified value is unique for the specified attribute.
     *
     * Returns TRUE if the value is not in use for the specified attribute. If
     * the value is in use once only (i.e. unique), returns the id of the asset
     * it is set for. Will return FALSE if the value is found more than once.
     *
     * @param string $assetType   The type of asset the attribute is for.
     * @param string $attributeid The id of the attribute.
     * @param string $attrType    The type of attribute.
     * @param mixed  $value       The attribute value to check.
     *
     * @see    ConfigurationDataSource::getAssetsByAttributeValue()
     * @return (boolean|array)
     */
    public static function isUniqueAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        $assets = ConfigurationDataSource::getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value);

        if (count($assets) === 0) {
            return TRUE;
        }

        if (count($assets) === 1) {
            return reset($assets);
        }

        return FALSE;

    }//end isUniqueAttributeValue()


    /**
     * Add a new text attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeArray($attributeid, $assetType)
    {

    }//end addAttributeArray()


    /**
     * Checks that the specified attribute has unique values for specified asset type.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $assetType   Type of the asset.
     * @param string $attrType    Type of the attribute.
     *
     * @return boolean
     */
    public static function attributeValsUnique($attributeid, $assetType, $attrType)
    {
        $result = DAL::executeAssoc('ConfigurationDataSource', 'duplicateValueCount');
        if (empty($result) === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end attributeValsUnique()


    /**
     * Returns the class name for the given attribute type.
     *
     * @param string $attrType Type of the attribute.
     *
     * @return string
     */
    private static function _getAttributeTypeClassName($attrType)
    {
        $name      = ucwords(str_replace('_', ' ', $attrType));
        $name      = str_replace(' ', '', $name);
        $className = 'ConfigurationDataSource'.$name;
        return $className;

    }//end _getAttributeTypeClassName()


    /**
     * Returns the file path of a given attribute type.
     *
     * @param string $className Class name of an attribute type.
     *
     * @return string
     */
    private static function _getAttributeTypePath($className)
    {
        return dirname(__FILE__).'/AttributeTypes/'.$className.'.inc';

    }//end _getAttributeTypePath()


    /**
     * Updates the attribute id of assets that are specific type.
     *
     * Returns number of affected rows.
     *
     * @param string $attributeid   Id of the attribute to change.
     * @param string $assetType     Type of the assets.
     * @param string $attributeType Attribute's type.
     * @param string $newId         New id for the attribute.
     *
     * @return integer
     */
    public static function updateAttributeid($attributeid, $assetType, $attributeType, $newId)
    {
        // Update all assets that use the specified attributeid.
        return DAL::executeQuery('ConfigurationDataSource', 'updateAttributeid');

    }//end updateAttributeid()


    /**
     * Updates the attribute data type of assets that are specific asset type.
     *
     * @param string $attributeid   Id of the attribute to change.
     * @param string $assetType     Type of the assets.
     * @param string $attributeType Attribute's type.
     * @param string $newDataType   New data type for the attribute.
     *
     * @return void
     * @throws ChannelException If the data type provided is invalid.
     */
    public static function updateAttributeDataType($attributeid, $assetType, $attributeType, $newDataType)
    {
        if (ConfigurationDataSource::isValidAttributeType($newDataType) === FALSE) {
            $msg = $newDataType.' is not a valid data type.';
            throw new ChannelException($msg);
        }

        // Move attribute values from old to new table.
        DAL::executeQuery('ConfigurationDataSource', 'updateAttributeDataType');

        ConfigurationDataSource::removeAttributeValues($attributeid, $assetType, $attributeType);

    }//end updateAttributeDataType()


    /**
     * Updates the attribute data type of assets that are specific asset type.
     *
     * @param string  $attributeid   Id of the attribute to change.
     * @param string  $assetType     Type of the assets.
     * @param string  $attributeType Attribute's type.
     * @param string  $value         Value of the attribute for the specified
     *                               asset.
     * @param boolean $isDefault     True if the value is default.
     *
     * @return void
     */
    public static function addAssetTypeAttributeValue($attributeid, $assetType, $attributeType, $value, $isDefault)
    {
        // Get the assetids that are of asset type.
        Channels::addToBasket('typeid', $assetType, TRUE);
        $result = DAL::executeAssoc('Asset', 'getAssetsByType');

        if (empty($result) === FALSE) {
            $className = self::_getAttributeTypeClassName($attributeType);
            include_once self::_getAttributeTypePath($className);

            foreach ($result as $assetid) {
                Channels::addToBasket('assetid', $assetid['assetid'], TRUE);
                call_user_func(array($className, 'addAttributeValue'), $assetid['assetid'], $attributeid, $assetType, $value, $isDefault);
            }
        }

    }//end addAssetTypeAttributeValue()


    /**
     * Returns batchGetAttributeValue Query object.
     *
     * @param string $assetids       Asset ids.
     * @param string $attributeids   Attribute ids.
     * @param string $attributeTypes Attribute types.
     *
     * @return object
     */
    public static function batchGetAttributeValueQc($assetids, $attributeids, $attributeTypes)
    {
        $baseQuery = DAL::getDALQuery('ConfigurationDataSource', 'batchGetAttributeValue', 'Assoc');
        return $baseQuery;

    }//end batchGetAttributeValueQc()


    /**
     * Replaces attribute context values.
     *
     * @param string $oldContext The context to get values from.
     * @param string $newContext The context to override.
     * @param string $assetid    Asset id to use.
     *
     * @return void
     * @throws ChannelException If context cannot be replaced.
     */
    public static function replaceContext($oldContext, $newContext, $assetid)
    {
        $attrTypes = array(
                      'array',
                      'boolean',
                     );

        Channels::includeSystem('Context');
        Channels::addToBasket('oldContextAssetid', Context::getAssetid($assetid, $oldContext, TRUE));
        Channels::addToBasket('newContextAssetid', Context::getAssetid($assetid, $newContext, TRUE));

        try {
            DAL::beginTransaction();

            foreach ($attrTypes as $attrType) {
                Channels::addToBasket('tableName', 'configuration_'.$attrType, TRUE);
                DAL::executeQuery('AttributeDataSource', 'removeContextValsForAsset');
                DAL::executeQuery('AttributeDataSource', 'replaceContextValsForAsset');
            }

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollBack();
            throw new ChannelException(_('Failed to replace context values'));
        }

    }//end replaceContext()


}//end class

?>