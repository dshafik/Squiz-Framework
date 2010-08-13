<?php
/**
 * Rebake all GUI templates.
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

$rootdir = $argv[1];
if (is_dir($rootdir) === FALSE) {
    echo "Please specify the root directory.\n";
    exit();
}


// Change to Product Root directory.
chdir($rootdir);

include_once 'Libs/FileSystem/FileSystem.inc';
include_once 'data/init.inc';

// Clean up existing templates.
$templateDirs = FileSystem::findDirectories($rootdir.'/data', 'Templates');
foreach ($templateDirs as $templateDirPath) {
    FileSystem::clearDirectory($templateDirPath);
}

// Re-run GUI installation function.
include_once 'Channels/Channels.inc';
Channels::includeSystem('GUI');
GUI::bakeTemplates();

// Fix perms.
system($rootdir.'/Scripts/fix_perms.sh');

?>