<?php
/**
 * FileSystem Lib.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Library for general file system operations.
 *
 * Methods for adding, deleting and modifying directories and files should
 * reside here.
 */
class FileSystem
{


    /**
     * The number of files within a hash directory.
     *
     * @var int
     */
    const HASH_DIR_FILE_COUNT = 10000;

    /**
     * Different units of measurement for sizes of files.
     *
     * @var string
     */
    const FILE_SIZE_BYTE     = 'B';
    const FILE_SIZE_KILOBYTE = 'kB';
    const FILE_SIZE_MEGABYTE = 'MB';
    const FILE_SIZE_GIGABYTE = 'GB';

    /**
     * The number of files within a hash directory.
     *
     * @var array
     */
    public static $changedFiles = array();

    // @codingStandardsIgnoreStart
    /**
     * The number of files within a hash directory.
     *
     * @var   array
     */
    private static $fileTypesInfo = array(
        // CSS Document Asset Type.
        'css'  => array('friendlyName' => 'Cascading Style Sheet', 'mimeType' => 'text/css',),

        // Document Asset Type.
        'html' => array('friendlyName' => 'Hypertext Markup Language', 'mimeType' => 'text/html',),
        'htm'  => array('friendlyName' => 'Hypertext Markup Language', 'mimeType' => 'text/html',),
        'c'  => array('friendlyName' => 'C/C++ Program File', 'mimeType' => 'text/plain',),
        'cpp'  => array('friendlyName' => 'C/C++ Program File', 'mimeType' => 'text/plain',),
        'asc'  => array('friendlyName' => 'ASCII Text', 'mimeType' => 'text/plain',),
        'log'  => array('friendlyName' => 'Log File', 'mimeType' => 'text/plain',),
        'conf'  => array('friendlyName' => 'Linux Configuration File', 'mimeType' => 'text/plain',),
        'text'  => array('friendlyName' => 'Text File', 'mimeType' => 'text/plain',),
        'txt'  => array('friendlyName' => 'Text File', 'mimeType' => 'text/plain',),
        'dtd'  => array('friendlyName' => 'SGML Document Definition File', 'mimeType' => 'text/xml',),
        'xml'  => array('friendlyName' => 'Extensible Markup Language File', 'mimeType' => 'text/xml',),
        'csv'  => array('friendlyName' => 'Comma-Separated Variables', 'mimeType' => 'text/csv',),
        'tsv'  => array('friendlyName' => 'Tab-Separated Values', 'mimeType' => 'text/tab-separated-values',),

        // Image Asset Type.
        'gif'  => array('friendlyName' => 'GIF Image', 'mimeType' => 'image/gif',),
        'jpeg' => array('friendlyName' => 'JPEG Image', 'mimeType' => 'image/jpeg',),
        'jpg'  => array('friendlyName' => 'JPEG Image', 'mimeType' => 'image/jpeg',),
        'jpe'  => array('friendlyName' => 'JPEG Image', 'mimeType' => 'image/jpeg',),
        'png'  => array('friendlyName' => 'PNG Image', 'mimeType' => 'image/png',),
        'tiff' => array('friendlyName' => 'Tagged Image Format File', 'mimeType' => 'image/tiff',),
        'tif'  => array('friendlyName' => 'Tagged Image Format File', 'mimeType' => 'image/tiff',),
        'bmp'  => array('friendlyName' => 'Windows OS/2 Bitmap Graphics', 'mimeType' => 'image/x-ms-bmp',),
        'pnm'  => array('friendlyName' => 'PBM Portable Any Map Graphic Bitmap', 'mimeType' => 'image/x-portable-anymap',),
        'pbm'  => array('friendlyName' => 'UNIX Portable Bitmap Graphic', 'mimeType' => 'image/x-portable-bitmap',),
        'pgm'  => array('friendlyName' => 'Portable Graymap Graphic', 'mimeType' => 'image/x-portable-graymap',),
        'ppm'  => array('friendlyName' => 'PBM Portable Pixelmap Graphic', 'mimeType' => 'image/x-portable-pixmap',),
        'ico'  => array('friendlyName' => 'Windows Icon', 'mimeType' => 'image/x-icon',),
        'sgml' => array('friendlyName' => 'Standard Generalized Markup Language', 'mimeType' => 'text/x-sgml',),
        'sgm'  => array('friendlyName' => 'Standard Generalized Markup Language', 'mimeType' => 'text/x-sgml',),

        // Javascript Asset Type
        'js'   => array('friendlyName' => 'Javascript', 'mimeType' => 'application/x-javascript',),
        'jgz'  => array('friendlyName' => 'Javascript', 'mimeType' => 'application/x-javascript',),

        // Movie Asset Type
        'mpeg' => array('friendlyName' => 'Movie', 'mimeType' => 'video/mpeg',),
        'mpg'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/mpeg',),
        'mpe'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/mpeg',),
        'qt'   => array('friendlyName' => 'Quicktime', 'mimeType' => 'video/quicktime',),
        'mov'  => array('friendlyName' => 'Quicktime', 'mimeType' => 'video/quicktime',),
        'avi'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/x-msvideo',),
        'wmv'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/x-wmv',),
        'wma'  => array('friendlyName' => 'Movie', 'mimeType' => 'audio/x-ms-wma',),
        'wvx'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/x-ms-wvx',),
        'wm'   => array('friendlyName' => 'Movie', 'mimeType' => 'video/x-ms-wm',),
        'mp4'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/mp4',),
        'flv'  => array('friendlyName' => 'Flash Video File', 'mimeType' => 'video/x-flv',),
        'm4v'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/mp4',),
        'movie'  => array('friendlyName' => 'Movie', 'mimeType' => 'video/x-sgi-movie',),
        'asf'  => array('friendlyName' => 'Advanced Streaming Format', 'mimeType' => 'video/x-ms-asf',),
        'asx'  => array('friendlyName' => 'Advanced Stream Redirector File', 'mimeType' => 'video/x-ms-asf',),

        // PDF Asset Type
        'pdf'  => array('friendlyName' => 'PDF', 'mimeType' => 'application/pdf',),

        // Word Document Asset Type
        'doc'  => array('friendlyName' => 'MS Word', 'mimeType' => 'application/msword',),
        'dot' => array('friendlyName' => 'MS Word', 'mimeType' => 'application/msword',),
        'docx' => array('friendlyName' => 'MS Word', 'mimeType' => 'application/octet-stream',),

        // All the rest.
        'xls'  => array('friendlyName' => 'Excel Worksheet', 'mimeType' => 'application/ms-excel',),
        'xlsx' => array('friendlyName' => 'Excel Microsoft Office Open XML Format Spreadsheet', 'mimeType' => 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',),
        'eps'  => array('friendlyName' => 'Encapsulated PostScript', 'mimeType' => 'application/postscript',),
        'ps'   => array('friendlyName' => 'PostScript', 'mimeType' => 'application/postscript',),
        'ppt'  => array('friendlyName' => 'PowerPoint Presentation', 'mimeType' => 'application/powerpoint',),
        'pps'  => array('friendlyName' => 'PowerPoint Slideshow', 'mimeType' => 'application/powerpoint',),
        'rtf'  => array('friendlyName' => 'Rich Text Format File', 'mimeType' => 'application/rtf',),
        'swf'  => array('friendlyName' => 'Flash Format File', 'mimeType' => 'application/x-shockwave-flash',),
        'tgz'  => array('friendlyName' => 'UNIX Tar File Gzipped', 'mimeType' => 'application/x-gzip',),
        'gz'   => array('friendlyName' => 'Gzip Compressed Archive', 'mimeType' => 'application/x-gzip',),
        'sit'  => array('friendlyName' => 'StuffIt Compressed Archive File', 'mimeType' => 'application/x-stuffit',),
        'tar'  => array('friendlyName' => 'Tape Archive File', 'mimeType' => 'application/x-tar',),
        'zip'  => array('friendlyName' => 'Compressed Archive File', 'mimeType' => 'application/zip',),
        'mid'  => array('friendlyName' => 'Musical Instrument Digital Interface MIDI-sequention Sound', 'mimeType' => 'audio/midi',),
        'midi' => array('friendlyName' => 'Musical Instrument Digital Interface MIDI-sequention Sound', 'mimeType' => 'audio/midi',),
        'mp2'  => array('friendlyName' => 'MPEG Audio Stream, Layer II', 'mimeType' => 'audio/mpeg',),
        'mp3'  => array('friendlyName' => 'MPEG Audio Stream, Layer III', 'mimeType' => 'audio/mpeg',),
        'aif'  => array('friendlyName' => 'Audio Interchange File', 'mimeType' => 'audio/x-aiff',),
        'aiff' => array('friendlyName' => 'Audio Interchange File', 'mimeType' => 'audio/x-aiff',),
        'wav'  => array('friendlyName' => 'Waveform Audio', 'mimeType' => 'audio/x-wav',),
        'svg'  => array('friendlyName' => 'Scalable Vector Graphics Image', 'mimeType' => 'image/svg+xml',),
       );
   // @codingStandardsIgnoreEnd


    /**
     * Serves file.
     *
     * @param string $filePath Path to the file.
     * @param string $mimeType Content type of the file.
     * @param array  $headers  Extra headers to send.
     *
     * @return mixed
     */
    public static function serveFile(
        $filePath,
        $mimeType,
        array $headers=array()
    ) {
        // Set the content type header.
        $headers['Content-type'] = $mimeType;
        $xSendFile               = FALSE;

        Channels::includeSystem('SystemConfig');
        $configs = SystemConfig::getConfig('FileAssets');
        if ($configs['x-send-file'] === TRUE) {
            $xSendFile                       = TRUE;
            $headers['X-Sendfile']           = $filePath;
            $headers['x-lighttpd-send-file'] = $filePath;
        }

        Channels::includeSystem('Editing');
        if (Editing::isEditing() === FALSE) {
            if (Channels::systemExists('Cache') === TRUE) {
                $modTs = filemtime($filePath);
                Channels::includeSystem('Cache');
                Cache::sendCacheableHeaders($modTs);
            }
        }

        if (headers_sent() === FALSE) {
            foreach ($headers as $header => $value) {
                header($header.': '.$value);
            }
        }

        if ($xSendFile === TRUE) {
            exit;
        }

        $contents = file_get_contents($filePath);
        echo $contents;
        return $contents;

    }//end serveFile()


    /**
     * Removes a directory from the file system.
     *
     * Enters subdirectories and removes all content, and deletes the parent
     * directory if specified.
     *
     * @param string  $path       The path of the directory to delete.
     * @param boolean $deleteRoot Whether to delete the parent directory.
     * @param array   $omitFiles  Array of file names that lie in the root
     *                            directory that should not be deleted. Note,
     *                            the $deleteRoot param should be false if this
     *                            is not empty.
     *
     * @return boolean
     * @throws RuntimeException When the environment prevents the directory from
     *                          being succesfully deleted.
     */
    public static function clearDirectory(
        $path,
        $deleteRoot=FALSE,
        array $omitFiles=array()
    ) {
        if (file_exists($path) === FALSE) {
            return FALSE;
        }

        $dir = new DirectoryIterator($path);
        while ($dir->valid() === TRUE) {
            if ($dir->isDot() === TRUE) {
                // This is '.' or '..'. Not interested.
                $dir->next();
                continue;
            }

            if ($dir->isDir() === TRUE) {
                // If $deleteRoot is FALSE, then we don't want to delete
                // .svn entries, as with directories like the oven directory,
                // this will break updates on parent directories.
                if ($deleteRoot === FALSE) {
                    if ($dir->getFileName() === '.svn') {
                        $dir->next();
                        continue;
                    }
                }

                // Recursively call this method to clear nested directories.
                if (self::clearDirectory($dir->getPathName(), FALSE) === FALSE) {
                    return FALSE;
                }

                // Directory is now clear, so we can remove the directory.
                if (rmdir($dir->getPathName()) === FALSE) {
                    // Unable to delete the directory. Throw Exception, as the
                    // operation will ultimately fail.
                    $error = 'Unable to delete directory: '.$dir->getPathName;
                    throw new RuntimeException($error);
                }

                $dir->next();
                continue;
            }//end if

            // We have a file or symbolic link, we need to unlink it.
            if (in_array($dir->getFileName(), $omitFiles) === FALSE) {
                if (unlink($dir->getPathName()) === FALSE) {
                    // Problem deleting the file, throw exception.
                    $error = 'Unable to delete file: '.$dir->getPathName;
                    throw new RuntimeException($error);
                }
            }

            $dir->next();
        }//end while

        if ($deleteRoot === TRUE) {
            if (file_exists($path) === TRUE) {
                if (rmdir($path) === FALSE) {
                    $error = 'Unable to delete directory: '.$dir->getPathName;
                    throw new RuntimeException($error);
                }
            }
        }

        return TRUE;

    }//end clearDirectory()


    /**
     * Lists the contents of a directory.
     *
     * Allows the specifying of file extensions, and whether or not to include
     * full paths in the resulting array, or just filenames. Returns an array
     * of file names.
     *
     * @param string  $path       The path of the directory to list.
     * @param array   $extensions An array of extensions (with the leading dots)
     *                            of files to return.
     * @param boolean $nested     Include subdirectories in the listing.
     * @param boolean $fullPath   Include the full path, or just the filename.
     * @param string  $postfix    The pattern at the end of a file name.
     *
     * @return array
     */
    public static function listDirectory(
        $path,
        array $extensions=array(),
        $nested=TRUE,
        $fullPath=TRUE,
        $postfix=NULL
    ) {
        $files = array();
        if (file_exists($path) === FALSE) {
            return $files;
        }

        $postfixLength = 0;
        if ($postfix !== NULL) {
            $postfixLength = strlen($postfix);
        }

        $dir = new DirectoryIterator($path);
        while ($dir->valid() === TRUE) {
            if ($dir->isDot() === TRUE) {
                // This is '.' or '..'. Not interested.
                $dir->next();
                continue;
            }

            if ($dir->isDir() === TRUE && $nested === TRUE) {
                if ($dir->getFileName() === '.svn') {
                    $dir->next();
                    continue;
                }

                // Recursively call this method to list nested directories.
                $nestedDirectory = self::listDirectory(
                    $dir->getPathName(),
                    $extensions,
                    $nested,
                    $fullPath,
                    $postfix
                );

                // Merge the files from the subdirectory with ours.
                $files = array_merge($files, $nestedDirectory);
                $dir->next();
                continue;
            }

            // We have a file, so we need to determine if it fits the specified
            // criteria.
            if ($dir->isFile() === TRUE) {
                $addFile  = FALSE;
                $filename = $dir->getFilename();

                // Skip hidden files.
                if ($filename{0} !== '.') {
                    if (empty($extensions) === TRUE) {
                        // No extensions, every file should be added.
                        $addFile = TRUE;
                    } else {
                        // Determine if the file has the right extension.
                        $fileName = $dir->getFileName();
                        $dotPos   = strrpos($fileName, '.');
                        $fileExt  = substr($fileName, $dotPos);
                        if (in_array($fileExt, $extensions) === TRUE) {
                            // Correct extension. Set our flag so
                            // it's added below.
                            $addFile = TRUE;
                        }
                    }

                    // File name must end with postfix (inc. extension).
                    if ($postfix !== NULL) {
                        $addFile     = FALSE;
                        $fileNameEnd = substr($filename, -$postfixLength);
                        if ($fileNameEnd === $postfix) {
                            $addFile = TRUE;
                        }
                    }

                    if ($addFile === TRUE) {
                        // File fits our criteria, now we only need to work out
                        // which details to add to our array.
                        if ($fullPath === TRUE) {
                            $files[] = $dir->getPathName();
                        } else {
                            $files[] = $dir->getFileName();
                        }
                    }
                }//end if
            }//end if

            $dir->next();
        }//end while

        sort($files);
        return $files;

    }//end listDirectory()


    /**
     * Recursively finds directories with the specified name.
     *
     * Returns the absolute paths to the directories, starting from the
     * specified path.
     *
     * @param string $path The path of the directory to start at.
     * @param string $name The name of the directories to find.
     *
     * @return array
     */
    public static function findDirectories($path, $name)
    {
        $dir = new RecursiveDirectoryIterator($path);

        $paths = array();
        foreach ($dir as $file) {
            if ($file->isDir() === TRUE) {
                if ($file->getFilename() === $name) {
                    $paths[] = $file->getPathname();
                } else {
                    $paths = array_merge(
                        $paths,
                        self::findDirectories($file->getPathName(), $name)
                    );
                }
            }
        }

        return $paths;

    }//end findDirectories()


    /**
     * List directories that are directly under specified path.
     *
     * @param string $path Path to look at.
     *
     * @return array
     */
    public static function listDirectories($path)
    {
        $paths = array();
        if (is_dir($path) === TRUE) {
            $dir = new RecursiveDirectoryIterator($path);
            foreach ($dir as $file) {
                $fileName = $file->getFilename();
                if ($file->isDir() === TRUE && $fileName{0} !== '.') {
                    $paths[$file->getFilename()] = $file->getPathname();
                }
            }
        }

        ksort($paths);
        return $paths;

    }//end listDirectories()


    /**
     * Returns the relative hash directory for the specified id.
     *
     * @param integer $id    The integer to return the hash dir for.
     * @param integer $count Group by how many.
     *
     * @return string
     */
    public static function getHashDir($id, $count=0)
    {
        if ($count === 0) {
            $count = self::HASH_DIR_FILE_COUNT;
        }

        $id = (int) $id;
        if ($id === 0) {
            // Special case for id zero.
            return '0';
        }

        $negation = '';
        if ($id < 0) {
            $negation = '-';
            $id       = abs($id);
        }

        $factor = floor($id / $count);
        if (($id % $count) !== 0) {
            $factor++;
        }

        $result = (($factor - 1) * $count + 1).'-'.($count * $factor);
        $result = $negation.$result;

        return $result;

    }//end getHashDir()


    /**
     * Return the size of a file with human readable units appended.
     *
     * @param string  $path The path to the file.
     * @param string  $unit The maximum unit that can be used to
     *                      display the file size.
     * @param integer $size If specified then $path argument will not be used.
     *
     * @return string
     */
    public static function readableSize($path, $unit=NULL, $size=FALSE)
    {
        if ($size === FALSE || is_numeric($size) !== TRUE) {
            $size = @filesize($path);
            if ($size === FALSE) {
                return FALSE;
            }
        }

        $readable = self::formatFileSize($size, $unit);

        return $readable;

    }//end readableSize()


    /**
     * Format File Size.
     *
     * @param integer $size If specified then $path argument will not be used.
     * @param string  $unit The maximum unit that can be used to display size.
     *
     * @return string
     * @access public
     */
    public static function formatFileSize($size, $unit=NULL)
    {
        $units   = array(
                    self::FILE_SIZE_BYTE,
                    self::FILE_SIZE_KILOBYTE,
                    self::FILE_SIZE_MEGABYTE,
                    self::FILE_SIZE_GIGABYTE,
                   );
        $maxUnit = (count($units) - 1);

        // Accept the maximum units as a parameter, maybe...
        $unit = array_search($unit, $units);
        if ($unit === NULL || $unit === FALSE) {
            $unit = $maxUnit;
        }

        $factor = 0;
        while ($unit !== $factor && $size >= 1024 && $factor < $maxUnit) {
            $size = ($size / 1024);
            $factor++;
        }

        $readable = sprintf('%01.2f%s', $size, $units[$factor]);

        return $readable;

    }//end formatFileSize()


    /**
     * Basename for unix and Windows.
     *
     * @param string $path Path for a file.
     *
     * @return string
     */
    public static function getBaseName($path)
    {
        $fileName = preg_replace('/^.+[\\\\\\/]/', '', $path);
        return $fileName;

    }//end getBaseName()


    /**
     * Copies the files and directories from srcPath to dstPath.
     *
     * Note: This method does not create the dstPath. All hidden files are
     * ignored.
     *
     * @param string $srcPath  Source directory.
     * @param string $dstPath  Destination directory.
     * @param string $perm     Permission for the files.
     * @param array  $callback Callback method.
     *
     * @return void
     */
    public static function copyDirectory(
        $srcPath,
        $dstPath,
        $perm=0777,
        array $callback=NULL
    ) {
        if (is_dir($srcPath) === TRUE) {
            if (file_exists($dstPath) === FALSE) {
                self::mkdir($dstPath, $perm, TRUE);
                // Chmod to make sure permission is set (see umask()).
                @chmod($dstPath, $perm);
            }

            $dir  = dir($srcPath);
            $file = $dir->read();
            while ($file !== FALSE) {
                // Ignore all hidden files.
                if (strpos($file, '.') !== 0) {
                    $nPath = $srcPath.'/'.$file;

                    if (is_dir($nPath) === TRUE) {
                        // Sub dir..
                        self::copyDirectory(
                            $nPath,
                            $dstPath.'/'.$file,
                            $perm,
                            $callback
                        );
                    } else {
                        // If callback provided then source path and dest path
                        // will be passed to the callback function. If it
                        // returns TRUE then file will be copied.
                        $copy = TRUE;
                        if ($callback !== NULL) {
                            $copy = call_user_func(
                                $callback,
                                $file,
                                $nPath,
                                $dstPath.'/'.$file
                            );
                        }

                        if ($copy === TRUE) {
                            copy($nPath, $dstPath.'/'.$file);
                            @chmod($dstPath.'/'.$file, $perm);
                        }
                    }//end if
                }//end if

                $file = $dir->read();
            }//end while

            $dir->close();
        }//end if

    }//end copyDirectory()


    /**
     * Writes an array to a file.
     *
     * The file can be required/included to retrieve the array again.
     *
     * @param array  $value    The array to write.
     * @param string $varName  The name of the variable when included.
     * @param string $filename The filename to write to.
     *
     * @return boolean
     */
    public static function arrayToFile(array $value, $varName, $filename)
    {
        if (is_array($value) === FALSE) {
            return FALSE;
        }

        include_once 'Libs/FileSystem/FileSystem.inc';
        $string = '<?php $'.$varName.' = '.var_export($value, TRUE).'; ?>';
        $ret    = self::filePutContents($filename, $string);
        return $ret;

    }//end arrayToFile()


    /**
     * Returns an extension of given filename.
     *
     * @param string $fileName The filename to write to.
     *
     * @return string
     */
    public static function getExtension($fileName)
    {
        if (is_string($fileName) === TRUE) {
            $name = strtolower(substr(strrchr($fileName, '.'), 1));
            return $name;
        } else {
            return '';
        }

    }//end getExtension()


    /**
     * Returns all file type info.
     *
     * @return array
     */
    public static function getAllFileTypeInfo()
    {
        return self::$fileTypesInfo;

    }//end getAllFileTypeInfo()


    /**
     * Returns a mime type of given file name.
     *
     * @param string $fileName The filename to get mime type.
     *
     * @return string
     */
    public static function getMimeType($fileName)
    {
        $mimeType = 'application/octet-stream';
        $ext      = self::getExtension($fileName);
        if (isset(self::$fileTypesInfo[$ext]) === TRUE) {
            $mimeType = self::$fileTypesInfo[$ext]['mimeType'];
        }

        return $mimeType;

    }//end getMimeType()


    /**
     * Returns a mime type of given file name.
     *
     * @param string $fileName The filename to get mime type.
     *
     * @return string
     */
    public static function getFriendlyName($fileName)
    {
        $mimeType = 'application/octet-stream';
        $ext      = self::getExtension($fileName);
        $mimeType = self::$fileTypesInfo[$ext]['friendlyName'];
        return $mimeType;

    }//end getFriendlyName()


    /**
     * Write file to the file system.
     *
     * @param string  $fileName The filename to write to.
     * @param string  $content  The content to write.
     * @param integer $flag     The value of flags  can be any combination of
     *                          the following flags (with some restrictions),
     *                          joined with the binary OR (|) operator.
     *
     * @return integer
     */
    public static function filePutContents($fileName, $content, $flag=0)
    {
        if (isset(BaseSystem::$dataId) === TRUE
            && BaseSystem::$dataId === 'testing'
        ) {
            if (isset(self::$changedFiles[$fileName]) === FALSE) {
                self::$changedFiles[$fileName] = array(
                                                  'type'   => 'filePutContents',
                                                  'flag'   => $flag,
                                                  'result' => FALSE,
                                                  'size'   => 0,
                                                 );

                if (file_exists($fileName) === TRUE) {
                    self::$changedFiles[$fileName]['type'] = 'fileUpdateContents';

                    self::$changedFiles[$fileName]['oldContent'] = file_get_contents(
                        $fileName
                    );
                }
            }

            $ret = file_put_contents($fileName, $content, $flag);

            self::$changedFiles[$fileName]['result'] = $ret;

            if ($ret !== FALSE) {
                self::$changedFiles[$fileName]['size'] = filesize($fileName);
            }
        } else {
            $ret = file_put_contents($fileName, $content, $flag);
        }//end if

        return $ret;

    }//end filePutContents()


    /**
     * Create a directory in the file system.
     *
     * @param string  $pathname  The directory path.
     * @param integer $mode      The mode is 0777 by default, which means the
     *                           widest possible access.
     * @param boolean $recursive Default to FALSE.
     *
     * @return boolean
     */
    public static function mkdir($pathname, $mode=0777, $recursive=FALSE)
    {
        $pathname = rtrim($pathname, '/');
        if (isset(BaseSystem::$dataId) === TRUE
            && BaseSystem::$dataId === 'testing'
        ) {
            self::$changedFiles[$pathname] = array(
                                              'type'      => 'mkdir',
                                              'mode'      => $mode,
                                              'recursive' => TRUE,
                                              'result'    => NULL,
                                             );

            $parentPath = '';
            if ($recursive === TRUE) {
                $parts  = explode('/', $pathname);
                $length = count($parts);
                for ($i = $length; $i > 1; $i--) {
                    $parentPath = implode('/', array_slice($parts, 0, $i));
                    if (file_exists($parentPath) === TRUE) {
                        $parentPath .= '/'.$parts[$i];
                        break;
                    }
                }

                self::$changedFiles[$pathname]['starting'] = $parentPath;
            }
        } else {
            $ret = mkdir($pathname, $mode, $recursive);
            return $ret;
        }//end if

        self::$changedFiles[$pathname]['result'] = mkdir(
            $pathname,
            $mode,
            $recursive
        );

        return self::$changedFiles[$pathname]['result'];

    }//end mkdir()


    /**
     * Sets access and modification time of file.
     *
     * @param string  $filename The name of the file being touched.
     * @param integer $time     The touch time. If time  is not supplied,
     *                          the current system time is used.
     * @param boolean $atime    If present, the access time of the given
     *                          filename is set to the value of atime.
     *                          Otherwise, it is set to time .
     *
     * @return integer
     */
    public static function touch($filename, $time=0, $atime=0)
    {
        if ($time === 0) {
            $time = time();
        }

        if ($atime === 0) {
            $ret = touch($filename, $time);
        } else {
            $ret = touch($filename, $time, $atime);
        }

        if (isset(BaseSystem::$dataId) === TRUE
            && BaseSystem::$dataId === 'testing'
        ) {
            self::$changedFiles[$filename] = array(
                                              'type'   => 'touch',
                                              'time'   => $time,
                                              'atime'  => $atime,
                                              'result' => $ret,
                                             );
        }

        return $ret;

    }//end touch()


    /**
     * Remove shell comment lines which start with '#' characters.
     *
     * @param string $string String to remove comment lines.
     *
     * @return string
     */
    public static function removeShellCommentLines($string)
    {
        $result = '';
        $lines  = explode("\n", $string);
        foreach ($lines as $line) {
            $trimmed = trim($line);
            if (empty($trimmed) === TRUE) {
                $result .= "\n";
            } else if ($line[0] !== '#') {
                $result .= $line."\n";
            }
        }

        return $result;

    }//end removeShellCommentLines()


}//end class

?>
