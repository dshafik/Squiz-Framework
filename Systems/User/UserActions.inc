<?php
/**
 * Actions for the User System.
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
 * @subpackage User
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * UserActions.
 *
 * Actions for the User system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 */
class UserActions
{


    /**
     * User System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
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
     * @return integer
     * @throws ChannelException If the user fails to create.
     */
    public static function createUser(
        $username,
        $firstName,
        $lastName,
        $email,
        $password
    ) {
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
        }//end try

        return $userid;

    }//end createUser()


    /**
     * Create a new user group.
     *
     * Returns the asset ID of the new group.
     *
     * @param string $name Name of user group.
     *
     * @return integer
     * @throws ChannelException If the group fails to create.
     */
    public static function createUserGroup($name)
    {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            include_once 'Libs/Security/Security.inc';

            $userid = Asset::create('userGroup');
            Attribute::setAttributeValue($userid, 'name', $name);
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
     * Returns information about the specified users.
     *
     * @param array $userids Array of userids.
     *
     * @return array
     *
     * @api read
     */
    public static function getUsers(array $userids)
    {
        $info = array();
        foreach ($userids as $userid) {
            $user = User::getUserById($userid);

            // Remove the password.
            unset($user['password']);

            $info[$userid] = $user;
        }

        return $info;

    }//end getUsers()


    /**
     * Get the user details by username.
     *
     * Returns the information of the user.
     *
     * @param integer $username Username of the user.
     *
     * @return integer
     */
    public static function getUserByUsername($username)
    {
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
     * @return boolean
     *
     * @api read
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
        Channels::includeSystem('Session');
        $challenge       = Session::getSessionData('User', $username);
        $passwordMatched = FALSE;

        if (Security::validateUserPassword($password, $currPassword, $challenge) === TRUE) {
            $passwordMatched = TRUE;
        }

        if ($passwordMatched === TRUE) {
            Channels::includeSystem('Session');
            Session::setSessionData(
                'User',
                'CurrentUserid',
                $userid
            );
        }

        return $passwordMatched;

    }//end loginUser()


    /**
     * Log out the currently logged in user.
     *
     * @return boolean
     *
     * @api read
     */
    public static function logoutUser()
    {
        $userid = User::getCurrentUserid();
        if ($userid === NULL) {
            return FALSE;
        }

        Channels::includeSystem('Session');
        Session::setSessionData('User', 'CurrentUserid', NULL);

        return TRUE;

    }//end logoutUser()


    /**
     * Return the userid of the currently logged in user.
     *
     * @return float
     *
     * @api read
     */
    public static function getCurrentUserid()
    {
        Channels::includeSystem('Session');
        $userid = Session::getSessionData('User', 'CurrentUserid');
        return $userid;

    }//end getCurrentUserid()


    /**
     * Process authentication requests.
     *
     * @return string
     *
     * @api read
     */
    public static function processAuthentication()
    {
        include_once 'Libs/Security/Security.inc';
        $authAction = Security::getRequestData('auth_action', NULL, FALSE);
        if ($authAction !== NULL) {
            return self::_handleLoginFormRequest($authAction);
        }

        $userid = User::getCurrentUserid();
        if ($userid === NULL) {
            // Print login screen and exit.
            Channels::includeSystem('GUI');
            GUI::printTemplate('User', 'LoginScreen.tpl');
            exit;
        }

    }//end processAuthentication()


    /**
     * Handles the login form requests.
     *
     * @param string $action The request action.
     *
     * @return string
     */
    private static function _handleLoginFormRequest($action)
    {
        $response = array();

        include_once 'Libs/Security/Security.inc';
        switch ($action) {
            case 'logout':
                $res = User::logoutUser();
                exit;
            break;

            case 'login':
                $step = Security::getRequestData('step', NULL, FALSE);
                if ($step === NULL) {
                    return FALSE;
                }

                $username = Security::getRequestData('username', NULL, FALSE);
                $username = html_entity_decode($username);
                switch (strtolower($step)) {
                    case 'req_chal':
                        try {
                            Channels::includeSystem('Session');
                            $challenge = Security::createChallengeString();
                            Session::setSessionData('User', $username, $challenge);
                        } catch (Exception $e) {
                            $challenge = array();
                        }

                        if (empty($challenge) === TRUE) {
                            // Cannot find this user from the database.
                            $response['challenge'] = 0;
                            $response['error']     = _('Incorrect username/password combination');
                        } else {
                            $response['challenge'] = $challenge;
                        }
                    break;

                    case 'req_auth':
                        $password = Security::getRequestData('password', NULL, FALSE);
                        $password = html_entity_decode($password);
                        if (User::loginUser($username, $password) === FALSE) {
                            $response['login'] = 0;
                            $response['error'] = _('Incorrect username/password combination');
                        } else {
                            $response['login'] = 1;
                        }
                    break;

                    default:
                        // Unknown step. Return error.
                    break;
                }//end switch
            break;

            default:
                // Don't know what to do.
            break;
        }//end switch

        return json_encode($response);

    }//end _handleLoginFormRequest()


    /**
     * Returns array of super users in the system.
     *
     * @param string  $userid  Id of the super user.
     * @param boolean $idsOnly If TRUE then only the userids returned.
     *
     * @return array
     */
    public static function getSuperUsers($userid=NULL, $idsOnly=FALSE)
    {
        $query = DAL::getQuery('User', 'getSuperUsers');
        $users = array();

        if ($idsOnly === TRUE) {
            $users = DAL::getAssoc($query, 'userid', 0);
        } else {
            $users = DAL::getAssoc($query, 'userid');
        }

        return $users;

    }//end getSuperUsers()


    /**
     * Returns array of super users in the system.
     *
     * @param string $userid  Asset id of a user.
     * @param string $addedBy Asset id of the user whos adding the new super user.
     *
     * @return void
     * @throws ChannelException If the user cannot be added.
     */
    public static function addSuperUser($userid, $addedBy=NULL)
    {
        if ($addedBy === NULL) {
            $addedBy = User::getCurrentUserid();
            Channels::modifyBasket('addedBy', $addedBy);
        }

        include_once 'Libs/String/String.inc';
        $addedDate = String::tsIso8601(time());
        Channels::addToBasket('addedDate', $addedDate);

        try {
            DAL::beginTransaction();
            $query = DAL::getQuery('User', 'addSuperUser');
            DAL::executeQuery($query);
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }

    }//end addSuperUser()


    /**
     * Returns array of super users in the system.
     *
     * @param string $userid Asset id of a user.
     *
     * @return void
     * @throws ChannelException If user cannot be removed.
     */
    public static function removeSuperUser($userid)
    {
        $currentUserid = User::getCurrentUserid();
        if ($currentUserid === $userid) {
            throw new ChannelException(_('You cannot remove your self from super users list'));
        }

        try {
            DAL::beginTransaction();
            $query = DAL::getQuery('User', 'removeSuperUser');
            DAL::executeQuery($query);
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }

    }//end removeSuperUser()


    /**
     * Returns TRUE if the user is a system administrator.
     *
     * @param integer $userid The id for user.
     *
     * @return integer
     */
    public static function isSystemAdministrator($userid=NULL)
    {
        if ($userid === NULL) {
            $userid = User::getCurrentUserid();
        }

        $users = User::getSuperUsers($userid);
        if (empty($users) === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end isSystemAdministrator()


    /**
     * Returns the list of rows for the Super Users table.
     *
     * @return array
     */
    public static function getSuperUsersList()
    {
        $list  = array();
        $users = User::getSuperUsers();

        foreach ($users as $user) {
            $list[$user['userid']] = User::getSuperUserTableColumns($user['userid']);
        }

        // If there is only 1 user then they cannot be removed so do not show remove icon.
        if (count($list) === 1) {
            $userid = key($list);
            $list[$userid]['_remove'] = '';
        }

        return $list;

    }//end getSuperUsersList()


    /**
     * Returns the contents for a new super users table row.
     *
     * @param string $userid Column data for the new row.
     *
     * @return array
     *
     * @api read
     */
    public static function getSuperUserTableColumns($userid)
    {
        $currentUser = User::getCurrentUserid();
        $user        = User::getUserById($userid);

        $columns = array();
        $columns['user']      = $user['first_name'].' '.$user['last_name'].' ('.$user['username'].')';
        $columns['email']     = $user['email'];
        $columns['lastLogin'] = $user['last_login'];

        if ((string) $currentUser === (string) $userid) {
            // Do not show the remove icon for this row.
            $columns['_remove'] = '';
        }

        if (empty($columns['lastLogin']) === TRUE) {
            $columns['lastLogin'] = _('Never');
        }

        return $columns;

    }//end getSuperUserTableColumns()


    /**
     * Save the SuperUsers screen settings.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function saveSuperUsersScreen(array $data)
    {
        $errors = array();

        $superUsersList = Util::getArrayIndex($data, 'superUsersList', NULL);
        if ($superUsersList === NULL) {
            // Nothing to do.
            $errors[] = _('Could not save super users');
            return $errors;
        }

        $superUsers   = Util::getArrayIndex($superUsersList, 'items', array());
        $removedUsers = Util::getArrayIndex($superUsersList, 'removed', array());
        $removedUsers = array_keys($removedUsers);

        // Get the current super users.
        $currentUsers = User::getSuperUsers();

        try {
            DAL::beginTransaction();
            foreach ($superUsers as $newUserid => $info) {
                if (in_array($newUserid, $currentUsers) === FALSE) {
                    User::addSuperUser($newUserid);
                }
            }

            // Remove users.
            foreach ($removedUsers as $removedUserid) {
                User::removeSuperUser($removedUserid);
            }

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollBack();
            $errors[] = _('Failed to add super users');
        }

        $retVal = TRUE;
        if (empty($errors) === FALSE) {
            $retVal = array('errors' => $errors);
        } else {
            $retVal = array('success' => TRUE);
        }

        return $retVal;

    }//end saveSuperUsersScreen()


}//end class

?>
