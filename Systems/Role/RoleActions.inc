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
     * @param string $name        Name of a new role.
     * @param string $description Description of a new role.
     *
     * @return integer
     */
    public static function createRole($name, $description)
    {
        $roleid = DAL::seqNextVal('seq_roleid');
        Channels::addToBasket('roleid', $roleid);

        $query = DAL::getQuery('Role', 'addRole');
        DAL::executeQuery($query);

        return $roleid;

    }//end createRole()


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
     * Update role details.
     *
     * Returns the number of rows updated.
     *
     * @param integer $roleid    ID of a role.
     * @param string  $attrName  Attribute name to update.
     * @param string  $attrValue New value for the attribute.
     *
     * @return integer
     */
    public static function updateRole($roleid, $attrName, $attrValue)
    {
        $query  = DAL::getQuery('Role', 'updateRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end updateRole()


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
    public static function grantPrivilegeToRole($roleid, $privilege)
    {
        $query  = DAL::getQuery('Role', 'grantPrivilegeToRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end grantPrivilegeToRole()


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
    public static function removePrivilegeFromRole($roleid, $privilege)
    {
        $query  = DAL::getQuery('Role', 'removePrivilegeFromRole');
        $result = DAL::executeQuery($query);
        return $result;

    }//end removePrivilegeFromRole()


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
        $result = DAL::getAssoc($query);
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
     * Remove the privilege.
     *
     * Returns the number rows been deleted.
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


}//end class

?>
