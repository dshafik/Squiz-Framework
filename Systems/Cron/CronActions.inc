<?php
/**
 * Actions for the Cron System.
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
 * Cron System Actions Class.
 */
class CronActions
{


    /**
     * Returns the contents of the widget (or in this case, template).
     *
     * @param array  $settings The list of settings for this widget.
     * @param string $tplFile  The template file to use.
     *
     * @return string
     */
    public static function paint(array $settings=array(), $tplFile=NULL)
    {
        $content = '';
        if ($tplFile !== NULL) {
            $content = file_get_contents($tplFile);
        }

        return $content;

    }//end paint()


    /**
     * Main action which runs the given type of cron jobs.
     *
     * @param string $typeid ID of cron job type.
     *
     * @return void
     */
    public static function runCronJobs($typeid)
    {
        Channels::includeSystem('Cron');

        $currTs           = time();
        $tData            = Cron::getCronType($typeid);
        $jobQueue         = array();
        $cronType         = $tData['typeid'];
        $cronTypeTitle    = $tData['type_title'];
        $cronTypeInterval = Cron::getCronTypeInterval($tData['condition']);

        $parts = explode(' ', $tData['condition']);
        $min   = $parts[0];
        $hour  = $parts[1];

        if ($tData['started'] !== NULL) {
            $tData['started'] = strtotime($tData['started']);
        }

        if ($tData['updated'] !== NULL) {
            $tData['updated'] = strtotime($tData['updated']);
        }

        // Check if this cron is due to run.
        $dueToRun     = FALSE;
        $startRounded = strtotime(date('m/d/Y H:i:00', $tData['started']));
        $nextRun      = ($startRounded + $cronTypeInterval);
        if ($nextRun <= $currTs) {
            // Correct interval since last run, do it.
            $dueToRun = TRUE;
        } else {
            if ($tData['typeid'] !== 'frequent') {
                // Also check if the time is exact for daily/hourly.
                // So if cron fails, we don't have to wait 1 day/hour.
                $nowTime = date('G:i', time());
                $minx    = $min;
                if ($minx < 10) {
                    $minx = '0'.$minx;
                }

                if ($tData['typeid'] === 'hourly') {
                    $exactTime = date('G', time()).':'.$minx;
                } else if ($tData['typeid'] === 'daily') {
                    $exactTime = $hour.':'.$minx;
                }

                if ($exactTime === $nowTime) {
                    $dueToRun = TRUE;
                }
            }
        }//end if

        $run = FALSE;
        if ($tData['running'] === TRUE) {
            if ($dueToRun === TRUE) {
                if ($tData['typeid'] === 'daily') {
                    // If a daily cron is due to run, do not skip it.
                    $tData['run_check'] = 0;
                    $tData['started']   = NULL;
                    $tData['running']   = FALSE;

                    $args = array(
                             $tData['typeid'],
                             $tData['type_title'],
                             $tData['condition'],
                             $tData['running'],
                             $tData['run_check'],
                             NULL,
                             NULL,
                             0,
                            );

                    call_user_func_array(
                        array(
                         'Cron',
                         'updateCronType',
                        ),
                        $args
                    );

                    $run = TRUE;
                } else if ($tData['run_check'] < 2) {
                    // Increase run check for cron type.
                    $args = array(
                             $tData['typeid'],
                             $tData['type_title'],
                             $tData['condition'],
                             $tData['running'],
                             ($tData['run_check'] + 1),
                             Cron::formatTime($tData['started']),
                             Cron::formatTime($tData['updated']),
                             0,
                            );

                    call_user_func_array(
                        array(
                         'Cron',
                         'updateCronType',
                        ),
                        $args
                    );

                    return;
                } else {
                    // Reset the cron as it appears to be stuck.
                    $tData['run_check'] = 0;
                    $tData['started']   = NULL;
                    $tData['running']   = FALSE;

                    $args = array(
                             $tData['typeid'],
                             $tData['type_title'],
                             $tData['condition'],
                             $tData['running'],
                             $tData['run_check'],
                             NULL,
                             NULL,
                             0,
                            );

                    call_user_func_array(
                        array(
                         'Cron',
                         'updateCronType',
                        ),
                        $args
                    );

                    // Finally, let this type run.
                    $run = TRUE;
                }//end if
            }//end if
        } else {
            if ($tData['updated'] === NULL) {
                // This is the first time cron jobs are running.
                // If the interval is longer than an hour, which means that
                // it has a fixed minute to run the job.
                // Work it out when to start to run.
                // Once it's been started, only the interval can be used to
                // determine to check the next cycle.
                $currT = time();
                if ($cronTypeInterval >= (60 * 60)) {
                    $currM = date('i', $currT);
                    if ($currM !== '0' && $currM[0] === '0') {
                        $currM = substr($currM, 1);
                    }

                    if ((int) $currM >= (int) $min) {
                        $run = TRUE;
                    }

                    if ($hour !== '*') {
                        $currH = date('G', $currT);
                        if ((int) $currH === (int) $hour) {
                            $run = TRUE;
                        } else {
                            $run = FALSE;
                        }
                    }
                } else {
                    $run = TRUE;
                }//end if
            } else {
                $run = $dueToRun;
            }//end if
        }//end if

        if ($run === FALSE) {
            return;
        }

        $jobs = Cron::getCronJobs(TRUE, NULL, $cronType);
        foreach ($jobs as $job) {
            $jobQueue[] = $job;
        }

        if (empty($jobQueue) === TRUE) {
            // Even though no jobs ran, record that this
            // cron type ran so we can see it is working.
            $time = time();
            Cron::updateCronType(
                $tData['typeid'],
                $tData['type_title'],
                $tData['condition'],
                FALSE,
                $tData['run_check'],
                Cron::formatTime($time),
                Cron::formatTime($time),
                0
            );
            return;
        }

        // Start Cron Jobs, $typeid.
        $startTs = time();
        Cron::updateCronType(
            $tData['typeid'],
            $tData['type_title'],
            $tData['condition'],
            TRUE,
            $tData['run_check'],
            Cron::formatTime($startTs),
            NULL,
            0
        );

        $jobDuration = 0;
        foreach ($jobQueue as $job) {
            $lastRun = $job['last_run'];
            if ($lastRun !== NULL) {
                $lastRun = strtotime($lastRun);
            }

            // Set cron job running TRUE.
            Cron::updateCronJob(
                $job['system'],
                $job['script'],
                Cron::formatTime($lastRun),
                $job['last_duration'],
                $job['interval'],
                TRUE,
                $job['enabled']
            );

            $systemPath = Channels::getSystemsPath($job['system']);
            include_once $systemPath.'/Scripts/'.$job['script'];

            $start     = time();
            $className = str_replace('.inc', '', $job['script']);
            $cronObj   = new $className();
            $result    = $cronObj->run();
            $end       = time();

            // Set cron job running FALSE.
            $scriptDuration = ($end - $start);
            Cron::updateCronJob(
                $job['system'],
                $job['script'],
                Cron::formatTime($start),
                $scriptDuration,
                $job['interval'],
                FALSE,
                TRUE
            );

            $jobDuration += $scriptDuration;
            Cron::updateCronType(
                $tData['typeid'],
                $tData['type_title'],
                $tData['condition'],
                TRUE,
                $tData['run_check'],
                Cron::formatTime($startTs),
                Cron::formatTime($end),
                $jobDuration
            );
        }//end foreach

        // End Cron Jobs, $typeid.
        $endTs = time();
        Cron::updateCronType(
            $tData['typeid'],
            $tData['type_title'],
            $tData['condition'],
            FALSE,
            $tData['run_check'],
            Cron::formatTime($startTs),
            Cron::formatTime($endTs),
            $jobDuration
        );

    }//end runCronJobs()


