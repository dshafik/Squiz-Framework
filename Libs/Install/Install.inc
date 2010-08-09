<?php
/**
 * Install Lib.
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

require_once 'DAL/DALBaker.inc';
require_once 'Libs/FileSystem/FileSystem.inc';
require_once 'Channels/ChannelsBaker.inc';
require_once 'Channels/Channels.inc';
require_once 'Systems/BaseSystem.inc';

/**
 * Install Library.
 *
 * This class contains methods that are useful during installation but nowhere
 * else.
 *
 */
class Install
{


    /**
     * Lists all of the directories in the provided path.
     *
     * This is currently only available in this script, but if it is needed
     * elsewhere, it should be moved into the FileSystem Lib.
     *
     * @param string $path The path of the directory to list.
     *
     * @return array
     */
    public static function listNestedDirectories($path)
    {
        $systemNames = array();
        $dir         = new DirectoryIterator($path);
        while ($dir->valid() === TRUE) {
            if ($dir->isDot() === TRUE) {
                // This is '.' or '..'. Not interested.
                $dir->next();
                continue;
            }//end if

            if ($dir->isDir() === TRUE) {
                $fileName = $dir->getFileName();
                if ($fileName !== '.svn' && $fileName !== 'inclue') {
                    $systemNames[] = array(
                                      'name' => $fileName,
                                      'path' => $dir->getPathName(),
                                     );
                }
            }//end if

            $dir->next();
        }//end while

        return $systemNames;

    }//end listNestedDirectories()


    /**
     * Returns the systems data dir path.
     *
     * @param string $rootDir The root path.
     * @param string $id      Unique id of data directory.
     *
     * @return string
     */
    public static function getDataDirPath($rootDir, $id='')
    {
        if (empty($id) === FALSE) {
            // Mainly used for Developer system/Unit test to create an
            // alternate data directory.
            $dataDir = $rootDir.'/'.$id.'Data';
        } else {
            $dataDir = $rootDir.'/data';
        }

        return $dataDir;

    }//end getDataDirPath()


    /**
     * Create data directory and init.inc file.
     *
     * @param string $rootDir The root path.
     * @param string $url     The root URL.
     * @param string $id      Unique id of data directory.
     *
     * @return void
     */
    public static function prepareDataDir($rootDir, $url, $id='')
    {
        $dataDir = self::getDataDirPath($rootDir, $id);

        if (file_exists($dataDir) === FALSE) {
            include_once 'Libs/FileSystem/FileSystem.inc';
            FileSystem::mkdir($dataDir, 0755);
        }

        // Create data directory if it is not present.
        // @codingStandardsIgnoreStart
        $init = "<?php

        class Init
        {
            const ROOT_DIR       = '$rootDir';
            const WEB_DIR        = '$rootDir/web';
            const SKIN           = 'DefaultSkin';
            const EDIT_SUFFIX    = '_edit';
            const PROFILE_SUFFIX = '_profile';
            const MODE_SUFFIX    = '_mode';

            private static \$_rootUrl = NULL;

            public static function getBaseURL(\$force=FALSE)
            {
                if (self::\$_rootUrl !== NULL) {
                    return self::\$_rootUrl;
                }

                include_once 'Libs/Web/Web.inc';
                include_once 'Systems/BaseSystem.inc';
                \$currentUrl = Web::getCurrentUrl(FALSE);

                Channels::includeSystem('Lookup');
                \$rootUrl = DAL::getAssoc(Lookup::getClosestRootUrl(\$currentUrl));
                if (empty(\$rootUrl) === FALSE) {
                    self::\$_rootUrl = \$rootUrl[0]['url'];
                    return \$rootUrl[0]['url'];
                }

                if (\$force === TRUE) {
                    include BaseSystem::getDataDir().'/urls.inc';
                    return \$mysourceUrls[0];
                }

                return '';

            }//end getBaseURL()

            public static function getWebURL(\$force=FALSE)
            {
                \$baseUrl = Init::getBaseURL(\$force);
                if (\$baseUrl !== '') {
                    return \$baseUrl.'/__web/';
                }

                return '';

            }//end getWebUrl()

        }

        ?>";
        // @codingStandardsIgnoreEnd

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($dataDir.'/init.inc', $init);
        include_once $dataDir.'/init.inc';

        // Create cache URL file.
        $contents  = '<?php'."\n";
        $contents .= '$mysourceUrls = array('."\n";
        $contents .= '\''.$url.'\''."\n";
        $contents .= ');'."\n";
        $contents .= '?>'."\n";

        $filename = $dataDir.'/urls.inc';
        FileSystem::filePutContents($filename, $contents);

        // Create redirect file.
        $contents  = '<?php'."\n";
        $contents .= '$mysourceRedirect = array();'."\n";
        $contents .= '?>'."\n";

        $filename = $dataDir.'/redirect.inc';
        FileSystem::filePutContents($filename, $contents);

    }//end prepareDataDir()


