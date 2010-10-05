<?php
/**
 * Actions for the Configuration System.
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
 * @subpackage Configuration
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'DAL/DAL.inc';
require_once 'Libs/Exceptions/GeneralException.inc';

/**
 * Configuration Actions Class.
 */
class ConfigurationActions
{


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * The attribute is examined and the processing passed to the relevant
     * data source. The value returned will be a PHP data type, which will have
     * been converted from the stored data type by the data source.
     * Throws a ChannelException if the value cannot be retrieved.
     *
     * @param integer $assetid     The asset to find the value for.
     * @param string  $attributeid The attribute to find.
     *
     * @return mixed
     * @throws ChannelException If the configuration attribute is not
     *                          found for the asset.
     */
    public static function getAttributeValue($assetid, $attributeid)
    {
        try {
            $attrInfo = Configuration::getAssetAttributeInfo(
                $assetid,
                $attributeid
            );
        } catch (ChannelException $e) {
            $msg  = 'Configuration: '.$attributeid.' not found for Asset: ';
            $msg .= $assetid.'.';
            throw new ChannelException($msg);
        }

        // Now retrieve the data source of the attribute, and include its'
        // source file.
        $dataSource = $attrInfo['data_source'];

        Channels::includeSystem($dataSource);
        $attributeType = $attrInfo['attribute_type'];

        Channels::includeSystem('Context');
        $assetid = Context::getAssetids($assetid);

        // Arguments to pass to Data Source action.
        $args = array(
                 'assetid'     => $assetid,
                 'attributeid' => $attributeid,
                 'attrType'    => $attributeType,
                );

        $value = call_user_func_array(array($dataSource, 'getAttributeValue'), $args);
        return $value;

    }//end getAttributeValue()


    /**
     * Retrieves the attribute info using the specified assetid.
     *
     * @param integer $assetid     Asset's id.
     * @param string  $attributeid Attribute id.
     *
     * @return array
     * @throws ChannelException If there is an error retriving the attribute info.
     */
    public static function getAssetAttributeInfo($assetid, $attributeid)
    {
        try {
            $attrInfo = DAL::getQuery(
                'Configuration',
                'getAttributeInfoUsingAssetid'
            );
            $attrInfo = DAL::getAssoc($attrInfo);
            if (empty($attrInfo) === TRUE) {
                $msg  = 'Configuration: Error Retrieving Attribute. (';
                $msg .= $assetid.':'.$attributeid.')';
                throw new ChannelException($msg);
            }
        } catch (PDOException $e) {
            throw new ChannelException('DBError: '.$e->getMessage());
        }

        $attrInfo = current($attrInfo);
        return $attrInfo;

    }//end getAssetAttributeInfo()


    /**
     * Retrieves the attribute info using the specified asset type.
     *
     * @param string $assetType   Asset's type.
     * @param string $attributeid Attribute id.
     *
     * @return array
     * @throws ChannelException If there is an error retrieving the attribute.
     */
    public static function getAssetTypeAttributeInfo($assetType, $attributeid)
    {
        $attrInfo = DAL::executeAssoc('Configuration', 'getAttributeInfo');
        if (empty($attrInfo) === TRUE) {
            $msg = 'Configuration: Error Retrieving Attribute.';
            throw new ChannelException($msg);
        }

        $attrInfo = current($attrInfo);
        return $attrInfo;

    }//end getAssetTypeAttributeInfo()


    /**
     * Gets any asset that have the specified value for the specified attribute.
     *
     * @param string  $assetType   The type of asset.
     * @param string  $attributeid Attribute id.
     * @param mixed   $value       Value to use for lookup.
     * @param boolean $descendants If TRUE will return for assetType and all
     *                             child asset types, otherwise just assetType.
     *
     * @return array
     * @throws ChannelException If the configuration attribute is not
     *                          found for the asset.
     */
    public static function getAssetsByAttributeValue(
        $assetType,
        $attributeid,
        $value,
        $descendants=FALSE
    ) {
        try {
            $attrInfo = Configuration::getAssetTypeAttributeInfo(
                $assetType,
                $attributeid
            );
        } catch (ChannelException $e) {
            $msg  = 'Configuration: '.$attributeid.' not found for: ';
            $msg .= $assetType.' asset type.';
            throw new ChannelException($msg);
        }

        Channels::includeSystem('Asset');
        Channels::includeSystem($attrInfo['data_source']);

        if ($descendants === TRUE) {
            $types = Asset::getChildTypes($assetType);

            $assetTypes = array();
            foreach ($types as $typeInfo) {
                $assetTypes[] = $typeInfo['typeid'];
            }

            $assetTypes[] = $assetType;
        } else {
            $assetTypes = array($assetType);
        }

        $getMethod = 'getAssetsByAttributeValue';
        $getCall   = array(
                      $attrInfo['data_source'],
                      $getMethod,
                     );
        $getParams = array(
                      $assetTypes,
                      $attributeid,
                      $attrInfo['attribute_type'],
                      $value,
                     );

        return call_user_func_array($getCall, $getParams);

    }//end getAssetsByAttributeValue()