    /**
     * Disable all the cron jobs.
     *
     * @return void
     */
    public static function disableAllCronJobs()
    {
        Channels::includeSystem('Cron');
        $jobs = Cron::getCronJobs();
        foreach ($jobs as $job) {
            if ($job['enabled'] === TRUE) {
                Cron::updateCronJob(
                    $job['system'],
                    $job['script'],
                    $job['last_run'],
                    $job['last_duration'],
                    $job['interval'],
                    $job['running'],
                    FALSE
                );
            }
        }

    }//end disableAllCronJobs()


    /**
     * Kill all the cron related processes.
     *
     * This actions is used when the administrator is restoring the system from
     * backup.
     *
     * @return void
     */
    public static function killAllCronProcesses()
    {
        // Let's kill the mail run.php command.
        $command  = 'ps a';
        $command .= '|grep "/usr/bin/php -f '.Init::ROOT_DIR.'/Systems/Cron/Scripts/run.php"';
        $command .= '|grep -v "grep /usr/bin/php"';

        $output = array();
        exec($command, $output);
        foreach ($output as $line) {
            $matches = array();
            preg_match('/^\d+ /', $line, $matches);
            if (empty($matches) === FALSE) {
                $pid     = trim($matches[0]);
                $command = 'kill -9 '.$pid;
                exec($command);
            }
        }

        // Then let's see any cron job is running. If so call stop() function so that
        // they can do their own clean up if required.
        Channels::includeSystem('Cron');
        $cronJobs = Cron::getCronJobs();
        foreach ($cronJobs as $cronJob) {
            if ($cronJob['running'] === TRUE) {
                include_once Init::ROOT_DIR.'/Systems/'.$cronJob['system'];
                $className = str_replace('.inc', '', $cronJob['script']);
                $jobObj    = new $className();
            }
        }

    }//end killAllCronProcesses()


