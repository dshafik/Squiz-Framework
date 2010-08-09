<?php
/**
 * File Attribute.
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
 * @subpackage FileDataSource
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/DataSource/AttributeTypes/DataSourceText.inc';

/**
 * FileDataSourceFile.
 *
 */
class FileDataSourceFile extends DataSourceText
{


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @return mixed
     */
    public static function getValue($assetid, $attributeid)
    {
        $file = self::getAttributeFilePath($assetid, $attributeid);
        if (file_exists($file) === TRUE) {
            return $file;
        }

        return '';

    }//end getValue()


    /**
     * Returns the path to the file.
     *
     * @param integer $assetid        Id of the asset.
     * @param string  $attributeid    Id of the attribute.
     * @param boolean $currentContext If TRUE then only current context will be used.
     *                                This should be TRUE when path is going to be
     *                                used for writing.
     *
     * @return string
     */
    public static function getAttributeFilePath($assetid, $attributeid, $currentContext=FALSE)
    {
        include_once 'Systems/BaseSystem.inc';
        include_once 'Libs/FileSystem/FileSystem.inc';
        $hashDir = FileSystem::getHashDir($assetid);

        if ($currentContext === TRUE) {
            Channels::includeSystem('Context');
            $assetids = Context::getAssetid($assetid);
            $path     = BaseSystem::getDataDir('FileDataSource').'/'.$hashDir.'/'.$assetids.'_'.$attributeid;
        } else {
            // The file might not have been modified in this context so its path could be
            // in parent context, so look at parent contexts until its path is found.
            Channels::includeSystem('Context');
            $assetids = Context::getAssetids($assetid);
            foreach ($assetids as $aid) {
                $path = BaseSystem::getDataDir('FileDataSource').'/'.$hashDir.'/'.$aid.'_'.$attributeid;
                if (file_exists($path) === TRUE) {
                    return $path;
                }
            }
        }

        return $path;

    }//end getAttributeFilePath()


    /**
     * Creates a hash directory for the specified assetid if it does not exist.
     *
     * @param integer $assetid The asset to create the hash directory for.
     *
     * @return void
     */
    private static function _createHashDir($assetid)
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $hashDir = BaseSystem::getDataDir('FileDataSource').'/'.FileSystem::getHashDir($assetid);
        if (file_exists($hashDir) === FALSE) {
            include_once 'Libs/FileSystem/FileSystem.inc';
            FileSystem::mkdir($hashDir);
        }

    }//end _createHashDir()


    /**
     * Sets the value of a specified attribute for a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     * @param mixed   $value       Value of the attribute.
     *
     * @return boolean
     * @throws InvalidArgumentException If the file doesn't exist.
     */
    public static function setValue($assetid, $attributeid, $value)
    {
        if (file_exists($value) === FALSE) {
            throw new InvalidArgumentException('File "'.$value.'" does not exist');
        }

        self::_createHashDir($assetid);
        $file = self::getAttributeFilePath($assetid, $attributeid, TRUE);
        // Remove the current file for this attribute.
        if (file_exists($file) === TRUE) {
            unlink($file);
        }

        rename($value, $file);
        return chmod($file, 0664);

    }//end setValue()


}//end class

?>