    /**
     * Retrieves the value of one or more attributes from a one or more assets.
     *
     * The attributes are examined and the processing passed to the relevant
     * data sources. The values returned will be a PHP data type, which will
     * have been converted from the stored data type by the data source.
     *
     * @param array|Query $assetids     Multiple asset ids to retrieve values
     *                                  for.
     * @param array       $attributeids The attributes to retrieve values for.
     *
     * @return array
     */
    public static function batchGetAttributeValue(
        $assetids,
        array $attributeids
    ) {
        Channels::includeSystem('Asset');

        $assetTypes  = Asset::batchGetAssetType($assetids);
        $ancestors   = Asset::batchGetTypeAncestors($assetTypes, TRUE);
        $dataSources = Configuration::batchGetAttributeDataSource(
            $ancestors,
            $attributeids
        );

        // Values array will contain all values from all data sources.
        $values = array();

        // Add context assetids.
        Channels::includeSystem('Context');
        $assetids = Context::getAssetids($assetids);

        foreach ($dataSources as $dataSource => $attributes) {
            Channels::includeSystem($dataSource);
            $dsAttributes = array();
            $dsAttrTypes  = array();
            foreach ($attributes as $attribute) {
                $dsAttributes[] = $attribute[0];
                $dsAttrTypes[]  = $attribute[1];
            }

            // Merge arrays to get results from each data source.
            $values += call_user_func(array($dataSource, 'batchGetAttributeValue'), $assetids, $dsAttributes, $dsAttrTypes);
        }

        return $values;

    }//end batchGetAttributeValue()


    /**
     * Groups specified attributes by their data source.
     *
     * @param array $attributes Array of attributes (assetype => attrid).
     *
     * @return array
     */
    public static function groupAttributeDataSources(array $attributes)
    {
        $dataSources = array();
        foreach ($attributes as $assetType => $attributeid) {
            $dataSource = Configuration::getAttributeDataSource(
                $attributeid,
                $assetType
            );
            if (isset($dataSources[$dataSource]) === FALSE) {
                $dataSources[$dataSource] = array();
            }

            $dataSources[$dataSource][] = $attributeid;
        }

        return $dataSources;

    }//end groupAttributeDataSources()


    /**
     * Returns the data source for a specified attribute.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $assetType   Type of the asset.
     *
     * @return string
     */
    public static function getAttributeDataSource($attributeid, $assetType)
    {
        $dataSource = DAL::executeOne(
            'Configuration',
            'getAttributeDataSource'
        );
        return $dataSource;

    }//end getAttributeDataSource()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * The attribute is examined and the processing passed to the relevant data
     * source. The value is provided as a PHP data type and will be converted
     * for storage by the data source.
     *
     * If attribute value is not set yet then data source will add it.
     *
     * @param integer $assetid     The asset to set the attribute for.
     * @param string  $attributeid The id of the attribute to set.
     * @param string  $value       The new value to set for the attribute.
     *
     * @see    DataSource::attributeValueExists()
     * @see    DataSource::setAttributeValue()
     * @see    DataSource::isUniqueAttributeValue()
     * @return boolean
     *
     * @throws ChannelException If the configuration attribute is not
     *                          found for the asset.
     */
    public static function setAttributeValue($assetid, $attributeid, $value)
    {
        try {
            $attrInfo = Configuration::getAssetAttributeInfo(
                $assetid,
                $attributeid
            );
        } catch (ChannelException $e) {
            $msg  = 'Configuration: '.$attributeid.' not found for Asset: ';
            $msg .= $assetid;
            throw new ChannelException($msg);
        }

        Channels::includeSystem('Context');
        $assetid = Context::getAssetid($assetid);

        $attributeid = $attrInfo['attributeid'];
        $assetType   = $attrInfo['asset_type'];
        $attrType    = $attrInfo['attribute_type'];
        $dataSource  = $attrInfo['data_source'];
        $isUnique    = $attrInfo['is_unique'];
        $initialVal  = $attrInfo['initial_val'];
        $defaultVal  = $attrInfo['default_val'];
        $required    = $attrInfo['required'];

        Channels::includeSystem($dataSource);

        $existsMethod = 'attributeValueExists';
        $existsCall   = array(
                         $dataSource,
                         $existsMethod,
                        );
        $existsParams = array(
                         'assetid'     => $assetid,
                         'attributeid' => $attributeid,
                         'attrType'    => $attrType,
                         'value'       => $value,
                        );

        $existsResult = call_user_func_array($existsCall, $existsParams);

        $setMethod = 'setAttributeValue';
        $setParams = array(
                      'assetid'     => $assetid,
                      'attributeid' => $attributeid,
                      'attrType'    => $attrType,
                      'value'       => $value,
                     );

        // We should add a new value, rather than updating an existing one.
        if ($existsResult === FALSE) {
            $setMethod              = 'addAttributeValue';
            $setParams['isDefault'] = 'f';
        }

        $setCall = array(
                    $dataSource,
                    $setMethod,
                   );

        // This is not a unique attribute, set and forget...
        if ($isUnique !== TRUE) {
            return call_user_func_array($setCall, $setParams);
        }

        $uniqueMethod = 'isUniqueAttributeValue';
        $uniqueCall   = array(
                         $dataSource,
                         $uniqueMethod,
                        );
        $uniqueParams = array(
                         $assetType,
                         $attributeid,
                         $attrType,
                         $value,
                        );

        $uniqueResult = call_user_func_array($uniqueCall, $uniqueParams);

        // This value has already been set for this attribute.
        if ($uniqueResult !== TRUE) {
            $msg  = 'Unique constraint for attribute "'.$attributeid;
            $msg .= '" violated';
            throw new ChannelException($msg);
        }

        $setResult    = call_user_func_array($setCall, $setParams);
        $uniqueResult = call_user_func_array($uniqueCall, $uniqueParams);

        // Success if the only value is the current asset.
        if ($uniqueResult === $assetid) {
            return TRUE;
        }

        /*
            Something bad has happenend, either the value wasn't inserted
            or more than one of the same value has been inserted...

            Recover somehow.
        */

        $msg = 'Unique constraint for attribute "'.$attributeid.'" violated';
        throw new ChannelException($msg);

    }//end setAttributeValue()


