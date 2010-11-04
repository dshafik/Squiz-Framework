<?php
/**
 * Installer for the Patching System.
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
 * @package    CMS
 * @subpackage Patching
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Patching System Class.
 */
class PatchingSystem extends BaseSystem
{


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Set some patching config pref.
     *
     * Revision     - SVN revision number, useful for getting/applying patches.
     * Last Updated - Timestamp of when patches were last applied.
     * System ID    - Unique ID generated when this sytem first communicate
     *                to the updating server.
     * Notify       - Boolean, whether to send patch notification email.
     * Recipient    - Array of userid to send patch notification email to.
     * Schedule     - Boolean, toggle button to enable/disable scheduling.
     *
     * @return void
     */
    public function install()
    {
        parent::install();
        if (Channels::systemExists('SystemConfig') === TRUE) {
            $configDefault = array(
                              'revision'       => 0,
                              'last_updated'   => 0,
                              'systemid'       => 'not-set',
                              'notify'         => FALSE,
                              'recipient'      => array(),
                              'schedule'       => FALSE,
                              'threshold'      => 2,
                              'last_checked'   => time(),
                              'check_interval' => 3600,
                             );
            Channels::includeSystem('SystemConfig');
            SystemConfig::setConfig('Patching', $configDefault);
        }//end if

    }//end install()


}//end class

?>
