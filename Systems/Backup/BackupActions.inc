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
     * Restores a backup file.
     *
     * Note that the system will be blown away during this process
     * and you're going to need access to delete the root dir of the
     * system as well.
     *
     * @param string $name The name of the backup file to restore.
     *
     * @return void
     *
     */
    public static function restoreBackup($name)
    {
        // Make sure we have a restore script.
        $script = dirname(__FILE__).'/Scripts/restore.sh';
        if (file_exists($script) === FALSE) {
            return;
        }

        $dataDir = BaseSystem::getDataDir('Backup');
        $file    = $dataDir.'/'.$name.'.tar.gz';
        if (file_exists($file) === FALSE) {
            exit();
        }

        exec($script.' '.$file.' '.Init::ROOT_DIR);

    }//end restoreBackup()


    /**
     * Queues up the restoration of a backup.
     *
     * The restore process is handled via a cron to allow the process
     * to run as root, so this method just creates a todo file in the
     * backup dir so the cron knows what to do.
     *
     * @param string $name The name of the backup file to restore.
     *
     * @return boolean
     *
     * @api            write
     * @api-permission superuser
     */
    public static function queueRestoreBackup($name)
    {
        $dataDir = BaseSystem::getDataDir('Backup');
        $file    = $dataDir.'/'.$name.'.tar.gz';
        if (file_exists($file) === FALSE) {
            return FALSE;
        }

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($dataDir.'/backup.todo', $name);

        return TRUE;

    }//end queueRestoreBackup()


    /**
     * Gets the name of the backup queued for restoration.
     *
     * @return string
     *
     * @api            read
     * @api-permission superuser
     */
    public static function getQueuedRestoreBackup()
    {
        $dataDir = BaseSystem::getDataDir('Backup');
        $file    = $dataDir.'/backup.todo';
        if (file_exists($file) === FALSE) {
            return '';
        }

        $name = file_get_contents($file);
        return $name;

    }//end getQueuedRestoreBackup()


    /**
     * Removes a backup queued for restoration.
     *
     * @return boolean
     *
     * @api            write
     * @api-permission superuser
     */
    public static function removeQueuedRestoreBackup()
    {
        $dataDir = BaseSystem::getDataDir('Backup');
        $file    = $dataDir.'/backup.todo';
        if (file_exists($file) === FALSE) {
            return TRUE;
        }

        $removed = unlink($file);
        return $removed;

    }//end removeQueuedRestoreBackup()


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
        Channels::includeSystem('GUI');
        $dbtnSettings = array(
                         'value'  => 'Download',
                         'click'  => array(
                                      'function'  => 'BackupBackupScreen.download',
                                      'arguments' => array(),
                                     ),
                        );
        $rbtnSettings = array(
                         'value'  => 'Restore',
                         'colour' => 'Dark',
                         'click'  => array(
                                      'function'  => 'GUI.getWidget(\'backupScreen-confirmRestore\').show',
                                      'arguments' => array(),
                                      'quoteArgs' => FALSE,
                                     ),
                        );

        $list    = array();
        $backups = Backup::getBackups();
        foreach ($backups as $day => $backup) {
            $dbtnSettings['click']['arguments'] = array($backup['name']);
            $rbtnSettings['click']['arguments'] = array('this', '\''.$backup['name'].'\'');
            $download = GUI::createWidget($day.'-download', 'GUIButton', $dbtnSettings);
            $restore  = GUI::createWidget($day.'-restore', 'GUIButton', $rbtnSettings);

            $list[$day] = array(
                           'date'     => $backup['date'],
                           'size'     => $backup['size'],
                           'download' => $download,
                           'restore'  => $restore,
                          );
        }

        return $list;

    }//end getBackupList()


    /**
     * Downloads a backup.
     *
     * Note that the file is served directly and the script
     * will exit when the file is sent.
     *
     * @param string $name The name of the backup file. Do not
     *                     include the extension.
     *
     * @return void
     *
     * @api            read
     * @api-permission superuser
     */
    public static function downloadBackup($name)
    {
        $dataDir = BaseSystem::getDataDir('Backup');
        $file    = $dataDir.'/'.$name.'.tar.gz';
        if (file_exists($file) === FALSE) {
            throw new ChannelException(_('The requested backup file does not exist'));
        }

        include_once 'Libs/FileSystem/FileSystem.inc';
        header('Content-Disposition: attachment; filename='.$name.'.tar.gz;');
        FileSystem::serveFile($file, 'application/x-tar-gz');

    }//end downloadBackup()


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