    /**
     * Sets the values of one of more attributes for one or more assets.
     *
     * The attributes are examined and the processing passed to the relevant
     * data sources.The values are provided as PHP data types and will be
     * converted for storage by the data source. The $attributeValues array
     * should be in the form array(attributeid => value).
     *
     * Throws ChannelException. Note this function assumes that all the
     * assetids specified are the same asset type.
     *
     * @param array $assetids        The assetids to set the attributes for.
     * @param array $attributeValues The attributes to set. The key represents
     *                               the attrbiuteid and the value is the
     *                               attribute's value.
     *
     * @return boolean
     *
     * @throws ChannelException If there is an error setting the value
     *                          for the attribute.
     */
    public static function batchSetAttributeValue(
        array $assetids,
        array $attributeValues
    ) {
        // Assetids or attributeids cannot be empty.
        if ((empty($assetids) === TRUE) || (empty($attributeValues) === TRUE)) {
            return FALSE;
        }

        try {
            // Group attributes by their data source.
            $attributeInfo = array();
            $dataSources   = array();
            foreach ($attributeValues as $attributeid => $value) {
                $attributeInfo = Configuration::getAssetAttributeInfo(
                    $assetids[0],
                    $attributeid
                );
                if (isset($dataSources[$attributeInfo['data_source']]) === FALSE) {
                    Channels::includeSystem($attributeInfo['data_source']);
                    $dataSources[$attributeInfo['data_source']] = array();
                }

                $dataSources[$attributeInfo['data_source']][$attributeid] = $value;
            }

            // Send request to each data source to batchSet attr vals.
            Channels::includeSystem('Context');
            $assetids = Context::getAssetid($assetids);
            foreach ($dataSources as $dataSource => $attributeInfo) {
                call_user_func(array($dataSource, 'batchSetAttributeValue'), $assetids, $attributeInfo);
            }
        } catch (Exception $e) {
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }//end try

        return TRUE;

    }//end batchSetAttributeValue()


    /**
     * Add a new attribute to an existing asset type.
     *
     * The data type and data source of the attribute must be supplied, along
     * with asset type and name of the new attribute.
     *
     * @param string  $assetType     The type to add the attribute to.
     * @param string  $attributeid   The id of the new attribute.
     * @param string  $attributeType The type of the new attribute.
     * @param string  $dataSource    The datasource the attribute will use.
     * @param boolean $isUnique      True if this attribute is unique.
     * @param mixed   $initialVal    Initial value of the attribute.
     * @param mixed   $defaultVal    The default value for this attribute.
     * @param boolean $required      True if attribute is required a value.
     * @param integer $assetid       The ID of the asset where the attribute
     *                               was added. This asset and all it's
     *                               children will have this attribute.
     *
     * @return void
     * @throws ChannelException If there was an error adding the attribute
     *                          for the asset type.
     */
    public static function addAttribute(
        $assetType,
        $attributeid,
        $attributeType,
        $dataSource,
        $isUnique,
        $initialVal,
        $defaultVal,
        $required,
        $assetid
    ) {
        if (Configuration::dataSourceExists($dataSource) === FALSE) {
            $msg  = 'Failed to add attribute. Data source ('.$dataSource.')';
            $msg .= ' does not exist.';
            throw new ChannelException($msg);
        }

        try {
            DAL::beginTransaction();
            // Get child types for the specified asset type.
            Channels::includeSystem('Asset');
            $childTypes = Asset::getChildTypes($assetType);

            // For each child type add the attribute.
            foreach ($childTypes as $childType) {
                if (Configuration::attributeExists($attributeid, $childType['typeid']) === FALSE) {
                    Channels::modifyBasket('assetType', $childType['typeid']);
                    self::_addAttribute();
                }
            }

            // Add it to base type.
            Channels::modifyBasket('assetType', $assetType);
            self::_addAttribute();

            // Call data source's addAttribute<type> method.
            Channels::includeSystem($dataSource);
            $addMethod = 'addAttribute'.ucwords($attributeType);
            if (method_exists($dataSource, $addMethod) === TRUE) {
                call_user_func(array($dataSource, $addMethod), $attributeid, $assetType);
            }

            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            $msg = 'DBError: '.$e->getMessage().$e->getTraceAsString();
            throw new ChannelException($msg);
        }//end try

        // Put the original required value to basket.
        Channels::modifyBasket('required', $required);

    }//end addAttribute()


