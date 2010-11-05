<?php
/**
 * This script creates a patch tar file between two given revision number.
 *
 * Minimum requirement: revision 8043.
 * Preparation.
 *     1. Edit patch_note.txt.
 *         - Make sure full patch note URL to mini site is correct.
 *     2. Modify upgrade scripts (php, sh and/or sql).
 *         - Copy upgrade scripts out of upgrade/ dir so they get included.
 *         - See 'Upgrade script' section below for more info.
 *     3. Update patch.inc
 *         - Update FROM and TO revision number.
 *         - BLOCK: deprecated, we apply 1 patch at a time since rev. 8285.
 *         - REBOOT: will restart the server if TRUE.
 *     4. Run 'php create_patch.php <FROM_REVISION> <TO_REVISION>'
 *         - Two systems will be rebaked.
 *         - patch_from_to.tar.gz file will be created.
 *     5. Notes
 *         - Do not re-use dir, always regenerate.
 * The command
 *     - php create_patch.php <FROM_REVISION> <TO_REVISION>
 *     - will be using currentDir.'/patch' as root dir.
 *     - will be using 'mysource_patch' as database name.
 * Upgrade script
 * svn diff --summarize -r<FROM_REVISION>:<TO_REVISION> | grep xml
 *     - Plugs and queries.xml can be ignored (handled by Channel and DAL).
 *     - Check schema.xml and update tables in upgrade.sql.
 *     - Check privilege changes, use Role method in upgrade.php.
 *     - If attributes.xml is modified, handle it (see 8043)
 * svn diff --summarize -r<FROM_REVISION>:<TO_REVISION> | grep sql
 *     - Find out what stored DB procedures have been changed, reload with.
 *       system('psql -U postgres mysource < '.Init::ROOT_DIR.'/'.$file);
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
 * @subpackage Scripts
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

if ($argc !== 4) {
    echo "Usage: php create_patch.php <PRODUCT_TYPE> <FROM_REV>:<FROM_FRAMEWORK_REV> <TO_REV>:<TO_FRAMEWORK_REV>\n";
    exit;
} else {
    $productTypes = array(
                     'cms',
                     'analytics',
                     'search',
                     'update',
                     'ci'
                    );

    $productT = strtolower(trim($argv[1]));
    if (in_array($productT, $productTypes) === FALSE) {
        echo "Invalid product type has been entered.\n";
        exit;
    }

    $from = $argv[2];
    $to   = $argv[3];

    $parts   = explode(':', $from);
    $from    = $parts[0];
    $fromFrm = $parts[1];

    $parts = explode(':', $to);
    $to    = $parts[0];
    $toFrm = $parts[1];

    rebakeSystem($to, $toFrm, 'TO');
    rebakeSystem($from, $fromFrm, 'FROM');

    // Generate the patch diff file.
    // This bit works out what files have been added/deleted.
    echo "Generating diff and script\t\t\t\t";

    // Diff excluding the data dir.
    $script  = "#!/bin/bash\n\n";
    $script .= generateAddScript($to, $from);
    $script .= generateDelScript($to, $from);
    exec("diff -aur -x data $from $to > patch.diff");

    if ($productT === 'cms') {
        // Diff the asset type wizard xml files in the data directory.
        $script .= generateWizardScript($to, $from);
        exec("diff -aur $from/data/Wizard/Wizards $to/data/Wizard/Wizards >> patch.diff");
    }

    // Replace all root dir in patch.diff to '/var/www/mysource4/htdocs'
    // Otherwise full path in patch file will cause patch rejection.
    $realPath     = '/var/www/mysource4/htdocs';
    $patchPath    = dirname(__FILE__).'/patch';
    $realPathEsc  = str_replace('/', '\/', $realPath);
    $patchPathEsc = str_replace('/', '\/', $patchPath);
    exec("sed 's/$patchPathEsc/$realPathEsc/g' patch.diff > patch.tmp");
    exec('mv patch.tmp patch.diff');

    file_put_contents('patch.sh', $script);
    exec("chmod 777 patch.sh && chmod 777 patch.diff");
    echoDone();

    // Generate the patch tar.
    echo "Generating tar.gz file\t\t\t\t\t";
    $upgradeScript = '';
    $upgradeFiles  = glob('upgrade.*');
    if (empty($upgradeFiles) === FALSE) {
        $upgradeScript = 'upgrade.*';
    }

    $preUpgradeFiles = glob('preupgrade.*');
    if (empty($preUpgradeFiles) === FALSE) {
        $upgradeScript .= ' preupgrade.*';
    }

    $patchName = 'patch_'.$from.'_'.$to.'.tar.gz';
    exec("tar -czf $patchName patch.diff patch.sh patch.inc $upgradeScript patch_note*");
    echoDone();

    // Print a summary.
    echo "Files included in $patchName:\n";
    $output = '';
    exec("gunzip -c $patchName | tar -tvf -", $output);
    $output = implode("\n\t", $output);
    echo "\t$output\n";

}//end if


/**
 * Checks out a rebake a particular system.
 *
 * @param integer $revision The revision of the system.
 *
 * @return void
 */