    /**
     * Rebake Channels Oven.
     *
     * This will regenerate plugs.xml, systems.xml and systemsInfo.txt plus
     * all .xml and .inc files for all systems.
     *
     * @param string $rootDir The root path.
     * @param string $url     The root URL.
     * @param string $oven    Allows you to optionally spec a different
     *                        oven from the default.
     *
     * @return boolean
     */
    public static function rebakeOven($rootDir, $url, $oven='')
    {
        // Full oven rebake.
        if (empty($oven) === FALSE) {
            Channels::$channelsMode = $oven;
        }

        $systemNames = self::getInstallOptions(FALSE, FALSE);
        self::installSystems($systemNames);

        $tempFilePath = $rootDir.'/Channels/';
        if (empty($oven) === FALSE) {
            $tempFilePath .= $oven;
        }

        $tempFilePath .= 'Oven/systemsInfo.txt';
        unlink($tempFilePath);
        return TRUE;

    }//end rebakeOven()


    /**
     * Install the Channels code of the supplied systems.
     *
     * @param array $systemNames List of systems to install.
     *
     * @return array
     * @throws Exception If the inheritance validation fails.
     */
    public static function installSystems(array $systemNames)
    {
        // Add the systems to the Channels.
        ChannelsBaker::addSystems($systemNames);
        // Bake out the channels code.
        ChannelsBaker::cleanCodeBake();

        // Validate the systems.
        $systemNames = Channels::getSystemList(TRUE, TRUE);
        foreach ($systemNames as $sysName) {
            try {
                ChannelsBaker::validateSystemInheritance($sysName);
            } catch (Exception $e) {
                throw new Exception($e->getMessage());
            }
        }

        return TRUE;

    }//end installSystems()


    /**
     * Installs the supplied systems' queries and schemas.
     *
     * @param array $systemNames List of systems whose queries need to be
     *                           installed.
     *
     * @return void
     */
    public static function installQueries(array $systemNames)
    {
        foreach ($systemNames as $sys) {
            $name = basename($sys);
            self::installQuery($name);
        }

    }//end installQueries()


    /**
     * Installs the supplied systems' queries and schemas.
     *
     * First the schema of the system is read and created. Then the queries,
     * fragments and sub-queries are added from each system. Finally, the
     * queries code file is created.
     *
     * @param string $systemName Name of system whose queries need to be
     *                           installed.
     *
     * @return void
     */
    public static function installQuery($systemName)
    {
        DALBaker::addSystemSchema($systemName);
        DALBaker::addSystemQueries($systemName);
        DALBaker::bakeQueriesFile($systemName);

    }//end installQuery()


    /**
     * Installs functions/procedures and triggers for given systems.
     *
     * SQL Functions and triggers for the given system must be in the
     * Systems/<SystemName>../DbRelations/<databaseType>/ as .sql files. Each
     * file content will be passed to DAL::execute() method without any
     * modifications.
     *
     * Returns TRUE if everything was installed.
     * Exception thrown if there was database error.
     *
     * @param array $systems List of system paths.
     *
     * @return boolean
     * @throws Exception If the functions were unable to be installed for the
     *                   database.
     */
    public static function installSqlFunctions(array $systems)
    {
        $dbType  = DAL::getDbType(TRUE);
        $pattern = '/^(\w+)\.sql$/';
        $matches = array();

        // Add the libs Database dir.
        array_unshift($systems, 'Libs');

        foreach ($systems as $systemPath) {
            // Check if we have anything to install.
            if ($systemPath === 'Libs') {
                $path = dirname(dirname(__FILE__)).'/Database/'.$dbType;
            } else {
                $path = $systemPath.'/DB/Scripts/'.$dbType;
            }

            if (is_dir($path) !== TRUE) {
                continue;
            }

            $scriptPath = '';
            try {
                $di = new DirectoryIterator($path);
                // Get all the .sql files.
                foreach ($di as $file) {
                    if (preg_match($pattern, $file, $matches) !== 1) {
                        continue;
                    }

                    if (isset($matches[1]) === TRUE && $matches[1] !== 'uninstall') {
                        // Reset connection.
                        DAL::dbClose();
                        DAL::dbConnect();
                        $scriptPath = $file->getPathname();
                        // Get file contents and execute
                        // functions/triggers.
                        $sqlQueries = DAL::getSqlQueriesFromFile(
                            $file->getPathname()
                        );

                        foreach ($sqlQueries as $sql) {
                            $sql = trim($sql);
                            if (empty($sql) !== '') {
                                DAL::executeSql($sql);
                            }
                        }
                    }
                }//end foreach
            } catch (PDOException $e) {
                $msg  = 'DBError while executing SQL scripts in '.$scriptPath;
                $msg .= " :\n".$e->getMessage();
                throw new Exception($msg);
            }//end try
        }//end foreach

        return TRUE;

    }//end installSqlFunctions()


