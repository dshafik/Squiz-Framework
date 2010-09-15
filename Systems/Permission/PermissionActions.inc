<?php
/**
 * Basic structure for permission handling systems.
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
 * @subpackage Permission
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Basic structure for all session handling systems.
 */
interface PermissionActions
{


    /**
     * Enable access to specified Asset for User using specified Role.
     *
     * @param integer $assetid Asset id to grant permission on.
     * @param integer $roleid  Id of the Role to use for permissions.
     * @param integer $userid  The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function grantPermission($assetid, $roleid, $userid);


    /**
     * Prevent access to specified Asset for User using specified Role.
     *
     * @param integer $assetid Asset id to grant permission on.
     * @param integer $roleid  Id of the Role to use for permissions.
     * @param integer $userid  The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function denyPermission($assetid, $roleid, $userid);


    /**
     * Revokes the permission from user for the specified asset.
     *
     * @param integer $assetid Asset id to grant permission on.
     * @param integer $roleid  Id of the Role to use for permissions.
     * @param integer $userid  The id of the User to grant permission.
     *
     * @return boolean
     */
    public static function revokePermission($assetid, $roleid, $userid);


    /**
     * Returns TRUE if User has specified Permission for Asset.
     *
     * @param integer $resource  Resource ID to check access to.
     * @param string  $privilege The privilege to check for.
     * @param integer $userid    Check access for this User ID.
     *
     * @return boolean
     */
    public static function hasPermission($assetid, $privilege, $userid=NULL);


    /**
     * Performs permission checks on API calls.
     *
     * Permission checks are defined in the function comment and are
     * in the form: api-permission [argument] [privilage]
     * Meaning that the value of [argument] will be checked to see if the current
     * user has [privilage].
     *
     * @param string $system   System name where action resides.
     * @param string $action   Action name to call.
     * @param array  $args     Parameters to pass to the action.
     * @param array  $settings API settings defined on the method being called.
     *
     * @return void
     */
    public static function checkAPIPermission($system, $action, array $args, array $settings);


}//end interface

?>