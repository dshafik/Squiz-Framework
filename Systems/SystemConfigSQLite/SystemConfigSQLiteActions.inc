<?php
/**
 * Actions for the SystemConfigSQLite System.
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

require_once 'Systems/SystemConfig/SystemConfigActions.inc';

/**
 * SystemConfigSQLite Actions Class.
 *
 * Configs are stored as a serialized array in the sqlite table. You can
 * update one or more config settings for a particular system.
 */
class SystemConfigSQLiteActions implements SystemConfigActions
{


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Works out the db file path and attempts to load it.
     *
     * @return object
     */
    private static function _getDb()
    {
        include_once 'Systems/BaseSystem.inc';
        $dataPath = BaseSystem::getDataDir('SystemConfigSQLite');
        $dbPath   = $dataPath.'/config.db';

        if (file_exists($dbPath) === FALSE) {
            return NULL;
        }

        try {
            $db = new PDO('sqlite:'.$dbPath);
        } catch (PDOException $e) {
            return NULL;
        }

        return $db;

    }//end _getDb()


    /**
     * Updates all configuration settings of a system.
     *
     * @param string $systemName   The system to set config setting for.
     * @param array  $configValues An array of config and value pairs.
     *
     * @return boolean
     */
    public static function setConfig($systemName, array $configValues)
    {
        $db = self::_getDb();
        if ($db === NULL) {
            return FALSE;
        }

        try {
            Channels::includeSystem('SystemConfig');
            $currentConfig = SystemConfig::getConfig($systemName);

            // A new config entry for this system.
            if (empty($currentConfig) === TRUE) {
                foreach ($configValues as $name => $val) {
                    if ($val === 'true') {
                        $configValues[$name] = TRUE;
                    } else if ($val === 'false') {
                        $configValues[$name] = FALSE;
                    }
                }

                $sql = 'INSERT INTO config
                        VALUES (
                         '.$db->quote($systemName).',
                         NULL,
                         '.$db->quote(serialize($configValues)).'
                        )';
            } else {
                // Nothing has changed.
                if ($currentConfig === $configValues) {
                    return TRUE;
                }

                // Convert true/false from JS to boolean.
                foreach ($configValues as $name => $val) {
                    if ($val === 'true') {
                        $currentConfig[$name] = TRUE;
                    } else if ($val === 'false') {
                        $currentConfig[$name] = FALSE;
                    } else {
                        // Keep old config values if it has not been changed.
                        $currentConfig[$name] = $val;
                    }
                }

                $sql = 'UPDATE config
                        SET value = '.$db->quote(serialize($currentConfig)).'
                        WHERE system = '.$db->quote($systemName);
            }//end if

            $db->query($sql);
        } catch (PDOException $e) {
            return FALSE;
        }//end try

        return TRUE;

    }//end setConfig()


    /**
     * Retrives all configuration settings of a system.
     *
     * @param string $systemName The system to load config setting for.
     *
     * @return array
     */
    public static function getConfig($systemName)
    {
        $db = self::_getDb();
        if ($db === NULL) {
            return array();
        }

        $results = array();
        try {
            $sql = 'SELECT value FROM config
                    WHERE system = '.$db->quote($systemName);

            $result = $db->query($sql);
            unset($db);

            $config = $result->fetch(PDO::FETCH_ASSOC);
            $value  = array();
            if ($config !== FALSE && isset($config['value']) === TRUE) {
                $value = unserialize($config['value']);
            }
        } catch (PDOException $e) {
            return array();
        }

        return $value;

    }//end getConfig()


    /**
     * Delete system configuration entry.
     *
     * @param string $systemName The system to delete config setting for.
     *
     * @return boolean
     */
    public static function deleteConfig($systemName)
    {
        $db = self::_getDb();
        if ($db === NULL) {
            return FALSE;
        }

        try {
            $sql = 'DELETE FROM config
                    WHERE system = '.$db->quote($systemName);
            $db->query($sql);
            unset($db);
        } catch (PDOException $e) {
            return FALSE;
        }

        return TRUE;

    }//end deleteConfig()


}//end class

?>
