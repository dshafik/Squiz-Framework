<?php
/**
 * Base System from which all Systems extend.
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
 * @subpackage Systems
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(dirname(__FILE__)).'/DAL/DAL.inc';

/**
 * Base System class.
 *
 * This class implements installation methods that are generic across all
 * systems. Extending classes in each system should call these methods, and then
 * extend them by adding operations that are specific to that system. These
 * methods will be run after a system's schema, channels code and queries code
 * are created, so things like adding new asset types should be done here.
 * Each system should have a file in its' base directory containing a class that
 * extends this class.
 */
class BaseSystem
{

    /**
     * The system options.
     *
     * @var array
     */
    protected $systemData = array(
                             'exclusive' => FALSE,
                             'databases' => array(
                                             'pgsql',
                                             'mysql',
                                             'dblib',
                                             'oci',
                                             'db2',
                                            ),
                            );

    /**
     * Unique id for data directory.
     *
     * @var array
     */
    public static $dataId = '';

    /**
     * Data dir cache.
     *
     * @var array
     */
    private static $_dataDirs;

    /**
     * Addons cache.
     *
     * @var array
     */
    private static $_addons = array();


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Returns the all system options.
     *
     * @return array
     */
    public function getAllSystemOptions()
    {
        return $this->systemData;

    }//end getAllSystemOptions()


    /**
     * Returns the option for the given key.
     *
     * @param string $key The key for the system option.
     *
     * @return mixed
     */
    protected function getSystemOption($key)
    {
        if (isset($this->systemData[$key]) === TRUE) {
            return $this->systemData[$key];
        }

        return NULL;

    }//end getSystemOption()


    /**
     * Sets the option for the given key.
     *
     * @param string $key The key for the system option.
     * @param mixed  $val The value for the system option.
     *
     * @return void
     */
    protected function setSystemOption($key, $val)
    {
        $this->systemData[$key] = $val;

    }//end setSystemOption()


    /**
     * Returns the data directory for the specified system name.
     *
     * The data dir can be used to store information related to the system.
     *
     * @return string
     */
    public static function getSystemRootPath()
    {
        return Init::ROOT_DIR;

    }//end getSystemRootPath()


    /**
     * Returns the data directory for the specified system name.
     *
     * The data dir can be used to store information related to the system.
     *
     * @param string $system The system name to retrieve the path for.
     *
     * @return string
     */
    public static function getDataDir($system='')
    {
        if (isset(self::$_dataDirs[self::$dataId]) === TRUE
            && isset(self::$_dataDirs[self::$dataId][$system]) === TRUE
        ) {
            return self::$_dataDirs[self::$dataId][$system];
        }

        $rootDir = dirname(dirname(__FILE__));

        if (self::$dataId === '') {
            $rootDir .= '/data';
        } else {
            $rootDir .= '/'.self::$dataId.'Data';
        }

        if ($system !== '') {
            $rootDir .= '/'.$system;
        }

        if (isset(self::$_dataDirs[self::$dataId]) === FALSE) {
            self::$_dataDirs[self::$dataId] = array();
        }

        self::$_dataDirs[self::$dataId][$system] = $rootDir;

        return $rootDir;

    }//end getDataDir()


    /**
     * Run any operations that are needed after this system is installed.
     *
     * @return boolean
     */
    public function install()
    {
        $className = get_class($this);
        if (substr($className, -6) === 'System') {
            $sys = substr($className, 0, -6);
        } else {
            $sys = $className;
        }

        // Create a system directory for this system.
        $sysDir = self::getDataDir($sys);
        if (file_exists($sysDir) === FALSE) {
            include_once 'Libs/FileSystem/FileSystem.inc';
            FileSystem::mkdir($sysDir, 0755, TRUE);
        }

        return TRUE;

    }//end install()


