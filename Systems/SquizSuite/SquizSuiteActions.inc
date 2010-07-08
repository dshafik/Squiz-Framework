<?php
/**
 * Actions for the SquizSuite System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * SquizSuite Actions Class.
 *
 * @since 4.0.0
 */
class SquizSuiteActions
{


    /**
     * Activate a product to the Suite.
     *
     * @param string $productID The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function activateProduct($productID)
    {
        return TRUE;

    }//end activateProduct()


    /**
     * Collect the Product Statistics.
     *
     * @param string $productID The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function collectProductStatistic($productID)
    {
        return TRUE;

    }//end collectProductStatistic()


    /**
     * Get Product's Connection details.
     *
     * @param string $productID The ID of the Product.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getProductConnection($productID)
    {
        return array();

    }//end getProductConnection()


    /**
     * Get information about the Product.
     *
     * @param string $productid The ID of the Product.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getProductInformation($productid)
    {
        $result = DAL::executeAll('SquizSuite', 'getProductInformation');
        return $result;

    }//end getProductInformation()


    /**
     * Get the Products in the Suite matching a status.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getSuiteProducts()
    {
        $productid = NULL;
        $result    = SquizSuite::getProductInformation($productid);
        return $result;

    }//end getSuiteProducts()


    /**
     * Modify a project in the Suite.
     *
     * @param string $productid The ID of the Product.
     * @param string $attrName  The name of the field to change.
     * @param string $attrValue The new value.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function modifyProduct($productid, $attrName, $attrValue)
    {
        $result = DAL::executeQuery('SquizSuite', 'modifyProduct');
        return $result;

    }//end modifyProduct()


    /**
     * Register a product.
     *
     * @param string $name       The name of the product.
     * @param string $type       The type of the product to invite.
     * @param string $connection The connection details of the product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function registerProduct($name, $type, $connection)
    {
        $id = DAL::seqNextVal('seq_productid');
        Channels::addToBasket('productid', $id);
        DAL::executeQuery('SquizSuite', 'registerProduct');

        return TRUE;

    }//end registerProduct()


    /**
     * Remove a product from the Suite.
     *
     * @param string $productid The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function removeProduct($productid)
    {
        $result = DAL::executeQuery('SquizSuite', 'removeProduct');
        return $result;

    }//end removeProduct()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


}//end class

?>