    /**
     * Resets the cron jobs and types.
     *
     * Set running flag to FALSE, last run time to now and run check to zero.
     *
     * @return void
     */
    public static function resetCron()
    {
        Channels::includeSystem('Cron');

        $jobs = Cron::getCronJobs();
        foreach ($jobs as $job) {
            Cron::updateCronJob(
                $job['system'],
                $job['script'],
                Cron::formatTime(time()),
                $job['last_duration'],
                $job['interval'],
                FALSE,
                $job['enabled']
            );
        }

        $types = Cron::getCronTypes();
        foreach ($types as $type) {
            Cron::updateCronType(
                $type['typeid'],
                $type['type_title'],
                $type['condition'],
                FALSE,
                0,
                NULL,
                NULL,
                $type['run_time']
            );
        }

    }//end resetCron()


    /**
     * Return the details of cron job.
     *
     * @param string $system     Name of the system cron job belongs to.
     * @param string $scriptName Name of the script to run.
     *
     * @return array
     */
    public static function getCronJob($system, $scriptName)
    {
        $cron         = array();
        $getCronQuery = DAL::getQuery('Cron', 'getCron');
        $cron         = DAL::getAssoc($getCronQuery);
        if (isset($cron[0]) === TRUE) {
            $cron = $cron[0];
        }

        $cron['scriptPath']  = Init::ROOT_DIR.'/Systems/'.$cron['system'];
        $cron['scriptPath'] .= '/Scripts/'.$cron['script'];
        return $cron;

    }//end getCronJob()


    /**
     * Return the list of cron jobs.
     *
     * @param mixed  $enabled If TRUE only returns enabled cron jobs.
     * @param mixed  $running If TRUE only returns running cron jobs.
     * @param string $type    Type of cron jobs.
     *
     * @return array
     */
    public static function getCronJobs($enabled=NULL, $running=NULL, $type='')
    {
        $getCronQuery = DAL::getQuery('Cron', 'getCrons');
        $crons        = DAL::getAssoc($getCronQuery);

        if ($enabled === TRUE || $running === TRUE) {
            $filtered = array();
            foreach ($crons as $cronData) {
                if ($enabled === TRUE && $cronData['enabled'] === FALSE) {
                    continue;
                }

                if ($running === TRUE && $cronData['running'] === FALSE) {
                    continue;
                }

                if (empty($type) === FALSE && $cronData['interval'] !== $type) {
                    continue;
                }

                $filtered[] = $cronData;
            }

            $crons = $filtered;
        }

        foreach ($crons as &$cron) {
            $cron['scriptPath']  = Init::ROOT_DIR.'/Systems/'.$cron['system'];
            $cron['scriptPath'] .= '/Scripts/'.$cron['script'];
        }

        return $crons;

    }//end getCronJobs()


