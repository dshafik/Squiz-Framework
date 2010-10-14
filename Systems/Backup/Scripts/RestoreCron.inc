<?php
/**
 * Cron script to restore a system from backup.
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
 * @subpackage Backup
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(dirname(dirname(__FILE__))).'/Cron/Scripts/CronScript.inc';

/**
 * RestoreCron Class.
 */
class RestoreCron implements CronScript
{


    /**
     * Runs the restore cron script.
     *
     * @return void
     */
    public function run()
    {
        Channels::includeSystem('Backup');
        $name = Backup::getQueuedRestoreBackup();
        if ($name === '') {
            return;
        }

        // Delete the queue file so nobody else tries this again.
        $removed = Backup::removeQueuedRestoreBackup();
        if ($removed === FALSE) {
            return;
        }

        // See you on the flip side.
        Backup::restoreBackup($name);

    }//end run()


    /**
     * Stop the running cron job.
     *
     * @return void
     */
    public function stop()
    {

    }//end stop()


}//end class

?>