function rebakeSystem($revision, $frameRev, $systemType='')
{
    // Get the clean code first from subversion repository.
    // Since the time to exporting is long, we will cache the
    // file and use it by copying it.
    // Also, the way it gets the repository should be dynamic as
    // we now have multiple product repositories.
    $command = 'svn info '.dirname(__FILE__).'/../../ | grep "URL:"';
    $isCMS   = FALSE;
    $output  = array();
    exec($command, $output);
    if (count($output) === 1) {
        $output = $output[0];
        if (strpos($output, 'URL: ') === 0) {
            $output    = str_replace('URL: ', '', $output);
            $sourceURL = trim($output);
        }
    } else {
        $isCMS     = TRUE;
        $sourceURL = 'https://cvs.squiz.net/svn/mysource/trunk';
    }

    $basename = basename($sourceURL);
    $command  = 'rm -rf '.$revision.' patch '.$basename;
    exec($command);

    exportVersion($revision, $frameRev, $sourceURL);

    echo "Copying from the exported revision $revision_export\n";
    $command = 'cp -R '.$revision.'_export '.$basename;
    exec($command);

    // These files must be removed and it is also used in install.inc
    // for all installations.
    include 'patch.inc';
    foreach ($paths_to_remove as $path) {
        exec("rm -rf $basename$path");
    }

    // Okay, now this is patch specific remove file list to generate
    // patch.diff file differently.
    // Do we have any files to be removed before baking?
    if ($systemType !== '' && $systemType === 'FROM') {
        foreach ($paths_to_remove_before_baking_FROM_system as $path) {
            $filePath = $basename.$path;
            if (file_exists($filePath) === TRUE) {
                unlink($filePath);
            }
        }
    } else if ($systemType !== '' && $systemType === 'TO') {
        foreach ($paths_to_remove_before_baking_TO_system as $path) {
            $filePath = $basename.$path;
            if (file_exists($filePath) === TRUE) {
                unlink($filePath);
            }
        }
    }

    $script = 'mv '.$basename.' patch && ';
    if ($isCMS === TRUE) {
        $script .= 'mkdir patch/web && ';
    }

    $script .= 'chmod 777 -R patch && ';

    exec('psql -U postgres -l | grep mysource | wc -l', $dbExist);
    if ($dbExist[0] !== '0') {
        $script .= 'dropdb -U postgres mysource && ';
    }

    $script .= 'createdb -U postgres mysource && ';
    $script .= 'createlang -U postgres plpgsql mysource';
    exec($script);

    // Change the root dir and db name.
    echo "\tChanging root dir and db name\n";
    $realPath     = '/var/www/mysource4/htdocs';
    $patchPath    = dirname(__FILE__).'/patch';
    $realPathEsc  = str_replace('/', '\/', $realPath);
    $patchPathEsc = str_replace('/', '\/', $patchPath);
    $command      = "find patch | xargs grep -l '$realPath' | xargs sed -i -e 's/$realPathEsc/$patchPathEsc/g'";
    echo $command."\n";
    exec($command);

    // Clean and rebake.
    echo "\tBaking revision $revision\n";
    $script  = 'cd patch && ';

    if ($isCMS === TRUE) {
        $script .= 'php clean.php --patch && ';
    } else {
        $script .= 'php Scripts/clean.php && ';
    }

    $script .= "cd ../ && mv patch $revision";
    echo $script."\n";
    exec($script);

    // Do we have any files to be removed before baking?
    if ($systemType !== '' && $systemType === 'FROM') {
        foreach ($paths_to_remove_after_baking_FROM_system as $path) {
            $filePath = $revision.$path;
            if (file_exists($filePath) === TRUE) {
                $res = unlink($filePath);
            }
        }
    } else if ($systemType !== '' && $systemType === 'TO') {
        foreach ($paths_to_remove_after_baking_TO_system as $path) {
            $filePath = $revision.$path;
            if (file_exists($filePath) === TRUE) {
                unlink($filePath);
            }
        }
    }

    echoDone();
    echo "\n\n";
}//end rebakeSystem()