    /**
     * Run any operations needed to remove any trace of this system.
     *
     * @param string $cleanOnly If TRUE will empty out this system's tables, but
     *                          will not completely remove them.
     *
     * @return void
     * @throws Exception If there is a problem uninstalling the system from
     *                   the database.
     */
    public function uninstall($cleanOnly=FALSE)
    {
        $className = get_class($this);
        if (substr($className, -6) === 'System') {
            $sys = substr($className, 0, -6);
        } else {
            $sys = $className;
        }

        $path = Channels::getSystemsPath($sys);
        // Uninstall DB Scripts.
        $uninstallFile  = $path.'/DB/Scripts/';
        $uninstallFile .= DAL::getDbType(TRUE).'/uninstall.sql';
        $uninstall      = DAL::getSqlQueriesFromFile($uninstallFile);
        try {
            DAL::beginTransaction();
            foreach ($uninstall as $sql) {
                DAL::executeSql($sql);
            }

            DAL::commit();
        } catch (PDOException $e) {
            DAL::rollBack();
            throw new Exception($e->getMessage());
        }

        // Remove the tables.
        $tables    = array();
        $sequences = array();
        if (file_exists($path.'/DB/schema.xml') === TRUE) {
            $schemaXml  = new DomDocument();
            $schemaXml  = $schemaXml->load($path.'/DB/schema.xml');
            $tableNodes = $schemaXml->getElementsByTagName('table');
            foreach ($tableNodes as $node) {
                $tables[] = $node->getAttribute('name');
            }

            $sequenceNodes = $schemaXml->getElementsByTagName('sequence');
            foreach ($sequenceNodes as $node) {
                $sequences[] = $node->getAttribute('name');
            }
        }

        $tables    = array_reverse($tables);
        $converter = DAL::getConverter();
        foreach ($tables as $tableName) {
            $tableExists = $converter->handleFunctionTableExists('\''.$tableName.'\'', '\''.DAL::getDbName().'\'');
            $result      = DAL::query($tableExists);
            $row         = $result->fetch();
            $tableCount  = (int) current($row);
            if ($tableCount !== 0) {
                if ($cleanOnly === FALSE) {
                    $sql = $converter->convertDropTable($tableName, TRUE);
                } else {
                    $sql = $converter->convertTruncateTable($tableName);
                }

                try {
                    DAL::beginTransaction();
                    DAL::executeSql($sql);
                    DAL::commit();
                } catch (PDOException $e) {
                    DAL::rollBack();
                    throw new Exception($e->getMessage());
                }
            }//end if
        }//end foreach

        $sequences = array_reverse($sequences);
        foreach ($sequences as $sequenceName) {
            $seqExists     = $converter->handleFunctionSequenceExists('\''.$sequenceName.'\'');
            $result        = DAL::query($seqExists);
            $row           = $result->fetchAll();
            $sequenceCount = (int) current($row);
            if ($result !== 0) {
                if ($cleanOnly === FALSE) {
                    $sql = $converter->convertDropSequence($sequenceName);
                } else {
                    $sql = $converter->convertResetSequence($sequenceName);
                }

                try {
                    DAL::beginTransaction();
                    DAL::executeSql($sql);
                    DAL::commit();
                } catch (PDOException $e) {
                    // Ignore.
                    DAL::rollBack();
                }
            }//end if
        }//end foreach

    }//end uninstall()


    /**
     * Cleans away this system.
     *
     * @return void
     */
    public function clean()
    {
        $this->uninstall(TRUE);

    }//end clean()


