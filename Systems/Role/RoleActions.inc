<?php
/**
 * Actions for the Role System.
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
 * @subpackage Role
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * RoleActions.
 *
 * Actions for the Role system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 */
class RoleActions
{


    /**
     * Role System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Create a new role.
     *
     * Returns the roleid created.
     *
     * @param string $name Name of a new role.
     *
     * @return integer
     */
    public static function createRole($name)
    {
        $roleid = DAL::seqNextVal('seq_roleid');
        Channels::addToBasket('roleid', $roleid);

        $query = DAL::getQuery('Role', 'addRole');
        DAL::executeQuery($query);

        return $roleid;

    }//end createRole()


    /**
     * Update role details.
     *
     * Returns the number of rows updated.
     *
     * @param integer $roleid ID of a role.
     * @param string  $name   New name for the role.
     *
     * @return integer
     */
    public static function updateRole($roleid, $name)
    {
        $query  = DAL::getQuery('Role', 'updateRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end updateRole()


    /**
     * Delete a role.
     *
     * Returns the number of rows deleted.
     *
     * @param integer $roleid ID of the role to be deleted.
     *
     * @return integer
     */
    public static function deleteRole($roleid)
    {
        $query  = DAL::getQuery('Role', 'deleteRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end deleteRole()


    /**
     * Create a new privilege.
     *
     * @param string $privilege   Privilege id.
     * @param string $name        Name of the privilege.
     * @param string $description Description of the privilege.
     *
     * @return integer
     */
    public static function createPrivilege($privilege, $name, $description)
    {
        $query  = DAL::getQuery('Role', 'addPrivilege');
        $result = DAL::executeQuery($query);
        return $result;

    }//end createPrivilege()


    /**
     * Update privilege details.
     *
     * Returns the number of rows updated.
     *
      * @param string $privilege   Privilege id.
     * @param string $name        Name of the privilege.
     * @param string $description Description of the privilege.
     *
     * @return integer
     */
    public static function updatePrivilege($roleid, $name, $description)
    {
        $query  = DAL::getQuery('Role', 'updatePrivilege');
        $result = DAL::executeQuery($query);
        return $result;

    }//end updatePrivilege()


    /**
     * Remove privilege.
     *
     * @param string $privilege Privilege to remove.
     *
     * @return integer
     */
    public static function deletePrivilege($privilege)
    {
        $query  = DAL::getQuery('Role', 'deletePrivilege');
        $result = DAL::executeQuery($query);
        return $result;

    }//end deletePrivilege()


    /**
     * Grant a privilege to a role.
     *
     * Returns the number of rows updated.
     *
     * @param integer $roleid    ID of the role to grant a privilege.
     * @param string  $privilege Privilege to grant.
     *
     * @return integer
     */
    public static function grantPrivilege($roleid, $privilege)
    {
        $query  = DAL::getQuery('Role', 'grantPrivilegeToRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end grantPrivilege()


    /**
     * Remove a privilege from a role.
     *
     * Returns the number of rows updated.
     *
     * @param integer $roleid    ID of the role to remove a privilege from.
     * @param string  $privilege Privilege to remove.
     *
     * @return integer
     */
    public static function revokePrivilege($roleid, $privilege)
    {
        $query  = DAL::getQuery('Role', 'removePrivilegeFromRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end revokePrivilege()


    /**
     * Returns all the roles in the system.
     *
     * @return array
     */
    public static function getRoles()
    {
        $query  = DAL::getQuery('Role', 'getRoles');
        $result = DAL::getAssoc($query);

        $roles = array();
        foreach ($result as $roleInfo) {
            $roles[$roleInfo['roleid']] = $roleInfo['name'];
        }

        return $roles;

    }//end getRoles()


    /**
     * Returns all the roles in the system.
     *
     * @return array
     */
    public static function getPrivileges()
    {
        $query  = DAL::getQuery('Role', 'getPrivileges');
        $result = DAL::getAssoc($query);

        $privileges = array();
        foreach ($result as $privInfo) {
            $privileges[$privInfo['privilege']] = array(
                                                   'name'        => $privInfo['name'],
                                                   'description' => $privInfo['description'],
                                                  );
        }

        return $privileges;

    }//end getPrivileges()


    /**
     * Returns all the privileges that role has.
     *
     * @param integer $roleid ID of role.
     *
     * @return array
     */
    public static function getPrivilegesByRole($roleid)
    {
        $query  = DAL::getQuery('Role', 'getPrivilegesByRole');
        $result = DAL::getAssoc($query, 0);
        return $result;

    }//end getPrivilegesByRole()


    /**
     * Returns all roles that have the privilege.
     *
     * @param string $privilege Privilege that role owns.
     *
     * @return array
     */
    public static function getRolesByPrivilege($privilege)
    {
        $query  = DAL::getQuery('Role', 'getRolesByPrivilege');
        $result = DAL::getAssoc($query);
        return $result;

    }//end getRolesByPrivilege()


    public static function getPrivilegeListForRolesScreen()
    {
        $privileges = Role::getPrivileges();

        ksort($privileges);

        // Get the list of top level privileges.
        $structured = array();
        $prevTop    = NULL;
        $previous   = NULL;
        $prevLevel  = 0;
        foreach ($privileges as $privilege => $info) {
            $privParts = explode('.', $privilege);
            if (strpos($privilege, $prevTop.'.') !== 0) {
                $prevTop  = $privParts[0];
                $structured[$prevTop] = array();
            }

            $level = count($privParts);
            if ($prevLevel < $level && $previous !== NULL) {
                $structured[$prevTop][$previous]['hasChildren'] = TRUE;
            }

            $structured[$prevTop][$privilege] = $info;

            $prevLevel = $level;
            $previous  = $privilege;
        }

        // Get the roles.
        $roles = Role::getRoles();
        foreach ($roles as $roleid => $name) {
            //error_log(print_r(Role::getPrivilegesByRole($roleid), 1));
        }

        $items = array();
        foreach ($structured as $topLevelName => $privs) {
            $items[] = array(
                        'id'          => $topLevelName,
                        'label'       => _($topLevelName),
                        'itemContent' => self::_getPrivilegeListItemContent($topLevelName, $privs),
                       );
        }

        return $items;

    }//end getPrivilegeListForRolesScreen()


    private static function _getPrivilegeListItemContent($topLevelName, $privileges)
    {
        $id    = $topLevelName;
        $class = 'RoleScreen';

        $content  = '<div class="'.$class.'-privListContents">';

        // Start unrestricted option box.
        $content .= '<div class="'.$class.'-unrestrictedCont">';
        $content .= '<span class="'.$class.'-unrestrictedText">';
        $content .= sprintf(_('Grant unrestricted access to %s privileges'), $topLevelName);
        $content .= '</span>';
        $content .= '<div class="'.$class.'-unrestrictedToggle">';
        $content .= GUI::createWidget($id.'-unrestricted', 'GUIToggleButton');
        $content .= '</div>';
        $content .= '</div>';

        $content .= '<div class="'.$class.'-privilegeList">';

        $class .= '-privListItem';
        foreach ($privileges as $privid => $info) {
            $content .= '<div id="'.$privid.'" class="'.$class;

            // Check if it has child pivileges.
            if (isset($info['hasChildren']) === TRUE) {
                $content .= ' expand';
            }

            // Get the level.
            $parts = explode('.', $privid);

            $content .= ' level-'.(count($parts) - 1);

            // End class attribute.
            $content .= '">';

            $content .= '<div class="'.$class.'-icon"></div>';

            // Name of the privilege.
            $content .= '<div class="'.$class.'-title">'._($info['name']).'</div>';

            // Enable/Disable icon.
            $content .= '<div class="'.$class.'-toggle"></div>';
            $content .= '</div>';
        }

        $content .= '</div>';
        $content .= '</div>';

        return $content;

    }//end _getPrivilegeListItemContent()


}//end class

?>
