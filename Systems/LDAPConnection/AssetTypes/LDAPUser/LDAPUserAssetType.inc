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
 * @subpackage LDAP
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

        try {
            self::_saveAssetParentGroups($data['assetid'], $data['parents']);
        } catch (Exception $e) {
            error_log($e);
            throw new ChannelException($e->getMessage());
        }

    }//end saveUserManagerScreen()


    /**
     * Updates the user groups that the specified asset belongs to.
     *
     * @param string $assetid Asset ID of a user or a user group.
     * @param array  $parents The parents information from List widget.
     *
     * @return void
     */
    private static function _saveAssetParentGroups($assetid, array $parents)
    {
        if (empty($parents) === TRUE) {
            return;
        }

        Channels::includeSystem('Linking');

        // Update parent groups.
        $currentGroups = User::getUserGroups($assetid, TRUE);
        $removedGroups = Util::getArrayIndex($parents, 'removed');
        foreach ($removedGroups as $groupid => $tmp) {
            if (in_array($groupid, $currentGroups) === TRUE) {
                Linking::deleteLink($groupid, $assetid);
            }
        }

        // Add the new groups.
        $items = Util::getArrayIndex($parents, 'items', array());
        foreach ($items as $itemid => $tmp) {
            if (strpos($itemid, 'new_') === 0) {
                $parts      = explode('_', $itemid);
                $newGroupid = $parts[1];
                if ($newGroupid !== $assetid
                    && in_array($newGroupid, $currentGroups) === FALSE
                ) {
                    Linking::createLink($newGroupid, $assetid);
                }
            }
        }

    }//end _saveAssetParentGroups()


}//end class

?>
