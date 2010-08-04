<?php
/**
 * Cron runner. run.php.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Cron
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
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
