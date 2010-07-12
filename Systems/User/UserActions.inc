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
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            include_once 'Libs/Security/Security.inc';

            $userid = Asset::create('user');
            $name   = ucfirst($firstName).' '.ucfirst($lastName);
            Attribute::setAttributeValue($userid, 'username', $username);
            Attribute::setAttributeValue($userid, 'name', $name);
            Attribute::setAttributeValue($userid, 'email', $email);
            Attribute::setAttributeValue($userid, 'first_name', $firstName);
            Attribute::setAttributeValue($userid, 'last_name', $lastName);
            Attribute::setAttributeValue($userid, 'password', Security::hashUserPassword($password));
            DAL::commit();
        } catch (Exception $e) {
            $msg  = 'Failed to create a new user.'."\n";
            $msg .= $e->getMessage();
            DAL::rollback();
            throw new ChannelException();
        }

        return $userid;

    }//end createUser()


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
    public static function createUserGroup($name, $groupName=NULL)
    {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            include_once 'Libs/Security/Security.inc';

            $userid = Asset::create('userGroup');
            Attribute::setAttributeValue($userid, 'name', $groupName);
            DAL::commit();
        } catch (Exception $e) {
            $msg  = 'Failed to create a new user.'."\n";
            $msg .= $e->getMessage();
            DAL::rollback();
            throw new ChannelException();
        }

        return $userid;

    }//end createUserGroup()


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
    public static function getUserById($userid)
    {
        Channels::includeSystem('Asset');
        $userExists = Asset::exists($userid);
        if ($userExists === FALSE) {
            return NULL;
        }

        Channels::includeSystem('Attribute');
        $attributes   = Attribute::getAttributes(Asset::getTypeOfAsset($userid));
        $attributeids = array_keys($attributes);
        $userInfo     = Attribute::getAttributeValue($userid, $attributeids);
        return $userInfo;

    }//end getUserById()


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
    public static function getUserByUsername($username)
    {
        Channels::includeSystem('Asset');
        Channels::includeSystem('Attribute');
        $users = Attribute::getAssetsByAttributeValue(
            'user',
            'username',
            $username
        );

        if (empty($users) === TRUE) {
            return NULL;
        }

        $userid       = array_pop($users);
        $result       = User::getUserById($userid);
        $result['id'] = $userid;
        return $result;

    }//end getUserByUsername()


    /**
     * Returns TRUE if the given username already exists.
     *
     * @param string $username Username to check.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function usernameExists($username)
    {
        Channels::includeSystem('Attribute');
        $users = Attribute::getAssetsByAttributeValue(
            'user',
            'username',
            $username
        );

        if (empty($users) === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end usernameExists()


    /**
     * Return TRUE if the given userid belongs to the given user group.
     *
     * @param float $userid  Userid to check.
     * @param float $groupid Groupid to check against the userid.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isMemberOf($userid, $groupid)
    {
        Channels::includeSystem('Linking');
        $childrenQ = Linking::getChildren($groupid);
        $userids   = DAL::getAssoc($childrenQ, 0);
        if (in_array($userid, $userids) === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end isMemberOf()


    /**
     * Return the list of user groups which the given user belongs to.
     *
     * @param float $userid Userid to check.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getUserGroups($userid)
    {
        Channels::includeSystem('Linking');
        $parentQ  = Linking::getParents($userid, 0, 65525, array('userGroup'));
        $groupids = DAL::getAssoc($parentQ, 0);
        return $groupids;

    }//end getUserGroups()


    /**
     * Log in the user with the given password.
     *
     * If the username is not found or the password is not matched,
     * it returns FALSE. Otherwise, it returns TRUE and sets the session
     * as a logged in user.
     *
     * @param string $username Username of the user to log in.
     * @param string $password Password of the user to log in.
     *
     * @api    read
     * @since  4.0.0
     * @return boolean
     */
    public static function loginUser($username, $password)
    {
        $user = User::getUserByUsername($username);
        if ($user === NULL) {
            // No user exists with the given username.
            return FALSE;
        }

        $userid       = $user['id'];
        $currPassword = $user['password'];

        // Password Matched.
        include_once 'Libs/Security/Security.inc';
        if ($currPassword !== Security::hashUserPassword($password)) {
            return FALSE;
        }

        Channels::includeSystem('Session');
        Session::setSessionData(
            'User',
            'CurrentUserid',
            $userid
        );

        return TRUE;

    }//end loginUser()


    /**
     * Log out the currently logged in user.
     *
     * @api    read
     * @since  4.0.0
     * @return boolean
     */
    public static function logoutUser()
    {
        $userid = User::getCurrentUserid();
        if ($userid === NULL) {
            return FALSE;
        }

        Channels::includeSystem('Session');
        Session::setSessionData('User', 'CurrentUserid', NULL);

        return TRUE ;

    }//end logoutUser()


    /**
     * Return the userid of the currently logged in user.
     *
     * @api    read
     * @since  4.0.0
     * @return float
     */
    public static function getCurrentUserid()
    {
        Channels::includeSystem('Session');
        $userid = Session::getSessionData('User', 'CurrentUserid');
        return $userid;

    }//end getCurrentUserid()


}//end class

?>
