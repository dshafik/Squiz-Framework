<?php
/**
 * Actions for the LDAP System.
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
 * @subpackage LDAP
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * LDAPActions.
 *
 * Actions for the LDAP system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 */
class LDAPActions
{


    /**
     * LDAP System Constructor.
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

            // Set config options.
            Configuration::setAttributeValue($connectionid, 'details', $details);
            Configuration::setAttributeValue($connectionid, 'userAttributes', $userAttributes);
            Configuration::setAttributeValue($connectionid, 'groupAttributes', $groupAttributes);
            Configuration::setAttributeValue($connectionid, 'uniqueIdSettings', $uniqueIdSettings);

            DAL::commit();
        } catch (ChannelException $e) {
            DAL::rollback();
            throw new ChannelException($e->getMessage());
        }

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
     * @param string $connectionid LDAP Connection asset id.
     *
     * @return array
     */
    public static function getConnectionInfo($connectionid=NULL)
    {
        $info = array();

        if ($connectionid === NULL) {
            include_once 'Libs/Security/Security.inc';
            $connectionid = Security::getRequestData('assetid', NULL, FALSE, 'post');
        }

        if ($connectionid === NULL) {
            return $info;
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


}//end class

?>
