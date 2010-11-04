<?php
/**
 * LDAPConnection Asset Type.
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
 * @subpackage LDAPConnection
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

Channels::includeSystem('Asset');
Asset::includeAsset('asset');

/**
 * LDAPConnection Asset Type.
 */
class LDAPConnectionAssetType extends AssetAssetType
{


    /**
     * Returns the contents of a folder asset.
     *
     * @param integer $assetid The ID of the folder asset.
     *
     * @return string
     */
    public static function getAssetContent($assetid)
    {
        return '';

    }//end getAssetContent()


    /**
     * Save handlers for UserManagerScreen changes for LDAP related asset types.
     *
     * @param array $data Save data.
     *
     * @return void
     *
     * @api            write
     * @api-permission superuser
     */
    public static function saveUserManagerScreen(array $data)
    {
        Channels::includeSystem('LDAPConnection');
        LDAPConnection::updateConnection(
            $data['assetid'],
            $data['name'],
            $data['details'],
            $data['userAttributes'],
            $data['groupAttributes'],
            $data['uniqueIdSettings']
        );

    }//end saveUserManagerScreen()



    /**
     * Returns the asset type for an LDAP id.
     *
     * Returns NULL if asset type cannot be determined.
     *
     * @param string $connectionid The LDAPConnection asset id.
     * @param string $shadowid     Id of LDAP resource.
     *
     * @return string
     */
    public static function getRemoteAssetType($connectionid, $shadowid)
    {
        Channels::includeSystem('LDAPConnection');
        $assetType = LDAPConnection::getRemoteAssetType($connectionid, $shadowid);
        return $assetType;

    }//end getRemoteAssetType()


}//end class

?>
