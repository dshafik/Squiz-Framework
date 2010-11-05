<?php
/**
 * Cron script for Patching, check new patch downloaded and apply.
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

require_once dirname(dirname(dirname(__FILE__))).'/Cron/Scripts/CronScript.inc';

/**
 * PatchingCron Class.
 */
class PatchingCron implements CronScript
{


    /**
     * Runs the patch cron script.
     *
     * @return void
     */
    public function run()
    {
        // Include everything we will potentially run here.
        // To make sure that old code is executed for this instance.
        include_once 'Libs/FileSystem/FileSystem.inc';
        Channels::includeSystem('Patching');
        Channels::includeSystem('SystemConfig');

        if (Channels::systemExists('Log') === TRUE) {
            Channels::includeSystem('Log');
        }

        if (Channels::systemExists('SystemConfigSplashScreenWidget') === TRUE) {
            Channels::includeSystem('SystemConfigSplashScreenWidget');
        }

        // Let it get the up-to-date Bugzilla login info and update the file.
        if (Channels::systemExists('Bugzilla') === TRUE) {
            Channels::includeSystem('Bugzilla');
            Bugzilla::writeBugzillaLoginInfo();
        }

        // Find the first scheduled patch and apply it.
        $config  = Patching::getPatchingConfig();
        $patches = Patching::getPendingPatches();
        $info    = Patching::getPatchingInfo();
        $oldRev  = $config['revision'];
        $reboot  = FALSE;
        if (empty($patches) === FALSE) {
            $name   = array_shift($patches);
            $parts  = explode('_', $name);
            $newRev = $parts[2];
            $status = self::applyPatch($name);
            if ($status === TRUE) {
                // Apply ok, archive it and notify central server.
                Patching::archivePatch($name);
                Patching::wsNotifyApplied($newRev, $oldRev);
                foreach ($info as $i) {
                    if ($i['name'] === $name) {
                        if (isset($i['reboot']) === TRUE) {
                            if ($i['reboot'] === TRUE) {
                                // Need to issue a system reboot command.
                                $reboot = TRUE;
                                break;
                            }
                        }
                    }
                }

                // Update the patching config.
                $config['revision']     = $newRev;
                $config['last_updated'] = time();
            } else {
                // Apply fail, unschedule all pending patches.
                $pendingPatches = Patching::getPendingPatches();
                foreach ($pendingPatches as $p) {
                    Patching::schedulePatch($p, NULL);
                }

                // Send error email and notify central server.
                Patching::sendInternalErrorMessage('ERR_PATCH_FAILED', $name);
                Patching::wsNotifyApplied($newRev, $oldRev, 'Failed to apply '.$name);
            }//end if
        } else {
            // Ask the central server for new updates, if any.
            $now = time();
            if ($now > ($config['last_checked'] + $config['check_interval'])) {
                Patching::wsCheckForUpdates();
                $config['last_checked'] = time();
                SystemConfig::setConfig('Patching', $config);
            }
        }//end if

        // No more pending patch, turn off the schedule toggle.
        $pendingPatches = Patching::getPendingPatches();
        if (empty($pendingPatches) === TRUE) {
            $config['schedule'] = FALSE;
        }

        SystemConfig::setConfig('Patching', $config);

        // Tells debian where to get operating system updates from.
        Channels::includeSystem('SquizSuite');
        $currProduct = SquizSuite::getProduct();
        $sourcesList = $currProduct['connection']['sourcesList'];
        $status      = Patching::wsReleaseStatus();

        if ($status === 'GA') {
            @FileSystem::filePutContents($sourcesList, 'deb http://aptrepo-stable.squiz.net/ stable main');
        } else {
            @FileSystem::filePutContents($sourcesList, 'deb http://aptrepo-beta.squiz.net/ stable main');
        }

        // Issues a system reboot command if required.
        if ($reboot === TRUE) {
            exec('/sbin/reboot');
        }

    }//end run()


