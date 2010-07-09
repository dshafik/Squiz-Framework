<?php
/**
 * Actions for the Attribute System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Attribute
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/DAL.inc';

/**
 * Attribute Actions Class.
 *
 * @since 4.0.0
 */
class AttributeActions
{

    /**
     * Cache for the attribute values.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_attrVals = array();


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     *
     * @since 4.0.0
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
     * @api           read
     * @api-privilege asset.read.content
     * @api-mustlock  false
     * @since         4.0.0
     * @return        mixed
     * @throws        ChannelException If the attribute is not found for the
     *                                 passed asset.
     */
    public static function getAttributeValue($assetid, $attributeid)
    {
        Channels::includeSystem('Context');
        $infoAssetid = $assetid;
        $cacheid     = Context::getAssetid($assetid, NULL, TRUE);
        $assetid     = Context::getAssetids($assetid);

        if (isset(self::$_attrVals[$cacheid]) === TRUE
            && isset(self::$_attrVals[$cacheid][$attributeid]) === TRUE
        ) {
            return self::$_attrVals[$cacheid][$attributeid];
        } else {
            try {
                $attrInfo = Attribute::getAssetAttributeInfo(
                    $infoAssetid,
                    $attributeid
                );
            } catch (ChannelException $e) {
                $msg  = 'Attribute: '.$attributeid.' not found for Asset: ';
                $msg .= $assetid;
                throw new ChannelException($msg);
            }

            if (empty($attrInfo) === TRUE) {
                // The attribute does not exist, so return a blank string
                // by default as we don't know what type the attribute should
                // be returning.
                return '';
            }

            // Now retrieve the data source of the attribute, and include its
            // source file.
            $dataSource = $attrInfo['data_source'];

            Channels::includeSystem($dataSource);
            $attributeType = $attrInfo['attribute_type'];
            // Arguments to pass to Data Source action.
            $args = array(
                     'assetid'     => $assetid,
                     'attributeid' => $attributeid,
                     'attrType'    => $attributeType,
                    );

            $value = call_user_func_array(
                array(
                 $dataSource,
                 'getAttributeValue',
                ),
                $args
            );

            self::$_attrVals[$cacheid][$attributeid] = $value;
        }//end if

        return $value;

    }//end getAttributeValue()


