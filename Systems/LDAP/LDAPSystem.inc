<?php
/**
 * LDAP System Class.
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

require_once 'Systems/BaseSystem.inc';

/**
 * LDAPSystem.
 *
 * Installs all local asset types for this system.*
 */
class LDAPSystem extends BaseSystem
{


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Register some metadata related log type.
     *
     * @return void
     */
    public function install()
    {
        parent::install();

        Channels::includeSystem('Asset');
        Channels::includeSystem('User');

        // Create a global connections folder.
        $ldapConnectionsFolder = Asset::create('folder');
        Attribute::setAttributeValue($ldapConnectionsFolder, 'name', 'LDAP Connections');

        $usersFolder = User::createUsersFolder(_('LDAP Connections'), 'LDAP', 'editFolder.tpl');
        User::addEditTemplate('LDAPConnection', 'LDAP', 'editLDAPConnection.tpl', 'UserUserManagerScreen_ldap');
        User::addEditTemplate('LDAPUser', 'LDAP', 'editLDAPUser.tpl', 'UserUserManagerScreen_ldap');
        User::addEditTemplate('LDAPUserGroup', 'LDAP', 'editLDAPUserGroup.tpl', 'UserUserManagerScreen_ldap');

    }//end install()


}//end class

?>