    /**
     * Executes the addAttribute Query.
     *
     * If this method is used in a loop then make sure you call modifyBasket to
     * update the attribute information.
     *
     * @return void
     */
    private static function _addAttribute()
    {
        DAL::executeQuery('Configuration', 'addAttribute');

    }//end _addAttribute()


    /**
     * Remove an existing attribute from an existing asset type.
     *
     * Only the attribute id and asset type needs to be supplied for this
     * action. Note that only non built-in attributes can be deleted.
     *
     * @param string $attributeid The id of the attribute to remove.
     * @param string $assetType   The type of the attribute to remove.
     *
     * @return integer
     * @throws ChannelException If the attribute was unable to be removed.
     */
    public static function removeAttribute($attributeid, $assetType)
    {
        // Get attribute's data source.
        $attr = DAL::executeAssoc('Configuration', 'getAttributeInfo');

        $rows = 0;
        // Remove attribute from the attribute table.
        try {
            DAL::beginTransaction();
            $rows = DAL::executeQuery('Configuration', 'removeAttribute');

            // Send request to data source to remove attribute values.
            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelException('DB Error:'.$e->getMessage());
        }

        return $rows;

    }//end removeAttribute()


    /**
     * Modify the ID of an attribute.
     *
     * When the ID is changed, the data source will need to be informed so that
     * it can re-assign the values it has stored from the old ID to the new ID.
     *
     * @param string $attributeid The ID of the attribute to modify.
     * @param string $assetType   The type of the attribute to update.
     * @param string $newId       The new ID of the attribute.
     *
     * @return void
     * @throws ChannelException If the attribute ID is unable to be updated.
     */
    public static function updateAttributeid($attributeid, $assetType, $newId)
    {
        try {
            DAL::beginTransaction();
            // Check that newid does not exist for asset type.
            if (($assetType === $newId)
                || (Configuration::attributeExists($newId, $assetType) === TRUE)
            ) {
                $msg = 'Cannot update attribute id to '.$newId;
                throw new ChannelException($msg);
            }

            // Get attribute's data source.
            $attr = array_shift(
                DAL::executeAssoc('Configuration', 'getAttributeInfo')
            );

            // Send request to the data source to update the attributeid.
            Channels::includeSystem($attr['data_source']);

            /*
                Due to foreign key restrictions we cannot update the
                attributeid. We need to create a new attribute, update assets
                and then remove the old attribute.
            */

            $isUnique = 'f';
            if ($attr['is_unique'] === TRUE) {
                $isUnique = 't';
            }

            // Add the new attribute.
            Configuration::addAttribute(
                $assetType,
                $newId,
                $attr['attribute_type'],
                $attr['data_source'],
                $isUnique,
                $attr['initial_val'],
                $attr['default_val'],
                $attr['required'],
                $attr['assetid']
            );

            // Update records.
            call_user_func(array($attr['data_source'], 'updateAttributeid'), $attributeid, $assetType, $attr['attribute_type'], $newId);

            // Remove the old attribute.
            Configuration::removeAttribute($attributeid, $assetType);

            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            $msg  = 'Failed to update attributeid from '.$attributeid;
            $msg .= ' to '.$newId.' for asset type '.$assetType;
            $msg .= '. Reason: '.$e->getMessage();
            throw new ChannelException($msg);
        }//end try

    }//end updateAttributeid()


