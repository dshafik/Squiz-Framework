<?php
/**
 * SquizSuite System Class.
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
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * SquizSuite System Class.
 */
class SquizSuiteSystem extends BaseSystem
{


    /**
     * Install this system.
     *
     * @return void
     */
    public function install()
    {
        parent::install();

        $logFilePath  = BaseSystem::getDataDir('SquizSuite');
        $logFilePath .= '/message_log.txt';

        if (file_exists($logFilePath) === FALSE) {
            touch($logFilePath);
        }

        if (Channels::systemExists('SystemConfig') === TRUE) {
            $configDefault = array(
                              'check_asap'     => FALSE,
                              'last_checked'   => time(),
                              'check_interval' => 3600,
                             );
            Channels::includeSystem('SystemConfig');
            SystemConfig::setConfig('SquizSuite', $configDefault);
        }//end if

    }//end install()


}//end class

?>
