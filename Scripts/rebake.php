<?php
/**
 * Squiz Framework script to rebake a product.
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

$url = 'http://framework.labs.squiz.net';

require_once $rootdir.'/Libs/Install/Install.inc';
require_once $rootdir.'/Channels/ChannelsBaker.inc';
require_once $rootdir.'/DAL/DALBaker.inc';
require_once $rootdir.'/Libs/FileSystem/FileSystem.inc';

// Work out what systems to install/reinstall.
$systemNames = Install::getInstallOptions(FALSE, FALSE);

echo '1. Cleaning oven directories';
system('rm -rf '.$rootdir.'/DAL/QueryStore/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/DAL/Oven/* >> /dev/null 2>&1');
system('rm -rf '.$rootdir.'/Channels/Oven/* >> /dev/null 2>&1');
echoDone();

echo '2. Rebaking oven';
Install::rebakeOven($rootdir, $url);
echoDone();

echo '3. Installing queries';
Install::installQueries($systemNames);
echoDone();

echo '4. Installing SQL functions';
Install::installSqlFunctions($systemNames);
echoDone();

exit;

/**
 * Echo done function.
 *
 * @return void
 */
function echoDone()
{
    echo exec('echo -en "\033[54G"; echo -n "[ " ; echo -en "\033[0;32mDone" ; tput sgr0 ; echo " ]"')."\n";

}//end echoDone()


?>