    /**
     * Applies a patch, and runs all included upgrade script.
     *
     * @param string $name Name of the patch to apply.
     *
     * @return boolean|string
     */
    public function applyPatch($name)
    {
        // Check if this patch is valid first, before applying.
        Channels::includeSystem('Patching');
        $valid = Patching::wsValidateUpdate($name);
        if ($valid === FALSE) {
            return FALSE;
        }

        $dataDir  = BaseSystem::getDataDir('Patching');
        $patchDir = $dataDir.'/'.$name;
        if (is_dir($patchDir) === FALSE) {
            return FALSE;
        }

        $rootDir = dirname(dirname(dirname(dirname(__FILE__))));

        $patchResultPath = $patchDir.'/patch.result';
        file_put_contents($patchResultPath, '[Applying Patch Begins]'."\n", FILE_APPEND);
        file_put_contents($patchResultPath, '[Start running Pre upgrade scripts]'."\n", FILE_APPEND);
        self::runUpgradeScripts('pre', $rootDir, $patchDir);

        // Add new files and delete old files.
        file_put_contents($patchResultPath, '[Start running patch.sh]'."\n", FILE_APPEND);
        if (file_exists($patchDir.'/patch.sh') === TRUE) {
            $script  = 'cd '.$rootDir.' && ';
            $script .= 'cp '.$patchDir.'/patch.sh . && ';
            $script .= 'sh patch.sh >> '.$patchResultPath.' && ';
            $script .= 'rm patch.sh';
            exec($script);
        }

        // Applies the patch.
        if (file_exists($patchDir.'/patch.diff') === TRUE) {
            // Do a dry run to check for error.
            file_put_contents($patchResultPath, '[Start running dry run]'."\n", FILE_APPEND);
            $dryRun = 'patch --dry-run -d '.$rootDir.' -p1 -i '.$patchResultPath;
            $output = array();
            $retval = 0;
            exec($dryRun, $output, $retval);
            if ($retval !== 0) {
                file_put_contents($patchResultPath, '[Patch file dry run failed]'."\n", FILE_APPEND);
                // Patch's exit status is 0 if all hunks are applied
                // successfully, 1 if some hunks cannot be applied, and 2 if
                // there is more serious trouble.
                $error = implode("\n", $output);
                file_put_contents($patchResultPath, $error."\n", FILE_APPEND);
                return $error;
            } else {
                file_put_contents($patchResultPath, '[Start running patch command with patch.diff]'."\n", FILE_APPEND);
                $script = 'patch -d '.$rootDir.' -p1 -i '.$patchDir.'/patch.diff >> '.$patchResultPath;
                exec($script);
            }
        }//end if

        file_put_contents($patchResultPath, '[Start running Post upgrade scripts]'."\n", FILE_APPEND);
        self::runUpgradeScripts('post', $rootDir, $patchDir);

        // Remove cached css files in the web dir.
        file_put_contents($patchResultPath, '[Removing cached CSS files in Web directory]'."\n", FILE_APPEND);
        $cachedCss = glob($rootDir.'/web/Skins/defaultSkin/*.css');
        foreach ($cachedCss as $file) {
            unlink($file);
        }

        // Re-generate Help documentation.
        file_put_contents($patchResultPath, '[Re-generate Help documentation]'."\n", FILE_APPEND);        
        $helpScriptCommand = 'php '.$rootDir.'/Systems/Help/Scripts/generate_docs.php';        
        exec($helpScriptCommand);

        // Update the *_create_wizard.xml file for asset type wizards.
        if (Channels::systemExists('Wizard') === TRUE) {
            file_put_contents($patchResultPath, '[Remove *_create_wizard.xml files.]'."\n", FILE_APPEND);
            Channels::includeSystem('Wizard');
            $wPath = Wizard::getWizardsPath();
            foreach (glob($wPath.'/*php') as $phpWizardFile) {
                unlink($phpWizardFile);
            }
        }

        // Clear Firefox cache.
        file_put_contents($patchResultPath, '[Clean up firefox cache.]'."\n", FILE_APPEND);
        exec('rm /root/.mozilla/firefox/squiz/Cache/*');

        file_put_contents($patchResultPath, '[applyPatch() function returning TRUE]'."\n", FILE_APPEND);
        return TRUE;

    }//end applyPatch()


    /**
     * Runs the post patch upgrade sql, php and sh scripts.
     *
     * @param string $type     Upgrade type, 'pre' or 'post'.
     *                         pre: preupgrade.sh, .sql then .php.
     *                         post: upgrade.sh, .sql then upgrade.php.
     * @param string $rootDir  Root directory path.
     * @param string $patchDir Patch data directory path.
     *
     * @return void
     */
    public function runUpgradeScripts($type, $rootDir, $patchDir)
    {
        $name = 'upgrade';
        if ($type === 'pre') {
            $name = 'preupgrade';
        }

        // Runs the (pre)upgrade.sh script.
        if (file_exists($patchDir.'/'.$name.'.sh') === TRUE) {
            $script  = 'cd '.$rootDir.' && ';
            $script .= 'cp '.$patchDir.'/'.$name.'.sh . && ';
            $script .= 'sh '.$name.'.sh '.$patchDir.' >> '.$patchDir.'/patch.result 2> '.$patchDir.'/patch.result && ';
            $script .= 'rm '.$name.'.sh';
            exec($script);
        }

        // Reconnect DB after server upgrade has been done.
        DAL::dbConnect();

        // Runs the upgrade.sql script.
        if (file_exists($patchDir.'/'.$name.'.sql') === TRUE) {
            $dbDsn    = DAL::getDsn();
            $dbName   = DAL::getDbName();
            $dsnParts = explode(';', $dbDsn['DSN']);
            $host     = str_replace('host=', '', $dsnParts[1]);
            $pgpass   = $host.':5432:*:'.$dbDsn['user'];
            if (empty($host) === FALSE) {
                $host = ' -h '.$host;
            }

            // Creates a pgpass file if it requires password to connect.
            $script = '';
            if ($dbDsn['password'] !== '') {
                $pgpass .= ':'.$dbDsn['password'];
                $script .= 'cd '.$rootDir.' && ';
                $script .= 'echo '.$pgpass." > ./.pgpass &&\n";
                $script .= "chmod 600 ./.pgpass &&\n";
                $script .= 'export PGPASSFILE='.$rootDir."/.pgpass;\n";
            }

            // Runs script.
            $script .= 'psql'.$host.' -U '.$dbDsn['user'].' '.$dbName;
            $script .= ' < '.$patchDir.'/'.$name.'.sql';
            $script .= ' >> '.$patchDir."/patch.result;\n";
            exec($script);

            // Removes the pgpass file.
            if ($dbDsn['password'] !== '') {
                $script  = 'cd '.$rootDir.' && ';
                $script .= "rm ./.pgpass\n";
                exec($script);
            }
        }//end if

        // Runs the upgrade.php script.
        if (file_exists($patchDir.'/'.$name.'.php') === TRUE) {
            $script  = 'cd '.$rootDir.' && ';
            $script .= 'cp '.$patchDir.'/'.$name.'.php . && ';
            $script .= 'php '.$name.'.php '.$patchDir.' >> '.$patchDir.'/patch.result 2> '.$patchDir.'/patch.result && ';
            $script .= 'rm '.$name.'.php';
            exec($script);
        }

    }//end runUpgradeScripts()


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
