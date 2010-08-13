<?php
/**
 * Actions for the AttributeDataSource System.
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

require_once 'Systems/DataSource/DataSourceActions.inc';

/**
 * AttributeDataSourceActions.
 *
 * Implementation of the abstract DataSource System. Uses
 * the local database for storage of attribute values.
 */
class AttributeDataSourceActions implements DataSourceActions
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
        Channels::addToBasket('tableName', 'attribute_'.$attrType);
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
     * @param array       $attrTypes  All attribute types being retrieved.
     *
     * @return mixed
     */
    public static function batchGetAttributeValue(
        $assetids,
        array $attributes,
        array $attrTypes
    ) {
        $values = array();
        foreach ($attrTypes as $attrType => $attrids) {
            $dalQuery = AttributeDataSource::batchGetAttributeValueQc(
                $assetids,
                $attrids,
                $attrType
            );

            $retVal = DAL::executeDALQuery($dalQuery, 'Assoc');
            foreach ($retVal as $result) {
                $assetid = $result['assetid'];
                if (isset($values[$assetid]) === FALSE) {
                    $values[$assetid] = array();
                }

                $values[$assetid][$result['attributeid']] = $result['value'];
            }
        }

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
        Channels::addToBasket('tableName', 'attribute_'.$attrType);
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
     * @throws ChannelException If there is an error retrieving the type
     *                          of an attribute, or an error retrieving
     *                          the values.
     */
    public static function batchSetAttributeValue($assetids, array $attrVals)
    {
        try {
            DAL::beginTransaction();
            $attrTypes = array();
            foreach ($assetids as $assetid) {
                foreach ($attrVals as $attrid => $value) {
                    // Get attribute's type.
                    if (isset($attrTypes[$attrid]) === FALSE) {
                        Channels::includeSystem('Attribute');
                        $attrInfo = Attribute::getAssetAttributeInfo(
                            $assetid,
                            $attrid
                        );

                        if (empty($attrInfo) === FALSE) {
                            $attrTypes[$attrid] = $attrInfo['attribute_type'];
                        } else {
                            $msg = 'Failed to get attribute type.';
                            throw new ChannelException($msg);
                        }
                    }

                    // Try to update the attribute value.
                    $result = AttributeDataSource::setAttributeValue(
                        $assetid,
                        $attrid,
                        $attrTypes[$attrid],
                        $value
                    );

                    if ($result === FALSE) {
                        // No rows were updated. Insert instead.
                        AttributeDataSource::addAttributeValue(
                            $assetid,
                            $attrid,
                            $attrTypes[$attrid],
                            $value,
                            'f'
                        );
                    }
                }//end foreach
            }//end foreach

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
    public static function addAttributeValue(
        $assetid,
        $attributeid,
        $attrType,
        $value,
        $isDefault
    ) {
        $className = self::_getAttributeTypeClassName($attrType);
        Channels::addToBasket('tableName', 'attribute_'.$attrType);
        $path = self::_getAttributeTypePath($className);
        include_once $path;

        $rows = call_user_func(
            array(
             $className,
             'addAttributeValue',
            ),
            $assetid,
            $attributeid,
            $value,
            $isDefault
        );

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
        Channels::addToBasket('tableName', 'attribute_'.$attributeType);
        $result = DAL::executeOne('AttributeDataSource', 'tableExists');
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
     * @throws ChannelException If there is an error retrieving the
     *                          attribute values.
     */
    public static function getAllAttributeValues($assetType, $attributeid, $attrType)
    {
        $ret = array();
        try {
            $ret = DAL::executeAssoc('AttributeDataSource', 'getAllAttributeValues');
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
        return DAL::executeQuery('AttributeDataSource', 'removeAttributeValues');

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
        $result = DAL::executeAll('AttributeDataSource', 'attributeValueExists');
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
     * @throws ChannelException If there is an error retrieving
     *                          the assets.
     */
    public static function getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        try {
            $ret = DAL::executeGrouped('AttributeDataSource', 'getAssetsByAttributeValue');
        } catch (PDOException $e) {
            throw new ChannelException('DB Error: '.$e->getMessage());
        }

        $ret = array_keys($ret);

        return $ret;

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
     * @see    AttributeDataSource::getAssetsByAttributeValue()
     * @return (boolean|array)
     */
    public static function isUniqueAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        $assets = AttributeDataSource::getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value);

        if (count($assets) === 0) {
            return TRUE;
        }

        if (count($assets) === 1) {
            $assets = reset($assets);
            return $assets;
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
    public static function addAttributeText($attributeid, $assetType)
    {

    }//end addAttributeText()


    /**
     * Add a new long text attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeLongtext($attributeid, $assetType)
    {

    }//end addAttributeLongtext()


    /**
     * Add a new integer attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeInteger($attributeid, $assetType)
    {

    }//end addAttributeInteger()


    /**
     * Add a new float attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeFloat($attributeid, $assetType)
    {

    }//end addAttributeFloat()


    /**
     * Add a new datetime attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeDatetime($attributeid, $assetType)
    {

    }//end addAttributeDatetime()


    /**
     * Add a new selection attribute.
     *
     * @param string $attributeid Id of the new attribute.
     * @param string $assetType   Asset type.
     *
     * @return void
     */
    public static function addAttributeSelection($attributeid, $assetType)
    {

    }//end addAttributeSelection()


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
        $result = DAL::executeAssoc('AttributeDataSource', 'duplicateValueCount');
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
        $className = 'AttributeDataSource'.$name;
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
        return DAL::executeQuery('AttributeDataSource', 'updateAttributeid');

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
     * @throws ChannelException The the attribute type is invalid.
     */
    public static function updateAttributeDataType($attributeid, $assetType, $attributeType, $newDataType)
    {
        if (AttributeDataSource::isValidAttributeType($newDataType) === FALSE) {
            $msg = $newDataType.' is not a valid data type.';
            throw new ChannelException($msg);
        }

        // Move attribute values from old to new table.
        DAL::executeQuery('AttributeDataSource', 'updateAttributeDataType');

        AttributeDataSource::removeAttributeValues($attributeid, $assetType, $attributeType);

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
     * Adds a rule to newly created text attribute.
     *
     * @param string $attributeid Attribute id.
     * @param string $assetType   Type of asset.
     *
     * @return void
     */
    public static function addAttributeTextRule($attributeid, $assetType)
    {
        // Adds a rule to the created attribute.
        $ruleid = 'maxLength';
        Channels::includeSystem('Attribute');
        Attribute::addAttributeRule($attributeid, $assetType, $ruleid);

    }//end addAttributeTextRule()


    /**
     * Returns batchGetAttributeValue Query object.
     *
     * @param string $assetids     List of asset ids.
     * @param string $attributeids List of attributes.
     * @param string $attrType     List of attribute types.
     *
     * @return object
     */
    public static function batchGetAttributeValueQc($assetids, $attributeids, $attrType)
    {
        Channels::addToBasket('attrType', $attrType);
        $baseQuery = DAL::getDALQuery('AttributeDataSource', 'batchGetAttributeValue', 'Assoc');
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
                      'text',
                      'long_text',
                      'integer',
                      'float',
                      'datetime',
                     );

        Channels::includeSystem('Context');
        Channels::addToBasket('oldContextAssetid', Context::getAssetid($assetid, $oldContext, TRUE));
        Channels::addToBasket('newContextAssetid', Context::getAssetid($assetid, $newContext, TRUE));

        try {
            DAL::beginTransaction();

            // Make sure the asset in the new context exists.
            Context::createAsset($assetid, $newContext);

            foreach ($attrTypes as $attrType) {
                Channels::addToBasket('tableName', 'attribute_'.$attrType, TRUE);
                DAL::executeQuery('AttributeDataSource', 'removeContextValsForAsset');
                DAL::executeQuery('AttributeDataSource', 'replaceContextValsForAsset');
            }

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollBack();
            throw new ChannelException(_('Failed to replace context values'));
        }

    }//end replaceContext()


    /**
     * Removes the entire context data for the specified context range.
     *
     * @param string $context Context string that could be used in LIKE queries.
     *                        E.g. _____01__ .
     *
     * @return void
     * @throws ChannelException If context data cannot be removed.
     */
    public static function removeAllContextData($context)
    {
        Channels::includeSystem('Context');
        if (Context::isMasterContext($context) === TRUE) {
            return;
        }

        try {
            $attrTypes = array(
                          'text',
                          'long_text',
                          'integer',
                          'float',
                          'datetime',
                         );

            DAL::beginTransaction();
            foreach ($attrTypes as $attrType) {
                Channels::addToBasket('tableName', 'attribute_'.$attrType, TRUE);
                $query = DAL::getQuery('AttributeDataSource', 'removeAllContextData');
                DAL::executeQuery($query);
            }

            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException($e);
        }//end try

    }//end removeAllContextData()


}//end class

?>
