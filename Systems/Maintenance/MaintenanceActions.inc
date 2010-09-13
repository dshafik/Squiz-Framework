<?php
/**
 * Actions for the Maintenance System.
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
 * @subpackage Maintenance
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * MaintenanceActions.
 *
 * Actions for the Maintenance system. This system provides a place for system
 * maintenance actions to hook into. It does nothing itself.
 */
class MaintenanceActions
{


    /**
     * Constructor.
     *
     * All methods should be called statically, and therefore this class should
     * never need to be instantiated.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * This action provides the hooking point for other systems.
     *
     * @return void
     */
    public static function perform()
    {
        include_once 'Channels/Channels.inc';
        include_once 'Systems/BaseSystem.inc';

    }//end perform()


}//end class

?>