    /**
     * Modify the asset type that an attribute is assigned to.
     *
     * This essentially moves the attribute from one type to another.
     * An attribute can only be moved up or down its current hierarchy. If it is
     * moved up, all assets of types that now contains this attribute must have
     * default values inserted. If it is moved down, all assets that no longer
     * has this attribute must have their values deleted.
     *
     * @param string $attributeid  The ID of the attribute to update.
     * @param string $assetType    The asset type.
     * @param string $newAssetType The new asset type to assign the attribute
     *                             type too.
     *
     * @return void
     * @throws ChannelException If the attribute doesn't exist or there is
     *                          a database error updating the attribute.
     */
    public static function updateAttributeAssetType(
        $attributeid,
        $assetType,
        $newAssetType
    ) {
        if ($assetType === $newAssetType) {
            // Dont do anything.
            return;
        }

        $attr = array_shift(
            DAL::executeAssoc('Configuration', 'getAttributeInfo')
        );
        if (empty($attr) === TRUE) {
            $msg = 'Attribute does not exist!';
            throw new ChannelException($msg);
        }

        Channels::includeSystem($attr['data_source']);

        try {
            // Determine if attribute is moved down or up.
            // If moving up then new asset type must be a parent of the
            // attribute.
            Channels::includeSystem('Asset');
            $result = Asset::getTypeLvl($assetType, $newAssetType);

            // If not up then new asset type must be a child of attribute.
            if ($result === 0) {
                $result = Asset::getTypeLvl($newAssetType, $assetType);
                if ($result !== 0) {
                    // Moving down.
                    // Send request to data source to delete values.
                    $res = call_user_func(array($attr['data_source'], 'removeAttributeValues'), $attributeid, $assetType, $attr['attribute_type']);

                    // Update the attribute table.
                    $oldTypesChildren = Asset::getChildTypes($assetType);
                    $newTypesChildren = Asset::getChildTypes($newAssetType);
                    foreach ($newTypesChildren as $nTypeInfo) {
                        foreach ($oldTypesChildren as $oTypeInfo) {
                            if ($oTypeInfo['typeid'] === $newAssetType) {
                                continue;
                            }

                            $found = FALSE;
                            if ($oTypeInfo['typeid'] !== $nTypeInfo['typeid']) {
                                Channels::modifyBasket(
                                    'assetType',
                                    $oTypeInfo['typeid']
                                );
                                DAL::executeQuery(
                                    'Configuration',
                                    'removeAttribute'
                                );
                            }
                        }
                    }

                    Channels::modifyBasket('assetType', $assetType);
                    DAL::executeQuery('Configuration', 'removeAttribute');
                } else {
                    DAL::rollBack();
                    // Cannot update asset type.
                    $msg  = 'Cannot update asset type of attribute "';
                    $msg .= $attributeid.'" from '.$assetType.' to ';
                    $msg .= $newAssetType;
                    throw new ChannelException($msg);
                }//end if
            } else {
                // Moving up.
                // AddAttribute will add the new attribute to its child types.
                Configuration::addAttribute(
                    $newAssetType,
                    $attributeid,
                    $attr['attribute_type'],
                    $attr['data_source'],
                    'f',
                    $attr['initial_val'],
                    $attr['default_val'],
                    $attr['required'],
                    $attr['assetid']
                );
                $res = call_user_func(array($attr['data_source'], 'addAssetTypeAttributeValue'), $attributeid, $newAssetType, $attr['attribute_type'], $attr['initial_val'], 'f');
            }//end if

            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelException('DBError: '.$e->getMessage());
        }//end try

    }//end updateAttributeAssetType()


    /**
     * Modifies the data type of an attribute, changing its storage method.
     *
     * When the data type is changed, the data source must be informed so that
     * it can modify the storage of the attribute values (eg. move them from one
     * DB table to another).
     *
     * @param string $attributeid The ID of the attribute to modify.
     * @param string $assetType   The asset type.
     * @param string $newDataType The data type to assign to the attribute.
     *
     * @return void
     * @throws ChannelException If the new data type is invalid for the
     *                          data source or if there is an error
     *                          updating the data type.
     */
    public static function updateAttributeDataType(
        $attributeid,
        $assetType,
        $newDataType
    ) {
        // Get attributes data source.
        $attr = array_shift(
            DAL::executeAssoc('Configuration', 'getAttributeInfo')
        );

        // Make sure new data type is valid for the data source.
        Channels::includeSystem($attr['data_source']);
        $valid = call_user_func(array($attr['data_source'], 'isValidAttributeType'), $newDataType);
        if ($valid === FALSE) {
            $msg  = 'New data type '.$newDataType.' is not valid for data ';
            $msg .= 'source '.$attr['data_source'];
            throw new ChannelException($msg);
        }

        // New data type is valid, make the changes.
        try {
            DAL::beginTransaction();
            // Update the attribute table.
            DAL::executeQuery('Configuration', 'updateAttributeType');

            // Send request to the data source.
            Channels::includeSystem($attr['data_source']);
            call_user_func(array($attr['data_source'], 'updateAttributeDataType'), $attributeid, $assetType, $attr['attribute_type'], $newDataType);

            // Save the changes.
            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelException('DBError: '.$e->getMessage());
        }

    }//end updateAttributeDataType()


