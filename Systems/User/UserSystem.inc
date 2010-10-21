<?php
/**
 * User System Class.
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

require_once 'Systems/BaseSystem.inc';

/**
 * UserSystem.
 *
 * Installs all local asset types for this system.*
 */
class UserSystem extends BaseSystem
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

        Channels::includeSystem('User');

        // Create Local Users folder and register edit templates.
        $usersFolder = User::createUsersFolder(_('Local Users & Groups'), 'User', 'editFolder.tpl');
        User::addEditTemplate('user', 'User', 'editUser.tpl');
        User::addEditTemplate('userGroup', 'User', 'editUserGroup.tpl');

        $admin = User::createUser('admin', 'Admin', 'User', 'admin@squiz.com.au', 'password', array($usersFolder));
        User::addSuperUser($admin);

    }//end install()


}//end class

?>