/**
 * Export the system with the revision.
 *
 * Extra work is required since each product's svn:externals does not
 * specify which revision to use resulting in the latest head to be
 * exported each time.
 *
 * Instead we need to get the correct version of framework systems.
 * Manually fetch externals directory and copy over them.
 */
function exportVersion($revision, $frameRev, $sourceURL)
{
    // First export the product revision first.
    echo "Exporting revision $revision\n";
    if (file_exists($revision.'_export') === FALSE) {
        $basename = basename($sourceURL);
        $command  = 'svn export -r '.$revision.' '.$sourceURL.' '.$revision.'_export';
        exec($command);
    }

    // Now get all the externals list from 3 directories.
    // Export the right framework folders into _framework directory.
    $externalDirs = array('', '/Systems', '/Libs');
    foreach ($externalDirs as $dir) {
        $externalCachePath  = $revision.'_externals';
        $externalCachePath .= str_replace('/', '_', $dir).'.txt';
        if (file_exists($externalCachePath) === FALSE) {
            $output  = array();
            $command = 'svn propget svn:externals '.$sourceURL.$dir.'@'.$revision;
            exec($command, $output);

            $externalTxt = trim(implode("\n", $output));
            file_put_contents($externalCachePath, $externalTxt);
        } else {
            $externalTxt = file_get_contents($externalCachePath);
        }

        if (file_exists($revision.'_framework') === FALSE) {
            mkdir($revision.'_framework');
        }

        if (file_exists($revision.'_framework'.$dir) === FALSE) {
            mkdir($revision.'_framework'.$dir);
        }

        $lines = explode("\n", $externalTxt);
        foreach ($lines as $line) {
            if (trim($line) === '') {
                continue;
            }

            $parts    = explode(' ', $line);
            $destPath = "${revision}_framework$dir/${parts[1]}";
            if (file_exists($destPath) === FALSE) {
                $externalURL = $parts[0];
                echo "    Getting $externalURL@$frameRev\n";
                $command  = "svn export $externalURL@$frameRev  ";
                $command .= $destPath;
                exec($command);
            }

            // Copy over the external directory with the one from
            // the framework export directory.
            $command = "rm -rf ${revision}_export$dir/${parts[1]} && cp -R ${revision}_framework$dir/${parts[1]} ${revision}_export$dir/${parts[1]}";
            exec($command);
        }//end foreach
    }//end foreach

}//end exportVersion()


