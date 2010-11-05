<?php
/**
 * Runs before the patch is file is applied.
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

echo "Running preupgrade.php ...\n";
require_once dirname(__FILE__).'/data/init.inc';
ini_set('include_path', ini_get('include_path').':'.Init::ROOT_DIR);
include_once 'Channels/Channels.inc';
include_once 'DAL/DAL.inc';
include_once 'Systems/BaseSystem.inc';

ini_set('error_log', 'error_log');
ini_set('memory_limit', '256M');
set_time_limit(0);

echo "Msg here.\n";
echo "\t\t[Done]\n";

?>