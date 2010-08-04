<?php
/**
 * Cron script for Backup, keep daily backup for 1 week and replace.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Backup
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(dirname(__FILE__))).'/Cron/Scripts/CronScript.inc';

/**
 * BackupCron Class.
 *
 * @since 4.0.0
 */
class BackupCron implements CronScript
{


    /**
     * Runs the backup cron script.
     *
     * @since  4.0.0
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
     * @since  4.0.0
     * @return void
     */
    public function stop()
    {

    }//end stop()


}//end class

?>
