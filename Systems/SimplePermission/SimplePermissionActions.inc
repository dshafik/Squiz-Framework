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
                         'exception' => _('You do not have permission to perform the requested action'),
                        );

            Channels::includeSystem('API');
            API::sendHeader(401);
            API::sendResponse($response);
            exit;
        }

    }//end checkAPIPermission()


    /**
     * Returns list of projects and their titles.
     *
     * @return array
     *
     * @api            read
     * @api-permission superuser
     */
    public static function getProjects()
    {
        return array();

    }//end getProjects()


    /**
     * Returns TRUE if there are projects in the system.
     *
     * @return boolean
     */
    public static function hasProjects()
    {
        $projects = Permission::getProjects();
        if (empty($projects) === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end hasProjects()


    /**
     * Returns the list of projects for the Permissions screen.
     *
     * @return array
     */
    public static function getProjectsListForScreen()
    {
        $projects = Permission::getProjects();

        return $projects;

    }//end getProjectsListForScreen()


    /**
     * Returns contents for the specified project's permissions.
     *
     * @param string $resourceid Id of the resource.
     *
     * @return void
     *
     * @api            read
     * @api-permission superuser
     */
    public static function getPermissionListForScreen($resourceid=NULL)
    {
        if ($resourceid === NULL) {
            $projects = Permission::getProjects();
            ksort($projects);
            $resourceid = key($projects);
        } else {
            Channels::includeSystem('GUI');
            GUI::setCurrentTemplateKey('SimplePermission::PermissionScreen');
        }

        // Get the list of roles that are available.
        Channels::includeSystem('Role');
        $roles = Role::getRoles();

        echo '<div id="PermissionScreen-rolesLists-'.$resourceid.'" class="PermissionScreen-rolesLists active">';
        if (empty($roles) === TRUE) {
            echo _('No roles have been created within the system.');
        } else {
            // Foreach role create a GUIBox and GUIList for each box with list of users.
            foreach ($roles as $roleid => $roleName) {
                echo '<div id="PermissionScreen-rolesList-'.$roleid.'" class="PermissionScreen-rolesList">';
                self::_printRoleForScreen($resourceid, $roleid, $roleName);
                echo '</div>';
            }
        }

        echo '</div>';

    }//end getPermissionListForScreen()


    /**
     * Prints a role with list of users assigned to it.
     *
     * @param string $resourceid Id of the resource.
     * @param string $roleid     Id of the role.
     * @param string $roleName   Name of the role.
     *
     * @return void
     */
    private static function _printRoleForScreen($resourceid, $roleid, $roleName)
    {
        Channels::includeSystem('GUI');

        $id    = 'PermissionScreen-role-'.$resourceid.'-'.$roleid;
        $class = 'PermissionScreen-role';
        echo '<div class="'.$class.'" id="'.$id.'">';
        echo '<h2>'.$roleName.'</h2>';

        // Get list of users/groups who are assigned to this role.
        $assets    = Permission::getUsers($resourceid, $roleid);
        $listItems = array();
        foreach ($assets as $assetid) {
            $listItems[$assetid] = Permission::generatePermissionScreenListItems($assetid);
        }

        $listSettings = array(
                         'items'          => $listItems,
                         'allowDelete'    => TRUE,
                         'noItemsMsg'     => _('There are no users assigned to this role'),
                         'itemsGenerator' => array(
                                              'system' => 'Permission',
                                              'action' => 'generatePermissionScreenListItems',
                                             ),
                        );

        $usersList = GUI::createWidget($id.'-list', 'GUIList', $listSettings);

        $buttonSettings = array(
                           'value' => _('Add New'),
                           'click' => 'SimplePermissionPermissionScreen.addNew(\''.$resourceid.'\', \''.$roleid.'\')',
                          );
        $buttonWidget   = GUI::createWidget($id.'-addUser', 'GUIButton', $buttonSettings);

        $boxSettings = array(
                        'title'         => _('Users & Groups'),
                        'headerContent' => $buttonWidget,
                        'content'       => $usersList,
                       );
        $box = GUI::createWidget($id.'-box', 'GUIBox', $boxSettings);

        echo $box;
        echo '</div>';

    }//end _printRoleForScreen()


    /**
     * Generates list items for specified userids.
     *
     * @param string $assetid List of user/group ids.
     *
     * @return array
     *
     * @api            read
     * @api-permission superuser
     */
    public static function generatePermissionScreenListItems($assetid)
    {
        Channels::includeSystem('Asset');
        Channels::includeSystem('Attribute');

        $iconPath = Asset::getTypeIconPath(Asset::getTypeOfAsset($assetid));
        $name     = Attribute::getAttributeValue($assetid, 'name');
        $data     = array(
                     'itemContent' => '<div class="PermissionScreen-listItem" title="#'.$assetid.'"><img src="'.$iconPath.'" /> '.$name.'</div>',
                    );

        return $data;

    }//end generatePermissionScreenListItems()


    /**
     * Returns array of users for the specified resource and role.
     *
     * @param string $resource Resource ID to check access to.
     * @param string $roleid   Id of the Role to use for permissions.
     *
     * @return array
     */
    public static function getUsers($resource, $roleid)
    {
        $query  = DAL::getQuery('SimplePermission', 'getUsersByRoleAndResource');
        $result = DAL::getAssoc($query, 0);

        return $result;

    }//end getUsers()


    /**
     * Saved the changes in Permission screen.
     *
     * @param array $data Data from the permission screen.
     *
     * @return mixed
     * @throws ChannelException If there is an error while saving.
     */
    public static function savePermissionScreen(array $data)
    {
        Channels::includeSystem('User');
        if (User::isSystemAdministrator() === FALSE) {
            throw new ChannelException(_('You must be a super user to access this screen'));
        }

        $errors = array();
        try {
            DAL::beginTransaction();

            Channels::includeSystem('Role');
            $projects = Permission::getProjects();
            $roles    = Role::getRoles();

            foreach ($projects as $projectid => $projectName) {
                foreach ($roles as $roleid => $roleName) {
                    $index = 'PermissionScreen-role-'.$projectid.'-'.$roleid.'-list';
                    if (isset($data[$index]) === FALSE) {
                        // Not found in the save array for some reason..
                        continue;
                    }

                    $items        = array_keys($data[$index]['items']);
                    $removedItems = array_keys($data[$index]['removed']);

                    foreach ($removedItems as $removedid) {
                        if (strpos($removedid, 'new-') === 0) {
                            continue;
                        }

                        Permission::revokePermission($projectid, $roleid, $removedid);
                    }

                    foreach ($items as $itemid) {
                        if (strpos($itemid, 'new-') === 0 && in_array($itemid, $removedid) === FALSE) {
                            $newItemid = str_replace('new-', '', $itemid);
                            if (empty($newItemid) === FALSE) {
                                Permission::grantPermission($projectid, $roleid, $newItemid);
                            }
                        }
                    }
                }//end foreach
            }//end foreach

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollback();
            $errors[] = $e->getMessage();
        }//end try

        $retVal = TRUE;
        if (empty($errors) === FALSE) {
            $retVal = array('errors' => $errors);
        } else {
            $retVal = array('success' => TRUE);
        }

        return $retVal;

    }//end savePermissionScreen()


}//end class

?>