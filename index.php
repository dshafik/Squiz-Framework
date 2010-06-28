<?php
/**
 * MySource 4 index.php.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage MySource4
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

// Set this to TRUE to generate SVG files of channel execution paths.
$enableDebugging = FALSE;

error_reporting(E_ALL | E_STRICT);
ini_set('error_log', 'error_log');
date_default_timezone_set('Australia/Sydney');
include_once dirname(__FILE__).'/Systems/BaseSystem.inc';
include_once BaseSystem::getDataDir().'/init.inc';
ini_set('include_path', ini_get('include_path').':'.Init::ROOT_DIR);
ini_set('default_charset', 'UTF-8');

require_once 'Channels/Channels.inc';
Channels::includeSystem('MySource');
include_once 'DAL/DAL.inc';

if ($enableDebugging === TRUE) {
    include_once 'Libs/Debug/DebugChannels.inc';
    register_tick_function(array('DebugChannels', 'tick'));
    declare (ticks = 1);
}

MySource::init();

if ($enableDebugging === TRUE) {
    unregister_tick_function(array('DebugChannels', 'tick'));
    $dotFile = dirname(__FILE__).'/report/test1.dot';
    DebugChannels::$dotCreator->createDotFile($dotFile);
    DebugChannels::$dotCreator->convertToSvg($dotFile, $dotFile.'.svg');
}

?>
