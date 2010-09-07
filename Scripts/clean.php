<?php
/**
 * Squiz Framework script to resinstall a product.
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
    echo "This script must be run from the command line.\n";
    exit();
}

$rootdir = dirname(dirname(__FILE__));
chdir($rootdir);

echo '1. Cleaning the data directory';
system('rm -rf '.$rootdir.'/data/* >> /dev/null 2>&1');
echoDone();

echo '2. Cleaning the web directory';
system('rm -rf '.$rootdir.'/web/* >> /dev/null 2>&1');
echoDone();

echo '3. Cleaning DAL QueryStore and Oven directories';
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
echoDone();

echo '4. Cleaning Channels Oven directory';
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '5. Cleaning ._ files from OS X';
system('find '.$rootdir.' -name "._*" -exec rm {} \; >> /dev/null 2>&1');
echoDone();

echo '6. Fixing file system permissions';
system($rootdir.'/Scripts/fix_perms.sh');
echoDone();

echo '7. Re-creating database';
require_once 'DAL/DALConf.inc';

$dbName     = $conf['DSN'];
$dbName     = substr($dbName, (strpos($dbName, 'dbname=') + 7));
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
echo '9. Cleaning data & oven directories';
system('rm -rf ./data/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '10. Preparing data directories';
Install::prepareDataDir($rootdir, $url);
echoDone();

echo '11. Rebaking oven';
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

echo '15. Initialising product';
$initProductScript = $rootdir.'/initialise_product.php';
if (file_exists($initProductScript) === TRUE) {
    include_once $initProductScript;
    echoDone();
} else {
    echoSkip();
}

echo '16. Generating help docs';
if (file_exists($rootdir.'/Systems/Help/Scripts/generate_docs.php') === TRUE) {
    system('/usr/bin/php '.$rootdir.'/Systems/Help/Scripts/generate_docs.php >> /dev/null 2>&1');
    echoDone();
} else {
    echoSkip();
}

echo '17. Copying web files';
Install::copyWebFiles();
echoDone();

require_once $rootdir.'/data/init.inc';
require_once $rootdir.'/Channels/Channels.inc';
require_once $rootdir.'/Systems/BaseSystem.inc';

echo '18. Fixing file system permissions';
system($rootdir.'/Scripts/fix_perms.sh');
system('chmod 777 '.$rootdir.'/error_log');
echoDone();


/**
 * Returns an array of system names passed from the rebake script.
 *
 * If the system has nested widget system, it will be worked out.
 * (e.g. LocalCache, CacheAdminScreenWidget).
 *
 * @param string $rootDir The root path.
 * @param string $system  A space-seperated list of systems.
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


/**
 * Echo skip function.
 *
 * @return void
 */
function echoSkip()
{
    echo exec('echo -en "\033[54G"; echo -n "[ " ; echo -en "\033[0;33mSkip" ; tput sgr0 ; echo " ]"')."\n";

}//end echoSkip()


?>
