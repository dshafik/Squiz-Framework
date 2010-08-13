<?php
// @codingStandardsIgnoreStart
/**
 * MySource 4 clean.php.
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
 * @subpackage Scripts
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

if ((php_sapi_name() !== 'cli')) {
    echo "This script can only be run from the command line.\n";
    exit();
}

if (isset($argv[1]) === FALSE) {
    echo "Please specify the root directory.\n";
    exit();
}

$rootdir = $argv[1];
if (is_dir($rootdir) === FALSE) {
    echo "The specified root directory does not exist.\n";
    exit();
}

$rootdir = realpath($rootdir);

// Change to mysource mini root directory.
chdir($rootdir);

echo '1. Removing files from the data directory';
system('rm -rf '.$rootdir.'/data/* >> /dev/null 2>&1');
echoDone();

echo '2. Cleaning out the web directory';
system('rm -rf '.$rootdir.'/web/* >> /dev/null 2>&1');
echoDone();

echo '3. Cleaning DAL QueryStore and Oven directories';
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
echoDone();

echo '4. Cleaning Channels Oven directory';
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '5. Clean up ._ files from OS X';
system('find '.$rootdir.' -name "._*" -exec rm {} \; >> /dev/null 2>&1');
echoDone();

echo '6. Fix file system permissions for broken systems';
system($rootdir.'/Scripts/fix_perms.sh');
echoDone();

echo '7. Re-creating database';
include_once 'DAL/DALConf.inc';

$dbName     = $conf['DSN'];
$dbName     = substr($dbName, strpos($dbName, 'dbname=') + 7);
$dbUserName = $conf['user'];
system('dropdb -U postgres '.$dbName);
system('createdb -U postgres '.$dbName.' -O '.$dbUserName.' -E SQL_ASCII');
echoDone();

echo '8. Granting database privileges';
file_put_contents('./grant_privileges.db', 'GRANT ALL PRIVILEGES ON DATABASE '.$dbName.' to '.$dbUserName.';');
system('createlang -U postgres plpgsql '.$dbName.' >> /dev/null 2>&1');
system('psql -U postgres < ./grant_privileges.db >> /dev/null 2>&1');
system('rm grant_privileges.db');
echoDone();

$url = 'http://framework.labs.squiz.net';

require_once $rootdir.'/Libs/Install/Install.inc';
require_once $rootdir.'/Channels/ChannelsBaker.inc';
require_once $rootdir.'/DAL/DALBaker.inc';
require_once $rootdir.'/Libs/FileSystem/FileSystem.inc';

// Work out what systems to install/reinstall.
$systemNames = Install::getInstallOptions(FALSE, FALSE);

// Clean up data and oven dir before full install.
echo '9. Clean data & oven dir';
system('rm -rf ./data/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '10. Preparing data directories';
Install::prepareDataDir($rootdir, $url);
echoDone();

echo '11. Rebaking Oven';
Install::rebakeOven($rootdir, $url);
echoDone();

echo '12. Installing queries';
Install::installQueries($systemNames);
echoDone();

echo '13. Installing SQL functions';
Install::installSqlFunctions($systemNames);
echoDone();

echo '14. Running system install methods';
Install::runSystemInstallMethods($systemNames);
echoDone();

echo '15. Copying web files';
Install::copyWebFiles();
echoDone();

require_once $rootdir.'/data/init.inc';
require_once $rootdir.'/Channels/Channels.inc';
require_once $rootdir.'/Systems/BaseSystem.inc';

echo '16. Give error_log 777 permission for developers';
if (file_exists($rootdir.'/error_log') === FALSE) {
    file_put_contents($rootdir.'/error_log', '');
}//end if

system('chmod 777 '.$rootdir.'/error_log');
echoDone();

echo '17. Fix file system permissions';
system($rootdir.'/Scripts/fix_perms.sh');
echoDone();

/**
 * Returns an array of system names passed from the rebake script.
 *
 * If the system has nested widget system, it will be worked out.
 * (e.g. LocalCache, CacheAdminScreenWidget).
 *
 * @param string $rootDir The root path.
 *
 * @return array
 */
function getSelectedSystems($rootDir, $system)
{
    $systemNames   = array();
    $nestedSystems = array();

    $selectedSys = explode(' ', $system);
    $realSys     = Install::getRealSystems();
    foreach ($selectedSys as $sys) {
        if (isset($realSys[$sys]) === TRUE) {
            $systemNames[$sys] = $realSys[$sys];

            // This part gets the nested widget systems.
            $nested = Install::getRealSystems($rootDir.'/Systems/'.$sys);
            if (empty($nested) === FALSE) {
                $nestedSystems[] = $nested;
            }
        }
    }

    foreach ($nestedSystems as $nested) {
        $systemNames = array_merge($systemNames, $nested);
    }

    return $systemNames;

}//end getSelectedSystems()


/**
 * Echo done function.
 *
 * @return void
 */
function echoDone()
{
    echo exec('echo -en "\033[54G"; echo -n "[ " ; echo -en "\033[0;32mDone" ; tput sgr0 ; echo " ]"')."\n";

}//end echoDone()

// @codingStandardsIgnoreEnd
?>