    /**
     * Creates sample project.
     *
     * @param string $url The root URL for this project.
     * @param string $dir The root path of this project.
     *
     * @return void
     * @throws Exception Error occurred during sample project creation.
     */
    public static function installSampleProject($url='', $dir='')
    {
        if (file_exists($dir.'/SampleProjectImport.inc') === FALSE) {
            return;
        }

        try {
            include_once $dir.'/SampleProjectImport.inc';
            $obj = new SampleProjectImport($url, $dir);
            $obj->createSampleProject($url, $dir);
        } catch (PDOException $e) {
            $msg  = 'Sample Project creation failed while running ';
            $msg .= $dir.'/SampleProjectImport.inc'."\n";
            $msg .= $e->getMessage();
            throw new Exception($msg);
        }

    }//end installSampleProject()


    /**
     * Returns list of real systems.
     *
     * Array('systemName' => 'path')
     *
     * @param string $systemPath Inspect this path for systems.
     *
     * @return array
     * @throws Exception If the actions.xml file is invalid.
     */
    public static function getRealSystems($systemPath=NULL)
    {
        if ($systemPath === NULL) {
            $systemPath = dirname(dirname(dirname(__FILE__))).'/Systems';
        }

        $systems  = array();
        $incFiles = FileSystem::listDirectory($systemPath, array('.inc'));

        $sysname = basename($systemPath);
        foreach ($incFiles as $incPath) {
            $baseName = basename($incPath);
            if (strpos($baseName, 'Actions.inc') === FALSE) {
                continue;
            }

            // We don not install systems in the Tests directory!
            if (stristr($incPath, '/Tests/') !== FALSE) {
                continue;
            }

            $name           = str_replace('Actions.inc', '', $baseName);
            $systems[$name] = dirname($incPath);
        }//end foreach

        return $systems;

    }//end getRealSystems()


    /**
     * Prompts the user and retrieves options for the install operation.
     *
     * Returns an array of systemNames to install.
     *
     * @param boolean $clearChannels Whether to clear the Channels oven or not.
     * @param boolean $clearDAL      Whether to clear the DAL oven or not.
     *
     * @return array
     */
    public static function getInstallOptions(
        $clearChannels=TRUE,
        $clearDAL=TRUE
    ) {
        $realSystems = self::getRealSystems();

        $systemString = '';

        if ($systemString === '') {
            $systemNames = $realSystems;
        } else {
            $systemNames = explode(' ', $systemString);
            foreach ($systemNames as $index => $name) {
                if (array_key_exists($name, $realSystems) === TRUE) {
                    $systemNames[$index] = $realSystems[$name];
                }
            }
        }//end if

        if ($clearChannels === TRUE) {
            FileSystem::clearDirectory(Channels::getOvenPath());
        }

        if ($clearDAL === TRUE) {
            FileSystem::clearDirectory(DAL::getOvenPath());
        }

        $systemNames = self::getInstallOrder($systemNames);
        return $systemNames;

    }//end getInstallOptions()


    /**
     * Resturns list of systems that are required by the specified system.
     *
     * @param string $systemName Name of the system.
     * @param string $path       Path of the system.
     * @param string $tables     List of system tables.
     *
     * @return array
     */
    private static function _getSystemDependencies($systemName, $path, $tables)
    {
        $deps = array();
        if (file_exists($path.'/DB/schema.xml') === TRUE) {
            $schemaXml = new DomDocument();
            $schemaXml->load($path.'/DB/schema.xml');
            $foreignKeys = $schemaXml->getElementsByTagName('foreign-key');

            foreach ($foreignKeys as $fk) {
                $fkTable = $fk->getAttribute('foreign-table');
                if (isset($tables[$fkTable]) === TRUE) {
                    if ($tables[$fkTable] !== $systemName) {
                        $deps[] = $tables[$fkTable];
                    }
                }
            }
        }

        $uniqueDeps = array_unique($deps);
        return $uniqueDeps;

    }//end _getSystemDependencies()


