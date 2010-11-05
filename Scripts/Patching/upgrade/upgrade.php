<?php
// @codingStandardsIgnoreStart
/**
 * Runs after  the patch is file is applied.
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
 * @package    CMS
 * @subpackage Scripts
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

echo "Running upgrade.php ...\n";
require_once dirname(__FILE__).'/data/init.inc';
ini_set('include_path', ini_get('include_path').':'.Init::ROOT_DIR);
include_once 'Channels/Channels.inc';
include_once 'DAL/DAL.inc';
include_once 'Systems/BaseSystem.inc';

ini_set('error_log', 'error_log');
ini_set('memory_limit', '256M');
set_time_limit(0);


echo "Install new Cron Script (Scheduled Publishing) and enable it.\n";
    Channels::includeSystem('Cron');
    Cron::installCronScripts('Publishing', '/var/www/mysource4/htdocs/Systems/Publishing');
    Cron::updateCronJob(
        'Publishing',
        'PublishingCron.inc',
        NULL,
        NULL,
        'frequently',
        FALSE,
        TRUE
    );

echo "Add 'created_by' attribute to asset type\n";
    Channels::includeSystem('Attribute');
    Attribute::addAttribute(
        'asset',
        'created_by',
        'integer',
        'AttributeDataSource',
        FALSE,
        NULL,
        NULL,
        FALSE,
        0,
        FALSE,
        ''
    );

echo "Add 'updated_by' attribute to asset type\n";
    Channels::includeSystem('Attribute');
    Attribute::addAttribute(
        'asset',
        'updated_by',
        'integer',
        'AttributeDataSource',
        FALSE,
        NULL,
        NULL,
        FALSE,
        0,
        FALSE,
        ''
    );

echo "Add 'published_by' attribute to asset type\n";
    Channels::includeSystem('Attribute');
    Attribute::addAttribute(
        'asset',
        'published_by',
        'integer',
        'RelationshipDataSource',
        FALSE,
        NULL,
        NULL,
        FALSE,
        0,
        FALSE,
        'publishing'
    );

echo "Add 'publish_scheduled_by' attribute to asset type\n";
    Channels::includeSystem('Attribute');
    Attribute::addAttribute(
        'asset',
        'publish_scheduled_by',
        'integer',
        'RelationshipDataSource',
        FALSE,
        NULL,
        NULL,
        FALSE,
        0,
        FALSE,
        'publishing'
    );


echo "Install new keyword formats.\n";
require_once 'Systems/Keyword/KeywordSystem.inc';
$keywordInfo = KeywordSystem::getFileSizeKeywordFormat();
Channels::includeSystem('Keyword');
Keyword::registerKeywordFormat('filesize', $keywordInfo['friendly_name'], $keywordInfo['description'], $keywordInfo['php_code'], $keywordInfo['custom']);
Keyword::updateKeywordFormatFile();

echo "Install new log types.\n";
Channels::includeSystem('Log');
Log::registerLogType(array('publishing.schedule' => TRUE));
Log::registerLogType(array('publishing.unschedule' => TRUE));

/*
    Revision 10579+
*/

// Register new Configuration attribute 'analytics' for Design asset types.
Channels::includeSystem('Configuration');
Configuration::addAttribute(
    'design',
    'analytics',
    'array',
    'ConfigurationDataSource',
    FALSE,
    NULL,
    '',
    FALSE,
    0
);
Configuration::addAttribute(
    'designCustomisation',
    'analytics',
    'array',
    'ConfigurationDataSource',
    FALSE,
    NULL,
    '',
    FALSE,
    0
);

// This should come at the very very END!
echo "Put the cron script back....";
    $cronContent = '* * * * * root /var/www/mysource4/htdocs/Scripts/network_update.sh
*/5 * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/Systems/Cron/Scripts/run.php &> /dev/null && /var/www/mysource4/htdocs/Scripts/fix_perms.sh
* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/restore.php
* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/activateProject.php';
    file_put_contents('/etc/cron.d/mysource4', $cronContent);
echo "[Done]\n";
?>
