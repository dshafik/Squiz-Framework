<?php
/**
 * Actions for the Patching System.
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
 * Patching Actions Class.
 */
class PatchingActions
{

    /**
     * Reference to HTTPRequest object.
     *
     * @var object
     */
    private static $_httpRequest = NULL;

    /**
     * Cache variable for update server.
     *
     * @var array
     */
    private static $_updateServerCache = NULL;


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the contents of the widget (or in this case, template).
     *
     * @param array  $settings The list of settings for this widget.
     * @param string $tplFile  The template file to use.
     *
     * @return string
     */
    public static function getTemplateContent(array $settings=array(), $tplFile=NULL)
    {
        $content = '';
        if ($tplFile !== NULL) {
            $content = file_get_contents($tplFile);
        }

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Add the revision number to the product summary.
     *
     * @param array $summary The summary of the current product.
     *
     * @return array
     */
    public static function addProductSummary(array $summary)
    {
        $summary[] = array(
                      'label' => 'Revision',
                      'value' => Patching::getCurrentRevision(),
                     );
        return $summary;

    }//end addProductSummary()


    /**
     * Returns Patching config data.
     *
     * @return array
     */
    public static function getPatchingConfig()
    {
        Channels::includeSystem('SystemConfig');
        $config = SystemConfig::getConfig('Patching');
        return $config;

    }//end getPatchingConfig()


    /**
     * Returns TRUE if the system is a client system.
     *
     * @return boolean
     */
    public static function isClientSystem()
    {
        if (file_exists(Init::ROOT_DIR.'/.svn') === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end isClientSystem()


    /**
     * Returns the revision number of the current installation.
     *
     * If the installation is a client system, it gets the revision number from
     * Pathing config data. Otherwise it gets directly from subversion.
     *
     * @return string
     */
    public static function getCurrentRevision()
    {
        $revision = '';
        if (Patching::isClientSystem() === FALSE) {
            // Developer's checkedout system.
            ob_start();
                system('/usr/bin/svn info '.Init::ROOT_DIR);
                $result = ob_get_contents();
            ob_end_clean();
            $matches = array();
            preg_match('/Revision: (\d{1,5})/i', $result, $matches);
            if (empty($matches) === FALSE) {
                $revision = $matches[1];
            } else {
                // SVN Fail, try looking up the Revision directly.
                ob_start();
                    system('cat '.Init::ROOT_DIR.'/.svn/entries | grep -B 1 "https://cvs.squiz.net/svn"');
                    $svnRevision = ob_get_contents();
                ob_end_clean();
                $svnRevision = explode("\n", $svnRevision);
                $revision    = $svnRevision[0];
            }
        } else {
            // Client system.
            $patchingConfig = Patching::getPatchingConfig();
            $revision       = $patchingConfig['revision'];
        }//end if

        return $revision;

    }//end getCurrentRevision()


    /**
     * Returns Patch information for the specific revision.
     *
     * @param string $revision Revision number to enquire.
     *
     * @return array
     */
    public static function getPatchInfoForRevision($revision)
    {
        $patchConfig = Patching::getPatchingConfig();
        if ($revision <= $patchConfig['revision']) {
            return 'installed';
        }

        $patchList = Patching::getPatchingInfo();
        $patchNotAvailable = TRUE;
        $availablePatch    = NULL;
        foreach ($patchList as $patch) {
            if ($revision >= $patch['from'] && $revision <= $patch['to']) {
                $patchNotAvailable = FALSE;
                $availablePatch    = $patch;
                break;
            }
        }

        if ($patchNotAvailable === TRUE) {
            return 'not_available';
        }

        if ($availablePatch['scheduled'] !== '') {
            return 'scheduled_for_installation';
        }

        return 'downloaded';

    }//end getPatchInfoForRevision()


    /**
     * It extracts the patch.
     *
     * @param string $name Name of the patch.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _extractPatch($name)
    {
        // Unzip tar file into data dir and register patch.
        $dataDir   = BaseSystem::getDataDir('Patching');
        $filename  = $name.'.tar.gz';
        $filepath  = $dataDir.'/'.$filename;
        $targetDir = $dataDir.'/'.$name;
        if (file_exists($targetDir) === TRUE) {
            exec('rm -rf '.$targetDir);
        }

        mkdir($targetDir, 0777);
        exec('tar -xvzf '.$filepath.' -C '.$targetDir);

    }//end _extractPatch()


    /**
     * It extracts patch_note.txt file out of the patch.
     *
     * @param string $name Name of the patch.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _extractPatchNote($name)
    {
        self::_extractPatch($name);

        $dataDir   = BaseSystem::getDataDir('Patching');
        $targetDir = $dataDir.'/'.$name;
        $command   = 'cp '.$targetDir.'/patch_note.txt ';

        $notePath = $dataDir.'/'.$name.'_note.txt';
        $command .= $notePath;

        exec($command);
        exec('rm -rf '.$targetDir);

        include_once 'Libs/String/String.inc';
        $patchNote = file_get_contents($notePath);
        $summary   = String::ellipsisize($patchNote, 350);
        file_put_contents($dataDir.'/'.$name.'_summary.txt', $summary);

    }//end _extractPatchNote()


    /**
     * Retrieves the patch note (or summary).
     *
     * Patch note (or summary) is basically a plain .txt file that comes
     * with the patch and will reside in the data dir once extracted.
     *
     * @param string  $name       Name of the patch to get patch note for.
     * @param boolean $getSummary Whether to get the full patch note or summary.
     *
     * @return string
     */
    public static function getPatchNote($name, $getSummary=FALSE)
    {
        $note    = '';
        $dataDir = BaseSystem::getDataDir('Patching');

        if ($getSummary === TRUE) {
            $filename = $dataDir.'/'.$name.'_summary.txt';
        } else {
            $filename = $dataDir.'/'.$name.'_note.txt';
        }

        if (file_exists($filename) === TRUE) {
            $note = file_get_contents($filename);
        }

        return $note;

    }//end getPatchNote()


    /**
     * Register a patch.
     *
     * @param string $name Name of the new patch to register.
     * @param string $sha1 SHA1 message digest of the new patch.
     *
     * @return boolean
     */
    public static function registerPatch($name, $sha1)
    {
        Channels::includeSystem('User');
        $userid = User::getCurrentUserid();
        if ($userid === NULL) {
            $userid = 0;
        }

        Channels::addToBasket('userid', $userid);

        try {
            $baseQuery = DAL::getDALQuery('Patching', 'registerPatch', 'executeQuery');
            $result    = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return FALSE;
        }

        return $result;

    }//end registerPatch()


    /**
     * Remove a patch.
     *
     * @param string $name Name of the new patch to remove.
     *
     * @return boolean
     */
    public static function removePatch($name)
    {
        try {
            $baseQuery = DAL::getDALQuery('Patching', 'removePatch', 'executeQuery');
            $result    = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return FALSE;
        }

        // Clean up the downloaded files and directories.
        $dataDir               = BaseSystem::getDataDir('Patching');
        $patchFilePath         = $dataDir.'/'.$name.'.tar.gz';
        $extractedDirPatchPath = $dataDir.'/'.$name;

        if (file_exists($patchFilePath) === TRUE) {
            unlink($patchFilePath);
        }

        if (file_exists($extractedDirPatchPath) === TRUE) {
            exec('rm -rf '.$extractedDirPatchPath);
        }

        return $result;

    }//end removePatch()


    /**
     * Schedule a patch to be applied at a specific time.
     *
     * If time is NULL,
     *
     * @param string $name The name of the patch to be scheduled.
     * @param string $time The timestamp of the scheduled time.
     *
     * @return boolean
     */
    public static function schedulePatch($name, $time)
    {
        $info = Patching::getPatchingInfo();
        foreach ($info as $i) {
            if ($i['name'] === $name) {
                if ($time !== NULL && strtotime($i['scheduled']) === $time) {
                    // Same time, don't update.
                    return TRUE;
                }
            }
        }

        if ($time !== NULL) {
            include_once 'Libs/String/String.inc';
            $scheduled = String::tsIso8601($time);
        } else {
            $scheduled = NULL;
        }

        Channels::includeSystem('Authentication');
        $userid = Authentication::getCurrentUserid();

        Channels::addToBasket('scheduled', $scheduled);
        Channels::addToBasket('userid', $userid);

        try {
            $baseQuery = DAL::getDALQuery('Patching', 'schedulePatch', 'executeQuery');
            $result    = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return FALSE;
        }

        if (Channels::systemExists('Log') === TRUE) {
            Channels::includeSystem('Log');
            if ($time === NULL) {
                $msg = 'Scheduled update <strong>'.$name.'</strong> removed';
                Log::addSystemLog('patching.unschedule', $msg);
            } else {
                include_once 'Libs/String/String.inc';
                $t    = String::easyDatetime($time);
                $msg  = 'Update <strong>'.$name.'</strong> scheduled for ';
                $msg .= '<strong>'.$t.'</strong>';
                Log::addSystemLog('patching.schedule', $msg);
            }
        }

        return $result;

    }//end schedulePatch()


    /**
     * Get the all downloaded patches, scheduled or not scheduled.
     *
     * @param boolean $installed TRUE to get the installed patches.
     *
     * @return array
     */
    public static function getPatches($installed=FALSE)
    {
        $baseQuery = DAL::getDALQuery('Patching', 'getPatches', 'Assoc');
        $patches   = DAL::executeDALQuery($baseQuery);
        $pattern   = '/patch_(\d+)_(\d+)/';
        foreach ($patches as &$patchInfo) {
            $matches = array();
            preg_match_all($pattern, $patchInfo['name'], $matches);

            $patchInfo['from'] = (int) $matches[1][0];
            $patchInfo['to']   = (int) $matches[2][0];
        }

        return $patches;

    }//end getPatches()


    /**
     * Returns a list of scheduled patches waiting to be applied.
     *
     * @return array
     */
    public static function getPendingPatches()
    {
        try {
            $baseQuery = DAL::getDALQuery('Patching', 'getPendingPatches', 'Assoc');
            $res       = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return array();
        }

        // Only get patches that are ready to be applied.
        $result = array();
        foreach ($res as $r) {
            if (time() > strtotime($r['scheduled'])) {
                $result[] = $r['name'];
            }
        }

        sort($result);
        return $result;

    }//end getPendingPatches()


    /**
     * Archives a patch after it has been applied.
     *
     * @param string $name Name of the patch to be archived.
     *
     * @return boolean
     */
    public static function archivePatch($name)
    {
        include_once 'Libs/String/String.inc';
        $patched = String::tsIso8601(time());
        Channels::addToBasket('patched', $patched);

        try {
            $baseQuery = DAL::getDALQuery('Patching', 'archivePatch', 'executeQuery');
            $result    = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return FALSE;
        }

        if (Channels::systemExists('Log') === TRUE) {
            Channels::includeSystem('Log');
            $msg = 'New update <strong>'.$name.'</strong> installed';
            Log::addSystemLog('patching.installed', $msg);
        }

        return $result;

    }//end archivePatch()


    /**
     * Gets all the patch info.
     *
     * @return array
     */
    public static function getPatchingInfo()
    {
        try {
            $baseQuery = DAL::getDALQuery('Patching', 'getPatchingInfo', 'Assoc');
            $result    = DAL::executeDALQuery($baseQuery);
        } catch (PDOException $e) {
            return array();
        }

        sort($result);

        // Get extra info from the .inc file included in the patch tar.gz.
        $dataDir = BaseSystem::getDataDir('Patching');
        foreach ($result as $i => $patch) {
            $targetDir = $dataDir.'/'.$patch['name'];
            if (is_dir($targetDir) === TRUE) {
                $filename = $targetDir.'/patch.inc';
                if (file_exists($filename) === TRUE) {
                    include $filename;
                    if (empty($patchInfo) === FALSE) {
                        $result[$i]['from']  = $patchInfo['from'];
                        $result[$i]['to']    = $patchInfo['to'];
                        $result[$i]['block'] = $patchInfo['block'];
                        if (isset($patchInfo['reboot']) === TRUE) {
                            $result[$i]['reboot'] = $patchInfo['reboot'];
                        }
                    }
                } else {
                    $result[$i]['from']   = 'NA';
                    $result[$i]['to']     = 'NA';
                    $result[$i]['block']  = FALSE;
                    $result[$i]['reboot'] = FALSE;
                }
            }
        }//end foreach

        return $result;

    }//end getPatchingInfo()


    /**
     * Gets the time till for next update check, based on config.
     *
     * Returns never if cron is not enabled.
     * This method is used by the Next Update Check section of up2date screen.
     * Default check interval in patch config is one hour.
     *
     * @return string
     */
    public static function getNextUpdateCheckTime()
    {
        // Check if patching cron is enabled, if so, gather some info.
        Channels::includeSystem('Cron');
        $job       = Cron::getCronJob('Patching', 'PatchingCron.inc');
        $enabled   = FALSE;
        $nextRun   = 0;
        $nextCheck = 0;
        $interval  = 0;
        if (empty($job) === FALSE) {
            // Find out when will cron run again.
            if (isset($job['enabled']) === TRUE && $job['enabled'] === TRUE) {
                $enabled  = TRUE;
                $lastRun  = strtotime($job['last_run']);
                $type     = Cron::getCronType($job['interval']);
                $interval = Cron::getCronTypeInterval($type['condition']);
                $nextRun  = ($lastRun + $interval);
            }
        }

        $config = Patching::getPatchingConfig();
        if ($enabled === TRUE && $interval > 0) {
            if (isset($config['last_checked']) === TRUE) {
                $lastChecked = $config['last_checked'];
                if (isset($config['check_interval']) === TRUE) {
                    $checkInterval = $config['check_interval'];
                    $nextCheck     = ($lastChecked + $checkInterval);

                    // Can use 'Check ASAP' button to reset.
                    if ($nextRun < $nextCheck) {
                        $config['asap_enabled'] = TRUE;
                    }

                    // Find out which cron run will fire check for update.
                    while ($nextRun < $nextCheck) {
                        $nextRun += $interval;
                    }
                }
            }

            $nextCheck = $nextRun;
        }//end if

        // Convert timestamp to readable "x hours y minutes z seconds" format.
        if ($nextCheck === 0) {
            $nextCheck = 'Never';
        } else {
            include_once 'Libs/String/String.inc';
            $timeLeft     = ($nextCheck - time());
            $nextCheckHms = String::secondsToHms($timeLeft);
            $parts        = explode(':', $nextCheckHms);
            if ($timeLeft < 5) {
                $nextCheck = 'Checking now...';
                $config['asap_enabled'] = FALSE;
            } else if ($timeLeft < 60) {
                $nextCheck = '< 1 minute';
            } else if ($timeLeft < 3600) {
                // Format: min:sec.
                $min = $parts[0].' minutes';
                if ($parts[0] === '1') {
                    $min = '1 minute';
                }

                $sec = ltrim($parts[1], '0');
                if ($sec !== '1') {
                    $sec = $sec.' seconds';
                } else {
                    $sec = '1 second';
                }

                $nextCheck = $min.' '.$sec;
            } else {
                // Format: hr:min:sec.
                $hr = $parts[0].' hours';
                if ($parts[0] === '1') {
                    $hr = '1 hour';
                }

                $min = ltrim($parts[1], '0');
                if ($min !== '1') {
                    $min = $min.' minutes';
                } else {
                    $min = '1 minute';
                }

                $nextCheck = $hr.' '.$min;
            }//end if
        }//end if

        Channels::includeSystem('SystemConfig');
        SystemConfig::setConfig('Patching', $config);

        return $nextCheck;

    }//end getNextUpdateCheckTime()


    /**
     * Gets all the subscription info.
     *
     * @return array
     */
    public static function getSubscriptionInfo()
    {
        include_once 'Libs/Util/Util.inc';
        $config    = Patching::getPatchingConfig();
        $lastCheck = Util::getArrayIndex($config, 'sub_last_checked', 0);
        $expired   = _('EXPIRED');
        if ($lastCheck === 0 || (time() - $lastCheck > 3600)) {
            // It is time to check again.
            $res = Patching::wsCheckSubscription();
            if (is_array($res) === FALSE) {
                // No subscription.
                return $expired;
            }
        }

        $config  = Patching::getPatchingConfig();
        $expiry  = Util::getArrayIndex($config, 'sub_expiry', 0);
        $current = Util::getArrayIndex($config, 'sub_current', 0);
        if ($expiry !== 0 && $current !== 0) {
            $remain = (strtotime($expiry) - strtotime($current));
            if ($remain <= 0) {
                return $expired;
            }

            $days = (int) floor(($remain / 86400));
            if ($days === 0 || $days === 1) {
                return _('1 day');
            } else {
                return _($days.' days');
            }
        }

        return $expired;

    }//end getSubscriptionInfo()


    /**
     * Gets the last updated time.
     *
     * @return string
     */
    public static function getLastUpdatedTime()
    {
        include_once 'Libs/String/String.inc';
        $lastUpdated = 'N/A';
        $config      = Patching::getPatchingConfig();
        if ($config['last_updated'] !== 0) {
            $lastUpdated = String::easyDatetime($config['last_updated']);
        }

        return $lastUpdated;

    }//end getLastUpdatedTime()


    /**
     * Sends "New patch available" notification email, 1 email per patch.
     *
     * @param array $names List of new patch names.
     *
     * @return void
     */
    private static function _sendNewPatchMessage(array $names)
    {
        $config = Patching::getPatchingConfig();
        if ($config['notify'] === FALSE) {
            return;
        }

        Channels::includeSystem('Message');
        $webUrl  = Init::getWebURL(TRUE).'Skins/defaultSkin/ButtonWidget';
        $editUrl = Init::getBaseURL(TRUE).'/'.Init::EDIT_SUFFIX;
        $params  = '?initialMode=SystemConfigScreenMode&initialScreen=PatchingSystemConfigScreenWidget';
        $url     = $editUrl.$params;

        $subject   = 'New MySource Mini update available';
        $mainText  = '<p>A new update is waiting to be installed on your ';
        $mainText .= 'system.</p><p>It is recommended that you install this ';
        $mainText .= 'update in order to receive new features and bug fixes.';
        $mainText .= '</p><p style="color:#666">The patch contains the following changes:</p>';
        foreach ($names as $patchName) {
            $note = '<p style="margin-left:2px">'.nl2br(Patching::getPatchNote($patchName)).'</p>';
            $body = '<table width="100%">
                     <tr><td colspan="2" style="height:22px;font: 16px arial; border-bottom:1px dotted #CCC; padding-bottom:10px;">'.$subject.'</td></tr>
                     <tr>
                       <td style="font: 12px arial;padding-top:10px;padding-right:10px;" valign="top">
                       %recipientName%,
                      <p style="padding-top:15px;margin:0px;">'.$mainText.$note.'</p>
                      <br />
                      <table style="font-size:0px;" border="0" cellspacing="0" cellpadding="0">
                        <tr>
                          <td><img src="'.$webUrl.'/grey_left.png"/></td>
                          <td style="color:#FFF;" background="'.$webUrl.'/grey_mid.png""><a style="font-size:12px;color:#FFF;text-decoration:none;" href="'.$url.'">View & Install Updates</a></td>
                          <td><img src="'.$webUrl.'/grey_right.png"/></td>
                        </tr>
                      </table>
                    </tr>
                    </table>';
            Message::addMessage($config['recipient'], $subject, $body, NULL, array('title' => 'Up to Date Notification'));
            Message::sendMessages();
        }//end foreach

    }//end _sendNewPatchMessage()


    /**
     * Returns the body string of the new patch message to send out.
     *
     * @param string $subject   The subject of the message.
     * @param string $patchName The name of the patch.
     *
     * @return string
     */
    private static function _getNewPatchMessageBody($subject, $patchName)
    {
        $webUrl  = Init::getWebURL(TRUE).'Skins/defaultSkin/ButtonWidget';
        $editUrl = Init::getBaseURL(TRUE).'/'.Init::EDIT_SUFFIX;
        $params  = '?initialMode=SystemConfigScreenMode&initialScreen=PatchingSystemConfigScreenWidget';
        $url     = $editUrl.$params;

        $mainText  = '<p>A new update is waiting to be installed on your ';
        $mainText .= 'system.</p><p>It is recommended that you install this ';
        $mainText .= 'update in order to receive new features and bug fixes.';
        $mainText .= '</p><p style="color:#666">The patch contains the following changes:</p>';

        $note = '<p style="margin-left:2px">'.nl2br(Patching::getPatchNote($patchName)).'</p>';
        $body = '<table width="100%">
                 <tr><td colspan="2" style="height:22px;font: 16px arial; border-bottom:1px dotted #CCC; padding-bottom:10px;">'.$subject.'</td></tr>
                 <tr>
                   <td style="font: 12px arial;padding-top:10px;padding-right:10px;" valign="top">
                   %recipientName%,
                  <p style="padding-top:15px;margin:0px;">'.$mainText.$note.'</p>
                  <br />
                  <table style="font-size:0px;" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td><img src="'.$webUrl.'/grey_left.png"/></td>
                      <td style="color:#FFF;" background="'.$webUrl.'/grey_mid.png""><a style="font-size:12px;color:#FFF;text-decoration:none;" href="'.$url.'">View & Install Updates</a></td>
                      <td><img src="'.$webUrl.'/grey_right.png"/></td>
                    </tr>
                  </table>
                </tr>
                </table>';
        return $body;

    }//end _getNewPatchMessageBody()


    /**
     * Sends "More than <threshold> patches unscheduled" notification email.
     *
     * @return void
     */
    public static function sendWarningMessage()
    {
        $config = Patching::getPatchingConfig();
        if ($config['notify'] === FALSE) {
            return;
        }

        $count   = 0;
        $patches = Patching::getPatchingInfo();
        foreach ($patches as $p) {
            if ($p['scheduled'] === NULL) {
                $count++;
            }
        }

        if ($count > $config['threshold']) {
            $webUrl  = Init::getWebURL(TRUE).'Skins/defaultSkin/ButtonWidget';
            $editUrl = Init::getBaseURL(TRUE).'/'.Init::EDIT_SUFFIX;
            $params  = '?initialMode=SystemConfigScreenMode&initialScreen=PatchingSystemConfigScreenWidget';
            $url     = $editUrl.$params;

            $subject   = 'MySource Mini updates are waiting to be installed';
            $mainText  = '<p>There are '.$count.' updates waiting to be ';
            $mainText .= 'installed on your system.</p><p>It is recommended ';
            $mainText .= 'that you keep your system up-to-date in order to ';
            $mainText .= 'receive new features and bug fixes.</p>';
            $body      = '<table width="100%">
                          <tr>
                            <td colspan="2" style="height:22px;font: 16px arial; border-bottom: 1px dotted #CCC; padding-bottom:10px;">'.$subject.'</td>
                          </tr>
                          <tr>
                            <td style="font: 12px arial;padding-top:10px;padding-right:10px;" valign="top">
                                 %recipientName%,
                                 <p style="padding-top:15px;margin:0px;">'.$mainText.'</p>
                                 <br />
                                 <table style="font-size:0px;" border="0" cellspacing="0" cellpadding="0">
                                 <tr>
                                   <td><img src="'.$webUrl.'/grey_left.png"/></td>
                                   <td style="color:#FFF;" background="'.$webUrl.'/grey_mid.png""><a style="font-size:12px;color:#FFF;text-decoration:none;" href="'.$url.'">View & Install Updates</a></td>
                                   <td><img src="'.$webUrl.'/grey_right.png"/></td>
                                 </tr>
                                 </table>
                            </td>
                          </tr>
                          </table>';

            Channels::includeSystem('Message');
            Message::addMessage($config['recipient'], $subject, $body, NULL, array('title' => 'Up to Date Notification'));
            Message::sendMessages();
        }//end if

    }//end sendWarningMessage()


    /**
     * Sends internal message when error occurs during web service calls.
     *
     * @param string $code      The error code.
     * @param string $patchName Revision number of the failed patch.
     *
     * @return void
     */
    public static function sendInternalErrorMessage($code, $patchName='')
    {
        $config = Patching::getPatchingConfig();
        if ($config['notify'] === FALSE) {
            return;
        }

        $webUrl  = Init::getWebURL(TRUE).'Skins/defaultSkin/ButtonWidget';
        $editUrl = Init::getBaseURL(TRUE).'/'.Init::EDIT_SUFFIX;
        $params  = '?initialMode=SystemConfigScreenMode&initialScreen=PatchingSystemConfigScreenWidget';
        $url     = $editUrl.$params;

        $note = '';
        if ($patchName !== '') {
            $note = '<p style="margin-left:2px">'.nl2br(Patching::getPatchNote($patchName)).'</p>';
        }

        $subject   = 'MySource Mini update failed';
        $mainText  = '<p>There was an error applying a patch to your system.</p>';
        $mainText .= '<p>If this is the first time you have seen this error, ';
        $mainText .= 'please reschedule the update. If the patch continues to ';
        $mainText .= 'fail, please contact Squiz Support.</p>';
        $mainText .= '<p style="color:#666">The failed patch contained the following changes:</p>';
        $mainText .= '<span style="visibility:hidden;display:none">'.$code.'</span>';
        $body      = '<table width="100%">
                      <tr>
                        <td colspan="2" style="height:22px;font: 16px arial; border-bottom: 1px dotted #CCC; padding-bottom:10px;">'.$subject.'</td>
                      </tr>
                      <tr>
                        <td style="font: 12px arial;padding-top:10px;padding-right:10px;" valign="top">
                             %recipientName%,
                             <p style="padding-top:15px;margin:0px;">'.$mainText.$note.'</p>
                             <br />
                             <table style="font-size:0px;" border="0" cellspacing="0" cellpadding="0">
                             <tr>
                               <td><img src="'.$webUrl.'/grey_left.png"/></td>
                               <td style="color:#FFF;" background="'.$webUrl.'/grey_mid.png""><a style="font-size:12px;color:#FFF;text-decoration:none;" href="'.$url.'">View & Install Updates</a></td>
                               <td><img src="'.$webUrl.'/grey_right.png"/></td>
                             </tr>
                             </table>
                        </td>
                      </tr>
                      </table>';

        Channels::includeSystem('Message');
        Message::addMessage($config['recipient'], $subject, $body, NULL, array('title' => 'Up to Date Notification'));
        Message::sendMessages();

    }//end sendInternalErrorMessage()


    /**
     * Post request to the up-to-date server and returns the result in an array.
     *
     * @param string  $type    ActivateInstall.
     * @param string  $xml     The XML with the request params.
     * @param boolean $toArray Flag to turn response into an array.
     *
     * @return array
     * @throws Exception $e Occurs when posting to up-to-date system fails.
     */
    public static function postRequest($type, $xml, $toArray=TRUE)
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return array();
        }

        include_once 'Libs/Web/HTTPRequest.inc';
        self::$_httpRequest = new HTTPRequest($updateServer['connection']['url'].$type.'/');
        self::$_httpRequest->setData($xml);
        try {
            self::$_httpRequest->sendRequest();
        } catch (Exception $e) {
            // We are not sure how to handle the error here. Let's re-throw it for now.
            // Code 404: Not found.
            // Code 500: No post data or malformed xml.
            // Patching::sendInternalErrorMessage('CONNECTION_ERROR').
            return array();
        }

        $response = self::$_httpRequest->getData();
        if ($toArray === TRUE) {
            $response = self::$_httpRequest->getXMLToArray();
        }

        return $response;

    }//end postRequest()


    /**
     * Returns the Update server to user.
     *
     * @return array
     */
    private static function _getUpdateServer()
    {
        if (self::$_updateServerCache !== NULL) {
            return self::$_updateServerCache;
        }

        Channels::includeSystem('SquizSuite');
        $updateServer = SquizSuite::getProductsByType('Squiz Update');
        if (empty($updateServer) === TRUE) {
            return FALSE;
        }

        self::$_updateServerCache = array_pop($updateServer);
        return self::$_updateServerCache;

    }//end _getUpdateServer()


    /**
     * Send API call to the update server and returns the result.
     *
     * @param string  $url         URL to the update server's API.
     * @param string  $serviceType Type of the service to make API call.
     * @param array   $ops         List of options to pass.
     * @param boolean $enc         TRUE if the encryption is needed.
     *
     * @return array
     */
    private static function _wsSendMessage($url, $serviceType, array $ops, $enc=TRUE)
    {
        Channels::includeSystem('SquizSuite');
        $currProduct  = SquizSuite::getProduct();
        $updateServer = self::_getUpdateServer();
        $msg          = array();
        if ($enc === TRUE) {
            $msg['_pubKeySystemid']     = $updateServer['systemid'];
            $msg['_enc']                = array();
            $msg['_enc']['systemid']    = $currProduct['systemid'];
            $msg['_enc']['serviceType'] = $serviceType;
            $msg['_enc']['options']     = json_encode($ops);
        } else {
            $msg['systemid']    = $currProduct['systemid'];
            $msg['serviceType'] = $serviceType;
            $msg['options']     = json_encode($ops);
        }

        $format = 'json';
        $system = 'SquizUpdate';
        $action = 'updateService';
        $result = SquizSuite::sendRequest(
            $updateServer['systemid'],
            $format,
            $system,
            $action,
            $msg
        );

        return $result;

    }//end _wsSendMessage()


    /**
     * Activate this installation on the MySource Update system.
     *
     * This method will be ran once, by the install script on a new system.
     * MySource Update system will activate this system, and return a revision
     * number for this system id.
     *
     * As of rev. 8825+, demo system can be activated by entering a valid
     * systemid provided by Squiz. This id must not be in used by any other
     * system. If system ID is changed using this method, previous system
     * history will be lost. A system can also be de-activated into a demo
     * system by entering 'sales_demo' as system ID.
     *
     * OK                    - activated successfully.
     * ERR_ALREADY_ACTIVATED - error, already activated.
     * Code 500              - unknown systemid.
     *
     * @param string $systemid The System ID provided by Squiz.
     *
     * @return string
     */
    public static function wsActivateInstall($systemid)
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        if ($updateServer['status'] === 'pending') {
            // Never contacted the update server.
            // Or failed to establish the connection.
            $patchingConfig             = Patching::getPatchingConfig();
            $patchingConfig['revision'] = Patching::getCurrentRevision();

            // Set my own systemid with the new one.
            Channels::includeSystem('SquizSuite');
            $currProduct = SquizSuite::getCurrentProduct();
            $oriSysId    = $currProduct['systemid'];
            SquizSuite::updateProduct(
                $currProduct['systemid'],
                'systemid',
                $systemid
            );

            $enc    = FALSE;
            $ops    = array(
                       'systemURL'  => $currProduct['connection']['url'],
                       'systemName' => $currProduct['connection']['name'],
                       'version'    => $patchingConfig['revision'],
                       'cert'       => $currProduct['connection']['cert'],
                      );
            $result = self::_wsSendMessage(
                $updateServer['connection']['url'],
                'ActivateInstall',
                $ops,
                $enc
            );

            // Request failed.
            if ($result['curlInfo']['http_code'] !== 200) {
                return FALSE;
            }

            $result = json_decode($result['result'], TRUE);
            $status = $result['result']['status'];
            switch ($status) {
                case 'OK':
                    // Set the update server to be live.
                    SquizSuite::updateProduct(
                        $updateServer['systemid'],
                        'status',
                        'live'
                    );

                    // Set patching config.
                    $patchingConfig             = Patching::getPatchingConfig();
                    $patchingConfig['systemid'] = $systemid;
                    Channels::includeSystem('SystemConfig');
                    SystemConfig::setConfig('Patching', $patchingConfig);
                    return TRUE;
                break;

                case 'ERR_ALREADY_ACTIVATED':
                default:
                    SquizSuite::updateProduct(
                        $systemid,
                        'systemid',
                        $oriSysId
                    );
                break;
            }//end switch
        }//end if

        return FALSE;

    }//end wsActivateInstall()