    /**
     * Orders the passed systems according to their dependencies.
     *
     * Gets the schemas of all the supplied systems, along with the already
     * existing systems, and determines in what order to install these
     * systems.
     *
     * @param array $systemNames The array('system_name' => 'path') containing
     *                           the systems to be ordered.
     *
     * @return array
     */
    public static function getInstallOrder(array $systemNames)
    {
        $tables = array();
        foreach ($systemNames as $name => $path) {
            $sysName               = $name;
            $systemPaths[$sysName] = $path;
            // Check the schema exists.
            if (file_exists($path.'/DB/schema.xml') === TRUE) {
                $schemaXml = new DomDocument();
                $schemaXml->load($path.'/DB/schema.xml');
                $schemaTables = $schemaXml->getElementsByTagName('table');
                foreach ($schemaTables as $table) {
                    $tableName          = $table->getAttribute('name');
                    $tables[$tableName] = $sysName;
                }
            }
        }//end foreach

        $depList = array();
        $list    = array();
        foreach ($systemNames as $name => $path) {
            $list[]         = $name;
            $depList[$name] = self::_getSystemDependencies(
                $name,
                $path,
                $tables
            );
        }

        // Should we check if there is a circular ref?
        $orderedList = array();
        $ordered     = self::orderDependencyList($list, $depList);
        foreach ($ordered as $systemName) {
            $orderedList[$systemName] = $systemNames[$systemName];
        }

        return $orderedList;

    }//end getInstallOrder()


    /**
     * Sorts the given list of elements using specified dependencies.
     *
     * First argument : array(A, B, C)
     * Second argument: array(A => array(B, C), B => array(),
     *                        C => array(B))
     *
     * @param array $list         List of elements to sort.
     * @param array $dependencies List of dependencies for the first argument.
     *
     * @return array
     */
    public static function orderDependencyList(array $list, array $dependencies)
    {
        if ((empty($list) === TRUE) || (empty($dependencies) === TRUE)) {
            return $list;
        }

        $curr = reset($list);
        do {
            $index   = key($list);
            $name    = $curr;
            $changed = FALSE;
            foreach ($dependencies[$name] as $dep) {
                $key = array_search($dep, $list);
                if ($key !== FALSE) {
                    if ($key > $index) {
                        unset($list[$key]);
                        array_unshift($list, $dep);
                        $changed = TRUE;
                    }
                }
            }

            if ($changed === TRUE) {
                $curr = reset($list);
            } else {
                $curr = next($list);
            }
        } while ($curr !== FALSE);

        return $list;

    }//end orderDependencyList()


    /**
     * Adds an asset type to the array of 'type_code' => 'path'.
     *
     * @param string $typeCode The type of the new asset.
     * @param string $path     The path of its' definition.
     *
     * @return boolean
     */
    public static function addAssetPath($typeCode, $path)
    {
        if ($typeCode === '' || $path === '') {
            return FALSE;
        }

        $paths             = array();
        $assetTypeDataPath = BaseSystem::getDataDir('AssetType');

        $fileName = $assetTypeDataPath.'/AssetPaths.inc';
        if (file_exists($fileName) === TRUE) {
            include $fileName;
        }

        $paths[strtolower($typeCode)] = $path;

        $exported = var_export($paths, TRUE);
        $output   = '<?php $paths = '.$exported.';  ?>';
        file_put_contents($fileName, $output);
        return TRUE;

    }//end addAssetPath()


