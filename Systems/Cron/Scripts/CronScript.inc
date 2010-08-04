<?php
/**
 * Interface for all cron job script.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Cron
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

interface CronScript
{


    /**
     * Do the actual job here.
     *
     * @since  4.0.0
     * @return void
     */
    public function run();


    /**
     * Stop the running cron job.
     *
     * @since  4.0.0
     * @return void
     */
    public function stop();


}//end interface

?>
