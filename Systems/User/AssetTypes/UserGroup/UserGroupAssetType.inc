<?php
/**
 * UserGroupAssetType.
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
 * UserGroupAssetType.
 *
 * A container for User or other User Group assets.
 *
 * @since 4.0.0
 */
class UserGroupAssetType extends AssetAssetType
{

}//end class

?>