    /**
     * Runs the install() method of each of the passed systems.
     *
     * @param array $systemNames The array('path') containing the systems to be
     *                           installed.
     *
     * @return void
     * @throws Exception If the system file cannot be found for one of the
     *                   passed systems.
     */
    public static function runSystemInstallMethods(array $systemNames)
    {
        // Works out the installation dependencies using a DAG.
        include_once 'Libs/Datastructure/Graph/DirectedAcyclicGraph.inc';
        $dep = Channels::getAllDependencies('install');
        $dag = new DirectedAcyclicGraph();
        foreach ($dep as $d) {
             $dag->connectNodes($d['from'], $d['to']);
        }

        $depOrder = $dag->getDependencySafeList();
        $listA    = array_flip($depOrder);
        $listB    = array();

        // Goes through all systems, and generate a dependencies list.
        foreach ($systemNames as $sysName => $sysPath) {
            if (in_array($sysName, $depOrder) === TRUE) {
                // Easy to insert, and retain order index.
                unset($depOrder[$listA[$sysName]]);
                $listA[$sysName] = $sysPath;
            } else {
                $listB[$sysName] = $sysPath;
            }
        }

        // Only include the systems we are installing.
        foreach ($depOrder as $notFound) {
            unset($listA[$notFound]);
        }

        // List those without dependencies first (listB).
        $fileList = array_merge($listB, $listA);

        // Run install on each system's System file.
        foreach ($fileList as $sysName => $sysPath) {
            $className  = $sysName.'System';
            $systemFile = $sysPath.'/'.$className.'.inc';
            if (file_exists($systemFile) === TRUE) {
                include_once $systemFile;
                $sysClass = new $className;
                $sysClass->install();
            } else {
                throw new Exception('System file not found: '.$systemFile.'.');
            }
        }

    }//end runSystemInstallMethods()


    /**
     * Orders an array of asset types so they are in valid installation order.
     *
     * @param array $assetTypes An array of asset types to order. Each entry
     *                          will be an array of the form:
     *                             array(
     *                              'type_code'   => type,
     *                              'parent_type' => parent_type_code,
     *                                          .....
     *                             )
     *                          Any additional information in the array will be
     *                          present in the resulting array, but is not
     *                          necessary for ordering.
     *
     * @return array
     */
    public static function orderAssets(array $assetTypes)
    {
        $dependencies = array();
        $lookups      = array();
        $ordered      = array();
        $list         = array();

        foreach ($assetTypes as $type) {
            $typeCode                = $type['type_code'];
            $lookups[$typeCode]      = $type;
            $list[]                  = $typeCode;
            $dependencies[$typeCode] = array($type['parent_type']);
        }

        $list = self::orderDependencyList($list, $dependencies);
        foreach ($list as $type) {
            $ordered[$type] = $lookups[$type];
        }

        return $ordered;

    }//end orderAssets()


    /**
     * Checks to see if the specified class is instanciable.
     *
     * Note that this function does not take into account a non-public
     * constructor for that object, it's designed simply to weed out
     * abstract classes and interfaces.
     *
     * This function needs to be relocated to a more fitting library
     * when one is developed.
     *
     * @param string $classname The name of the class you want to check.
     *
     * @return boolean
     */
    public static function isInstanciableClass($classname)
    {
        $class = new ReflectionClass($classname);

        $abstract  = $class->isAbstract();
        $interface = $class->isInterface();

        return (($abstract === FALSE) && ($interface === FALSE));

    }//end isInstanciableClass()


    /**
     * Copies system & installer web/ dir contents to web accessible directory.
     *
     * @param array $paths List of paths.
     *
     * @return void
     */
    public static function copyWebFiles(array $paths=array())
    {
        // Copy web dirs in Systems directory to root/web/ dir.
        include_once 'Libs/FileSystem/FileSystem.inc';

        if (empty($paths) === TRUE) {
            $paths = array(Init::ROOT_DIR.'/Systems' => 'Systems');
        }

        foreach ($paths as $systemsPath => $webName) {
            $webDirds = FileSystem::findDirectories($systemsPath, 'web');
            $webDir   = Init::WEB_DIR.'/'.$webName;
            foreach ($webDirds as $dir) {
                $baseName = FileSystem::getBaseName(substr($dir, 0, -4));
                $newPath  = $webDir.'/'.$baseName;

                // Create System web dir.
                if (file_exists($newPath) === FALSE) {
                     mkdir($newPath, 0755, TRUE);
                     @chmod($newPath, 0755);
                }

                FileSystem::copyDirectory($dir, $newPath);
            }
        }

        // Also update each asset type icons.
        if (Channels::systemExists('AssetType') === TRUE) {
            Channels::includeSystem('AssetType');
            AssetType::copyAssetTypeIcons();
        }

    }//end copyWebFiles()


