<?php
/**
 * Cron System Class.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Cron
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Cron System Class.
 *
 * @since 4.0.0
 */
class CronSystem extends BaseSystem
{


    /**
     * Install this system.
     *
     * @since  4.0.0
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
