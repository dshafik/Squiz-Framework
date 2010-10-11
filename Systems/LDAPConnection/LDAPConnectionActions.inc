<?php
/**
 * Actions for the LDAPConnection System.
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
 * @subpackage LDAPConnection
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * LDAPConnectionActions.
 *
 * Actions for the LDAPConnection system.
 */
class LDAPConnectionActions
{

    /**
     * Array of LDAP connections.
     *
     * @var array
     */
    private static $_connections = array();

    /**
     * Object class values (lower case) that represent groups in LDAP.
     *
     * @var array
     */
    private static $_groupTypes = array(
                                   'organizationalunit',
                                   'posixgroup',
                                   'group',
                                   'groupofnames',
                                   'groupofuniquenames',
                                   'domain',
                                   'container',
                                   'organization',
                                   'country',
                                  );

    /**
     * Object class values (lower case) that represent users in LDAP.
     *
     * @var array
     */
    private static $_userTypes = array(
                                  'organizationalperson',
                                  'inetorgperson',
                                 );

    /**
     * Filter string containing all valid user/group class.
     *
     * @var string
     */
    private static $_filter = '';

    /**
     * Attribute map cache.
     *
     * @var array
     */
    private static $_attributeMap = array();

    /**
     * Cache for LDAP search results.
     *
     * @var array
     */
    private static $_ldapCache = array();


    /**
     * LDAPConnection System Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Creates a new LDAP connection.
     *
     * @param string $name             Name of the connection asset.
     * @param string $parentAssetid    The ID of the parent asset fot this connection.
     * @param array  $details          The connection details (e.g. host, port, etc.).
     * @param array  $userAttributes   List of user attributes.
     * @param array  $groupAttributes  List of group attributes.
     * @param array  $uniqueIdSettings Unique ID attribute for users and groups.
     *
     * @return string
     * @throws ChannelException If the given details are invalid.
     *
     * @api            write
     * @api-permission superuser
     */
    public static function createConnection(
        $name,
        $parentAssetid,
        array $details,
        array $userAttributes,
        array $groupAttributes,
        array $uniqueIdSettings=array()
    ) {
        // First make sure all required fields are filled.
        $name = trim($name);
        if (empty($name) === TRUE) {
            throw new ChannelException(_('Connection name cannot be empty'));
        }

        $requiredFields = array(
                           'host',
                           'port',
                           'baseDN',
                           'bindDN',
                          );
        self::_checkFields($details, $requiredFields);

        $requiredFields = array(
                           'uid',
                           'cn',
                           'givenname',
                           'sn',
                           'mail',
                          );
        self::_checkFields($userAttributes, $requiredFields);

        $requiredFields = array('ou');
        self::_checkFields($groupAttributes, $requiredFields);

        // Create the asset.
        try {
            Channels::includeSystem('Asset');
            Channels::includeSystem('Attribute');
            Channels::includeSystem('Linking');
            Channels::includeSystem('Configuration');

            DAL::beginTransaction();
            $assetid = Asset::create('LDAPConnection');
            Attribute::setAttributeValue($assetid, 'name', $name);

            // Set config options.
            Configuration::setAttributeValue($assetid, 'details', $details);
            Configuration::setAttributeValue($assetid, 'userAttributes', $userAttributes);
            Configuration::setAttributeValue($assetid, 'groupAttributes', $groupAttributes);
            Configuration::setAttributeValue($assetid, 'uniqueIdSettings', $uniqueIdSettings);

            // Create link between parent and new asset.
            Linking::createLink($parentAssetid, $assetid);

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }//end try

        return $assetid;

    }//end createConnection()


