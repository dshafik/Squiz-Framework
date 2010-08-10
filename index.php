<?php
/**
 * MySource 4 index.php.
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
 * @subpackage Core
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

// Set this to TRUE to generate SVG files of channel execution paths.
$enableDebugging = FALSE;

error_reporting(E_ALL | E_STRICT);
ini_set('error_log', 'error_log');
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
