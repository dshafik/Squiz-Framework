<?php
/**
 * Base Asset Type.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage AssetType
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Asset Asset Type Class.
 *
 * @since 4.0.0
 */
class AssetAssetType
{


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
     * Returns the contents of this asset.
     *
     * By default, a blank string is returned.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getContents()
    {
        return '';

    }//end getContents()


    /**
     * Validates that the name for a page is valid.
     *
     * @param string $name The name for the newly created page.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function validateName($name)
    {
        if (trim($name) === '') {
            return 'You must supply a name.';
        }

    }//end validateName()


    /**
     * Returns the system name the asset belongs to.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSystemName()
    {
        $systemPath = dirname(dirname(dirname(__FILE__)));
        return basename($systemPath);

    }//end getSystemName()


}//end class

?>