<?php
/**
 * Cron script for Backup, keep daily backup for 1 week and replace.
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
 * BackupCron Class.
 *
 */
class BackupCron implements CronScript
{


    /**
     * Runs the backup cron script.
     *
     * @return void
     */
    public function run()
    {
        Channels::includeSystem('Backup');
        Backup::createBackup();

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
