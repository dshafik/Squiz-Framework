<?php
/**
 * File Attribute.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage FileDataSource
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/DataSource/AttributeTypes/DataSourceText.inc';

/**
 * FileDataSourceFile.
 *
 * @since 4.0.0
 */
class FileDataSourceFile extends DataSourceText
{


    /**
     * Retrieves the value of a specified attribute from a specified asset.
     *
     * @param integer $assetid     Id of the asset.
     * @param string  $attributeid Id of the attribute.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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