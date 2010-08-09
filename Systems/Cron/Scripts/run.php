<?php
/**
 * Cron runner. run.php.
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
 * @subpackage Cron
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

if ((php_sapi_name() !== 'cli')) {
    exit;
}

$mysourceRoot = dirname(dirname(dirname(dirname(__FILE__))));
require_once $mysourceRoot.'/data/init.inc';
ini_set('include_path', ini_get('include_path').':'.Init::ROOT_DIR);
error_reporting(E_ALL | E_STRICT);
ini_set('error_log', $mysourceRoot.'/error_log');
ini_set('memory_limit', '256M');

require_once 'Channels/Channels.inc';
require_once 'DAL/DAL.inc';
Channels::includeSystem('Cron');

$types      = Cron::getCronTypes();
$typesToRun = array();
foreach ($types as $type) {
    $interval                    = Cron::getCronTypeInterval($type['condition']);
    $typesToRun[$type['typeid']] = $interval;
}

// Sort the types by job type interval.
asort($typesToRun);

foreach ($typesToRun as $typeid => $int) {
    Cron::runCronJobs($typeid);
}

exit;
?>
