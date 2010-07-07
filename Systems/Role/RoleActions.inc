<?php
/**
 * Actions for the Role System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Role
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * RoleActions.
 *
 * Actions for the Role system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 *
 * @since 4.0.0
 */
class RoleActions
{


    /**
     * Role System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     *
     * @since 4.0.0
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
     * @param string $description Descript of a new role.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