    /**
     * Modifies the data source of an attribute, changing its storage method.
     *
     * When the data source is changed, the attribute system must fetch all
     * stored values from the old data source and insert them into the new data
     * source, before removing all values from the old data source.
     *
     * @param string $attributeid   The ID of the attribute to modify.
     * @param string $assetType     The type of the asset the attribute applies
     *                              to.
     * @param string $newDataSource The new data source to store the attribute
     *                              with.
     *
     * @return boolean
     * @throws ChannelException If the attribute doesn't exist for the asset type.
     */
    public static function updateAttributeDataSource(
        $attributeid,
        $assetType,
        $newDataSource
    ) {
        $attrInfo = DAL::executeAssoc('Configuration', 'getAttributeInfo');
        if (empty($attrInfo) === TRUE) {
            $msg  = 'Attribute "'.$attributeid.'" does not exist for "';
            $msg .= $assetType.'"';
            throw new ChannelException($msg);
        }

        $attrInfo   = current($attrInfo);
        $dataSource = $attrInfo['data_source'];
        $attrType   = $attrInfo['attribute_type'];
        if ($dataSource === $newDataSource) {
            // Already in this data source. Return TRUE.
            return TRUE;
        }

        Channels::includeSystem($dataSource);
        // Arguments to pass to Data Source action.
        $args = array(
                 'assetid'     => $assetType,
                 'attributeid' => $attributeid,
                 'attrType'    => $attrType,
                );

        // Class and method to call.
        $methodArray = array(
                        $dataSource,
                        'getAllAttributeValues',
                       );

        // Current Values will contain all current values for the attribute.
        $currentValues = call_user_func_array($methodArray, $args);
        // Channels::includeSystem($newDataSource);
        // The appropriate data source system included above, and values to
        // insert gathered, now loop through the values, and insert them into
        // the new data source.
        DAL::beginTransaction();
        // Insert each of the currentValues into the new data source.
        foreach ($currentValues as $data) {
            $assetid = $data['assetid'];
            $value   = $data['value'];
            $args    = array(
                        'assetid'     => $assetid,
                        'attributeid' => $attributeid,
                        'value'       => $value,
                        'attrType'    => $attrType,
                       );

            // Call setAttributeValue().
            $methodArray = array(
                            $newDataSource,
                            'setAttributeValue',
                           );

            $setReturn = TRUE;
            try {
                // TODO: What's going on here?
                // $setReturn = call_user_func_array($methodArray, $args);.
            } catch (ChannelException $e) {
                DAL::rollBack();
                throw new ChannelException($e->getMessage());
            }

            if ($setReturn === FALSE) {
                // An insert failed, rollback and return FALSE.
                DAL::rollBack();
                return FALSE;
            }
        }//end foreach

        // If execution has reached here, the insertion into the new data source
        // must have succeeded, so we need to update the attribute table.
        if (DAL::executeQuery('Configuration', 'setAttributeDataSource') === 0) {
            DAL::rollBack();
            return FALSE;
        }

        // Remove the values from the original data source.
        $removeMethod = array(
                         $dataSource,
                         'removeAttributeValues',
                        );
        $removeArgs   = array(
                         'attributeid'   => $attributeid,
                         'assetType'     => $assetType,
                         'attributeType' => $attrType,
                        );

        try {
            $removeRet = call_user_func_array($removeMethod, $removeArgs);
        } catch (ChannelException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }

        DAL::Commit();

        return TRUE;

    }//end updateAttributeDataSource()


    /**
     * Applies or removes uniqueness of an attribute.
     *
     * This action makes the attribute unique across the whole system, or
     * removes its uniqueness. If applying uniqueness, all existing values must
     * be checked to ensure they are unique. If they are not, uniqueness can not
     * be applied.
     *
     * Retruns true on success, false if no changes were made.
     *
     * @param string  $attributeid The ID of the attribute to modify.
     * @param string  $assetType   Type of the assets.
     * @param boolean $unique      TRUE if the attribute is to be made unique,
     *                             FALSE if the attribute's uniqueness is being
     *                             removed.
     *
     * @return boolean
     */
    public static function updateAttributeUniqueness(
        $attributeid,
        $assetType,
        $unique
    ) {
        // First check if this attribute is unique or not.
        $result = array_shift(
            DAL::executeAssoc('Configuration', 'getAttributeInfo')
        );
        if ((int) $unique !== (int) $result['is_unique']) {
            if ($unique === TRUE) {
                $attrType = $result['attribute_type'];

                // Send request to Data Source to check uniqueness of values.
                Channels::includeSystem($result['data_source']);
                $isUnique = call_user_func(array($result['data_source'], 'attributeValsUnique'), $attributeid, $assetType, $attrType);
                if ($isUnique === TRUE) {
                    // All values are unique update the attribute uniqueness.
                    if (DAL::executeQuery('Configuration', 'setAttributeUniqueness') !== 0) {
                        return TRUE;
                    }
                }
            } else {
                // Attribute value is no longer unique just update table.
                if (DAL::executeQuery('Configuration', 'setAttributeUniqueness') !== 0) {
                    return TRUE;
                }
            }
        }

        return FALSE;

    }//end updateAttributeUniqueness()


