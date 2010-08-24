<?php
/**
 * Squiz Framework script to rebake GUI templates.
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

require_once 'Channels/Channels.inc';
require_once 'Libs/FileSystem/FileSystem.inc';
require_once 'data/init.inc';

echo '1. Cleaning existing template cache';
$templateDirs = FileSystem::findDirectories($rootdir.'/data', 'Templates');
foreach ($templateDirs as $templateDirPath) {
    FileSystem::clearDirectory($templateDirPath);
}

echoDone();

echo '2. Baking templates';
Channels::includeSystem('GUI');
GUI::bakeTemplates();
echoDone();

echo '3. Fixing file system permissions';
system($rootdir.'/Scripts/fix_perms.sh');
echoDone();


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