    /**
     * Creates a new LDAP connection.
     *
     * @param string $connectionid     LDAP Connection asset id.
     * @param string $name             Name of the connection asset.
     * @param array  $details          The connection details (e.g. host, port, etc.).
     * @param array  $userAttributes   List of user attributes.
     * @param array  $groupAttributes  List of group attributes.
     * @param array  $uniqueIdSettings Unique ID attribute for users and groups.
     *
     * @return void
     * @throws ChannelException IF the given details are invalid.
     *
     * @api            write
     * @api-permission superuser
     */
    public static function updateConnection(
        $connectionid,
        $name,
        array $details,
        array $userAttributes,
        array $groupAttributes,
        array $uniqueIdSettings=array()
    ) {
        // First make sure all required fields are filled.
        $name = trim($name);
        if (empty($name) === TRUE) {
            throw new ChannelException(_('Connection name cannot be empty'));
        }

        $requiredFields = array(
                           'host',
                           'port',
                           'baseDN',
                           'bindDN',
                          );
        self::_checkFields($details, $requiredFields);

        $requiredFields = array(
                           'uid',
                           'cn',
                           'givenname',
                           'sn',
                           'mail',
                          );
        self::_checkFields($userAttributes, $requiredFields);

        $requiredFields = array('ou');
        self::_checkFields($groupAttributes, $requiredFields);

        try {
            Channels::includeSystem('Attribute');
            Channels::includeSystem('Configuration');

            DAL::beginTransaction();
            Attribute::setAttributeValue($connectionid, 'name', $name);

            // If the password is empty then get the current one from DB.
            if (isset($details['password']) === FALSE
                || empty($details['password']) === TRUE
            ) {
                $currDetails         = Configuration::getAttributeValue(
                    $connectionid,
                    'details'
                );
                $details['password'] = $currDetails['password'];
            }

            // Set config options.
            Configuration::setAttributeValue($connectionid, 'details', $details);
            Configuration::setAttributeValue($connectionid, 'userAttributes', $userAttributes);
            Configuration::setAttributeValue($connectionid, 'groupAttributes', $groupAttributes);
            Configuration::setAttributeValue($connectionid, 'uniqueIdSettings', $uniqueIdSettings);

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }//end try

    }//end updateConnection()


    /**
     * Throws Exception if any of the required fields is empty.
     *
     * @param array $fields         Array of field names and their values.
     * @param array $requiredFields Array of required field names.
     *
     * @return void
     * @throws ChannelException If any of the required fields is empty.
     */
    private static function _checkFields(array $fields, array $requiredFields)
    {
        foreach ($requiredFields as $fieldName) {
            if (isset($fields[$fieldName]) === TRUE) {
                $value = trim($fields[$fieldName]);
                if (empty($value) === FALSE) {
                    continue;
                }
            }

            throw new ChannelException(sprintf(_('%s field cannot be empty'), $fieldName));
        }

    }//end _checkFields()


    /**
     * Returns information for specified connection asset.
     *
     * If specified asset is not an LDAPConnection asset then returns NULL.
     *
     * @param string $connectionid LDAP Connection asset id.
     *
     * @return mixed
     */
    public static function getConnectionInfo($connectionid=NULL)
    {
        $info = array();

        if ($connectionid === NULL) {
            include_once 'Libs/Security/Security.inc';
            $connectionid = Security::getRequestData('assetid', NULL, FALSE, 'post');
        }

        if ($connectionid === NULL) {
            return NULL;
        }

        Channels::includeSystem('Asset');
        if (Asset::getTypeOfAsset($connectionid) !== 'LDAPConnection') {
            return NULL;
        }

        Channels::includeSystem('Attribute');
        Channels::includeSystem('Configuration');
        $info['name']             = Attribute::getAttributeValue($connectionid, 'name');
        $info['details']          = Configuration::getAttributeValue($connectionid, 'details');
        $info['userAttributes']   = Configuration::getAttributeValue($connectionid, 'userAttributes');
        $info['groupAttributes']  = Configuration::getAttributeValue($connectionid, 'groupAttributes');
        $info['uniqueIdSettings'] = Configuration::getAttributeValue($connectionid, 'uniqueIdSettings');

        return $info;

    }//end getConnectionInfo()


    /**
     * Get the LDAP connection object.
     *
     * @param integer $connectionid The connection ID.
     *
     * @return object
     */
    public static function getConnection($connectionid)
    {
        if (isset(self::$_connections[$connectionid]) === FALSE) {
            $connInfo = LDAPConnection::getConnectionInfo($connectionid);
            if ($connInfo === NULL) {
                return NULL;
            }

            include_once 'Libs/LDAP/LDAP.inc';
            $conn = LDAP::connectToLDAP(
                $connInfo['details']['host'],
                $connInfo['details']['port'],
                $connInfo['details']['bindDN'],
                $connInfo['details']['password']
            );

            self::$_connections[$connectionid] = $conn;
        }

        return self::$_connections[$connectionid];

    }//end getConnection()