    /**
     * Combines all CSS files and minify it.
     *
     * This function will create 3 files for different browser types.
     *
     * @return void
     */
    public static function minifyCSSFiles()
    {
        if (file_exists(Init::ROOT_DIR.'/Systems/Skin/Skins/defaultSkin') === FALSE) {
            return;
        }

        $cssFiles = array();
        $skins    = FileSystem::listDirectories(
            Init::ROOT_DIR.'/Systems/Skin/Skins/defaultSkin'
        );

        $mozFiles    = array();
        $ie7Files    = array();
        $ie8Files    = array();
        $webkitFiles = array();
        $mozCSS      = '';
        $ie7CSS      = '';
        $ie8CSS      = '';
        $webkitCSS   = '';

        $skins[''] = Init::ROOT_DIR.'/Systems/ImageGallery/web';
        foreach ($skins as $widgetName => $widgetPath) {
            $files = FileSystem::listDirectory($widgetPath, array('.css'));

            foreach ($files as $file) {
                if (strpos($file, '_moz.css') !== FALSE
                    || strpos($file, '_ie.css') !== FALSE
                    || strpos($file, '_ie7.css') !== FALSE
                    || strpos($file, '_ie8.css') !== FALSE
                    || strpos($file, '_webkit.css') !== FALSE
                ) {
                    continue;
                }

                $content = file_get_contents($file);
                $content = self::replaceURLforCSSminification($content);

                // ImageGallery has a custom CSS displayed in the backend vipers and
                // frontend together. For the backend display, it needs to be prefixed
                // for each WYSIWYG locations such as custom form edit or content sharing
                // WYSIWYG. This code does the whole thing at once.
                if ($file === 'ImageGallery.css') {
                    $editingPrefixed       = Util::replaceCSSPrefix($content, 'EditingAsset');
                    $screenPrefixed        = Util::replaceCSSPrefix($content, 'EditingScreenModes');
                    $astSettingsPrefixed   = Util::replaceCSSPrefix($content, 'EditEditingModeWidgetType-assetSettings');
                    $cntentSharingPrefixed = Util::replaceCSSPrefix($content, 'ViperContentSharingDialog');

                    $content .= $editingPrefixed;
                    $content .= $screenPrefixed;
                    $content .= $astSettingsPrefixed;
                    $content .= $cntentSharingPrefixed;
                }

                $mozFiles[]    = $file;
                $ie7Files[]    = $file;
                $ie8Files[]    = $file;
                $webkitFiles[] = $file;

                $mozCSS    .= $content;
                $ie7CSS    .= $content;
                $ie8CSS    .= $content;
                $webkitCSS .= $content;

                $mozCSSFilename    = str_replace('.css', '_moz.css', $file);
                $ieCSSFilename     = str_replace('.css', '_ie.css', $file);
                $ie7CSSFilename    = str_replace('.css', '_ie7.css', $file);
                $ie8CSSFilename    = str_replace('.css', '_ie8.css', $file);
                $webkitCSSFilename = str_replace('.css', '_webkit.css', $file);

                if (file_exists($mozCSSFilename) === TRUE) {
                    $bContent = file_get_contents($mozCSSFilename);
                    $bContent = self::replaceURLforCSSminification($bContent);
                    $mozCSS  .= $bContent;
                } else if (file_exists($ieCSSFilename) === TRUE) {
                    $bContent = file_get_contents($ieCSSFilename);
                    $bContent = self::replaceURLforCSSminification($bContent);
                    $ie7CSS  .= $bContent;
                    $ie8CSS  .= $bContent;
                } else if (file_exists($ie7CSSFilename) === TRUE) {
                    $bContent = file_get_contents($ie7CSSFilename);
                    $bContent = self::replaceURLforCSSminification($bContent);
                    $ie7CSS  .= $bContent;
                } else if (file_exists($ie8CSSFilename) === TRUE) {
                    $bContent = file_get_contents($ie8CSSFilename);
                    $bContent = self::replaceURLforCSSminification($bContent);
                    $ie8CSS  .= $bContent;
                } else if (file_exists($webkitCSSFilename) === TRUE) {
                    $bContent   = file_get_contents($webkitCSSFilename);
                    $bContent   = self::replaceURLforCSSminification($bContent);
                    $webkitCSS .= $bContent;
                }//end if
            }//end foreach
        }//end foreach

        include_once 'Libs/Util/Util.inc';
        file_put_contents(
            Init::WEB_DIR.'/styles_moz.css',
            Util::minifyCSS($mozCSS, 80)
        );
        file_put_contents(
            Init::WEB_DIR.'/styles_ie7.css',
            Util::minifyCSS($ie7CSS, 80)
        );
        file_put_contents(
            Init::WEB_DIR.'/styles_ie8.css',
            Util::minifyCSS($ie8CSS, 80)
        );
        file_put_contents(
            Init::WEB_DIR.'/styles_webkit.css',
            Util::minifyCSS($webkitCSS, 80)
        );

    }//end minifyCSSFiles()