    /**
     * Finds all files or dirs of a certain name residing under enabled systems.
     *
     * If $addonName is passed as 'DesignAreas' and $directories is TRUE and
     * Sys1 and Sys2 are the only two enabled systems with this directory
     * defined, the resulting array would be:
     *          array(
     *           'Sys1' => '/path/to/system/Sys1',
     *           'Sys2' => '/path/to/system/Sys2',
     *          );
     *
     * @param string  $addonName   The name of the directory to search for.
     * @param boolean $directories If TRUE addon directories will be checked, if
     *                             FALSE or omitted then only files will be
     *                             checked and returned.
     *
     * @return array
     */
    public static function getAddons($addonName, $directories=TRUE)
    {
        if (isset(self::$_addons[$addonName]) === TRUE) {
            return self::$_addons[$addonName];
        }

        // Left as variables as this may be assigned to a parameter soon.
        $enabled    = TRUE;
        $interfaces = TRUE;
        // Retrieve the paths of the systems required.
        $systems = Channels::getSystemList($enabled, $interfaces, FALSE);
        $addons  = array();
        foreach ($systems as $name => $path) {
            $addonPath = $path.'/'.$addonName;
            if ($directories === TRUE) {
                if (is_dir($addonPath) === TRUE) {
                    $addons[$name] = $addonPath;
                }
            } else {
                if (is_file($addonPath) === TRUE) {
                    $addons[$name] = $addonPath;
                }
            }
        }

        self::$_addons[$addonName] = $addons;

        return $addons;

    }//end getAddons()


    /**
     * Returns the list of all Javascript files under web directory.
     *
     * @param string $sysPath Path to system directory.
     *
     * @return array
     */
    public function getJSFiles($sysPath)
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $jsFiles = FileSystem::listDirectory($sysPath, array('.js'));
        return $jsFiles;

    }//end getJSFiles()


    /**
     * Returns the list of Javascript files regarding the inclusion dependency.
     *
     * The order of the file list is in fact meaningful.
     *
     * @param string $sysPath Path to system directory.
     *
     * @return array
     * @throws Exception When a JS file returned from each system
     *                   getJSDependencyList() function does not exist in web
     *                   directory.
     */
    public function getDependencySortedJSFiles($sysPath)
    {
        $jsFilesPaths = $this->getJSFiles($sysPath);
        $jsFiles      = array();
        foreach ($jsFilesPaths as $path) {
            $jsFiles[basename($path)] = $path;
        }

        $finalList = array();
        if (empty($jsFiles) === FALSE) {
            $depList           = $this->getJSDependencyList();
            $jsFileNames       = array_keys($jsFiles);
            $fileDoesNotExists = array_diff($depList, $jsFileNames);
            if (empty($fileDoesNotExists) === FALSE) {
                $msg  = 'Some files listed in '.$sysPath.' system`s ';
                $msg .= 'getJSDependencyList() function does not exists in web';
                $msg .= ' directory.';
                throw new Exception($msg);
            }

            $noDepFiles = array_diff($jsFileNames, $depList);

            // Make sure that the files in dependency list comes first.
            $finalList = array_merge($depList, $noDepFiles);
        }

        $result = array();
        if (empty($finalList) === FALSE) {
            foreach ($finalList as $filename) {
                $result[$filename] = $jsFiles[$filename];
            }

            return $result;
        }

        return $jsFiles;

    }//end getDependencySortedJSFiles()


    /**
     * List of Javascript files which have dependencies.
     *
     * Each system only needs to list the files with dependencies.
     *
     * @return array
     */
    public function getJSDependencyList()
    {
        $list = array();
        return $list;

    }//end getJSDependencyList()


    /**
     * Returns TRUE if all Javascript files are minified.
     *
     * @return boolean
     */
    public static function isJavascriptMinified()
    {
        if (file_exists(Init::WEB_DIR.'/mysource.js') === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end isJavascriptMinified()


    /**
     * Returns TRUE if all Javascript files are minified and gzipped.
     *
     * @return boolean
     */
    public static function isJavascriptGzipped()
    {
        if (file_exists(Init::WEB_DIR.'/mysource.jgz') === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end isJavascriptGzipped()


    /**
     * Returns TRUE if all CSS files are minified.
     *
     * @return boolean
     */
    public static function isCSSMinified()
    {
        if (file_exists(Init::WEB_DIR.'/styles_moz.css') === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end isCSSMinified()


}//end class

?>