    /**
     * Returns the asset type for an LDAP id.
     *
     * Returns NULL if asset type cannot be determined.
     *
     * @param string $connectionid The LDAPConnection asset id.
     * @param string $remoteid     Id of LDAP resource.
     *
     * @return string
     */
    public static function getRemoteAssetType($connectionid, $remoteid)
    {
        Channels::includeSystem('Connection');
        $shadowid = Connection::getShadowid($connectionid, $remoteid);
        $data     = self::_getEntry($shadowid);
        if ($data === NULL) {
            return NULL;
        }

        return $data['assetType'];

    }//end getRemoteAssetType()


    /**
     * Read LDAP entry based on DN (all attributes), NULL on error.
     *
     * @param string $shadowid A shadow id.
     *
     * @return mixed
     */
    private static function _getEntry($shadowid)
    {
        Channels::includeSystem('Connection');
        $parts = Connection::splitShadowid($shadowid);
        if ($parts === FALSE) {
            return NULL;
        }

        $pattern = self::_getCustomFormatRegex();
        $matches = array();
        preg_match($pattern, $shadowid, $matches);

        if (empty($matches) === TRUE) {
            return NULL;
        }

        $ldapKey   = $matches[1];
        $ldapValue = $matches[2];

        $data = self::_getFromCache($shadowid);
        if ($data !== NULL) {
            return $data;
        }

        // Not loaded yet, load it now.
        $attrs = array('*');
        if ($ldapKey === 'dn') {
            // Search based on DN.
            $filter      = self::_getFilter();
            $dn          = $ldapValue;
            $multiLevel  = TRUE;
            $multiResult = TRUE;
            $limit       = 1;
        } else {
            // Search based on the customised key.
            $dn     = self::_getConfigValue($parts['connection'], 'details', 'baseDN');
            $filter = '('.$ldapKey.'='.$ldapValue.')';

            $multiLevel  = TRUE;
            $multiResult = TRUE;
            $limit       = 0;
            $attrs       = array('*');
        }

        include_once 'Libs/LDAP/LDAP.inc';
        $conn = LDAPConnection::getConnection($parts['connection']);
        $sr   = LDAP::search(
            $conn,
            $dn,
            $filter,
            $multiLevel,
            $multiResult,
            $attrs,
            $limit
        );

        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);
        if (empty($data) === TRUE) {
            return NULL;
        }

        $data = $data[0];

        $data['assetType'] = self::_getAssetTypeFromObjectClass($data['objectclass']);

        self::_addToCache($shadowid, $data);