    /**
     * Replace url() values in CSS for the minification.
     *
     * @param string $content Content to replace.
     *
     * @return string
     */
    public static function replaceURLforCSSminification($content)
    {
        $matches = array();
        preg_match_all('/url(\([^\)]+\))/', $content, $matches);
        $matches = array_unique($matches[1]);
        foreach ($matches as $fromStr) {
            if (strpos($fromStr, '../../') === 0) {
                $newFromStr = '('.str_replace('../../', '', $fromStr).')';
                $content    = str_replace($fromStr, $newFromStr, $content);
            } else if (strpos($fromStr, '../') === 0) {
                $newFromStr = '('.str_replace('../', '', $fromStr).')';
                $content    = str_replace($fromStr, $newFromStr, $content);
            } else {
                $content = str_replace(
                    $fromStr,
                    '(Skins/defaultSkin/'.rtrim(ltrim($fromStr, '('), ')').')',
                    $content
                );
            }
        }

        return $content;

    }//end replaceURLforCSSminification()


    /**
     * Combines all the javascript files and create one single file.
     *
     * By default it create mysource.js.gz file in web directory.
     *
     * @param boolean $minify TRUE minify the javascript.
     * @param boolean $gzip   TRUE gzip the javascript file.
     *
     * @return void
     */
    public static function minifyGzipJSFiles($minify=TRUE, $gzip=TRUE)
    {
        if (file_exists(Init::ROOT_DIR.'/Resources/jsmin-1.1.1.php') === FALSE) {
            return;
        }

        if (file_exists(Init::ROOT_DIR.'/Libs/Datastructure/Graph/DirectedAcyclicGraph.inc') === FALSE) {
            return;
        }

        include_once Init::ROOT_DIR.'/Resources/jsmin-1.1.1.php';
        include_once Init::ROOT_DIR.'/Libs/Datastructure/Graph/DirectedAcyclicGraph.inc';
        $dag = new DirectedAcyclicGraph();

        $nonWdgtJsContent = '';
        $viperJsContent   = '';
        $wdgtJsContent    = '';
        $dfxJsContent     = '';
        $jsContentsArr    = array();
        $jsFiles          = self::getAllJSFiles();

        // Handle all widget JS files first.
        foreach ($jsFiles as $filename => $file) {
            $jsContent = file_get_contents($file);

            if (strpos($file, 'WidgetType.js') !== FALSE) {
                $jsClassName = str_replace('WidgetType.js', '', $filename);
                if ($jsClassName === 'AbstractWidget') {
                    $jsClassName = 'Widget';
                }

                $jsContentsArr[$jsClassName] = file_get_contents($file);

                // Widget type JS file.
                $pattern = '/(dfx\.inherits\(\'([^\']+)\', \'([^\']+)\'\);)/';
                $matches = array();
                preg_match_all($pattern, $jsContent, $matches);
                if (empty($matches[0]) === FALSE) {
                    $dag->connectNodes($matches[2][0], $matches[3][0]);
                    $inheritsCode[$jsClassName] = $matches[1][0];
                } else {
                    try {
                        $dag->addNode($jsClassName);
                    } catch (Exception $e) {
                        // When the node is already been added before,
                        // DirectedAcyclicGraph will throw an exception.
                        // We can ignore this.
                    }
                }
            }//end if
        }//end foreach

        // Handle non-widget JS files.
        $systems = Channels::getSystemList(TRUE, FALSE, FALSE);
        foreach ($systems as $sysName => $sysPath) {
            $systemClassName = $sysName.'System';
            include_once $sysPath.'/'.$systemClassName.'.inc';
            $sysObj  = new $systemClassName;
            $jsFiles = $sysObj->getDependencySortedJSFiles($sysPath);
            foreach ($jsFiles as $filename => $file) {
                $jsContent = file_get_contents($file);
                if (strpos($file, 'WidgetType.js') !== FALSE) {
                    // Ignore file.
                    continue;
                } else if (strpos($file, 'WYSIWYGEditorWidget') !== FALSE) {
                    $viperJsContent .= $jsContent."\n";
                } else {
                    $nonWdgtJsContent .= $jsContent."\n";
                }
            }

            if ($sysName === 'DfxJSLib') {
                $dfxFiles = $sysObj->getDfxJSFiles();
                foreach ($dfxFiles as $dfxFilename) {
                    $dfxFilePath   = dirname(__FILE__).'/../../Systems/DfxJSLib/web/';
                    $dfxFilePath  .= $dfxFilename;
                    $dfxJsContent .= file_get_contents($dfxFilePath);
                }
            }
        }//end foreach

        $depOrder = $dag->getDependencySafeList();
        foreach ($depOrder as $jsClassName) {
            if (isset($jsContentsArr[$jsClassName]) === TRUE) {
                $wdgtJsContent .= $jsContentsArr[$jsClassName]."\n";
            } else {
                echo $jsClassName.' content not found'."\n";
            }
        }

        $wdgtJsContent = $nonWdgtJsContent."\n".$wdgtJsContent."\n".$viperJsContent;
        if ($minify === TRUE) {
            // @codingStandardsIgnoreStart
            // JSMin is the 3rd party library file and the way we include is
            // different from MySource standards.
            $wdgtJsContent = JSMin::minify($wdgtJsContent);
            // @codingStandardsIgnoreEnd
        }

        file_put_contents(Init::ROOT_DIR.'/data/mysource.js', $wdgtJsContent);
        file_put_contents(Init::ROOT_DIR.'/data/dfx.js', $dfxJsContent);
        if ($gzip === TRUE) {
            $gzipCommand  = 'gzip -c -n '.Init::ROOT_DIR.'/data/mysource.js > ';
            $gzipCommand .= Init::WEB_DIR.'/mysource.jgz';
            system($gzipCommand);

            $gzipCommand  = 'gzip -c -n '.Init::ROOT_DIR.'/data/dfx.js > ';
            $gzipCommand .= Init::WEB_DIR.'/dfx.jgz';
            system($gzipCommand);
        } else {
            $copyCommand  = 'cp '.Init::ROOT_DIR.'/data/mysource.js ';
            $copyCommand .= Init::WEB_DIR.'/mysource.js';
            system($copyCommand);

            $copyCommand  = 'cp '.Init::ROOT_DIR.'/data/dfx.js ';
            $copyCommand .= Init::WEB_DIR.'/dfx.js';
            system($copyCommand);
        }

        unlink(Init::ROOT_DIR.'/data/mysource.js');
        unlink(Init::ROOT_DIR.'/data/dfx.js');

    }//end minifyGzipJSFiles()


