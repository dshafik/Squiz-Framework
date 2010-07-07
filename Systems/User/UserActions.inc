<?php
/**
 * Actions for the User System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage User
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * UserActions.
 *
 * Actions for the User system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 *
 * @since 4.0.0
 */
class UserActions
{


    /**
     * User System Constructor.
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
     * Create a new user.
     *
     * Returns an userid.
     *
     * @param string $username  Username for the user.
     * @param string $firstName First name of the user.
     * @param string $lastName  Last name of the user.
     * @param string $email     Email of the user.
     * @param string $password  Plain text password of the user.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function createUser($username, $firstName, $lastName, $email, $password)
    {

    }//end createUser()


    /**
     * Delete a user.
     *
     * Returns the number of users being deleted.
     *
     * @param integer $userid ID of the user.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function deleteUser($userid)
    {

    }//end deleteUser()


    /**
     * Update user details.
     *
     * Returns the number of users being updated.
     *
     * @param integer $userid    ID of the user.
     * @param string  $attrName  Attribute name to update.
     * @param string  $attrValue New value for the attribute.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function updateUser($userid, $attrName, $attrValue)
    {

    }//end updateUser()


    /**
     * Create a new user group.
     *
     * Returns an groupid.
     *
     * @param string $name  Name of user group.
     * @param string $email Optional user group email.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function createUserGroup($name, $email=NULL)
    {

    }//end createUserGroup()


    /**
     * Delete a user group.
     *
     * Returns the number of user groups being deleted.
     *
     * @param integer $groupid ID of the user group.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function deleteUserGroup($groupid)
    {

    }//end deleteUserGroup()


    /**
     * Update user group details.
     *
     * Returns the number of user groups being updated.
     *
     * @param integer $groupid   ID of the user group.
     * @param string  $attrName  Attribute name to update.
     * @param string  $attrValue New value for the attribute.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function updateUserGroup($groupid, $attrName, $attrValue)
    {

    }//end updateUserGroup()


    /**
     * Get the user details.
     *
     * Returns the information of the user.
     *
     * @param integer $userid ID of the user.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function getUserInfo($userid)
    {

    }//end getUserInfo()


    /**
     * Get the user details by username.
     *
     * Returns the information of the user.
     *
     * @param integer $userid ID of the user.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function getUserInfoByUsername($username)
    {

    }//end getUserInfoByUsername()


    /**
     * Get the user group details.
     *
     * Returns the information of the user group.
     *
     * @param integer $groupid ID of the user group.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function getUserGroupInfo($groupid)
    {

    }//end getUserGroupInfo()


    /**
     * Add a user to the group.
     *
     * Returns the number of users being added.
     *
     * @param integer $userid  ID of the user to add to the group.
     * @param integer $groupid ID of the user group.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function addUserToGroup($userid, $groupid)
    {

    }//end addUserToGroup()


    /**
     * Remove a user from the group.
     *
     * Returns the number of users being removed.
     *
     * @param integer $userid  ID of the user to add to the group.
     * @param integer $groupid ID of the user group.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function removeUserFromGroup($userid, $groupid)
    {

    }//end removeUserFromGroup()


    public static function isMemberOf($userid, $groupid)
    {

        return FALSE;

    }//end isMemberOf()


    public static function getUserGroups($userid)
    {

    }//end getUserGroups()


    public static function getGroupMembers($groupid)
    {

    }//end getGroupMembers()


    /**
     *
     *
     * @param 
     *
     * @since  4.0.0
     * @return 
     */
    public static function loginUser($userid, $password)
    {
    }//end loginUser()


    /**
     *
     *
     * @param 
     *
     * @since  4.0.0
     * @return 
     */
    public static function logoutUser()
    {
    }//end logoutUser()


    /**
     *
     *
     * @param 
     *
     * @since  4.0.0
     * @return 
     */
    public static function getCurrentUserid()
    {
    }//end getCurrentUserid()


    /**
     *
     *
     * @param 
     *
     * @since  4.0.0
     * @return 
     */
    public static function getCurrentUserInfo()
    {
    }//end getCurrentUserInfo()


}//end class

?>
