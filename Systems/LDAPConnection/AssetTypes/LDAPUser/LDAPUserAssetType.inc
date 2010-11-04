<?php
/**
 * LDAPUserAssetType.
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
 * LDAPUserAssetType.
 *
 * Represents an individual user.
 */
class LDAPUserAssetType extends AssetAssetType
{


    /**
     * Saves User Manager Screen changes for an LDAP user.
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
        Channels::includeSystem('Asset');
        if (Asset::isAssetid($data['assetid']) === TRUE) {
            Channels::includeSystem('Connection');
            $shadowid = Connection::getShadowidFromLocalid($data['assetid']);
            if ($shadowid === NULL) {
                throw new ChannelException(_('Not a valid LDAP User'));
            }
        }

    }//end saveUserManagerScreen()


}//end class

?>
