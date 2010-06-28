<?php
// @codingStandardsIgnoreStart
/**
 * MySource 4 clean.php.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage MySource4
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

if ((php_sapi_name() !== 'cli')) {
    echo "This script can only be run from the command line.\n";
    exit();
}

$rootdir = dirname(__FILE__);

// Change to mysource mini root directory.
chdir($rootdir);

echo '1. Removing files from the data directory           ';
system('rm -rf '.$rootdir.'/data/* >> /dev/null 2>&1');
echoDone();

echo '2. Cleaning out the web directory                   ';
system('rm -rf '.$rootdir.'/web/* >> /dev/null 2>&1');
echoDone();

echo '3. Cleaning DAL QueryStore and Oven directories     ';
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
echoDone();

echo '4. Cleaning Channels Oven directory                 ';
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '5. Clean up ._ files from OS X                      ';
system('find '.$rootdir.' -name "._*" -exec rm {} \; >> /dev/null 2>&1');
echoDone();

echo '6. Fix file system permissions for broken systems   ';
system('chown -R root:root '.$rootdir.'/..');
system('chmod -R 755 '.$rootdir.'/..');
echoDone();

echo '7. Re-creating database                             ';
include_once 'DAL/DALConf.inc';

$dbName     = $conf['DSN'];
$dbName     = substr($dbName, strpos($dbName, 'dbname=') + 7); 
$dbUserName = $conf['user'];
system('dropdb -U postgres '.$dbName);
system('createdb -U postgres '.$dbName.' -O '.$dbUserName.' -E SQL_ASCII');
echoDone();

echo '8. Granting database privileges                     ';
file_put_contents('./grant_privileges.db', 'GRANT ALL PRIVILEGES ON DATABASE '.$dbName.' to '.$dbUserName.';');
system('createlang -U postgres plpgsql '.$dbName.' >> /dev/null 2>&1');
system('psql -U postgres < ./grant_privileges.db >> /dev/null 2>&1');
system('rm grant_privileges.db');
echoDone();

echo '9. Running rebake.php for installation              '."\n";
system('/usr/bin/php rebake.php');

require_once dirname(__FILE__).'/data/init.inc';
require_once 'Channels/Channels.inc';
require_once 'Systems/BaseSystem.inc';

echo '12. Give error_log 777 permission for developers    ';
if (file_exists($rootdir.'/error_log') === FALSE) {
    file_put_contents($rootdir.'/error_log', '');
}//end if

system('chmod 777 '.$rootdir.'/error_log');
echoDone();

/**
 * Echo done function.
 *
 * @since  4.0.0
 * @return void
 */
function echoDone()
{
    echo exec('echo -n "[ " ; echo -en "\033[0;32mDone" ; tput sgr0 ; echo " ]"')."\n";

}//end echoDone()

// @codingStandardsIgnoreEnd
?>
