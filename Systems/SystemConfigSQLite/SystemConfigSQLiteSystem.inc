<?php
/**
 * Installer for the SystemConfigSQLite System.
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
 * @subpackage SystemConfigSQLite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/SystemConfig/SystemConfigSystem.inc';

/**
 * SystemConfigSQLite System Class.
 *
 * Stores system-wide configuration settings in a sqlite db file (located in
 * this system's data directory). The settings will be global project based
 * (projectId is NULL), and is customisable on a project level basis
 */
class SystemConfigSQLiteSystem extends SystemConfigSystem
{


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Initialise config table in the data dir of this system.
     *
     * @return boolean
     */
    public function install()
    {
        parent::install();

        $dataPath = self::getDataDir('SystemConfigSQLite');
        $dbPath   = $dataPath.'/config.db';

        try {
            $db = new PDO('sqlite:'.$dbPath);

            $db->query(
                'CREATE TABLE config
                  (system    VARCHAR(100),
                   projectId INTEGER,
                   value     TEXT,
                   PRIMARY KEY (system, projectId)
                  )'
            );

            $db->query('CREATE INDEX config_index ON config (system, projectId)');
        } catch (PDOException $e) {
            return FALSE;
        }

        return TRUE;

    }//end install()


}//end class

?>
