<?php
/**
 * Actions for the Attribute System.
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
 * @subpackage Attribute
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'DAL/DAL.inc';

/**
 * Attribute Actions Class.
 */
class AttributeActions
{

    /**
     * Cache for the attribute values.
     *
     * @var array
     */
    private static $_attrVals = array();


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Retrieves the value of specified attribute from a specified asset.
     *
     * The attribute is examined and the processing passed to the relevant
     * data source. The value returned will be a PHP data type, which will have
     * been converted from the stored data type by the data source. More than
     * one attributeids can be passed in, it will return an array of
     * attribute values.
     *
     * @param integer      $assetid     The asset to find the value for.
     * @param string|array $attributeid The attribute(s) to find.
     *
     * @return mixed
     *
     * @api            read
     * @api-permission assetid asset.read.content
     */
    public static function getAttributeValue($assetid, $attributeid)
    {
        if (is_array($attributeid) === TRUE) {
            $result = array();
            foreach ($attributeid as $attrid) {
                $result[$attrid] = self::_getAttributeValue($assetid, $attrid);
            }
        } else if (is_string($attributeid) === TRUE) {
            $result = self::_getAttributeValue($assetid, $attributeid);
        }

        return $result;

    }//end getAttributeValue()


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * The attribute is examined and the processing passed to the relevant
     * data source. The value returned will be a PHP data type, which will have
     * been converted from the stored data type by the data source.
     *
     * @param integer $assetid     The asset to find the value for.
     * @param string  $attributeid The attribute to find.
     *
     * @return mixed
     */
    private static function _getAttributeValue($assetid, $attributeid)
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
            $attrInfo = Attribute::getAttributeInfoByAsset(
                $infoAssetid,
                $attributeid
            );

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

    }//end _getAttributeValue()


    /**
     * Returns the values for all the attributes of the specified asset.
     *
     * @param integer $assetid Id of an asset.
     *
     * @return array
     */
    public static function getAttributeValues($assetid)
    {
        Channels::includeSystem('Asset');
        $assetType    = Asset::getTypeOfAsset($assetid);
        $attributeids = Attribute::getAttributes($assetType);
        $attributeids = array_keys($attributeids);

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
     * @throws ChannelException If the attribute is not found for the asset.
     * @throws ChannelException If there is an attribute constraints violation.
     *
     * @api            write
     * @api-permission assetid asset.edit.content
     */
    public static function setAttributeValue($assetid, $attributeid, $value)
    {
        Channels::includeSystem('Context');
        $assetid = Context::getAssetid($assetid);

        // Get attribute info.
        $attrInfo = Attribute::getAttributeInfoByAsset(
            $assetid,
            $attributeid
        );

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

        // This is not a unique attribute, set and forget.
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
            // For the time being lets mimic the output format of readAttributes().
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
     * Retrieves the attribute info using the specified assetid.
     *
     * If the attribute does not exist for the asset, an
     * empty array is returned.
     *
     * @param integer $assetid     Asset's id.
     * @param string  $attributeid Attribute id.
     *
     * @return array
     * @throws ChannelException If there is an error retrieving the
     *                          attribute info.
     */
    public static function getAttributeInfoByAsset($assetid, $attributeid)
    {
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

        $attrInfo = current($attrInfo);
        return $attrInfo;

    }//end getAttributeInfoByAsset()


    /**
     * Retrieves the attribute info using the specified asset type.
     *
     * @param string $assetType   Asset's type.
     * @param string $attributeid Attribute id.
     *
     * @return array
     * @throws ChannelException If there is an error retrieving the
     *                          attribute.
     */
    public static function getAttributeInfoByType($assetType, $attributeid)
    {
        $attrInfo = DAL::executeAssoc('Attribute', 'getAttributeInfo');
        if (empty($attrInfo) === TRUE) {
            $msg = 'Attribute: Error Retrieving Attribute.';
            throw new ChannelException($msg);
        }

        $attrInfo = current($attrInfo);
        return $attrInfo;

    }//end getAttributeInfoByType()


    /**
     * Gets any assets that have the specified value for specified attribute.
     *
     * @param string  $assetType   The type of asset.
     * @param string  $attributeid Attribute id.
     * @param mixed   $value       Value to use for lookup.
     * @param boolean $decendants  If TRUE assets of a type that is a
     *                             descendant of $assetType will be included.
     *
     * @return array
     */
    public static function getAssetsByAttributeValue(
        $assetType,
        $attributeid,
        $value,
        $decendants=FALSE
    ) {
        $attrInfo = Attribute::getAttributeInfoByType(
            $assetType,
            $attributeid
        );

        if (empty($attrInfo) === TRUE) {
            return array();
        }

        Channels::includeSystem('Asset');
        Channels::includeSystem($attrInfo['data_source']);

        if ($decendants === TRUE) {
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

        $assets = call_user_func_array($getCall, $getParams);
        return $assets;

    }//end getAssetsByAttributeValue()


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
     * @return void
     * @throws ChannelException If the data source doesn't exist, or if
     *                          there is an error adding the attribute.
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
        try {
            DAL::beginTransaction();
            // Get child types for the specified asset type.
            Channels::includeSystem('Asset');
            $childTypes = Asset::getChildTypes($assetType);

            // For each child type add the attribute.
            foreach ($childTypes as $childType) {
                if (Attribute::attributeExists($childType['typeid'], $attributeid) === FALSE) {
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
     * @param string $assetType   The type of the attribute to remove.
     * @param string $attributeid The id of the attribute to remove.
     *
     * @return integer
     * @throws ChannelException If the attribute is not successfully
     *                          removed.
     */
    public static function removeAttribute($assetType, $attributeid)
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
     * Checks if the given attribute exists.
     *
     * @param string $assetType   Asset type specified attribute belongs to.
     * @param string $attributeid Attribute's id.
     *
     * @return boolean
     */
    public static function attributeExists($assetType, $attributeid)
    {
        $result = DAL::executeOne('Attribute', 'attributeExists');
        if (empty($result) === FALSE) {
            return TRUE;
        }

        return FALSE;

    }//end attributeExists()


    /**
     * Retrieves the path of a data source.
     *
     * @param string $dataSource Name of the data source.
     *
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
     * @param string $assetType   Type of the asset.
     * @param string $attributeid Id of the attribute.
     * @param string $ruleid      Rule's id.
     *
     * @return integer
     */
    public static function addAttributeRule($assetType, $attributeid, $ruleid)
    {
        $result = DAL::executeQuery('Attribute', 'addAttributeRule');
        return $result;

    }//end addAttributeRule()


    /**
     * This action removes an existing adata validation rule from an attribute.
     *
     * The attribute ID,
     * the asset type, the ruleID, and the rule value must be supplied.
     *
     * @param string $assetType   Type of the asset.
     * @param string $attributeid Id of the attribute.
     * @param string $ruleid      Rule's id.
     *
     * @return integer
     */
    public static function removeAttributeRule(
        $assetType,
        $attributeid,
        $ruleid
    ) {
        $result = DAL::executeQuery('Attribute', 'removeAttributeRule');
        return $result;

    }//end removeAttributeRule()


    /**
     * Returns findAssetsWithValue query.
     *
     * @param string $value  Value to search for.
     * @param object $inList The query object to restrict search within a
     *                       parent.
     *
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
