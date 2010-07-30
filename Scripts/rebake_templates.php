<?php
/**
 * Rebake all GUI templates.
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

?>