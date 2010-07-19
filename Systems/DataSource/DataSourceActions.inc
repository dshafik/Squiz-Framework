<?php
/**
 * Actions for the DataSource System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DataSource
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * DataSourceActions.
 *
 * Abstract actions for handling retreival of attribute values.
 *
 * Provides the interface for getting and setting attribute
 * values. All other data source systems must implement these
 * actions.
 *
 * @since 4.0.0
 */
interface DataSourceActions
{


    /**
     * Retrieves the value if a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    Type of the attribute.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function getAttributeValue($assetid, $attributeid, $attrType);


    /**
     * Retrieves the value of one ore more attributes from one or more assets.
     *
     * @param array|Query $assetids   List of asset ids.
     * @param array       $attributes List of attribute ids.
     * @param array       $attrTypes  An array of all attribute types being retrieved.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function batchGetAttributeValue($assetids, array $attributes, array $attrTypes);


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
     * @since  4.0.0
     * @see    DataSourceActions::addAttributeValue()
     * @return boolean
     */
    public static function setAttributeValue($assetid, $attributeid, $attrType, $value);


    /**
     * Sets the values of one or more attributes for one or more assets.
     *
     * Value will be converted for storage by the data source. Returns true if
     * action was successful.
     *
     * @param array|Query $assetids List of asset ids.
     * @param array       $attrVals Array of attribute ids with their values.
     *                              array(attributeid => value).
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function batchSetAttributeValue($assetids, array $attrVals);


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
     * @since  4.0.0
     * @see    DataSourceActions::setAttributeValue()
     * @see    DataSourceActions::attributeValueExists()
     * @return boolean
     */
    public static function addAttributeValue($assetid, $attributeid, $attrType, $value, $isDefault);


    /**
     * Checks if specified attribute type is valid for the data source.
     *
     * @param string $attributeType Attribute type to validate.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isValidAttributeType($attributeType);


    /**
     * Retrieves all values set for an attribute.
     *
     * @param string $assetType   Asset type the attribute is set for.
     * @param string $attributeid The ID of the attribute.
     * @param string $attrType    The attribute type.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAllAttributeValues($assetType, $attributeid, $attrType);


    /**
     * Removes all values set for an attribute.
     *
     * @param string $attributeid   The ID of the attribute.
     * @param string $assetType     Asset type the attribute is set for.
     * @param string $attributeType The attribute type.
     *
     * @since  4.0.0
     * @return array
     */
    public static function removeAttributeValues($attributeid, $assetType, $attributeType);


    /**
     * Returns TRUE if the asset contains a value for the specified attribute.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param string  $attrType    The type of attribute.
     * @param string  $value       The value of the attribute.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function attributeValueExists($assetid, $attributeid, $attrType, $value=NULL);


    /**
     * Retrieves assets based off the value of the specified attribute.
     *
     * @param string $assetType   Asset type the attribute is set for.
     * @param string $attributeid The ID of the attribute.
     * @param string $attrType    The attribute type.
     * @param mixed  $value       Value of attribute used to match assets.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value);


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
     * @since  4.0.0
     * @return (boolean|array)
     */
    public static function isUniqueAttributeValue($assetType, $attributeid, $attrType, $value);


}//end interface

?>