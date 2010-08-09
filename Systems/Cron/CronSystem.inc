<?php
/**
 * Cron System Class.
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
 * @subpackage Cron
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Cron System Class.
 *
 */
class CronSystem extends BaseSystem
{


    /**
     * Install this system.
     *
     * @return void
     */
    public function install()
    {
        parent::install();
        Channels::includeSystem('Cron');

        if (Channels::systemExists('SystemConfig') === TRUE) {
            Channels::includeSystem('SystemConfig');
            $defaultConfigs = array(
                               'frequent_running'      => FALSE,
                               'frequent_last_updated' => NULL,
                               'frequent_run_check'    => 0,
                               'hourly_running'        => FALSE,
                               'hourly_last_updated'   => NULL,
                               'hourly_run_check'      => 0,
                               'daily_running'         => FALSE,
                               'daily_last_updated'    => NULL,
                               'daily_run_check'       => 0,
                              );
            SystemConfig::setConfig('Cron', $defaultConfigs);
        }

        Cron::addCronType('frequent', 'Frequent', '*/5 *');
        Cron::addCronType('hourly', 'Hourly', '20 *');
        Cron::addCronType('daily', 'Daily', '30 0');

        // Install cron jobs.
        $systems = Channels::getSystemList(TRUE, FALSE, FALSE);
        foreach ($systems as $system => $path) {
            Cron::installCronScripts($system, $path);
        }

    }//end install()


}//end class

?>