/**
 * Finds new files added in the newer revision and update patch.sh.
 *
 * Generate script to create new files/directories added in the new version.
 *
 * @param integer $new The TO revision.
 * @param integer $old The FROM revision.
 *
 * @return string
 */
function generateAddScript($new, $old)
{
    $result  = TRUE;
    $command = array();
    // Loop until we have no more file/dir to add.
    while ($result) {
        $result = FALSE;
        exec("diff -x data -rq  $old $new | grep 'Only in $new'  | cut -d ' ' -f3,4", $result);
        foreach ($result as $filepath) {
            $path = explode(': ', $filepath);
            $path = implode('/', $path);
            if (is_dir($path) === TRUE) {
                $newDir = str_replace($new, $old, $path);
                mkdir($newDir);

                $newDir    = str_replace("$new/", '', $path);
                $command[] = "mkdir $newDir";
            } else if (is_file($path) === TRUE) {
                $newFile = str_replace($new, $old, $path);
                touch($newFile);

                $newFile   = str_replace("$new/", '', $path);
                $command[] = "touch $newFile";
            }
        }
    }//end while

    sort($command, SORT_STRING);

    $script  = "# Create new directories and files.\n";
    $script .= implode("\n", $command)."\n\n";

    return $script;

}//end generateAddScript()


/**
 * Finds deleted files in the newer revision and update patch.sh.
 *
 * Generate script to delete files/directories removed in the new version.
 *
 * @param integer $new The TO revision.
 * @param integer $old The FROM revision.
 *
 * @return string
 */
function generateDelScript($new, $old)
{
    $result  = TRUE;
    $command = array();
    exec("diff -x data -rq $old $new | grep 'Only in $old' | cut -d ' ' -f3,4", $result);
    // Not recursive, because files will be gone when the dir is deleted.
    foreach ($result as $filepath) {
        $path = explode(': ', $filepath);
        $path = implode('/', $path);
        exec("rm -rf $path");
        $path      = str_replace("$old/", '', $path);
        $command[] = "rm -rf $path";
    }

    sort($command, SORT_STRING);
    $script  = "# Delete old directories and files.\n";
    $script .= implode("\n", $command)."\n\n";

    return $script;

}//end generateDelScript()


/**
 * Handles the special wizard dir in the data dir that we skipped.
 *
 * @param integer $new The TO revision.
 * @param integer $old The FROM revision.
 *
 * @return string
 */
function generateWizardScript($new, $old)
{
    // If any wizard xml has been added/deleted between old and new version.
    // In the data/Wizard/Wizards directory.
    $oldXml = array();
    $newXml = array();
    foreach (glob($old.'/data/Wizard/Wizards/*.xml') as $oXml) {
        $oldXml[] = basename($oXml);
    }

    foreach (glob($new.'/data/Wizard/Wizards/*.xml') as $nXml) {
        $newXml[] = basename($nXml);
    }

    $added   = array_diff($newXml, $oldXml);
    $deleted = array_diff($oldXml, $newXml);
    $command = array();
    foreach ($added as $a) {
        $command[] = 'touch data/Wizard/Wizards/'.$a;
        touch($old.'/data/Wizard/Wizards/'.$a);
    }

    foreach ($deleted as $d) {
        $command[] = 'rm -rf data/Wizard/Wizards/'.$d;
        exec('rm -rf '.$old.'/data/Wizard/Wizards'.$d);
    }

    sort($command, SORT_STRING);
    $script  = "# Updating wizard xml files.\n";
    $script .= implode("\n", $command)."\n\n";

    return $script;

}//end generateWizardScript()


/**
 * Prints a nice green [ Done ] msg.
 *
 * @since  4.0.0
 * @return void
 */
function echoDone()
{
    echo exec('echo -n "[ " ; echo -en "\033[0;32mDone" ; tput sgr0 ; echo " ]"')."\n";

}//end echoDone()


?>