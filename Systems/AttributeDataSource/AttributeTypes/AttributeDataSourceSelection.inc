<?php
/**
 * Selection Attribute.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage AttributeDataSource
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/DataSource/AttributeTypes/DataSourceSelection.inc';

/**
 * AttributeDataSourceSelection.
 *
 * @since 4.0.0
 */
class AttributeDataSourceSelection extends DataSourceSelection
{

    /**
     * Table postfix.
     *
     * @var   string $_tablePostfix Table postfix.
     * @since 4.0.0
     */
    private static $_tablePostfix = 'selection';


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function getValue($assetid, $attributeid)
    {
        $value = unserialize(DAL::executeOne('AttributeDataSource', 'getAttributeValue'));

        return $value;

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
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If there is an error updating the value
     *                          in the database.
     */
    public static function setValue($assetid, $attributeid, $value)
    {
        $rows = 0;
        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'attribute_text');
            Channels::modifyBasket('value', serialize($value));
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
     * @since  4.0.0
     * @return boolean
     * @throws ChannelException If there is an error adding the value
     *                          to the database.
     */
    public static function addAttributeValue($assetid, $attributeid, $value, $isDefault)
    {
        try {
            DAL::beginTransaction();
            Channels::addToBasket('tableName', 'attribute_'.self::$_tablePostfix);
            Channels::modifyBasket('value', serialize($value));
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