        return $data;

    }//end _getEntry()


    /**
     * Blocker for the Linking::getImmediateChildren().
     *
     * @param string       $parentid      The ID of the parent asset.
     * @param boolean      $withSortOrder If TRUE, the result query returns
     *                                    sort_order column together.
     * @param array|object $types         Optional list of asset types to get.
     *
     * @return mixed
     * @see    Linking::getImmediateChildren
     */
    public static function getImmediateChildren(
        $parentid,
        $withSortOrder=FALSE,
        $types=array()
    ) {
        $conn         = NULL;
        $remote       = FALSE;
        $connectionid = NULL;

        Channels::includeSystem('Asset');
        if (Asset::isAssetid($parentid) === TRUE) {
            // Is a valid assetid check if its an LDAPConnection asset, if it is
            // get the LDAP connection.
            $conn = LDAPConnection::getConnection($parentid);
            if ($conn === NULL) {
                // Do not block not an LDAP asset.
                return FALSE;
            }

            $connectionid = $parentid;
        } else {
            // Could be a connection asset.
            Channels::includeSystem('Connection');
            $parts = Connection::splitShadowid($parentid);
            if ($parts === FALSE) {
                // Do not block not a connection asset.
                return FALSE;
            }

            $remote       = TRUE;
            $conn         = LDAPConnection::getConnection($parts['connection']);
            $connectionid = $parts['connection'];
        }//end if

        if ($conn === NULL) {
            return FALSE;
        }

        $connInfo = LDAPConnection::getConnectionInfo($connectionid);

        $groupMembers = array();
        $startdn      = '';
        if ($remote === FALSE) {
            // This is the LDAPConnection asset.
            $startdn = $connInfo['details']['baseDN'];
        } else {
            // LDAP user or group.
            $startdn = self::_getDNFromRemoteid($parentid);

            // Group Member has been specified. Combine the result
            // if there are any.
            $gm = strtolower(self::_getConfigValue($parts['connection'], 'groupAttributes', 'groupMembers'));
            if ($gm !== '') {
                $asset = self::_getEntry($parentid);
                if (isset($asset[$gm]) === TRUE && empty($asset[$gm]) === FALSE) {
                    // TODO: Do a single search instead of multiple.
                    if (is_array($asset[$gm]) === TRUE) {
                        foreach ($asset[$gm] as $dn) {
                            $entry = self::_getEntry($parts['connection'].'-_dn---'.$dn.'_');
                            if ($entry !== NULL) {
                                $groupMembers[] = $parts['connection'].'-'.self::_getRemoteId($parts['connection'], $entry);
                            }
                        }
                    } else {
                        $entry = self::_getEntry($parts['connection'].'-_dn---'.$asset[$gm].'_');
                        if ($entry !== NULL) {
                            $groupMembers[] = $parts['connection'].'-'.self::_getRemoteId($parts['connection'], $entry);
                        }
                    }
                }
            }
        }//end if

        $filter     = self::_getFilter();
        $multiLevel = FALSE;
        $attributes = array('*');
        include_once 'Libs/LDAP/LDAP.inc';
        $sr = LDAP::search(
            $conn,
            $startdn,
            $filter,
            $multiLevel,
            TRUE,
            $attributes
        );

        $entries  = LDAP::getEntries($conn, $sr);
        $data     = LDAP::getData($entries);
        $children = array();

        foreach ($data as &$d) {
            $d['assetType'] = self::_getAssetTypeFromObjectClass($d['objectclass']);
            $id             = self::_getRemoteId($connectionid, $d);
            $rid            = $connectionid.'-'.$id;

            self::_addToCache($parentid, $d);

            if ($startdn === $d['dn']) {
                // Exclude itself.
                continue;
            }

            $children[] = $rid;
        }

        if (empty($groupMembers) === FALSE) {
            $children = array_merge($children, $groupMembers);
            $children = array_unique($children);
        }

        return $children;

    }//end getImmediateChildren()


    /**
     * Get attribute value for remote asset.
     *
     * @param string       $shadowid    Shadow ID.
     * @param string|array $attributeid The attribute(s) to find.
     *
     * @return array
     * @see    Attribute::getAttributeValue()
     */
    public static function getAttributeValue($shadowid, $attributeid)
    {
        Channels::includeSystem('Asset');
        $localAssetid = NULL;
        if (Asset::isAssetid($shadowid) === TRUE) {
            // This is a local asset but could be an LDAP asset that has a localid.
            $localAssetid = $shadowid;

            Channels::includeSystem('Connection');
            $shadowid = Connection::getShadowidFromLocalid($shadowid);
            if ($shadowid === NULL || $attributeid === 'last_login') {
                return FALSE;
            }
        }

        Channels::includeSystem('Connection');
        $parts = Connection::splitShadowid($shadowid);
        $asset = self::_getEntry($shadowid);

        $isArray     = is_array($attributeid);
        $attributeid = (array) $attributeid;
        $values      = array();

        foreach ($attributeid as $attrid) {
            $value = '';

            switch ($attrid) {
                case 'name':
                    $attr = '';
                    // Get the settings for name attribute.
                    if ($asset['assetType'] === 'user') {
                        $attr = self::_getConfigValue($parts['connection'], 'userAttributes', 'name');
                    } else if ($asset['assetType'] === 'userGroup') {
                        $attr = self::_getConfigValue($parts['connection'], 'groupAttributes', 'groupName');
                    }

                    // Try the name attribute setting first and 'cn', 'o', 'c', 'ou' and 'dn'
                    // if it's not found.
                    if (isset($asset[$attr]) === TRUE) {
                        $value = $asset[$attr];
                    } else if (isset($asset['cn']) === TRUE) {
                        $value = $asset['cn'];
                    } else if (isset($asset['o']) === TRUE) {
                        $value = $asset['o'];
                    } else if (isset($asset['c']) === TRUE) {
                        $value = $asset['c'];
                    } else if (isset($asset['ou']) === TRUE) {
                        if (is_array($asset['ou']) === FALSE) {
                            $value = $asset['ou'];
                        }
                    } else {
                        // Nothing suitable found, use DN.
                        $value = $asset['dn'];
                    }
                break;

                case 'dn':
                    $value = $asset['dn'];
                break;

                case 'first_name':
                    $attr  = self::_getConfigValue($parts['connection'], 'userAttributes', 'givenname');
                    $value = Util::getArrayIndex($asset, $attr, '');
                break;

                case 'last_name':
                    $attr  = self::_getConfigValue($parts['connection'], 'userAttributes', 'sn');
                    $value = Util::getArrayIndex($asset, $attr, '');
                break;

                case 'username':
                    $attr  = self::_getConfigValue($parts['connection'], 'userAttributes', 'uid');
                    $value = Util::getArrayIndex($asset, $attr, '');
                break;

                case 'email':
                    $attr  = self::_getConfigValue($parts['connection'], 'userAttributes', 'mail');
                    $value = Util::getArrayIndex($asset, $attr, '');
                break;

                case 'last_login':
                    // Last login only exists for LDAP users who has a local assetid.
                    if ($localAssetid !== NULL) {
                        Channels::includeSystem('Attribute');
                        $value = Attribute::getAttributeValue($localAssetid, 'last_login');
                    }
                break;

                default:
                    $value = '';
                break;
            }//end switch

            $values[$attrid] = $value;
        }//end foreach

        if ($isArray === FALSE && isset($values[$attributeid[0]]) === TRUE) {
            return $values[$attributeid[0]];
        }

        return $values;

    }//end getAttributeValue()


    /**
     * Blocker for the Asset::getTypeOfAsset method.
     *
     * @param mixed $shadowid Shadow ID of an LDAP asset.
     *
     * @return string
     * @see    Asset::getTypeOfAsset
     */
    public static function getTypeOfAsset($shadowid)
    {
        $assetTypes = 'asset';
        Channels::includeSystem('Asset');
        if (is_array($shadowid) === FALSE) {
            if (Asset::isAssetid($shadowid) === TRUE) {
                return FALSE;
            }

            $data = self::_getEntry($shadowid);
            if ($data !== NULL && isset($data['assetType']) === TRUE) {
                $assetTypes = $data['assetType'];
            }
        } else {
            // First get Asset system to return local asset types.
            $localAssetids = array();
            foreach ($shadowid as $index => $id) {
                if (Asset::isAssetid($id) === TRUE) {
                    $localAssetids[] = $id;
                    unset($shadowid[$index]);
                }
            }

            // If no shadow ids then do not block.
            if (empty($shadowid) === TRUE) {
                return FALSE;
            }

            $localTypes = array();
            if (empty($localAssetids) === FALSE) {
                $localTypes = Asset::getTypeOfAsset($localAssetids);
            }

            // Get LDAP types.
            $ldapTypes = array();
            foreach ($shadowid as $id) {
                $data           = self::_getEntry($id);
                $ldapTypes[$id] = $data['assetType'];
            }

            $assetTypes = array_merge($ldapTypes, $localTypes);
        }//end if

        return $assetTypes;

    }//end getTypeOfAsset()


    /**
     * Returns user groups for an LDAP user.
     *
     * @param string $shadowid LDAP user id.
     * @param mixed  $retVal   Return value of User::getUserGroups().
     *
     * @return array
     */
    public static function getUserGroups($shadowid, $retVal)
    {
        Channels::includeSystem('Asset');
        if (Asset::isAssetid($shadowid) === TRUE) {
            return $retVal;
        }

        $data = self::_getEntry($shadowid);

        return $retVal;

    }//end getUserGroups()


    /**
     * Disabled the password hashing if there are LDAP connections.
     *
     * If this method returns FALSE then password hashing will be disabled since
     * this is a refiner plug to User::isPasswordHashingEnabled().
     *
     * @param boolean $retVal Return value of User::isPasswordHashingEnabled().
     *
     * @return boolean
     */
    public static function disablePasswordHashing($retVal)
    {
        if ($retVal === FALSE) {
            return FALSE;
        }

        Channels::includeSystem('Asset');
        $assets = Asset::getAssetsByType('LDAPConnection');
        if (empty($assets) === TRUE) {
            return $retVal;
        }

        return FALSE;

    }//end disablePasswordHashing()


    /**
     * Refiner plug to User::loginUser().
     *
     * @param string  $username Username of the user to log in.
     * @param string  $password Password of the user to log in.
     * @param boolean $retVal   Return value of User::loginUser().
     *
     * @return boolean
     */
    public static function loginUser($username, $password, $retVal)
    {
        if ($retVal === TRUE) {
            return $retVal;
        }

        // User::loginUser() method failed to login user so check LDAP connections.
        Channels::includeSystem('Asset');
        $assets = Asset::getAssetsByType('LDAPConnection');

        foreach ($assets as $connectionid) {
            $localUserid = LDAPConnection::authenticate(
                $connectionid,
                $username,
                $password
            );

            if ($localUserid !== FALSE) {
                Channels::includeSystem('Session');
                Session::setSessionData(
                    'User',
                    'CurrentUserid',
                    $localUserid
                );

                return TRUE;
            }
        }

        return FALSE;

    }//end loginUser()


    /**
     * Returns TRUE if the specified user can login.
     *
     * @param float  $connectionid Connection assetid to use for login.
     * @param string $username     Username for login.
     * @param string $password     Password for login.
     *
     * @return boolean
     */
    public static function authenticate($connectionid, $username, $password)
    {
        include_once 'Libs/LDAP/LDAP.inc';
        $attrs = LDAPConnection::getConnectionInfo($connectionid);
        $field = Util::getArrayIndex($attrs['userAttributes'], 'uid');

        // User defined authentication DN.
        $authDN = Util::getArrayIndex($attrs['details'], 'authDN');
        if ($authDN !== '') {
            $basedn = $authDN;
        } else {
            $basedn = Util::getArrayIndex($attrs['details'], 'baseDN');
        }

        // User defined authentication filter.
        $filter     = '('.$field.'='.$username.')';
        $authFilter = self::_getConfigValue($connectionid, 'details', 'authFilter');
        $authFilter = ltrim($authFilter, '(');
        $authFilter = rtrim($authFilter, '(');
        if ($authFilter !== '') {
            $filter = '(&'.$filter.'('.$authFilter.'))';
        }

        $conn    = LDAPConnection::getConnection($connectionid);
        $sr      = LDAP::search($conn, $basedn, $filter);
        $entries = LDAP::getEntries($conn, $sr);
        $data    = LDAP::getData($entries);

        if ($data === FALSE || empty($data) === TRUE) {
            // User not found with authentication filter.
            return FALSE;
        }

        $user              = array_shift($data);
        $user['assetType'] = self::_getAssetTypeFromObjectClass($user['objectclass']);
        if ($user['assetType'] !== 'LDAPUser') {
            // Entry found but it's not a user.
            return FALSE;
        }

        // Try to authenticate through LDAP bridge.
        $conn = LDAP::connectToLDAP(
            Util::getArrayIndex($attrs['details'], 'host'),
            Util::getArrayIndex($attrs['details'], 'port'),
            Util::getArrayIndex($attrs['details'], 'bindDN'),
            Util::getArrayIndex($attrs['details'], 'password')
        );

        $status = LDAP::connectAsUser($conn, $user['dn'], $password);
        if ($status === TRUE) {
            Channels::includeSystem('Connection');
            $remoteid    = self::_getRemoteId($connectionid, $user);
            $localUserid = Connection::createAsset($connectionid, $remoteid);
            return $localUserid;
        }

        return FALSE;

    }//end authenticate()


    /**
     * Add an LDAP result to temp cache.
     *
     * @param string $shadowid Shadow id.
     * @param array  $data     Data to store.
     *
     * @return void
     */
    private static function _addToCache($shadowid, array $data)
    {
        self::$_ldapCache[$shadowid] = $data;

    }//end _addToCache()


    /**
     * Returns cached LDAP data or NULL if not cached.
     *
     * @param string $shadowid Shadow id.
     *
     * @return mixed
     */
    private static function _getFromCache($shadowid)
    {
        if (isset(self::$_ldapCache[$shadowid]) === TRUE) {
            return self::$_ldapCache[$shadowid];
        }

        return NULL;

    }//end _getFromCache()


    /**
     * Get default filter string containing all valid user/group class.
     *
     * @param string $assetType Optionally 'user' or 'userGroup'.
     *
     * @return string
     */
    private static function _getFilter($assetType='')
    {
        if (empty(self::$_filter) === FALSE) {
            return self::$_filter;
        }

        $filter = '';

        if ($assetType === '' || $assetType === 'userGroup') {
            $filter = self::_getLDAPUserGroupFilter($filter);
        }

        if ($assetType === '' || $assetType === 'user') {
            $filter = self::_getLDAPUserFilter($filter);
        }

        self::$_filter = $filter;

        return $filter;

    }//end _getFilter()


    /**
     * Returns LDAP User filter string.
     *
     * If the existing filter is specified, then it adds user filter
     * as OR condition.
     *
     * @param string $filter Optional filter to be ORed.
     *
     * @return string
     */
    private static function _getLDAPUserFilter($filter='')
    {
        foreach (self::$_userTypes as $value) {
            if (empty($filter) === TRUE) {
                $filter = '(objectClass='.$value.')';
            } else {
                $filter = '(|'.$filter.'(objectClass='.$value.'))';
            }
        }

        return $filter;

    }//end _getLDAPUserFilter()


    /**
     * Returns LDAP User Group filter string.
     *
     * If the existing filter is specified, then it adds user group filter
     * as OR condition.
     *
     * @param string $filter Optional filter to be ORed.
     *
     * @return string
     */
    private static function _getLDAPUserGroupFilter($filter='')
    {
        foreach (self::$_groupTypes as $value) {
            if (empty($filter) === TRUE) {
                $filter = '(objectClass='.$value.')';
            } else {
                $filter = '(|'.$filter.'(objectClass='.$value.'))';
            }
        }

        return $filter;

    }//end _getLDAPUserGroupFilter()


    /**
     * Returns the remoteid for the LDAP entry passed in.
     *
     * @param float $connectionid Assetid of connection asset.
     * @param array $asset        Result from getEntry().
     *
     * @return array
     */
    private static function _getRemoteId($connectionid, array $asset)
    {
        $ldapAttr = '';
        if ($asset['assetType'] === 'user') {
            $ldapAttr = self::_getConfigValue($connectionid, 'uniqueIdSettings', 'uniqueIdUsers');
        } else if ($asset['assetType'] === 'userGroup') {
            $ldapAttr = self::_getConfigValue($connectionid, 'uniqueIdSettings', 'uniqueIdGroups');
        }

        if (isset($asset[strtolower($ldapAttr)]) === FALSE) {
            $ldapAttr = 'dn';
        }

        $remoteid = '_'.$ldapAttr.'---'.$asset[strtolower($ldapAttr)].'_';
        return $remoteid;

    }//end _getRemoteId()


    /**
     * It returns the DN of remoteid.
     *
     * This function finds DN for remoteid respecting custom unique ID settings.
     * Returns FALSE when it fails to get DN.
     *
     * @param string $shadowid Shadow id.
     *
     * @return mixed
     */
    private static function _getDNFromRemoteid($shadowid)
    {
        Channels::includeSystem('Connection');
        $parts = Connection::splitShadowid($shadowid);
        if ($parts === FALSE) {
            return FALSE;
        }

        $pattern = self::_getCustomFormatRegex();
        $matches = array();
        preg_match_all($pattern, $parts['remoteid'], $matches);

        if (empty($matches) === TRUE) {
            return FALSE;
        }

        if ($matches[1] === 'dn') {
            return $matches[2];
        }

        // Need to search for DN.
        $entry = self::_getEntry($shadowid);
        $dn    = $entry['dn'];

        return $dn;

    }//end _getDNFromRemoteid()


    /**
     * Returns Regex string for custom unique ID format.
     *
     * @return string
     */
    private static function _getCustomFormatRegex()
    {
        $pattern = '/\_([^:]+)---([^\]]+)\_/';
        return $pattern;

    }//end _getCustomFormatRegex()


    /**
     * Gets LDAP Attribute map for local attribute.
     *
     * @param string $connectionid ID of the connection asset.
     * @param string $configType   The type of the configuration (e.g. details
     *                             userAttributes, groupAttributes, uniqueIdSettings).
     * @param string $attributeid  Local attribute to get LDAP attribute name.
     *
     * @return string
     */
    private static function _getConfigValue($connectionid, $configType, $attributeid)
    {
        if (isset(self::$_attributeMap[$connectionid]) === FALSE) {
            self::$_attributeMap[$connectionid] = array();
        }

        if (isset(self::$_attributeMap[$connectionid][$configType]) === FALSE) {
            Channels::includeSystem('Configuration');
            self::$_attributeMap[$connectionid][$configType] = Configuration::getAttributeValue($connectionid, $configType);
        }

        if (isset(self::$_attributeMap[$connectionid][$configType][$attributeid]) === FALSE) {
            // The attribute is not mapped to anything.
            return FALSE;
        }

        return self::$_attributeMap[$connectionid][$configType][$attributeid];

    }//end _getConfigValue()


    /**
     * Returns the asset type (user or userGroup) for the passed objectlcass.
     *
     * @param mixed $objectClass The objectclass attribute from LDAP entry.
     *
     * @return string
     */
    private static function _getAssetTypeFromObjectClass($objectClass)
    {
        // By default it returns 'asset'.
        $type = 'asset';
        if (is_array($objectClass) === TRUE) {
            // Multiple classes.
            $classes = array();
            foreach ($objectClass as $c) {
                $classes[] = strtolower($c);
            }

            $gMatch = array_intersect(self::$_groupTypes, $classes);
            if (empty($gMatch) === FALSE) {
                $type = 'LDAPUserGroup';
            }

            $uMatch = array_intersect(self::$_userTypes, $classes);
            if (empty($uMatch) === FALSE) {
                $type = 'LDAPUser';
            }
        } else {
            // Single class.
            $objectClass = strtolower($objectClass);
            if (in_array($objectClass, self::$_groupTypes) === TRUE) {
                $type = 'LDAPUserGroup';
            } else if (in_array($objectClass, self::$_userTypes) === TRUE) {
                $type = 'LDAPUser';
            }
        }//end if

        return $type;

    }//end _getAssetTypeFromObjectClass()


    /**
     * Returns information about an asset for a template.
     *
     * @return array
     */
    public static function getAssetInfoForTemplate()
    {
        $info = array();

        include_once 'Libs/Security/Security.inc';
        $assetid = Security::getRequestData('assetid', NULL, FALSE, 'post');

        if ($assetid === NULL) {
            return $info;
        }

        Channels::includeSystem('Attribute');
        Channels::includeSystem('Asset');
        $assetType = Asset::getTypeOfAsset($assetid);

        switch ($assetType) {
            case 'LDAPUser':
                $info['first_name'] = Attribute::getAttributeValue($assetid, 'first_name');
                $info['last_name']  = Attribute::getAttributeValue($assetid, 'last_name');
                $info['email']      = Attribute::getAttributeValue($assetid, 'email');
                $info['username']   = Attribute::getAttributeValue($assetid, 'username');
            break;

            case 'LDAPUserGroup':
                $info['name'] = Attribute::getAttributeValue($assetid, 'name');
            break;

            default:
                // Unknown type.
            break;
        }

        return $info;

    }//end getAssetInfoForTemplate()


}//end class

?>
