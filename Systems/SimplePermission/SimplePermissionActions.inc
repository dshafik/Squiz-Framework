<?php
/**
 * Actions for the SimplePermission System.
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
 * @subpackage SimplePermission
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/Permission/PermissionActions.inc';

/**
 * SimplePermission Actions Class.
 */
class SimplePermissionActions implements PermissionActions
{


    /**
     * Enable access to specified Asset for User using specified Role.
     *
     * @param integer $resource Asset id to grant permission on.
     * @param integer $roleid   Id of the Role to use for permissions.
     * @param integer $userid   The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function grantPermission($resource, $roleid, $userid)
    {
        $query  = DAL::getQuery('SimplePermission', 'grantPermission');
        $result = (bool) DAL::executeQuery($query);
        return $result;

    }//end grantPermission()


    /**
     * Prevent access to specified Asset for User using specified Role.
     *
     * @param integer $resource Asset id to grant permission on.
     * @param integer $roleid   Id of the Role to use for permissions.
     * @param integer $userid   The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function denyPermission($resource, $roleid, $userid)
    {
        return Permission::revokePermission($resource, $roleid, $userid);

    }//end denyPermission()


    /**
     * Revokes the permission from user for the specified asset.
     *
     * @param integer $resource Asset id to grant permission on.
     * @param integer $roleid   Id of the Role to use for permissions.
     * @param integer $userid   The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function revokePermission($resource, $roleid, $userid)
    {
        $query  = DAL::getQuery('SimplePermission', 'denyPermission');
        $result = (bool) DAL::executeQuery($query);
        return $result;

    }//end revokePermission()


    /**
     * Returns TRUE if User has specified Permission for Asset.
     *
     * @param integer $resource  Asset id to check access to.
     * @param integer $userid    Check access for this User id.
     * @param string  $privilege The privilege to check for.
     *
     * @return boolean
     */
    public static function hasPermission($resource, $userid, $privilege)
    {
        $query  = DAL::getQuery('SimplePermission', 'hasPermission');
        $result = (bool) DAL::getOne($query);
        return $result;

    }//end hasPermission()


}//end class

?>