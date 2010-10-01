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
     *
     * @api write
     * @api-permission superuser
     */
    public static function createUser(
        $username,
        $firstName,
        $lastName,
        $email,
        $password,
        array $userGroups=array()
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

            User::addUsersToGroups(array($userid), $userGroups);

            DAL::commit();
        } catch (Exception $e) {
            $msg  = 'Failed to create a new user.'."\n";
            $msg .= $e->getMessage();
            DAL::rollback();
            throw new ChannelException($msg);
        }//end try

        return $userid;

    }//end createUser()


    /**
     * Create a new user group.
     *
     * Returns the asset ID of the new group.
     *
     * @param string $name   Name of user group.
     * @param string $parent Asset ID of the parent group/folder.
     *
     * @return integer
     * @throws ChannelException If the group fails to create.
     *
     * @api write
     * @api-permission superuser
     */
    public static function createUserGroup($name, $parent)
    {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            Channels::includeSystem('Linking');
            include_once 'Libs/Security/Security.inc';

            $groupid = Asset::create('userGroup');
            Attribute::setAttributeValue($groupid, 'name', $name);

            // Link it under specified parent.
            Linking::createLink($parent, $groupid);
            DAL::commit();
        } catch (Exception $e) {
            $msg  = 'Failed to create a new user.'."\n";
            $msg .= $e->getMessage();
            DAL::rollback();
            throw new ChannelException($msg);
        }

        return $groupid;

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
     * Adds user to given user groups.
     *
     * @param string $userids    Id of the user.
     * @param array  $userGroups Array of user group assetids.
     *
     * @return boolean
     *
     * @api            write
     * @api-permission superuser
     */
    public static function addUsersToGroups(array $userids, array $userGroups)
    {
        Channels::includeSystem('Linking');
        foreach ($userids as $userid) {
            foreach ($userGroups as $userGroupid) {
                // TODO: Add a check to make sure that its a valid usergroup.
                Linking::createLink($userGroupid, $userid);
            }
        }

        return TRUE;

    }//end addUsersToGroups()


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
     * @api            read
     * @api-permission public
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
     * @api            read
     * @api-permission public
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
     * @api            read
     * @api-permission public
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
     * @api            read
     * @api-permission public
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
        Channels::includeSystem('SquizSuite');
        if (SquizSuite::isSuiteSystemUser() === TRUE) {
            return TRUE;
        }

        if ($userid === NULL) {
            $userid = User::getCurrentUserid();
        }

        if ($userid === NULL) {
            return FALSE;
        }

        $users = User::getSuperUsers($userid, TRUE);
        if (empty($users) === TRUE) {
            return FALSE;
        }

        if (in_array($userid, $users) === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end isSystemAdministrator()


    /**
     * Create a new users folder with specified name.
     *
     * Returns the assetid of the new folder.
     *
     * @param string $title    The title of the folder.
     * @param string $subTitle The sub title of the folder.
     *
     * @return string
     * @throws ChannelException If the folder cannot be created.
     */
    public static function createUsersFolder($title, $subTitle)
    {
        $folderid = NULL;
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            Channels::includeSystem('Linking');

            $folderid = Asset::create('folder');
            Attribute::setAttributeValue($folderid, 'name', $title);

            $userFolders = array();
            $cacheFile   = BaseSystem::getDataDir('User').'/folders.inc';
            if (file_exists($cacheFile) === TRUE) {
                include $cacheFile;
            }

            $userFolders[$folderid] = array(
                                                  'title'    => $title,
                                                  'subTitle' => $subTitle,
                                                 );

            include_once 'Libs/FileSystem/FileSystem.inc';
            $content = '<?php $usersFolder = '.var_export($userFolders, TRUE).' ?>';
            FileSystem::filePutContents($cacheFile, $content);

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollback();

            $msg  = _('Failed to create a user folder.')."\n";
            $msg .= $e->getMessage();
            throw new ChannelException($msg);
        }

        return $folderid;

    }//end createUsersFolder()


    /**
     * Edit specified users folder's name.
     *
     * @param string $folderid Asset id of a user folder.
     * @param string $title    The title of the folder.
     * @param string $subTitle The sub title of the folder.
     *
     * @return void
     * @throws ChannelException If the folder cannot be edited.
     */
    public static function editUsersFolder($folderid, $title, $subTitle)
    {
        $cacheFile = BaseSystem::getDataDir('User').'/folders.inc';
        if (file_exists($cacheFile) === FALSE) {
            throw new ChannelException(_('There are no user folders in the system'));
        }

        include $cacheFile;

        if (isset($userFolders[$folderid]) === FALSE) {
            throw new ChannelException(_('User folder does not exist'));
        }

        try {
            DAL::beginTransaction();
            Channels::includeSystem('Attribute');
            Attribute::setAttributeValue($folderid, 'name', $title);

            $userFolders[$folderid] = array(
                                       'title'    => $title,
                                       'subTitle' => $subTitle,
                                      );

            $content = '<?php $usersFolder = '.var_export($userFolders, TRUE).' ?>';
            FileSystem::filePutContents($cacheFile, $content);

            DAL::commit();
        } catch (Exception $e) {
            DAL::rollback();

            $msg  = _('Failed to update user folder.')."\n";
            $msg .= $e->getMessage();
            throw new ChannelException($msg);
        }

    }//end editUsersFolder()


    /**
     * Remove specified users folder.
     *
     * @param string $folderid Asset id of a user folder.
     *
     * @return void
     */
    public static function removeUsersFolder($folderid)
    {
        Channels::includeSystem('Linking');
        Linking::trashAsset($folderid);

    }//end removeUsersFolder()


    /**
     * Returns the list of user folders and their information.
     *
     * @param boolean $idsOnly If TRUE then the array will have assetids only.
     *
     * @return array
     */
    public static function getUsersFolders($idsOnly=FALSE)
    {
        $usersFolder = array();
        $cacheFile   = BaseSystem::getDataDir('User').'/folders.inc';

        if (file_exists($cacheFile) === TRUE) {
            include $cacheFile;
        }

        if ($idsOnly === TRUE) {
            $usersFolder = array_keys($usersFolder);
        }

        return $usersFolder;

    }//end getUsersFolders()


    /**
     * Register a new asset type editing template .
     *
     * @param string $assetType      Type of a user asset.
     * @param string $templateSystem System the template belongs to.
     * @param array  $templateName   Name of the template file.
     *
     * @return void
     */
    public static function addEditTemplate($assetType, $templateSystem, $templateName)
    {
        $editTemplates = array();
        $cacheFile     = BaseSystem::getDataDir('User').'/editTemplates.inc';
        if (file_exists($cacheFile) === TRUE) {
            include $cacheFile;
        }

        $editTemplates[$assetType] = array(
                                      'system'   => $templateSystem,
                                      'template' => $templateName,
                                     );

        include_once 'Libs/FileSystem/FileSystem.inc';
        $content = '<?php $editTemplates = '.var_export($editTemplates, TRUE).' ?>';
        FileSystem::filePutContents($cacheFile, $content);

    }//end addEditTemplate()


    public static function getEditTemplate($assetType)
    {
        $cacheFile = BaseSystem::getDataDir('User').'/editTemplates.inc';
        if (file_exists($cacheFile) === FALSE) {
            return FALSE;
        }

        include $cacheFile;

        $contents = FALSE;
        if (isset($editTemplates[$assetType]) === TRUE) {
            Channels::includeSystem('GUI');
            $info = $editTemplates[$assetType];
            ob_start();
            GUI::printTemplate($info['system'], $info['template']);
            $contents = ob_get_contents();
            ob_end_clean();
        }

        return $contents;

    }//end getEditTemplate()


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
     * Returns the child assets for specified parent asset.
     *
     * @param string $parentid The parent assetid.
     *
     * @return array
     *
     * @api            read
     * @api-permission public
     */
    public static function getChildren($parentid=NULL)
    {
        include_once 'Libs/Security/Security.inc';
        $jsSettings = Security::getRequestData('templateOptions', NULL, TRUE);
        if ($jsSettings !== NULL) {
            $parentid = Util::getArrayIndex($jsSettings, 'parentid', $parentid);
        }

        Channels::includeSystem('Permission');
        Channels::includeSystem('Linking');

        $userFolders = array();
        $children    = array();
        if ($parentid === NULL && $jsSettings === NULL) {
            // List the User folders.
            $userFolders = User::getUsersFolders();
            foreach ($userFolders as $folderid => $info) {
                if (Permission::hasPermission($folderid, 'asset.read.content') === FALSE) {
                    continue;
                }

                $children[] = $folderid;
            }
        } else {
            // Check read permission here incase the function is called by another system
            // (e.g. by a dataProvider).
            if (Permission::hasPermission($parentid, 'asset.read.content') === FALSE) {
                return array();
            }

            Channels::includeSystem('Linking');
            $children = DAL::getAssoc(Linking::getImmediateChildren($parentid, FALSE), 0);
        }

        $data = array();

        Channels::includeSystem('Attribute');

        Channels::includeSystem('Asset');
        $types = Asset::getTypeOfAsset($children);

        foreach ($children as $child) {
            $iconPath = '/__web/Systems/User/Web/icon_'.$types[$child].'_s.png';

            // If this child is a user folder then get info from userFolders array.
            $class = 'UserManagerScreen';
            if (isset($userFolders[$child]) === TRUE) {
                $title     = explode(' ', $userFolders[$child]['title']);
                $mainTitle = array_shift($title);
                $subTitle  = implode(' ', $title);

                $itemContent  = '<div class="'.$class.'-userManager-item">';
                $itemContent .= '<div class="'.$class.'-userManager-item-title">'.$mainTitle.'</div>';
                $itemContent .= '<div class="'.$class.'-userManager-item-subTitle">'.$subTitle.'</div>';
                $itemContent .= '</div>';
                $item = array(
                         'id'          => $child,
                         'title'       => $userFolders[$child]['title'],
                         'itemContent' => $itemContent,
                         'isButton'    => TRUE,
                        );
            } else {
                $name = Attribute::getAttributeValue($child, 'name');
                $item = array(
                         'id'          => $child,
                         'title'       => $name,
                         'itemContent' => '<img class="'.$class.'-userManager-item-icon" src="'.$iconPath.'" />'.$name,
                        );
            }

            // Get number of kids.
            $childKids          = DAL::getAssoc(Linking::getImmediateChildren($child, FALSE), 0);
            $item['childCount'] = count($childKids);

            $data[] = $item;
        }

        return $data;

    }//end getChildren()


    /**
     * Loads the asset type edit template for the specified assetid.
     *
     * This method is used by the UserManager template.
     *
     * @param string $assetid ID of the asset thats being edited.
     *
     * @return void
     *
     * @api            read
     * @api-permission superuser
     */
    public static function loadUserManagerEditContents($assetid)
    {
        Channels::includeSystem('Asset');
        $assetType        = Asset::getTypeOfAsset($assetid);
        $templateContents = User::getEditTemplate($assetType);

        if ($templateContents === FALSE) {
            $templateContents = 'Failed to load template';
        }

        return $templateContents;

    }//end loadUserManagerEditContents()


    /**
     * Returns the contents for a new super users table row.
     *
     * @param string $userid Column data for the new row.
     *
     * @return array
     *
     * @api            read
     * @api-permission superuser
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
     * @throws ChannelException If the active user is not a super user.
     */
    public static function saveSuperUsersScreen(array $data)
    {
        Channels::includeSystem('User');
        if (User::isSystemAdministrator() === FALSE) {
            throw new ChannelException(_('You must be a super user to access this screen'));
        }

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


    /**
     * Returns user group information for the requested user group.
     *
     * Values are used in the editUsetGroup template.
     *
     * @return array
     */
    public static function getUserGroupInfoForTemplate()
    {
        $info = array();

        include_once 'Libs/Security/Security.inc';
        $assetid = Security::getRequestData('assetid', NULL, FALSE, 'post');

        if ($assetid === NULL) {
            return $info;
        }

        // Get group name.
        Channels::includeSystem('Attribute');
        Channels::includeSystem('Asset');
        Channels::includeSystem('Linking');

        $name     = Attribute::getAttributeValue($assetid, 'name');
        $children = DAL::getAssoc(Linking::getImmediateChildren($assetid, FALSE), 0);
        $types    = Asset::getTypeOfAsset($children);

        $info['name']            = $name;
        $info['childGroupCount'] = 0;
        $info['childUserCount']  = 0;

        foreach ($types as $type) {
            if ($type === 'userGroup') {
                $info['childGroupCount']++;
            } else if ($type === 'user') {
                $info['childUserCount']++;
            }
        }

        $info['parentGroups'] = array();

        return $info;

    }//end getUserGroupInfoForTemplate()


    /**
     * Returns user group information for the requested user group.
     *
     * Values are used in the editUsetGroup template.
     *
     * @return array
     */
    public static function getUserInfoForTemplate()
    {
        $info = array();

        include_once 'Libs/Security/Security.inc';
        $assetid = Security::getRequestData('assetid', NULL, FALSE, 'post');

        if ($assetid === NULL) {
            return $info;
        }

        $info = User::getUserById($assetid);

        // TODO: How do we know if user is active? Publishing system needed?
        $info['status'] = TRUE;

        return $info;

    }//end getUserInfoForTemplate()


}//end class

?>
