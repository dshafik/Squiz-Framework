<?php
/**
 * Actions for the FileDataSource System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage FileDataSource
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/DataSource/DataSourceActions.inc';
require_once 'Libs/FileSystem/FileSystem.inc';
require_once 'Systems/FileDataSource/AttributeTypes/FileDataSourceFile.inc';

/**
 * FileDataSourceActions.
 *
 * @since 4.0.0
 */
class FileDataSourceActions implements DataSourceActions
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
    public static function getAttributeValue($assetid, $attributeid, $attrType)
    {
        if (is_array($assetid) === TRUE) {
            $assetid = $assetid[0];
        }

        return FileDataSourceFile::getValue($assetid, $attributeid);

    }//end getAttributeValue()


    /**
     * Retrieves the value of one ore more attributes from one or more assets.
     *
     * @param array|Query $assetids     List of asset ids.
     * @param array       $attributeids List of attribute ids.
     * @param array       $attrTypes    An array of all attribute types being retrieved.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function batchGetAttributeValue($assetids, array $attributeids, array $attrTypes)
    {
        $values = array();
        foreach ($assetids as $pos => $assetid) {
            if (is_array($assetid) === TRUE) {
                $assetid = $assetid[0];
            }

            $values[] = FileDataSourceFile::getValue($assetid, $attributeids[$pos]);
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
     * @since  4.0.0
     * @see    DataSourceActions::addAttributeValue()
     * @return void
     */
    public static function setAttributeValue($assetid, $attributeid, $attrType, $value)
    {
        FileDataSourceFile::setValue($assetid, $attributeid, $value);

    }//end setAttributeValue()


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
     * @return void
     */
    public static function batchSetAttributeValue($assetids, array $attrVals)
    {
        reset($attrVals);
        foreach ($assetids as $assetid) {
            FileDataSourceFile::setValue($assetid, key($attrVals), value($attrVals));
            next($attrVals);
        }

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
     * @since  4.0.0
     * @see    DataSourceActions::setAttributeValue()
     * @see    DataSourceActions::attributeValueExists()
     * @return void
     */
    public static function addAttributeValue($assetid, $attributeid, $attrType, $value, $isDefault)
    {
        // We just need to do the same as setting an attribute value.
        FileDataSourceFile::setValue($assetid, $attributeid, $value);

    }//end addAttributeValue()


    /**
     * Checks if specified attribute type is valid for the data source.
     *
     * @param string $attrType Attribute type to validate.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isValidAttributeType($attrType)
    {
        return ($attrType === 'file');

    }//end isValidAttributeType()


    /**
     * Retrieves all values set for an attribute.
     *
     * @param string $assetType   Asset type the attribute is set for.
     * @param string $attributeid The ID of the attribute.
     * @param string $attrType    The attribute type.
     *
     * @since  4.0.0
     * @return void
     */
    public static function getAllAttributeValues($assetType, $attributeid, $attrType)
    {

    }//end getAllAttributeValues()


    /**
     * Removes all values set for an attribute.
     *
     * @param string $attributeid   The ID of the attribute.
     * @param string $assetType     Asset type the attribute is set for.
     * @param string $attributeType The attribute type.
     *
     * @since  4.0.0
     * @return void
     */
    public static function removeAttributeValues($attributeid, $assetType, $attributeType)
    {

    }//end removeAttributeValues()


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
    public static function attributeValueExists($assetid, $attributeid, $attrType, $value=NULL)
    {
        $file = FileDataSourceFile::getAttributeFilePath($assetid, $attributeid);
        return (file_exists($file) === TRUE);

    }//end attributeValueExists()


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
    public static function getAssetsByAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        return array();

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
     * @since  4.0.0
     * @return (boolean|array)
     */
    public static function isUniqueAttributeValue($assetType, $attributeid, $attrType, $value)
    {
        return TRUE;

    }//end isUniqueAttributeValue()


    /**
     * Returns the class name for the given attribute type.
     *
     * @param string $attrType Type of the attribute.
     *
     * @since  4.0.0
     * @return string
     * @throws ChannelException If the attribute type is invalid.
     */
    private static function _getAttributeTypeClassName($attrType)
    {
        if ($attrType === 'file') {
            return 'FileDataSourceFile';
        }

        throw new ChannelException('Unknown Attribute type: '.$attrType);

    }//end _getAttributeTypeClassName()


    /**
     * Returns the file path of a given attribute type.
     *
     * @param string $className Class name of an attribute type.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _getAttributeTypePath($className)
    {

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
     * @since  4.0.0
     * @return void
     */
    public static function updateAttributeid($attributeid, $assetType, $attributeType, $newId)
    {

    }//end updateAttributeid()


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
     * @since  4.0.0
     * @return void
     */
    public static function addAssetTypeAttributeValue($attributeid, $assetType, $attributeType, $value, $isDefault)
    {

    }//end addAssetTypeAttributeValue()


    /**
     * Replaces attribute context values.
     *
     * @param string $oldContext The context to get values from.
     * @param string $newContext The context to override.
     * @param string $assetid    Asset id to use.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context cannot be replaced.
     */
    public static function replaceContext($oldContext, $newContext, $assetid)
    {
        Channels::includeSystem('Context');
        Channels::includeSystem('Asset');
        Channels::includeSystem('Attribute');
        $oldContextAssetid = Context::getAssetid($assetid, $oldContext, TRUE);
        $newContextAssetid = Context::getAssetid($assetid, $newContext, TRUE);

        // Get the list of attribute types to merge. Right now, it only has 'file_content'
        // attribute, but it can be easily added later.
        $attrsToMerge = array();
        $attrTypes    = array('file_content');
        foreach ($attrTypes as $attributeid) {
            $hasAttribute = Attribute::attributeExists($attributeid, Asset::getAssetType($assetid));
            if ($hasAttribute === TRUE) {
                $attrsToMerge[] = $attributeid;
            }
        }

        foreach ($attrsToMerge as $attributeid) {
            $oldContextPath = FileDataSourceFile::getValue($oldContextAssetid, $attributeid);
            FileDataSourceFile::setValue($newContextAssetid, $attributeid, $oldContextPath);
        }

    }//end replaceContext()


    /**
     * Removes the context data for the specified context and asset.
     *
     * @param string $context The context to remove.
     * @param string $assetid Asset id to use.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context data cannot be removed.
     */
    public static function removeContextData($context, $assetid=NULL)
    {
        Channels::includeSystem('Context');
        Channels::includeSystem('Asset');
        Channels::includeSystem('Attribute');
        if (Context::isMasterContext($context) !== TRUE) {
            $attrsToMerge = array();
            $attrTypes    = array('file_content');
            foreach ($attrTypes as $attributeid) {
                $hasAttribute = Attribute::attributeExists($attributeid, Asset::getAssetType($assetid));
                if ($hasAttribute === TRUE) {
                    $attrsToMerge[] = $attributeid;
                }
            }

            foreach ($attrsToMerge as $attributeid) {
                $contextAssetid  = Context::getAssetid($assetid, $context, TRUE);
                $contextFilePath = FileDataSourceFile::getAttributeFilePath($contextAssetid, $attributeid);
                unlink($contextFilePath);
            }
        }

    }//end removeContextData()


    /**
     * Removes the entire context data for the specified context range.
     *
     * @param string $context Context string that could be used in LIKE queries.
     *                        E.g. _____01__ .
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException If context data cannot be removed.
     */
    public static function removeAllContextData($context)
    {
        Channels::includeSystem('Context');
        if (Context::isMasterContext($context) !== TRUE) {
            include_once 'Libs/FileSystem/FileSystem.inc';
            $dataPath = BaseSystem::getDataDir('FileDataSource');
            $files    = FileSystem::listDirectory($dataPath);

            $needle = '.'.$context.'_';
            foreach ($files as $fullPath) {
                if (strpos($fullPath, '.'.$context.'_') !== FALSE) {
                    unlink($fullPath);
                }
            }
        }

    }//end removeAllContextData()


}//end class

?>