    /**
     * Retrieves the attribute info using the specified assetid.
     *
     * If the attribute does not exist for the asset, an
     * empty array is returned.
     *
     * @param integer $assetid     Asset's id.
     * @param string  $attributeid Attribute id.
     *
     * @since  4.0.0
     * @return array
     * @throws ChannelException If there is an error retrieving the
     *                                attribute info.
     */
    public static function getAssetAttributeInfo($assetid, $attributeid)
    {
        try {
            Channels::includeSystem('Context');
            Channels::modifyBasket('assetid', Context::getMasterid($assetid));
            $attrInfo = DAL::getQuery(
                'Attribute',
                'getAttributeInfoUsingAssetid'
            );
            $attrInfo = DAL::getAssoc($attrInfo);
            if (empty($attrInfo) === TRUE) {
                return array();
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
     * @since  4.0.0
     * @return array
     * @throws ChannelException If there is an error retrieving the
     *                                attribute.
     */
    public static function getAssetTypeAttributeInfo($assetType, $attributeid)
    {
        $attrInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
        if (empty($attrInfo) === TRUE) {
            $msg = 'Attribute: Error Retrieving Attribute.';
            throw new ChannelException($msg);
        }

        $attrInfo = current($attrInfo);
        return $attrInfo;

    }//end getAssetTypeAttributeInfo()


    /**
     * Gets any assets that have the specified value for specified attribute.
     *
     * @param string  $assetType   The type of asset.
     * @param string  $attributeid Attribute id.
     * @param mixed   $value       Value to use for lookup.
     * @param boolean $decendants  If TRUE assets of a type that is a
     *                             descendant of $assetType will be included.
     *
     * @since  4.0.0
     * @return array
     * @throws ChannelException If the attribute is not found for the
     *                                asset type.
     */
    public static function getAssetsByAttributeValue(
        $assetType,
        $attributeid,
        $value,
        $decendants=FALSE
    ) {
        try {
            $attrInfo = Attribute::getAssetTypeAttributeInfo(
                $assetType,
                $attributeid
            );
        } catch (ChannelException $e) {
            $msg  = 'Attribute: '.$attributeid.' not found for: '.$assetType;
            $msg .= ' asset type.';
            throw new ChannelException($msg);
        }

        Channels::includeSystem('AssetType');
        Channels::includeSystem($attrInfo['data_source']);

        if ($decendants === TRUE) {
            $types = AssetType::getChildTypes($assetType);

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

        $assets = call_user_func_array($getCall, $getParams);
        return $assets;

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
     * @since  4.0.0
     * @return array
     */
    public static function batchGetAttributeValue(
        $assetids,
        array $attributeids
    ) {
        if (is_array($assetids) === TRUE) {
            // If assetids is an array, it may contain remote ids.
            $safeIds = array();
            foreach ($assetids as $assetid) {
                if (strpos($assetid, '~') === FALSE) {
                    $safeIds[] = $assetid;
                }
            }

            $assetids = $safeIds;
            if (empty($assetids) === TRUE) {
                return array();
            } else {
                Channels::addToBasket('assetids', $assetids, TRUE);
            }
        }

        Channels::includeSystem('Asset');
        Channels::includeSystem('AssetType');

        $assetTypes  = Asset::batchGetAssetType($assetids);
        $ancestors   = AssetType::batchGetTypeAncestors($assetTypes, TRUE);
        $dataSources = Attribute::batchGetAttributeDataSource(
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
                if (isset($dsAttrTypes[$attribute[1]]) === FALSE) {
                    $dsAttrTypes[$attribute[1]] = array();
                }

                $dsAttrTypes[$attribute[1]][] = $attribute[0];
            }

            // Merge arrays to get results from each data source.
            $values += call_user_func(
                array(
                 $dataSource,
                 'batchGetAttributeValue',
                ),
                $assetids,
                $dsAttributes,
                $dsAttrTypes
            );
        }//end foreach

        return $values;

    }//end batchGetAttributeValue()


    /**
     * Groups specified attributes by their data source.
     *
     * @param array $attributes Array of attributes (assetype => attrid).
     *
     * @since  4.0.0
     * @return array
     */
    public static function groupAttributeDataSources(array $attributes)
    {
        $dataSources = array();
        foreach ($attributes as $assetType => $attributeid) {
            $dataSource = Attribute::getAttributeDataSource(
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
     * @since  4.0.0
     * @return string
     */
    public static function getAttributeDataSource($attributeid, $assetType)
    {
        $dataSource = DAL::executeOne('Attribute', 'getAttributeDataSource');
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
     * @api           write
     * @api-privilege asset.edit.content
     * @api-mustlock  false
     * @since         4.0.0
     * @see           DataSource::attributeValueExists()
     * @see           DataSource::setAttributeValue()
     * @see           DataSource::isUniqueAttributeValue()
     * @return        boolean
     * @throws        ChannelException If the attribute is not found for the
     *                                 asset.
     * @throws        ChannelException If there is a violation of the attribute's
     *                                 constraints.
     */
    public static function setAttributeValue($assetid, $attributeid, $value)
    {
        try {
            $attrInfo = Attribute::getAssetAttributeInfo(
                $assetid,
                $attributeid
            );
        } catch (ChannelException $e) {
            $msg = 'Attribute: '.$attributeid.' not found for Asset: '.$assetid;
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
            if (call_user_func_array($setCall, $setParams) === TRUE) {
                // Update cache.
                if (isset(self::$_attrVals[$assetid]) === TRUE
                    && isset(self::$_attrVals[$assetid][$attributeid]) === TRUE
                ) {
                    self::$_attrVals[$assetid][$attributeid] = $value;
                }

                return TRUE;
            } else {
                return FALSE;
            }
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
        if ($uniqueResult !== TRUE && (string) $uniqueResult !== (string) $assetid) {
            $msg = 'Unique constraint for attribute "'.$attributeid.'" violated';
            throw new ChannelException($msg);
        }

        $setResult    = call_user_func_array($setCall, $setParams);
        $uniqueResult = call_user_func_array($uniqueCall, $uniqueParams);

        // Success if the only value is the current asset.
        if ($uniqueResult === TRUE || (string) $uniqueResult === (string) $assetid) {
            // Update cache.
            if (isset(self::$_attrVals[$assetid]) === TRUE
                && isset(self::$_attrVals[$assetid][$attributeid]) === TRUE
            ) {
                self::$_attrVals[$assetid][$attributeid] = $value;
            }

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
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If there is an error setting any of the
     *                                attribute values.
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
                $attributeInfo = Attribute::getAssetAttributeInfo(
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
     * @param boolean $isCustom      True if attribute is a custom attribute.
     * @param mixed   $system        NULL or system name. If the value is a
     *                               string of system name, then it is treated
     *                               as system attribute.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If the data source doesn't exist, or if
     *                                there is an error adding the attribute.
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
        $assetid,
        $isCustom=FALSE,
        $system=NULL
    ) {
        if (Attribute::dataSourceExists($dataSource) === FALSE) {
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
                if (Attribute::attributeExists($attributeid, $childType['typeid']) === FALSE) {
                    Channels::modifyBasket('assetType', $childType['typeid']);

                    // Added attribute can be a custom. However, it is not
                    // a custom attribute for child types.
                    Channels::modifyBasket('isCustom', FALSE);
                    self::_addAttribute();
                }
            }

            // Add it to base type.
            Channels::modifyBasket('assetType', $assetType);
            Channels::modifyBasket('isCustom', $isCustom);
            Channels::modifyBasket('system', $system);
            self::_addAttribute();

            // Call data source's addAttribute<type> method.
            Channels::includeSystem($dataSource);
            $addMethod = 'addAttribute'.ucwords($attributeType);
            if (method_exists($dataSource, $addMethod) === TRUE) {
                call_user_func(
                    array(
                     $dataSource,
                     $addMethod,
                    ),
                    $attributeid,
                    $assetType
                );
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
     * @since  4.0.0
     * @return void
     */
    private static function _addAttribute()
    {
        DAL::executeQuery('Attribute', 'addAttribute');

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
     * @since  4.0.0
     * @return integer
     * @throws ChannelException If the attribute is not successfully
     *                                removed.
     */
    public static function removeAttribute($attributeid, $assetType)
    {
        // Get attribute's data source.
        $attr = DAL::executeAssoc('Attribute', 'getAttributeInfo');

        $rows = 0;
        // Remove attribute from the attribute table.
        try {
            DAL::beginTransaction();
            $rows = DAL::executeQuery('Attribute', 'removeAttribute');

            // Remove the attribute from the child types.
            Channels::includeSystem('Asset');
            $childTypes = Asset::getChildTypes($assetType);
            foreach ($childTypes as $childType) {
                Attribute::removeAttribute($attributeid, $childType['typeid']);
            }

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
     * @since  4.0.0
     * @return void
     * @throws ChannelException If the attribute ID is unable to be
     *                                succesfully updated.
     */
    public static function updateAttributeid($attributeid, $assetType, $newId)
    {
        try {
            DAL::beginTransaction();
            // Check that newid does not exist for asset type.
            if (($assetType === $newId)
                || (Attribute::attributeExists($newId, $assetType) === TRUE)
            ) {
                $msg = 'Cannot update attribute id to '.$newId;
                throw new ChannelException($msg);
            }

            // Get attribute's data source.
            $attributeInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
            $attr          = array_shift($attributeInfo);

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
            Attribute::addAttribute(
                $assetType,
                $newId,
                $attr['attribute_type'],
                $attr['data_source'],
                $isUnique,
                $attr['initial_val'],
                $attr['default_val'],
                $attr['required'],
                $attr['assetid'],
                $attr['is_custom']
            );

            // Update records.
            call_user_func(
                array(
                 $attr['data_source'],
                 'updateAttributeid',
                ),
                $attributeid,
                $assetType,
                $attr['attribute_type'],
                $newId
            );

            // Remove the old attribute.
            Attribute::removeAttribute($attributeid, $assetType);

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
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If the attribute doesn't exist.
     */
    public static function updateAttributeAssetType(
        $attributeid,
        $assetType,
        $newAssetType
    ) {
        if ($assetType === $newAssetType) {
            // Dont do anything.
            return FALSE;
        }

        $attr = array_shift(DAL::executeAssoc('Attribute', 'getAttributeInfo'));
        if (empty($attr) === TRUE) {
            $msg = 'Attribute does not exist!';
            throw new ChannelException($msg);
        }

        Channels::includeSystem($attr['data_source']);

        try {
            // Determine if attribute is moved down or up.
            // If moving up then new asset type must be a parent of the
            // attribute.
            Channels::includeSystem('AssetType');
            $result = AssetType::getTypeLvl($assetType, $newAssetType);

            // If not up then new asset type must be a child of attribute.
            if ($result === 0) {
                $result = AssetType::getTypeLvl($newAssetType, $assetType);
                if ($result !== 0) {
                    // Moving down.
                    // Send request to data source to delete values.
                    $res = call_user_func(
                        array(
                         $attr['data_source'],
                         'removeAttributeValues',
                        ),
                        $attributeid,
                        $assetType,
                        $attr['attribute_type']
                    );

                    // Update the attribute table.
                    $oldTypesChildren = AssetType::getChildTypes($assetType);
                    $newTypesChildren = AssetType::getChildTypes($newAssetType);
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
                                    'Attribute',
                                    'removeAttribute'
                                );
                            }
                        }
                    }

                    Channels::modifyBasket('assetType', $assetType);
                    DAL::executeQuery('Attribute', 'removeAttribute');
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
                Attribute::addAttribute(
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
     * @since  4.0.0
     * @return void
     * @throws ChannelException If the data type is invalid for the data
     *                                source or if there is an error updating
     *                                data type.
     */
    public static function updateAttributeDataType(
        $attributeid,
        $assetType,
        $newDataType
    ) {
        // Get attributes data source.
        $attr = array_shift(DAL::executeAssoc('Attribute', 'getAttributeInfo'));

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
            DAL::executeQuery('Attribute', 'updateAttributeType');

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
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If the attribute doesn't exist for the
     *                                asset ID or if there is a problem removing
     *                                the old attribute data source.
     */
    public static function updateAttributeDataSource(
        $attributeid,
        $assetType,
        $newDataSource
    ) {
        $attrInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
        if (empty($attrInfo) === TRUE) {
            $msg = 'Attribute '.$attributeid.' does not exist for '.$assetType;
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
                // TODO: What's happening here?
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
        if (DAL::executeQuery('Attribute', 'setAttributeDataSource') === 0) {
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
     * @since  4.0.0
     * @return boolean
     */
    public static function updateAttributeUniqueness(
        $attributeid,
        $assetType,
        $unique
    ) {
        // First check if this attribute is unique or not.
        $attrInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
        $result   = array_shift($attrInfo);
        if ((int) $unique !== (int) $result['is_unique']) {
            if ($unique === TRUE) {
                $attrType = $result['attribute_type'];

                // Send request to Data Source to check uniqueness of values.
                Channels::includeSystem($result['data_source']);
                $isUnique = call_user_func(array($result['data_source'], 'attributeValsUnique'), $attributeid, $assetType, $attrType);
                if ($isUnique === TRUE) {
                    // All values are unique update the attribute uniqueness.
                    if (DAL::executeQuery('Attribute', 'setAttributeUniqueness') !== 0) {
                        return TRUE;
                    }
                }
            } else {
                // Attribute value is no longer unique just update table.
                if (DAL::executeQuery('Attribute', 'setAttributeUniqueness') !== 0) {
                    return TRUE;
                }
            }
        }

        return FALSE;

    }//end updateAttributeUniqueness()


    /**
     * Sets the required field of the specified attribute.
     *
     * @param string  $attributeid The ID of the attribute to modify.
     * @param string  $assetType   Type of the assets.
     * @param boolean $required    If TRUE then the attribute must have a value.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function updateAttributeRequired(
        $attributeid,
        $assetType,
        $required
    ) {
        // First check if this attribute is unique or not.
        $attrInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
        $result   = array_shift($attrInfo);
        if ((int) $required !== (int) $result['required']) {
            // Attribute value is no longer unique just update table.
            if (DAL::executeQuery('Attribute', 'setAttributeRequired') !== 0) {
                return TRUE;
            }
        }

        return FALSE;

    }//end updateAttributeRequired()


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
     * @since  4.0.0
     * @return integer
     * @throws ChannelException If there is an error updating the
     *                                attribute.
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
                'Attribute',
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
     * @param string  $assetType    Type of the asset.
     * @param boolean $customOnly   If TRUE then only the custom attributes are
     *                              returned.
     * @param boolean $standardOnly If TRUE then only the standard attributes
     *                              are returned.
     * @param boolean $systemOnly   If TRUE then only the system attributes
     *                              are returned.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAttributes(
        $assetType,
        $customOnly=FALSE,
        $standardOnly=FALSE,
        $systemOnly=FALSE
    ) {
        Channels::addToBasket('isCustom', $customOnly, TRUE);
        $attributeids = DAL::getQuery('Attribute', 'getAttributesByAssetType');

        Channels::addToBasket('attributeids', $attributeids);

        Channels::addToBasket('assetType', $assetType);
        $attributesData = DAL::executeAssoc(
            'Attribute',
            'batchGetAttributeInfo'
        );
        $attributesInfo = array();

        foreach ($attributesData as $attributeData) {
            // For the time being lets mimic the output format of
            // readAttributes().
            $attributeInfo = array(
                              'id'          => $attributeData['attributeid'],
                              'type'        => $attributeData['attribute_type'],
                              'data_source' => $attributeData['data_source'],
                              'unique'      => $attributeData['is_unique'],
                              'required'    => $attributeData['required'],
                              'default_val' => $attributeData['default_val'],
                              'assetid'     => $attributeData['assetid'],
                              'initial_val' => $attributeData['initial_val'],
                              'system'      => $attributeData['system'],
                             );

            $attributesInfo[$attributeData['attributeid']] = $attributeInfo;
        }//end foreach

        return $attributesInfo;

    }//end getAttributes()


    /**
     * Returns the values for all the attributes of the specified asset.
     *
     * @param integer $assetid      Id of an asset.
     * @param boolean $customOnly   If TRUE then only the custom attributes are
     *                              returned.
     * @param boolean $standardOnly If TRUE then only the standard attributes
     *                              are returned.
     * @param boolean $systemOnly   If TRUE then only the system attributes
     *                              are returned.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAttributeValues(
        $assetid,
        $customOnly=FALSE,
        $standardOnly=FALSE,
        $systemOnly=FALSE
    ) {
        Channels::includeSystem('Asset');
        $assetType = DAL::getOne(Asset::getAssetType($assetid));
        Channels::addToBasket('assetType', $assetType);

        Channels::addToBasket('customOnly', $customOnly);
        Channels::addToBasket('standardOnly', $standardOnly);
        Channels::addToBasket('systemOnly', $systemOnly);

        $attributeids = DAL::getAssoc(
            DAL::getQuery('Attribute', 'getAttributesByAssetType'),
            0
        );

        $values = array();
        foreach ($attributeids as $attribute) {
            $values[$attribute] = Attribute::getAttributeValue(
                $assetid,
                $attribute
            );
        }

        return $values;

    }//end getAttributeValues()


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
     * @since  4.0.0
     * @return array
     * @throws ChannelException If the attribute file doesn't exist.
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
     * @param string $assetType   Asset type specified attribute belongs to.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function attributeExists($attributeid, $assetType)
    {
        $result = DAL::executeOne('Attribute', 'attributeExists');
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
     * @since  4.0.0
     * @return boolean
     */
    public static function dataSourceExists($dataSource)
    {
        Channels::includeSystem('Attribute');
        $dataSourcePath = Attribute::getDataSourcePath($dataSource);
        if (file_exists($dataSourcePath) === TRUE) {
            $dataSourcePathExists = is_dir($dataSourcePath);
            return $dataSourcePathExists;
        }

        return FALSE;

    }//end dataSourceExists()


    /**
     * Retrieves the path of a data source.
     *
     * @param string $dataSource Name of the data source.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function getDataSourcePath($dataSource)
    {
        $dataSourcePath = Channels::getSystemsPath($dataSource);
        return $dataSourcePath;

    }//end getDataSourcePath()


    /**
     * This action adds a data validation rule to an attribute.
     *
     * For example, value must be less than 10. The attribute ID,
     * the asset type, the ruleID, and the rule value must be supplied.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $assetType   Type of the asset.
     * @param string $ruleid      Rule's id.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function addAttributeRule($attributeid, $assetType, $ruleid)
    {
        $queryResult = DAL::executeQuery('Attribute', 'addAttributeRule');
        return $queryResult;

    }//end addAttributeRule()


    /**
     * This action removes an existing adata validation rule from an attribute.
     *
     * The attribute ID,
     * the asset type, the ruleID, and the rule value must be supplied.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $assetType   Type of the asset.
     * @param string $ruleid      Rule's id.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function removeAttributeRule(
        $attributeid,
        $assetType,
        $ruleid
    ) {
        $queryResult = DAL::executeQuery('Attribute', 'removeAttributeRule');
        return $queryResult;

    }//end removeAttributeRule()


    /**
     * Returns the data sources for multiple attributes.
     *
     * The result is an array of (attributeid => data_source).
     *
     * @param array|Query $assetTypes   Type of the asset.
     * @param array|Query $attributeids Id of the attribute.
     *
     * @since  4.0.0
     * @return array
     */
    public static function batchGetAttributeDataSource(
        $assetTypes,
        $attributeids
    ) {
        $dataSources = Attribute::batchGetAttributeDataSourceQc(
            $assetTypes,
            $attributeids
        );
        $dataSources = DAL::executeDALQuery($dataSources);
        return $dataSources;

    }//end batchGetAttributeDataSource()


    /**
     * Installs attributes for the given asset type.
     *
     * Runs after an asset type is installed.
     *
     * @param string $typeid Installed asset type.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If there was an installation error.
     */
    public static function installAssetTypeAttributes($typeid)
    {
        // Get the attributes for the given asset type.
        $attributes = Attribute::readAttributes($typeid);

        foreach ($attributes as $id => $attr) {
            // Check if the attribute already exists.
            if (Attribute::attributeExists($id, $typeid) === TRUE) {
                $msg = 'Attribute "'.$id.'" already exists for '.$typeid;
                throw new ChannelException('Install Error: '.$msg);
            }

            // Check if attribute data source exists.
            if (Attribute::dataSourceExists($attr['data_source']) === FALSE) {
                $msg  = 'Attribute '.$id.' using a data source (';
                $msg .= $attr['data_source'].') that does not exist.';
                throw new ChannelException('Install Error: '.$msg);
            }

            // TODO: Check if attribute is a valid type for its data source.
            if ($attr['required'] === 'false') {
                $attr['required'] = 'f';
            } else {
                $attr['required'] = 't';
            }

            if ($attr['unique'] === 'false') {
                $attr['unique'] = 'f';
            } else {
                $attr['unique'] = 't';
            }

            // Add attribute.
            Attribute::addAttribute(
                $typeid,
                $id,
                $attr['type'],
                $attr['data_source'],
                $attr['unique'],
                $attr['initial_val'],
                $attr['default_val'],
                $attr['required'],
                $attr['assetid']
            );
        }//end foreach

    }//end installAssetTypeAttributes()


    /**
     * Returns batchGetAttributeDataSource Query object.
     *
     * @param array|Query $assetTypes   Type of the asset.
     * @param array|Query $attributeids Id of the attribute.
     *
     * @since  4.0.0
     * @return object
     */
    public static function batchGetAttributeDataSourceQc(
        $assetTypes,
        $attributeids
    ) {
        $baseQuery = DAL::getDALQuery(
            'Attribute',
            'batchGetAttributeDataSource',
            'Grouped'
        );
        return $baseQuery;

    }//end batchGetAttributeDataSourceQc()


    /**
     * Retrieves the replacements for the keywords.
     *
     * @param array $keywords Keywords list.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getKeywordReplacements(array $keywords)
    {
        Channels::includeSystem('Attribute');

        $batchValues = array();
        foreach ($keywords as $key => $scopes) {
            if ($key === 'page_content') {
                continue;
            }

            $assetids = array();
            foreach ($scopes as $scope) {
                $assetids[] = $scope['scope'];
            }

            $values = Attribute::batchGetAttributeValue($assetids, array($key));
            $batchValues[$key] = $values;
        }

        foreach ($keywords as $key => &$scopes) {
            foreach ($scopes as &$scope) {
                if ($key === 'page_content') {
                    $scope['value'] = Attribute::getKeywordReplacement(
                        $scope['scope'],
                        $key
                    );
                } else {
                    $assetid = $scope['scope'];
                    if (isset($batchValues[$key][$assetid]) === TRUE) {
                        $scope['value'] = $batchValues[$key][$assetid][$key];
                    } else {
                        $scope['value'] = '';
                    }
                }
            }
        }

        return $keywords;

    }//end getKeywordReplacements()


    /**
     * Returns the replacement for a single keyword.
     *
     * @param integer $assetid     The scope of the keyword.
     * @param string  $attributeid The keyword.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getKeywordReplacement($assetid, $attributeid)
    {
        $replacement = '';
        try {
            $replacement = Attribute::getAttributeValue($assetid, $attributeid);
        } catch (ChannelException $e) {
            $replacement = '';
        }

        return $replacement;

    }//end getKeywordReplacement()


    /**
     * Returns findAssetsWithValue query.
     *
     * @param string $value  Value to search for.
     * @param object $inList The query object to restrict search within a
     *                       parent.
     *
     * @since  4.0.0
     * @return Query
     */
    public static function findAssetsWithValue($value, $inList=NULL)
    {
        $value = str_replace('_', '\_', $value);
        $value = str_replace('%', '\%', $value);
        $value = str_replace(' ', '_', $value);
        Channels::modifyBasket('value', $value);

        $query = DAL::getDALQuery('Attribute', 'findAssetsWithValue', 'Assoc');
        return $query;

    }//end findAssetsWithValue()


}//end class

?>