    /**
     * Update the initial value for an existing attribute.
     *
     * The attribute ID, the asset type, and the new initial value must be
     * supplied. Supplying NULL for $newValue will result in the initial value
     * being removed.
     *
     * @param string  $attributeid The ID of the attribute to modify.
     * @param boolean $assetType   The asset type of the attribute in question.
     * @param boolean $newValue    The new initial value.
     *
     * @return integer
     * @throws ChannelException Error occurred during updating attribute initial value.
     */
    public static function updateAttributeInitialValue(
        $attributeid,
        $assetType,
        $newValue
    ) {
        $rows = 0;
        try {
            DAL::beginTransaction();
            $rows = DAL::executeQuery(
                'Configuration',
                'updateAttributeInitialValue'
            );
            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new ChannelException('DB Error: '.$e->getMessage());
        }

        return $rows;

    }//end updateAttributeInitialValue()


    /**
     * Returns list of attributes for a specific asset type.
     *
     * This function should be used at runtime, it sources the attribute
     * list from the database and will include inherited attributes.
     *
     * Returned array structure:
     * (attributeid)[type]
     *              [data_source]
     *              [unique]
     *              [initial_val]
     *
     * @param string $assetType Type of the asset.
     *
     * @return array
     */
    public static function getAttributes($assetType)
    {
        $attributeids = DAL::getQuery(
            'Configuration',
            'getAttributesByAssetType'
        );
        Channels::addToBasket('attributeids', $attributeids);

        $attributesData = DAL::executeAssoc(
            'Configuration',
            'batchGetAttributeInfo'
        );
        $attributesInfo = array();

        foreach ($attributesData as $attributeData) {
            // For the time being lets mimic the output format
            // of readAttributes().
            $attributeInfo = array(
                              'type'        => $attributeData['attribute_type'],
                              'data_source' => $attributeData['data_source'],
                              'unique'      => $attributeData['is_unique'],
                              'required'    => $attributeData['required'],
                              'default_val' => $attributeData['default_val'],
                              'assetid'     => $attributeData['assetid'],
                              'initial_val' => $attributeData['initial_val'],
                             );

            $attributesInfo[$attributeData['attributeid']] = $attributeInfo;
        }//end foreach

        return $attributesInfo;

    }//end getAttributes()


    /**
     * Returns list of attributes for a specific asset type.
     *
     * This function is only used at install time, it sources the
     * attribute list from the attributes.xml file for an asset type
     * and does not include any inherited attributes.
     *
     * Returned array structure:
     * (attributeid)[type]
     *              [data_source]
     *              [unique]
     *              [initial_val]
     *
     * @param string $assetType Type of the asset.
     *
     * @return array
     * @throws ChannelException If the attribute file doesn't exist for the asset type.
     */
    public static function readAttributes($assetType)
    {
        $attributes = array();
        // Read the attributes.xml file for the given system.
        Channels::includeSystem('Asset');
        $path = Asset::getTypeDirPath($assetType).'/attributes.xml';
        if (file_exists($path) === TRUE) {
            $doc = new DomDocument();
            $doc->load($path);

            if ($doc !== NULL) {
                $attrs = $doc->getElementsByTagName('attribute');
                foreach ($attrs as $attribute) {
                    $id              = $attribute->getAttribute('id');
                    $attributes[$id] = array();

                    // Info for this attribute.
                    $attributes[$id]['type']        = $attribute->getAttribute('type');
                    $attributes[$id]['data_source'] = $attribute->getAttribute('data_source');
                    $attributes[$id]['unique']      = $attribute->getAttribute('unique');
                    $attributes[$id]['required']    = $attribute->getAttribute('required');
                    $attributes[$id]['default_val'] = $attribute->getAttribute('default_val');
                    $attributes[$id]['assetid']     = $attribute->getAttribute('assetid');

                    // Get the initial value if there is one.
                    $value      = '';
                    $initialVal = $attribute->getElementsByTagName('initial_value')->item(0);
                    if ($initialVal !== NULL) {
                        $value = $initialVal->nodeValue;
                    } else {
                        $value = NULL;
                    }

                    $attributes[$id]['initial_val'] = $value;
                }//end foreach
            }//end if
        } else {
            $msg = 'Attribute file for '.$assetType.' does not exist!';
            throw new ChannelException($msg);
        }//end if

        return $attributes;

    }//end readAttributes()


    /**
     * Checks if the given attribute exists.
     *
     * @param string $attributeid Attribute's id.
     * @param string $assetType   Asset type that specified attribute
     *                            belongs to.
     *
     * @return boolean
     */
    public static function attributeExists($attributeid, $assetType)
    {
        $result = DAL::executeOne('Configuration', 'attributeExists');
        if (empty($result) === FALSE) {
            return TRUE;
        }

        return FALSE;

    }//end attributeExists()


    /**
     * Verifies that the specified data source exists.
     *
     * @param string $dataSource Name of the data source.
     *
     * @return boolean
     */
    public static function dataSourceExists($dataSource)
    {
        $dataSourcePath = Configuration::getDataSourcePath($dataSource);
        if (file_exists($dataSourcePath) === TRUE) {
            return is_dir($dataSourcePath);
        }

        return FALSE;

    }//end dataSourceExists()


