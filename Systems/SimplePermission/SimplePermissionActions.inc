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
     * @param integer $resource  Resource ID to check access to.
     * @param string  $privilege The privilege to check for.
     * @param integer $userid    Check access for this User ID.
     *
     * @return boolean
     */
    public static function hasPermission($resource, $privilege, $userid=NULL)
    {
        Channels::includeSystem('User');
        if (User::isSystemAdministrator($userid) === TRUE) {
            return TRUE;
        }

        if ($userid === NULL) {
            $userid = User::getCurrentUserid();
            Channels::modifyBasket('userid', $userid);
        }

        $query  = DAL::getQuery('SimplePermission', 'hasPermission');
        $result = (bool) DAL::getOne($query);
        return $result;

    }//end hasPermission()


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
    public static function checkAPIPermission($system, $action, array $args, array $settings)
    {
        // Assume no access, even if they are a super user. This forces
        // developers to define permission checks for all API functions.
        $perm = FALSE;

        Channels::includeSystem('User');
        $userid = User::getCurrentUserid();

        if (isset($settings['permission']) === TRUE) {
            foreach ($settings['permission'] as $permission) {
                // Check our special cases.
                switch ($permission) {
                    case 'public':
                        // API method can be accessed by everyone.
                        return;
                    break;

                    case 'superuser':
                        // Must be a super user.
                        $perm = User::isSystemAdministrator($userid);
                    break(2);

                    default:
                        list($arg, $priv) = explode(' ', $permission);
                        if (isset($args[$arg]) === FALSE) {
                            continue;
                        }

                        $resourceid = $args[$arg];
                        $perm       = Permission::hasPermission($resourceid, $priv, $userid);
                        if ($perm === FALSE) {
                            break(2);
                        } else {
                            $perm = TRUE;
                        }
                    break;
                }//end switch
            }//end foreach
        }//end if

        if ($perm === FALSE) {
            $response = array(
                        'system'    => $system,
                        'action'    => $action,
                        'exception' => 'You do not have permission to perform the requested action',
                       );

            Channels::includeSystem('API');
            API::sendHeader(401);
            API::sendResponse($response);
            exit;
        }

    }//end checkAPIPermission()


}//end class

?>