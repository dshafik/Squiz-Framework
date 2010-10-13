<?php
/**
 * Actions for the Backup System.
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
 * @subpackage Backup
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Backup System Actions Class.
 */
class BackupActions
{


    /**
     * Runs backup.sh, replace file if the one from previous week exists.
     *
     * Naming convention for file: DayOfWeek_YYYY-MM-DD_HH-MM.tar.gz.
     * Example: Thu_2010-10-13_14-03.tar.gz.
     *
     * @return void
     */
    public static function createBackup()
    {
        // Make sure we have a backup script.
        $script = dirname(__FILE__).'/Scripts/backup.sh';
        if (file_exists($script) === FALSE) {
            return;
        }

        // Remove the backup from the previous week.
        $dayOfWeek = date('D');
        $backups   = Backup::getBackups();
        $dataDir   = BaseSystem::getDataDir('Backup');
        if (in_array($dayOfWeek, array_keys($backups)) === TRUE) {
            $file = $dataDir.'/'.$backups[$dayOfWeek]['name'].'.tar.gz';
            unlink($file);
        }

        Backup::prepareBackup();

        $name   = date('D_Y-m-d_H-i', time());
        $target = $name.'.tar.gz';
        exec($script.' '.Init::ROOT_DIR.' '.$dataDir.' '.$target);

        Backup::afterBackup();

    }//end createBackup()


    /**
     * Returns information about backups that have been created.
     *
     * @return array
     */
    public static function getBackups()
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $dataDir = BaseSystem::getDataDir('Backup');
        $backups = glob($dataDir.'/*.tar.gz');
        $tmp     = array();
        foreach ($backups as $file) {
            $fileName  = basename($file, '.tar.gz');
            $parts     = explode('_', $fileName);
            $dayOfWeek = $parts[0];
            $time      = explode('-', $parts[2]);
            $date      = strtotime($parts[1].' '.$time[0].':'.$time[1]);

            $tmp[$date] = array(
                           'name' => $fileName,
                           'dow'  => $dayOfWeek,
                           'size' => FileSystem::readableSize($file),
                          );
        }

        // Reverse sort by date, for nice display.
        include_once 'Libs/String/String.inc';
        $result = array();
        krsort($tmp);
        foreach ($tmp as $date => $info) {
            $result[$info['dow']] = array(
                                     'name'      => $info['name'],
                                     'timestamp' => $date,
                                     'date'      => date('D, ', $date).String::easyDatetime($date),
                                     'size'      => $info['size'],
                                    );
        }

        return $result;

    }//end getBackups()


    /**
     * Returns a list of available backups.
     *
     * @return array
     */
    public static function getBackupList()
    {
        $backups = Backup::getBackups();

        $list = array();
        foreach ($backups as $day => $backup) {
            $list[$day] = array(
                           'date'    => $backup['date'],
                           'size'    => $backup['size'],
                           'actions' => '[download] [restore]',
                          );
        }

        return $list;

    }//end getBackupList()


    /**
     * Channel that runs before a backup is created.
     *
     * Systems can plug into this method to remove any old and unnecessary data.
     *
     * @return void
     */
    public static function prepareBackup()
    {

    }//end prepareBackup()


    /**
     * Channel that runs after a backup is created.
     *
     * @return void
     */
    public static function afterBackup()
    {

    }//end afterBackup()


}//end class

?>
