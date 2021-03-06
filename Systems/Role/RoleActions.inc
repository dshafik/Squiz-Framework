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
    public static function updatePrivilege($privilege, $name, $description)
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
     * @throws ChannelException If there is a DB error.
     */
    public static function grantPrivilege($roleid, $privilege)
    {
        try {
            DAL::beginTransaction();
            if (strpos($privilege, '%') !== FALSE) {
                // First remove all the sub privileges and this one.
                $query  = DAL::getQuery('Role', 'removePrivilegesLike');
                $result = DAL::executeQuery($query);
            }

            $query  = DAL::getQuery('Role', 'grantPrivilegeToRole');
            $result = DAL::executeQuery($query);
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }

        return $result;

    }//end grantPrivilege()


    /**
     * Grant privileges to a set of roles.
     *
     * @param array $roleids    Array of role ids.
     * @param array $privileges Array of privileges.
     *
     * @return void
     */
    public static function batchGrantPrivileges(array $roleids, array $privileges)
    {
        foreach ($roleids as $roleid) {
            foreach ($privileges as $privilege) {
                Role::grantPrivilege($roleid, $privilege);
            }
        }

    }//end batchGrantPrivileges()


    /**
     * Remove a privilege from a role.
     *
     * Returns the number of rows updated.
     *
     * @param integer $roleid    ID of the role to remove a privilege from.
     * @param string  $privilege Privilege to remove.
     *
     * @return integer
     * @throws ChannelException If there is a DB error.
     */
    public static function revokePrivilege($roleid, $privilege)
    {
        try {
            DAL::beginTransaction();
            if (strpos($privilege, '%') !== FALSE) {
                // First remove all the sub privileges and this one.
                $query  = DAL::getQuery('Role', 'removePrivilegesLike');
                $result = DAL::executeQuery($query);
            } else {
                $query  = DAL::getQuery('Role', 'removePrivilegeFromRole');
                $result = DAL::executeQuery($query);
            }

            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }

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
     * Returns the number of roles in the system.
     *
     * Used for showing or hiding
     *
     * @return integer
     */
    public static function getRoleCount()
    {
        $roles = Role::getRoles();
        return count($roles);

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


    /**
     * Returns content for the privilege list in Roles screen.
     *
     * @return array
     */
    public static function getPrivilegeListForRolesScreen()
    {
        $privileges = Role::getPrivileges();
        ksort($privileges);

        // Get the roles privileges for the first role.
        $roles = Role::getRoles();
        ksort($roles);

        $initRoleid   = key($roles);
        $grantedPrivs = Role::getPrivilegesByRole($initRoleid);
        sort($grantedPrivs);

        // Get the list of top level privileges.
        $structured = array();
        $prevTop    = NULL;
        $previous   = NULL;
        $prevLevel  = 0;

        $unrestricted = array();
        foreach ($privileges as $privilege => $info) {
            $privParts = explode('.', $privilege);
            if (strpos($privilege, $prevTop.'.') !== 0) {
                $prevTop              = $privParts[0];
                $structured[$prevTop] = array();
            }

            $level = count($privParts);
            if ($prevLevel < $level && $previous !== NULL) {
                $structured[$prevTop][$previous]['hasChildren'] = TRUE;
            }

            $structured[$prevTop][$privilege] = $info;

            $selected = FALSE;
            foreach ($grantedPrivs as $grantedPriv) {
                if ($grantedPriv === $prevTop.'.%') {
                    $unrestricted[] = $prevTop;
                    $selected       = 'sel-inherit';
                    break;
                } else if ($privilege === $grantedPriv || $privilege.'.%' === $grantedPriv) {
                    $selected = TRUE;
                    break;
                } else if (strpos($privilege, str_replace('%', '', $grantedPriv)) === 0) {
                    $selected = 'sel-inherit';
                    break;
                }
            }

            $structured[$prevTop][$privilege]['selected'] = $selected;

            $prevLevel = $level;
            $previous  = $privilege;
        }//end foreach

        $items = array();
        foreach ($structured as $topLevelName => $privs) {
            $items[] = array(
                        'id'          => $topLevelName,
                        'label'       => ucfirst(_($topLevelName)),
                        'itemContent' => self::_getPrivilegeListItemContent($topLevelName, $privs, $unrestricted),
                       );
        }

        return $items;

    }//end getPrivilegeListForRolesScreen()


    /**
     * Returns content for the privilege list in Roles screen.
     *
     * @param string $topLevelName Name of the top level privilege.
     * @param array  $privileges   List of privileges in the system.
     * @param array  $unrestricted List of top leve privileges that have unrestricted access.
     *
     * @return string
     */
    private static function _getPrivilegeListItemContent($topLevelName, array $privileges, array $unrestricted)
    {
        Channels::includeSystem('GUI');

        $id    = $topLevelName;
        $class = 'RoleScreen';

        $content = '<div class="'.$class.'-privListContents">';

        // Start unrestricted option box.
        $content .= '<div class="'.$class.'-unrestrictedCont">';
        $content .= '<span class="'.$class.'-unrestrictedText">';
        $content .= sprintf(_('Grant unrestricted access to %s privileges'), $topLevelName);
        $content .= '</span>';
        $content .= '<div class="'.$class.'-unrestrictedToggle">';

        $settings = array();
        if (in_array($topLevelName, $unrestricted) === TRUE) {
            $settings['value'] = TRUE;
        }

        $content .= GUI::createWidget($id.'-unrestricted', 'GUIToggleButton', $settings);
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

            if ($info['selected'] === TRUE) {
                $content .= ' enabled';
            } else if ($info['selected'] === 'sel-inherit') {
                $content .= ' enabledByParent';
            }

            // End class attribute.
            $content .= '">';

            $content .= '<div class="'.$class.'-icon"></div>';

            // Name of the privilege.
            $content .= '<div class="'.$class.'-title">'._($info['name']).'</div>';

            // Enable/Disable icon.
            $content .= '<div class="'.$class.'-toggle"></div>';
            $content .= '</div>';
        }//end foreach

        $content .= '</div>';
        $content .= '</div>';

        return $content;

    }//end _getPrivilegeListItemContent()


    /**
     * Returns the JS code to initialise the Roles Screen.
     *
     * @return string
     */
    public static function getRolesScreenInitCode()
    {
        $data  = array();
        $roles = Role::getRoles();
        ksort($roles);

        $grantedPrivs = array();
        foreach ($roles as $roleid => $roleName) {
            $grantedPrivs = Role::getPrivilegesByRole($roleid);
            sort($grantedPrivs);

            $unrestricted = array();
            foreach ($grantedPrivs as $priv) {
                $parts = explode('.%', $priv);
                if ($parts[0].'.%' === $priv) {
                    $unrestricted[] = $parts[0];
                }
            }

            $data[$roleid] = array(
                              'name'         => $roleName,
                              'granted'      => $grantedPrivs,
                              'unrestricted' => $unrestricted,
                             );
        }

        $data = json_encode($data);
        return '<script>RoleRoleScreen.initScreen('.$data.');</script>';

    }//end getRolesScreenInitCode()


    /**
     * Save the role screen changes.
     *
     * @param array $data The new settings.
     *
     * @return array
     */
    public static function saveRoleScreen(array $data)
    {
        try {
            DAL::beginTransaction();
            $errors       = array();
            $templateData = Util::getArrayIndex($data, 'templateData', NULL);

            if ($templateData === NULL) {
                $errors[] = _('Empty template data');
            } else {
                $roles        = Util::getArrayIndex($templateData, 'roles', array());
                $currentRoles = Role::getRoles();

                // Delete roles.
                $removedRoles = Util::getArrayIndex($templateData, 'removedRoles', array());
                foreach ($removedRoles as $roleid => $state) {
                    if (strpos($roleid, 'new_') === FALSE) {
                        // Not a new role so delete it.
                        Role::deleteRole($roleid);
                    }
                }

                // Add or update roles.
                foreach ($roles as $roleid => $info) {
                    if (isset($removedRoles[$roleid]) === TRUE) {
                        // Role was removed so do not update or add it.
                        continue;
                    } else if (isset($currentRoles[$roleid]) === TRUE) {
                        // Existing role.
                        // Check if name nees updating.
                        if ($currentRoles[$roleid] !== $info['name']) {
                            Role::updateRole($roleid, $info['name']);
                        }

                        $currentPrivs = Role::getPrivilegesByRole($roleid);
                        $removedPrivs = array_diff($currentPrivs, $info['granted']);
                        $newPrivs     = array_diff(self::_filterPrivileges($info['granted']), $currentPrivs);

                        // Check unrestricted setting..
                        $unrestrictedAccess = array();
                        foreach ($info['unrestricted'] as $privilege) {
                            $unrestrictedAccess[] = $privilege;
                            Role::grantPrivilege($roleid, $privilege.'.%');

                            // Remove all privileges under this privilege.
                            foreach ($removedPrivs as $i => $removedPriv) {
                                if (strpos($removedPriv, $privilege.'.') === 0) {
                                    unset($removedPrivs[$i]);
                                }
                            }

                            foreach ($newPrivs as $i => $newPriv) {
                                if (strpos($newPriv, $privilege.'.') === 0) {
                                    unset($newPrivs[$i]);
                                }
                            }
                        }

                        foreach ($removedPrivs as $privilege) {
                            Role::revokePrivilege($roleid, $privilege);
                        }

                        foreach ($newPrivs as $privilege) {
                            Role::grantPrivilege($roleid, $privilege);
                        }
                    } else if (strpos($roleid, 'new_') === 0) {
                        // New role.
                        $newRoleid  = Role::createRole($info['name']);
                        $privileges = self::_filterPrivileges($info['granted']);
                        Role::batchGrantPrivileges(array($newRoleid), $privileges);
                    }//end if
                }//end foreach
            }//end if

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollBack();
            $errors[] = $e->getMessage();
        }//end try

        $retVal = TRUE;
        if (empty($errors) === FALSE) {
            $retVal = array('errors' => $errors);
        }

        return $retVal;

    }//end saveRoleScreen()


    /**
     * Filters list of privileges.
     *
     * Filtering will remove child privileges from the list.
     *
     * @param array $privileges List of privileges.
     *
     * @return array
     */
    private static function _filterPrivileges(array $privileges)
    {
        $filtered   = array();
        $prevParent = NULL;
        foreach ($privileges as $privilege) {
            if (strpos($privilege, $prevParent) !== FALSE) {
                continue;
            }

            if (strpos($privilege, '.%') !== FALSE) {
                $prevParent = str_replace('%', '', $privilege);
            }

            $filtered[] = $privilege;
        }

        return $filtered;

    }//end _filterPrivileges()


}//end class

?>
