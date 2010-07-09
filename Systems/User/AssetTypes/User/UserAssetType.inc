<?php
/**
 * UserAssetType.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage User
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

Channels::includeSystem('Asset');
Asset::includeAsset('asset');

/**
 * UserAssetType.
 *
 * Represents an individual user.
 *
 * @since 4.0.0
 */
class UserAssetType extends AssetAssetType
{


    /**
     * Validates that a usename for a user is valid.
     *
     * @param string $assetType The asset type being created.
     * @param string $username  The username being created.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function validateUsername($assetType, $username)
    {
        Channels::includeSystem('AttributeDataSource');
        if (trim($username) === '') {
            return 'You must supply a username.';
        } else if (AttributeDataSource::isUniqueAttributeValue($assetType, 'username', 'text', $username) !== TRUE) {
            $error = 'The username "'.$username.'" is already registered with the system.';
            return $error;
        }

    }//end validateUsername()


}//end class

?>