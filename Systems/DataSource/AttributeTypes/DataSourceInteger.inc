<?php
/**
 * Integer Attribute.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DataSource
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/DataSource/AttributeTypes/AttributeType.inc';

/**
 * DataSourceInteger.
 *
 * @since 4.0.0
 */
class DataSourceInteger implements AttributeType
{


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public static function getValue($assetid, $attributeid)
    {

    }//end getValue()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param mixed   $value       Value of the attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public static function setValue($assetid, $attributeid, $value)
    {

    }//end setValue()


    /**
     * Finds all assets that have the specified value for the specified attribute.
     *
     * @param string $attributeid Id of the attribute.
     * @param string $typeid      Id of the attribute type.
     * @param mixed  $value       Value of the attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public static function findAssetsByValue($attributeid, $typeid, $value)
    {

    }//end findAssetsByValue()


}//end class

?>