    /**
     * Retrieves the path of a data source.
     *
     * @param string $dataSource Name of the data source.
     *
     * @return boolean
     */
    public static function getDataSourcePath($dataSource)
    {
        return Channels::getSystemsPath($dataSource);

    }//end getDataSourcePath()


    /**
     * Returns the data sources for multiple attributes.
     *
     * The result is an array of (attributeid => data_source).
     *
     * @param array|Query $assetTypes   Type of the asset.
     * @param array|Query $attributeids Id of the attribute.
     *
     * @return array
     */
    public static function batchGetAttributeDataSource(
        $assetTypes,
        $attributeids
    ) {
        $dataSources = Configuration::batchGetAttributeDataSourceQc(
            $assetTypes,
            $attributeids
        );
        $dataSources = DAL::executeDALQuery($dataSources);
        return $dataSources;

    }//end batchGetAttributeDataSource()


    /**
     * Read configuration from an asset types configuration.xml file.
     *
     * @param string $assetType The asset type to read configuration for.
     *
     * @return array
     */
    public static function readAssetTypeConfiguration($assetType)
    {
        $assetConfigs = array();
        // Read the config.xml file for the given system.
        Channels::includeSystem('Asset');
        $path = Asset::getTypeDirPath($assetType).'/configuration.xml';
        if (file_exists($path) === TRUE) {
            $doc = new DomDocument();
            $doc->load($path);

            if ($doc !== NULL) {
                $configs = $doc->getElementsByTagName('config');
                foreach ($configs as $config) {
                    $id                = $config->getAttribute('id');
                    $assetConfigs[$id] = array();

                    // Information for this asset config.
                    $assetConfigs[$id]['type']        = $config->getAttribute('type');
                    $assetConfigs[$id]['unique']      = $config->getAttribute('unique');
                    $assetConfigs[$id]['required']    = $config->getAttribute('required');
                    $assetConfigs[$id]['default_val'] = $config->getAttribute('default_val');
                    $assetConfigs[$id]['assetid']     = $config->getAttribute('assetid');
                    $assetConfigs[$id]['dataSource']  = $config->getAttribute('dataSource');

                    if (empty($assetConfigs[$id]['dataSource']) === TRUE) {
                        // If they haven't specified a data source, assume we're
                        // using the one intended for configurations.
                        $assetConfigs[$id]['dataSource'] = 'ConfigurationDataSource';
                    }

                    // Get the initial value if there is one.
                    $value      = NULL;
                    $initialVal = $config->getElementsByTagName('initial_value')->item(0);
                    if ($initialVal !== NULL) {
                        $value = $initialVal->nodeValue;
                    }

                    $assetConfigs[$id]['initial_val'] = $value;
                }//end foreach
            }//end if
        }//end if

        return $assetConfigs;

    }//end readAssetTypeConfiguration()


    /**
     * Install configuration for the specified asset type.
     *
     * @param string $assetType The type of asset to install configuration for.
     *
     * @return void
     * @throws ChannelException If the configuration already exists for the asset type.
     */
    public static function installAssetTypeConfiguration($assetType)
    {
        $assetTypeConfiguration = Configuration::readAssetTypeConfiguration(
            $assetType
        );

        foreach ($assetTypeConfiguration as $configurationid => $configuration) {
            // Fail if we've encountered the same configuration for this
            // asset type previously.
            if (Configuration::attributeExists($configurationid, $assetType) === TRUE) {
                $msg  = 'Configuration "'.$configurationid.'" already exists ';
                $msg .= 'asset type "'.$assetType.'".';
                throw new ChannelException('Install Error: '.$msg);
            }

            // Add the configuration to the system.
            Configuration::addAttribute(
                $assetType,
                $configurationid,
                $configuration['type'],
                $configuration['dataSource'],
                $configuration['unique'],
                $configuration['initial_val'],
                $configuration['default_val'],
                $configuration['required'],
                $configuration['assetid']
            );
        }//end foreach

    }//end installAssetTypeConfiguration()


    /**
     * Plug (Runner) for Configuration::installAssetTypeConfiguration() actions.
     *
     * @param string $assetType The type of asset to install configuration for.
     *
     * @return void
     */
    public static function installConfiguration($assetType)
    {
        Channels::includeSystem('Configuration');
        Configuration::installAssetTypeConfiguration($assetType);

    }//end installConfiguration()


    /**
     * Returns batchGetAttributeDataSource Query object.
     *
     * @param string $assetTypes   List of asset types.
     * @param string $attributeids List of attributes.
     *
     * @return object
     */
    public static function batchGetAttributeDataSourceQc(
        $assetTypes,
        $attributeids
    ) {
        $baseQuery = DAL::getDALQuery(
            'Configuration',
            'batchGetAttributeDataSource',
            'Grouped'
        );
        return $baseQuery;

    }//end batchGetAttributeDataSourceQc()


}//end class

?>