    /**
     * Update the cron job details.
     *
     * @param string  $system       Name of the system cron job belongs to.
     * @param string  $scriptName   Name of the script to run.
     * @param integer $lastRun      Timestamp when the job ran last time.
     * @param integer $lastDuration Timestamp of the length of the job ran.
     * @param string  $interval     Type of cron job.
     * @param boolean $running      TRUE if the cron job is running.
     * @param boolean $enabled      TRUE if the cron job is enabled.
     *
     * @return void
     * @throws ChannelException Error occurred during query execution.
     */
    public static function updateCronJob(
        $system,
        $scriptName,
        $lastRun,
        $lastDuration,
        $interval,
        $running,
        $enabled
    ) {
        try {
            DAL::beginTransaction();
            Channels::includeSystem('Cron');
            $cronJob = Cron::getCronJob($system, $scriptName);
            if ($cronJob['interval'] !== $interval) {
                $logMsg  = '<strong>'.$scriptName.'</strong> script';
                $logMsg .= ' interval changed from <strong>';
                $logMsg .= $cronJob['interval'].'</strong> to <strong>';
                $logMsg .= $interval.'</strong>';

                Channels::includeSystem('Log');
                Log::addSystemLog('cron.updated', $logMsg);
            }

            if ($cronJob['enabled'] !== $enabled) {
                $logMsg = '<strong>'.$scriptName.'</strong> script';
                if ($enabled === TRUE) {
                    $logMsg .= ' was enabled';
                } else {
                    $logMsg .= ' was disabled';
                }

                Channels::includeSystem('Log');
                Log::addSystemLog('cron.updated', $logMsg);
            }

            DAL::executeQuery('Cron', 'updateCron');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e);
        }//end try

    }//end updateCronJob()


    /**
     * Install cron scripts listed in a system.xml file.
     *
     * @param string $system The name of the system we are installing jobs for.
     * @param string $path   The file system location of the system.
     *
     * @return void
     * @throws ChannelException Occurs when script has no description.
     */
    public static function installCronScripts($system, $path)
    {
        Channels::includeSystem('Cron');

        // Load system XML file.
        $fileName = $path.'/'.$system.'.xml';
        if (file_exists($fileName) === FALSE) {
            return;
        }

        $doc = new DomDocument();
        $doc->load($fileName);

        if ($doc === NULL) {
            return;
        }

        $cronJobsTag = $doc->getElementsByTagName('cron_jobs');
        if ($cronJobsTag->length === 1) {
            $cronJobs = $doc->getElementsByTagName('cron_job');
            foreach ($cronJobs as $cronJob) {
                $script   = $cronJob->getAttribute('script');
                $name     = $cronJob->getAttribute('name');
                $interval = $cronJob->getAttribute('interval');

                // Get description, required.
                $descriptionNode = $cronJob->getElementsByTagName('description')->item(0);
                if ($descriptionNode === NULL) {
                    throw new ChannelException('Must supply description for cron job.');
                }

                $description = $descriptionNode->nodeValue;

                // Register the job.
                Cron::addCronScript(
                    $system,
                    $script,
                    $name,
                    $description
                );

                Cron::updateCronJob(
                    $system,
                    $script,
                    NULL,
                    NULL,
                    $interval,
                    FALSE,
                    FALSE
                );
            }//end foreach
        }//end if

    }//end installCronScripts()


    /**
     * Add a new cron job.
     *
     * New script added as daily and disabled by default.
     *
     * @param string $system      Name of the system cron job belongs to.
     * @param string $script      Name of the script to run.
     * @param string $name        Friendly name of the cron job.
     * @param string $description Short description of what the job does.
     *
     * @return void
     * @throws ChannelException Error occurred during query execution.
     */
    public static function addCronScript($system, $script, $name, $description)
    {
        try {
            DAL::beginTransaction();
            DAL::executeQuery('Cron', 'addCronScript');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e);
        }

    }//end addCronScript()


    /**
     * Add a new cron type.
     *
     * @param string $typeid    Unique ID for the cron type.
     * @param string $typeTitle Display name for cron type.
     * @param string $condition Crontab style definition of frequency.
     *
     * @return void
     * @throws ChannelException Error occurred during query execution.
     */
    public static function addCronType($typeid, $typeTitle, $condition)
    {
        try {
            DAL::beginTransaction();
            DAL::executeQuery('Cron', 'addCronType');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e);
        }

    }//end addCronType()


    /**
     * Return the details of cron type.
     *
     * @param string $typeid Unique ID for the cron type.
     *
     * @return array
     * @throws ChannelException Error occurred during query execution.
     */
    public static function getCronType($typeid)
    {
        $getCronQuery = DAL::getQuery('Cron', 'getCronType');
        $type         = DAL::getAssoc($getCronQuery);

        if (isset($type[0]) === TRUE) {
            return $type[0];
        }

        return $type;

    }//end getCronType()


    /**
     * Return the list of cron types.
     *
     * @return array
     */
    public static function getCronTypes()
    {
        $getCronQuery = DAL::getQuery('Cron', 'getCronTypes');
        $types        = DAL::getAssoc($getCronQuery);

        return $types;

    }//end getCronTypes()


    /**
     * Update the cron type details.
     *
     * @param string  $typeid    Unique ID of cron type.
     * @param string  $typeTitle Display name of cron type.
     * @param string  $condition Crontab style frequency definition.
     * @param boolean $running   TRUE if the cron job is running.
     * @param boolean $runCheck  Run check counter per frequency.
     * @param integer $started   Timestamp when the cron type started.
     * @param integer $updated   Timestamp when the cron type last updated.
     * @param integer $runTime   Timestamp of the length of the job type ran.
     *
     * @return void
     * @throws ChannelException Error occurred during query execution.
     */
    public static function updateCronType(
        $typeid,
        $typeTitle,
        $condition,
        $running,
        $runCheck,
        $started,
        $updated,
        $runTime
    ) {
        try {
            Channels::includeSystem('Cron');
            DAL::beginTransaction();

            $currCronInfo = Cron::getCronType($typeid);
            if ($currCronInfo['condition'] !== $condition) {
                Channels::includeSystem('Log');
                $msg = '<strong>'.$typeTitle.' cron</strong> changed from running';

                switch ($typeid) {
                    case 'frequent':
                        $msg .= ' every <strong>'.current(sscanf($currCronInfo['condition'], '*/%d *')).'</strong> minutes';
                        $msg .= ' to every <strong>'.current(sscanf($condition, '*/%d *')).'</strong> minutes';
                    break;

                    case 'hourly':
                        $msg .= ' at <strong>'.current(sscanf($currCronInfo['condition'], '%d *')).'</strong> minutes past the hour';
                        $msg .= ' to <strong>'.current(sscanf($condition, '%d *')).'</strong> minutes past the hour';
                    break;

                    case 'daily':
                        list($newMin, $newHour) = sscanf($condition, '%d %d');
                        list($oldMin, $oldHour) = sscanf($currCronInfo['condition'], '%d %d');
                        $msg .= ' at <strong>'.$oldHour.':'.$oldMin.'</strong>';
                        $msg .= ' to <strong>'.$newHour.':'.$newMin.'</strong>';
                    break;

                    default:
                        // Not Yet.
                    break;
                }//end switch

                Log::addSystemLog('cron.updated', $msg);
            }//end if

            DAL::executeQuery('Cron', 'updateCronType');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollback();
            throw new ChannelException($e);
        }//end try

    }//end updateCronType()


    /**
     * Returns the interval in seconds for given frequency string.
     *
     * @param string $cond Frequency string.
     *
     * @return integer
     */
    public static function getCronTypeInterval($cond)
    {
        $parts = explode(' ', $cond);
        $min   = $parts[0];
        $hour  = $parts[1];

        if ($hour === '*') {
            if (strpos($min, '/') !== FALSE) {
                // Every some minutes.
                $matches = array();
                preg_match('/\*\/([0-9]{1,2})/', $min, $matches);
                if (empty($matches) === FALSE) {
                    $interval = (((int) $matches[1]) * 60);
                }
            } else {
                // Every hour.
                $interval = (60 * 60);
            }
        } else {
            // Every day.
            $interval = (24 * 60 * 60);
        }

        return $interval;

    }//end getCronTypeInterval()


    /**
     * Returns the interval in human readable form.
     *
     * @param string $cond Frequency string.
     *
     * @return string
     */
    public static function getCronTypeString($cond)
    {
        $parts = explode(' ', $cond);
        $min   = $parts[0];
        $hour  = $parts[1];
        $str   = '';
        if ($hour === '*') {
            if (strpos($min, '/') !== FALSE) {
                // Every some minutes.
                $matches = array();
                preg_match('/\*\/([0-9]{1,2})/', $min, $matches);
                if (empty($matches) === FALSE) {
                    $str = 'Every '.$matches[1].' minute(s).';
                }
            } else {
                // Every hour.
                $str = 'Every hour '.$min.' minute(s).';
            }
        } else {
            // Every day.
            $str = 'Every day at '.$hour.':'.$min.'.';
        }

        return $str;

    }//end getCronTypeString()


    /**
     * Formats the timestamp accordingly.
     *
     * @param object|string $timestamp The timestamp to format.
     *
     * @return object|string
     */
    public static function formatTime($timestamp)
    {
        $res = $timestamp;
        if ($timestamp !== NULL && $timestamp !== 0) {
            include_once 'Libs/String/String.inc';
            $res = String::tsIso8601($timestamp);
        }

        return $res;

    }//end formatTime()


    /**
     * Get the config data for the given cron script.
     *
     * @param string $system The name of the system cron belongs to.
     * @param string $script The name of the script.
     *
     * @return mixed
     */
    public static function getCronConfig($system, $script)
    {
        $query  = DAL::getQuery('Cron', 'getCronConfig');
        $result = DAL::getAssoc($query);
        return $result;

    }//end getCronConfig()


    /**
     * Set the config data for the given cron script.
     *
     * @param string $system The name of the system cron belongs to.
     * @param string $script The name of the script.
     * @param string $config The config data to save.
     *
     * @return mixed
     */
    public static function setCronConfig($system, $script, $config)
    {
        $result = DAL::executeQuery('Cron', 'setCronConfig');
        return $result;

    }//end setCronConfig()


    /**
     * Returns the list of cron jobs for the Cron Screen table.
     *
     * @param string $widgetid Id of the widget thats painting the jobs list.
     *
     * @return array
     */
    public static function getCronScreenJobsList($widgetid)
    {
        $jobs        = array();
        $cronScripts = Cron::getCronJobs();

        foreach ($cronScripts as $cronScript) {
            $rowid        = $cronScript['system'].'_'.$cronScript['script'];
            $jobs[$rowid] = self::_getCronScreenJobInfo($cronScript, $widgetid);
        }

        return $jobs;

    }//end getCronScreenJobsList()


    /**
     * Returns a single row content for the Cron Screen jobs table.
     *
     * @param array  $script   Cron Job script info.
     * @param string $widgetid Id of the widget thats painting the jobs list.
     *
     * @return string
     */
    private static function _getCronScreenJobInfo(array $script, $widgetid)
    {
        $info = array();
        $id   = $widgetid.'-'.$script['system'].'_'.$script['script'];

        $jobDesc  = '<strong>'.$script['name'].'</strong><br />';
        $jobDesc .= '<em>'.$script['description'].'</h4>';

        $info['job'] = $jobDesc;

        $runTime = '--';

        $script['last_run'] = time();

        if ($script['last_run'] !== NULL) {
            include_once 'Libs/String/String.inc';
            $runTime = String::easyDatetime(strtotime($script['last_run']));
            if ($script['last_duration'] !== NULL) {
                include_once 'Libs/String/String.inc';
                $runTime .= ' ('.String::readableSeconds($script['last_duration']).')';
            }
        }

        $info['lastRun'] = $runTime;

        $types = Cron::getCronTypes();
        foreach ($types as $type) {
            $intervalOpts[$type['typeid']] = $type['type_title'];
        }

        CHannels::includeSystem('GUI');

        $settings          = array(
                              'selected' => $script['interval'],
                              'options'  => $intervalOpts,
                             );
        $templateCode      = GUI::createWidget($id.'-frequency', 'GUISelect', $settings);
        $info['frequency'] = $templateCode;

        $settings        = array(
                            'value' => $script['enabled'],
                           );
        $templateCode    = GUI::createWidget($id.'-enabled', 'GUIToggleButton', $settings);
        $info['enabled'] = $templateCode;

        return $info;

    }//end _getCronScreenJobInfo()


    /**
     * Returns the run interval for the specified cron type.
     *
     * @param string $cronType Type of the cron.
     *
     * @return string
     */
    public static function getCronInterval($cronType)
    {
        $typeData = DAL::getAssoc(Cron::getCronType($cronType), 2);
        $interval = Cron::getCronTypeInterval($typeData['condition']);

        return $interval;

    }//end getCronInterval()


    /**
     * Returns the last run time for the specified cron type.
     *
     * @param string $cronType Type of the cron.
     *
     * @return string
     */
    public static function getLastRunTime($cronType)
    {
        include_once 'Libs/String/String.inc';
        $runTime = String::easyDatetime(time());

        return $runTime;

    }//end getLastRunTime()


    /**
     * Returns the current date and time.
     *
     * @return string
     */
    public static function getCurrentDateTime()
    {
        include_once 'Libs/String/String.inc';
        $time = String::easyDatetime(time());

        return $time;

    }//end getCurrentDateTime()


    /**
     * Save the CronScreen settings.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function saveCronScreen(array $data)
    {
        $errors = array();

        try {
            DAL::beginTransaction();
            // Save Cron Job changes.
            $jobList = Util::getArrayIndex($data, 'cronJobsList', array());
            foreach ($jobList as $systemScript => $info) {
                list($system, $scriptName) = explode('_', $systemScript);

                $frequency = Util::getArrayIndex($info, 'frequency', 'daily');
                $enabled   = Util::getArrayIndex($info, 'enabled', FALSE);

                Channels::addToBasket('interval', $frequency, TRUE);
                Channels::addToBasket('enabled', $enabled, TRUE);
                Channels::addToBasket('system', $system, TRUE);
                Channels::addToBasket('scriptName', $scriptName, TRUE);
                DAL::executeQuery('Cron', 'updateCronIntervalAndStatus');
            }

            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            $errors[] = $e->getMessage();
        }//end try

        // Update frequent interval.
        $cronSettings = Util::getArrayIndex($data, 'cronSettings', array());
        if (isset($cronSettings['frequentRunInterval']) === TRUE) {
            $intervalStr = '*/'.$cronSettings['frequentRunInterval'].' *';
            self::_updateFrequencyInterval('frequent', $intervalStr);
        }

        if (isset($cronSettings['hourlyRunInterval']) === TRUE) {
            $intervalStr = $cronSettings['hourlyRunInterval'].' *';
            self::_updateFrequencyInterval('hourly', $intervalStr);
        }

        if (isset($cronSettings['dailyRunInterval-hour']) === TRUE
            && isset($cronSettings['dailyRunInterval-min']) === TRUE
        ) {
            $intervalStr = $cronSettings['dailyRunInterval-min'].' '.$cronSettings['dailyRunInterval-hour'];
            self::_updateFrequencyInterval('daily', $intervalStr);
        }

        $retVal = TRUE;
        if (empty($errors) === FALSE) {
            $retVal = array('errors' => $errors);
        }

        return $retVal;

    }//end saveCronScreen()


    /**
     * Updates the run interval of a cron type.
     *
     * @param string $typeid      Type of the cron.
     * @param string $intervalStr A valid cron time string.
     *
     * @since  4.0.0
     * @return boolean
     */
    private static function _updateFrequencyInterval($typeid, $intervalStr)
    {
        Channels::createEventBasket();
        try {
            DAL::beginTransaction();
            Channels::addToBasket('typeid', $typeid);
            Channels::addToBasket('condition', $intervalStr);
            DAL::executeQuery('Cron', 'updateFrequencyInterval');
            DAL::commit();
        } catch (DALException $e) {
            DAL::rollBack();
            Channels::removeEventBasket();
            return $e->getMessage();
        }

        Channels::removeEventBasket();
        return TRUE;

    }//end _updateFrequencyInterval()


}//end class

?>