    /**
     * Check for the updates.
     *
     * First, it does the interval check. Then, if the updates are
     * available, it downloads them. If the already downloaded updates
     * become no longer valid then it sends out email notifications.
     * Once the patch is downloaded, it only extracts the patch_note out.
     *
     * @return mixed
     */
    public static function wsCheckForUpdates()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $config = Patching::getPatchingConfig();
        if (isset($config['last_checked']) === TRUE) {
            if (isset($config['check_interval']) === TRUE) {
                $nextCheck = ($config['last_checked'] + $config['check_interval']);
                if ($nextCheck > time()) {
                    // No need.
                    return -1;
                }
            }
        }

        $ops    = array('currentRevision' => $config['revision']);
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'CheckForUpdates',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];
        switch ($status) {
            case 'OK_UPDATES_AVAILABLE':
                // Updates are available. Let's download them.
                $updates = Util::getArrayIndex($result['result'], 'updates', array());

                // Get the all downloaded patches, scheduled or not scheduled.
                $patches = Patching::getPatches();

                // Check the existing (downloaded) patches.
                $toDownload   = array();
                $validPatches = array();
                include_once 'Libs/Web/Email.inc';
                foreach ($patches as $patchInfo) {
                    if (array_key_exists($patchInfo['to'], $updates) === FALSE) {
                        // The downloaded patch is no longer valid.
                        // Remove it and notify the user if the patch was already scheduled.
                        if ($patchInfo['scheduled'] !== NULL) {
                            Patching::removePatch($patchInfo['name']);

                            if ($config['notify'] === TRUE) {
                                // TODO: Body needs to be fixed.
                                $subject = 'Invalid Patch has been removed';
                                $body    = $patchInfo['name'].' is no longer valid, so it has been removed.';
                                Email::sendHtmlEmail($config['recipient'], $subject, $body);
                            }
                        }
                    } else {
                        // The downloaded patch's hash has been changed.
                        // Re-download it and notify the user if the patch was already scheduled.
                        if ($updates[$patchInfo['to']]['patch_sha1'] !== $patchInfo['patch_sha1']) {
                            if ($patchInfo['scheduled'] !== NULL) {
                                Patching::schedulePatch($patchInfo['name'], NULL);

                                $toDownload[$patchInfo['to']] = $updates[$patchInfo['to']];
                                if ($config['notify'] === TRUE) {
                                    // TODO: Body needs to be fixed.
                                    $subject = 'The scheduled patch has been re-downloaded';
                                    $body    = $patchInfo['name'].' is no longer valid, so it has been redownloaded and unscheduled.';
                                    Email::sendHtmlEmail($config['recipient'], $subject, $body);
                                }
                            }
                        } else {
                            echo 'hash matched'."\n";
                        }

                        $validPatches[] = $patchInfo['to'];
                    }//end if
                }//end foreach

                // Now any new patches to download?
                $newPatches = array_diff(array_keys($updates), $validPatches);
                foreach ($newPatches as $newPatchRevision) {
                    $toDownload[$newPatchRevision] = $updates[$newPatchRevision];
                }

                foreach ($toDownload as $revision => $info) {
                    $update[$revision] = $info;
                    $status            = Patching::wsDownloadUpdate($update);
                    if ($status === TRUE) {
                        // Extract the patch and register.
                        $patchName = str_replace('.tar.gz', '', $update[$revision]['filename']);
                        self::_extractPatchNote($patchName);
                        Patching::registerPatch(
                            $patchName,
                            $update[$revision]['patch_sha1']
                        );

                        if ($config['notify'] === TRUE) {
                            $subject = 'New MySource Mini update available';
                            $body    = self::_getNewPatchMessageBody($subject, $patchName);
                            Email::sendHtmlEmail($config['recipient'], $subject, $body);
                        }
                    } else {
                        if ($config['notify'] === TRUE) {
                            $subject = 'Downloading the Patch has been FAILED';
                            $body    = $patchInfo['name'].' is no longer valid, so it has been removed.';
                            Email::sendHtmlEmail($config['recipient'], $subject, $body);
                        }
                    }//end if
                }//end foreach

                $status = TRUE;
            break;

            case 'ERR_NO_SUBSCRIPTION':
            case 'ERR_VERSION_MISMATCH':
            default:
                $status = FALSE;
            break;
        }//end switch

        return $status;

    }//end wsCheckForUpdates()


    /**
     * Check the subscription details of the installation.
     *
     * @return array
     */
    public static function wsCheckSubscription()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $config = Patching::getPatchingConfig();
        $ops    = array();
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'CheckSubscription',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];
        switch ($status) {
            case 'OK':
                $config['sub_last_check'] = time();
                $config['sub_start']      = Util::getArrayIndex($result['result'], 'start', 0);
                $config['sub_expiry']     = Util::getArrayIndex($result['result'], 'expiry', 0);
                $config['sub_current']    = Util::getArrayIndex($result['result'], 'current', 0);
                Channels::includeSystem('SystemConfig');
                SystemConfig::setConfig('Patching', $config);
                return $result['result'];
            break;

            case 'ERR_NO_SUBSCRIPTION':
            case 'ERR_SUBSCRIPTION_EXPIRED':
            default:
                // Do nothing.
            break;
        }

        return $status;

    }//end wsCheckSubscription()


    /**
     * Notify the MySource Update system that a patch has been applied.
     *
     * @param string $new The revision of the latest applied patch.
     * @param string $old The revision of the previously installed patch.
     * @param string $msg Optional message to log if there is an error.
     *
     * @return boolean
     */
    public static function wsNotifyApplied($new, $old, $msg='')
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $ops = array(
                'installedTime'   => date('c'),
                'previousVersion' => $old,
                'currentVersion'  => $new,
               );

        if (empty($msg) === TRUE) {
            $ops['status'] = 'OK';
        } else {
            $ops['status']  = 'ERR';
            $ops['message'] = $msg;
        }

        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'NotifyApplied',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        return TRUE;

    }//end wsNotifyApplied()


    /**
     * Update the revision number after a restore from backup process.
     *
     * @return boolean
     */
    public static function wsNotifyRestored()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $config = Patching::getPatchingConfig();
        $ops    = array(
                   'currentVersion' => $config['revision'],
                   'restoredTime'   => date('c'),
                  );
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'NotifyRestored',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        return TRUE;

    }//end wsNotifyRestored()


    /**
     * Download the patch file.
     *
     * Patch (a tar.gz file) will be downloaded into Patching data dir and
     * cron will extract it. Path info includes: update_id, version,
     * patch_notes, available_time, size and sha1.
     *
     * @param array $updates List of updates to download.
     *
     * @return boolean
     */
    public static function wsDownloadUpdate(array $updates)
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        Channels::includeSystem('Patching');
        Channels::includeSystem('SquizSuite');
        $currProduct = SquizSuite::getProduct();
        $config      = Patching::getPatchingConfig();
        $enc         = TRUE;

        $patchingDataPath = BaseSystem::getDataDir('Patching');
        foreach ($updates as $version => $updateInfo) {
            $ops    = array(
                       'version' => $version,
                       'type'    => $currProduct['type'],
                      );
            $result = self::_wsSendMessage(
                $updateServer['connection']['url'],
                'DownloadUpdate',
                $ops,
                $enc
            );

            // Request failed.
            if ($result['curlInfo']['http_code'] !== 200) {
                return FALSE;
            }

            $result = json_decode($result['result'], TRUE);
            if ($result['result']['status'] === 'OK') {
                $filename      = $result['result']['filename'];
                $base64Decoded = base64_decode($result['result']['base64Encoded_patch']);
                $size          = mb_strlen($base64Decoded);
                $hash          = sha1($base64Decoded);
                if ($updateInfo['patch_size'] === $size
                    && $updateInfo['patch_sha1'] === $hash
                ) {
                    // The downloaded patch has been verified.
                    file_put_contents($patchingDataPath.'/'.$filename, $base64Decoded);
                }
            } else {
                return $result['result']['status'];
            }
        }//end foreach

        return TRUE;

    }//end wsDownloadUpdate()


    /**
     * Check to see if this system has premium offsite backup enabled.
     *
     * @return boolean
     */
    public static function wsCheckPremiumSubscription()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        $result = Patching::wsCheckSubscription();
        if ($result === FALSE) {
            return FALSE;
        }

        $services = Util::getArrayIndex($result, 'services', '');
        $services = explode(';', $services);
        if (is_array($services) === TRUE && in_array('Mini Backup', $services) === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end wsCheckPremiumSubscription()


    /**
     * Retrieves the AWS keys for premium offsite connection as an array.
     *
     * @return array
     */
    public static function wsGetAWSKeys()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $ops    = array();
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'GetAWSKeys',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];

        switch ($status) {
            case 'OK':
                return $result['result'];
            break;

            case 'ERROR_NO_PREMIUM':
            default:
                // Do nothing.
            break;
        }

        return array();

    }//end wsGetAWSKeys()


    /**
     * Check to see if this system has premium offsite backup enabled.
     *
     * @return boolean
     */
    public static function wsReleaseStatus()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $ops    = array();
        $enc    = TRUE;
        $msg    = self::_wsMsgTemplate('ReleaseStatus', $ops, $enc);
        $result = self::_wsSendMessage($updateServer['connection']['url'], $msg);

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];

        switch ($status) {
            case 'OK':
                return $result['result']['beta_status'];
            break;

            case 'ERROR_NO_PREMIUM':
            default:
                // Do nothing.
            break;
        }

        return array();

    }//end wsReleaseStatus()


    /**
     * Check to see if this patch is valid before applying.
     *
     * Possible error codes:
     * - ERR_VERSION_INACTIVE
     *     Returned if the version is now marked "inactive". This can be
     *     done to revoke an update temporarily, until it has been applied
     *     by one install.
     * - ERR_VERSION_RE_BETA
     *     Returned only for installs that are set up for general updates only.
     *     This flag denotes the patch has been placed back into beta since
     *     downloaded.
     * - ERR_FILE_MISMATCH
     *     Validation failed because either the file length or SHA1 has
     *     changed. This could be because the patch file has been modified
     *     inside MySource Update since it was downloaded, or the download was
     *     incomplete or corrupted. The patching system MAY re-download the
     *     file now or at the next scheduled update, then run this method again.
     * - ERR_NOT_FOUND
     *     Could not find the update being checked for.
     *
     * @param string $name Name of the patch to check.
     *
     * @return boolean
     */
    public static function wsValidateUpdate($name)
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Load patch info file.
        $dataDir   = BaseSystem::getDataDir('Patching');
        $patchFile = $dataDir.'/'.$name.'.tar.gz';
        if (file_exists($patchFile) === FALSE) {
            return FALSE;
        }

        $size = filesize($patchFile);
        $sha1 = sha1_file($patchFile);

        // Check if need to check for update according to interval setting.
        $ops    = array(
                   'name' => $name,
                   'sha1' => $sha1,
                   'size' => $size,
                  );
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'ValidateUpdate',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];
        switch ($status) {
            case 'OK':
                return TRUE;
            break;

            case 'ERR_VERSION_INACTIVE':
            case 'ERR_VERSION_RE_BETA':
            case 'ERR_FILE_MISMATCH':
            case 'ERR_NOT_FOUND':
            default:
                // Do nothing.
            break;
        }//end switch

        return FALSE;

    }//end wsValidateUpdate()


    /**
     * Post request to the up-to-date server and returns the result in an array.
     *
     * @since  4.0.0
     * @return array
     * @throws Exception $e Bugzilla webservies returns error code.
     */
    public static function wsGetBugzilla()
    {
        $updateServer = self::_getUpdateServer();
        if ($updateServer === FALSE) {
            return 'ERR_NO_UPDATE_SERVER_CONFIGURED';
        }

        // Check if need to check for update according to interval setting.
        $ops    = array();
        $enc    = TRUE;
        $result = self::_wsSendMessage(
            $updateServer['connection']['url'],
            'GetBugzilla',
            $ops,
            $enc
        );

        // Request failed.
        if ($result['curlInfo']['http_code'] !== 200) {
            return FALSE;
        }

        $result = json_decode($result['result'], TRUE);
        $status = $result['result']['status'];

        switch ($status) {
            case 'OK':
                return $result['result'];
            break;

            case 'ERROR_NO_BUGZILLA':
            default:
                // Do nothing.
            break;
        }

        return array();

    }//end wsGetBugzilla()


    /**
     * Returns the JS code to initialise the Roles Screen.
     *
     * @return string
     */
    public static function getUpToDateInitCode()
    {
        $data = array();
        $data = json_encode($data);
        return '<script>PatchingPatchingScreen.initScreen('.$data.');</script>';

    }//end getUpToDateInitCode()


    /**
     * Returns the rows for Update Notification section.
     *
     * @return array
     */
    public static function getUpdateNotificationRows()
    {
        Channels::includeSystem('GUI');
        $rows = array();

        $config      = Patching::getPatchingConfig();
        $recipients  = $config['recipient'];
        $noRecipient = TRUE;
        if (empty($recipients) === FALSE) {
            $noRecipient = FALSE;
        }

        // Notification Toggle button.
        $toggleBtnRow = '<div class="PatchingScreen-activateWrap"><span>'._('Notify of New Updates').'</span>%s</div>';
        $toggleBtn    = GUI::createWidget(
            'PatchingScreen-notifyUpdates',
            'GUIToggleButton',
            array()
        );

        $rows[] = sprintf($toggleBtnRow, $toggleBtn);
        $rows[] = '<div style="background-color: red; padding: 3px;"><strong>TODO: Once the user system is finalised, finish this!</strong></div>';

        // Notification Add New User button.
        $addNewBtnRow = '<div class="PatchingScreen-activateWrap"><span>'._('Notify of New Updates').'</span>%s</div>';
        $addNewBtn    = GUI::createWidget(
            'PatchingScreen-addNewUser',
            'GUIButton',
            array(
             'value' => _('Add Users'),
             'click' => 'PatchingPatchingScreen.addNotificationUser()',
            )
        );

        $rows[] = sprintf($addNewBtnRow, $addNewBtn);

        return $rows;

    }//end getUpdateNotificationRows()


    /**
     * Returns the rows for new updates tale.
     *
     * @return array
     */
    public static function getNewUpdateRows()
    {
        Channels::includeSystem('GUI');
        $rows    = array();
        $patches = Patching::getPatches();

        if (empty($patches) === FALSE) {
            $scheduleMsg  = '<div class="PatchingScreen-scheduleMsg">'._('Schedule installation of updates').'</div>';
            $scheduleMsg .= GUI::createWidget(
                'PatchingScreen-schedulePatch',
                'GUIToggleButton',
                array()
            );

            $scheduleMsg .= self::_getScheduleTimePicker(time());

            $rows[] = array(
                       'patchId' => array(
                                     'colspan'    => 4,
                                     'colContent' => $scheduleMsg,
                                    ),
                      );
        }

        $expander = '<span class="PatchingScreen-expander">&nbsp;</span>';
        foreach ($patches as $patch) {
            $summary  = '<div class="PatchingScreen-summary">';
            $summary .= nl2br(Patching::getPatchNote($patch['name'], TRUE)).$expander.'</div>';
            $summary .= '<div class="PatchingScreen-summary full hidden">';
            $summary .= nl2br(Patching::getPatchNote($patch['name'])).$expander.'</div>';

            if ($patch['scheduled'] === NULL) {
                $scheduled = '<div class="PatchingScreen-scheduled not">'._('Not Sceduled').'</div>';
            } else {
                $scheduled = '<div class="PatchingScreen-scheduled">'._('Sceduled').'</div>';
            }

            $rows[] = array(
                       'patchId'    => $patch['name'],
                       'summary'    => $summary,
                       'scheduledT' => $scheduled,
                      );
        }

        return $rows;

    }//end getNewUpdateRows()


    /**
     * Create a date input field combo for user to schedule a patch run time.
     *
     * Prefills with the closest time, or based on the cron frequency settings.
     * If daily, hour and minute cannot be set.
     *
     * @param integer $time The previously scheduled patch time.
     *
     * @return object
     */
    private static function _getScheduleTimePicker($time)
    {
        Channels::includeSystem('GUI');
        Channels::includeSystem('Cron');
        $content      = '<div class="PatchingScreen-scheduleDatePicker hidden">';
        $cronTypes    = Cron::getCronTypes();
        $cronJob      = Cron::getCronJob('Patching', 'PatchingCron.inc');
        $cronSettings = array();
        foreach ($cronTypes as $type) {
            if ($type['typeid'] === $cronJob['interval']) {
                $cronSettings = $type;
            }
        }

        if ($time === 0) {
            $time = time();
        }

        $today    = getdate();
        $selected = array();

        $selected['day']   = ($today['mday'] - 1);
        $selected['month'] = ($today['mon'] - 1);
        $selected['year']  = $today['year'];
        $selected['hour']  = ($today['hours'] - 1);
        $selected['min']   = ($today['minutes'] - 1);

        $days     = range(1, 31);
        $content .= GUI::createWidget(
            'PatchingScreen-scheduleDay',
            'GUISelect',
            array(
             'options'  => $days,
             'selected' => $selected['day'],
            )
        );

        include_once 'Libs/Util/Util.inc';
        $content .= GUI::createWidget(
            'PatchingScreen-scheduleMonth',
            'GUISelect',
            array(
             'options'  => Util::getMonthsString(),
             'selected' => $selected['month'],
            )
        );

        $years    = range($today['year'], ($today['year'] + 5));
        $yearsOps = array();
        foreach ($years as $year) {
            $yearsOps[$year] = $year;
        }

        $content .= GUI::createWidget(
            'PatchingScreen-scheduleYear',
            'GUISelect',
            array(
             'options'  => $yearsOps,
             'selected' => $selected['year'],
            )
        );

        // Work out what to print based on the cron type.
        $parts = explode(' ', $cronSettings['condition']);
        $min   = $parts[0];
        $hour  = $parts[1];
        if ($cronSettings['typeid'] === 'daily') {
            // Daily, show what time the script will run.
            $content .= $hour.':'.$min.'</div>';
            return $content;
        } else {
            $hours    = range(0, 23);
            $content .= GUI::createWidget(
                'PatchingScreen-scheduleHour',
                'GUISelect',
                array(
                 'options'  => $hours,
                 'selected' => $selected['hour'],
                )
            );

            if ($cronSettings['typeid'] === 'hourly') {
                // Hourly, let user set hour, show minute.
                $content .= ':'.$min;
            } else if ($cronSettings['typeid'] === 'frequent') {
                // Frequently, let user set hour and min.
                $mins     = range(0, 59);
                $content .= ':'.GUI::createWidget(
                    'PatchingScreen-scheduleMin',
                    'GUISelect',
                    array(
                     'options'  => $mins,
                     'selected' => $selected['hour'],
                    )
                );
            }//end if

            $content .= '</div>';
        }//end if

        return $content;

    }//end _getScheduleTimePicker()


    /**
     * Returns the rows for installed updates tale.
     *
     * @return array
     */
    public static function getInstalledUpdateRows()
    {
        $rows = array();
        return $rows;

    }//end getInstalledUpdateRows()


    /**
     * Save the SuperUsers screen settings.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function savePatchingScreen(array $data)
    {
        include_once 'Libs/Util/Util.inc';
        $result = array();

        $activationID = Util::getArrayIndex(
            $data,
            'PatchingScreen-activateIdText',
            ''
        );

        $activationID = trim($activationID);
        if (empty($activationID) === FALSE) {
            $result = Patching::wsActivateInstall($activationID);
            if ($result === FALSE) {
                $result['errors'] = _('Activation Failed.');
            }
        }

        if (isset($result['errors']) === FALSE) {
            $result = array('success' => TRUE);
        }

        return $result;

    }//end savePatchingScreen()


}//end class

?>
