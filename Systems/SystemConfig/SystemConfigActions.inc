<?php
/**
 * Basic structure for all system configuration handling systems.
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
 * @subpackage SystemConfig
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Basic structure for all system configuration handling systems.
 */
interface SystemConfigActions
{


    /**
     * Updates all configuration settings of a system.
     *
     * @param string $systemName   The system to set config setting for.
     * @param array  $configValues An array of config and value pairs.
     *
     * @return boolean
     */
    public static function setConfig($systemName, array $configValues);


    /**
     * Retrives all configuration settings of a system.
     *
     * @param string $systemName The system to load config setting for.
     *
     * @return array
     */
    public static function getConfig($systemName);


    /**
     * Delete system configuration entry.
     *
     * @param string $systemName The system to delete config setting for.
     *
     * @return boolean
     */
    public static function deleteConfig($systemName);


}//end interface

?>