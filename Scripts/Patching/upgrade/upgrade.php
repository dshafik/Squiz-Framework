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


// Install new asset types.
echo "Adding asset types...";
Channels::includeSystem('AssetType');
AssetType::install('redirect', 'Redirect', 'CMSAssets', 'asset');
AssetType::install('searchPage', 'Search Page', 'Search', 'page');
include '/var/www/mysource4/htdocs/data/AssetType/AssetPaths.inc';
$paths['redirect']   = '/var/www/mysource4/htdocs/Systems/CMSAssets/AssetTypes/RedirectAssetType';
$paths['searchpage'] = '/var/www/mysource4/htdocs/Systems/Search/AssetTypes/SearchPageAssetType';
file_put_contents(
    '/var/www/mysource4/htdocs/data/AssetType/AssetPaths.inc',
    '<?php'."\n".'$paths = '.var_export($paths, 1).';'."\n".'?>'."\n"
);
echo "[Done]\n";


echo "Reload Wizard XML Files...\n";
// Create the data/Wizard dir if it does not exist.
require_once 'Libs/FileSystem/FileSystem.inc';
$dataPath    = '/var/www/mysource4/htdocs/data/Wizard/Wizards';
$wizards     = array();
$systemsPath = Channels::getSystemsPath();
if (file_exists($systemsPath) === TRUE) {
    $postfix = '_wizard.xml';
    $wizards = FileSystem::listDirectory($systemsPath, array('.xml'), TRUE, TRUE, $postfix);
}

// Copy all the wizards in to the data/Wizard/Wizards directory.
foreach ($wizards as $wizardPath) {
    $baseName = FileSystem::getBaseName($wizardPath);
    copy($wizardPath, $dataPath.'/'.$baseName);
}
echo "[Done]\n";


echo "Rename MySource Matrix connection...\n";
AssetType::updateTypeName('mySourceMatrixConnection', 'Squiz Matrix Connection');
echo "[Done]\n";


echo "Install SearchBoxDesignArea.\n";
Channels::includeSystem('Design');
Design::writeDesignAreasCacheFile();
echo "[Done]\n";


// This should come at the very very END!
echo "Put the cron script back....";
    $cronContent = '* * * * * root /var/www/mysource4/htdocs/Scripts/network_update.sh
*/5 * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/Systems/Cron/Scripts/run.php &> /dev/null && /var/www/mysource4/htdocs/Scripts/fix_perms.sh
* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/restore.php
* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/activateProject.php';
    file_put_contents('/etc/cron.d/mysource4', $cronContent);
echo "[Done]\n";
?>