    /**
     * Remove minified JS file from web directory.
     *
     * @return void
     */
    public static function removeMinifiedJSFile()
    {
        $minifiedFilePath = Init::WEB_DIR.'/mysource.js';
        if (file_exists($minifiedFilePath) === TRUE) {
            unlink($minifiedFilePath);
        }

    }//end removeMinifiedJSFile()


    /**
     * Remove minified gzipped JS file from web directory.
     *
     * @return void
     */
    public static function removeGzippedJSFile()
    {
        $minifiedFilePath = Init::WEB_DIR.'/mysource.jgz';
        if (file_exists($minifiedFilePath) === TRUE) {
            unlink($minifiedFilePath);
        }

    }//end removeGzippedJSFile()


    /**
     * Remove minified gzipped JS file from web directory.
     *
     * @return void
     */
    public static function removeMinifiedCSSFiles()
    {
        $files = array(
                  'styles_moz.css',
                  'styles_ie7.css',
                  'styles_ie8.css',
                 );

        foreach ($files as $file) {
            $cssFilePath = Init::WEB_DIR.'/'.$file;
            if (file_exists($cssFilePath) === TRUE) {
                unlink($cssFilePath);
            }
        }

    }//end removeMinifiedCSSFiles()


    /**
     * Returns the list of all javascrpt files in the system.
     *
     * @return array
     */
    public static function getAllJSFiles()
    {
        $jsFiles = array();
        $rootDir = Init::ROOT_DIR;
        $paths   = array($rootDir.'/Systems' => 'Systems');
        foreach ($paths as $systemsPath => $webName) {
            $webDirs = FileSystem::findDirectories($systemsPath, 'web');
            foreach ($webDirs as $dir) {
                $files = FileSystem::listDirectory($dir, array('.js'));
                foreach ($files as $file) {
                    $parts = explode('/', $file);
                    $filename = array_pop($parts);
                    $jsFiles[$filename] = $file;
                }
            }
        }

        ksort($jsFiles);
        return $jsFiles;

    }//end getAllJSFiles()


}//end class

?>