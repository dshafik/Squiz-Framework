<?php
/**
 * Actions for the GUI System.
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
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Actions Class.
 */
class GUIActions
{

    /**
     * The docType to use for DOMDocument.
     *
     * @var string
     */
    private static $_docType = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">';

    /**
     * The JS files to include.
     *
     * @var array
     */
    private static $_jsIncludes = array();

    /**
     * The CSS files to include.
     *
     * @var array
     */
    private static $_cssIncludes = array();

    /**
     * The name of the publicly accessible directory for systems.
     *
     * @var string
     */
    private static $_publicDirName = 'Web';

    /**
     * Variable to keep track of baking process.
     *
     * If TRUE then templates are being baked.
     *
     * @var boolean
     */
    private static $_isBaking = FALSE;

    /**
     * Current template key used during template baking.
     *
     * @var string
     */
    private static $_currentTemplateKey = '';

    /**
     * Template variable counter used during template baking.
     *
     * @var integer
     */
    private static $_templateVarCounter = 0;

    /**
     * Template variable initialisation codes.
     *
     * @var string
     */
    private static $_templateVariablesCode = '';

    /**
     * List of available widget settings.
     *
     * @var array
     */
    private static $_widgetSettings = array();

    /**
     * Template widget settings that can be accessed during run time.
     *
     * @var array
     * @see GUI::getWidgetSettings()
     */
    private static $_templateSettings = array();

    /**
     * List of browser types that are valid for the GUI system.
     *
     * Used to include browser specific CSS files.
     *
     * @var array
     */
    private static $_browserTypes = array(
                                     'chrome'  => NULL,
                                     'firefox' => NULL,
                                     'msie7'   => 'msie',
                                     'msie8'   => 'msie',
                                     'msie9'   => 'msie',
                                     'msie'    => NULL,
                                     'safari'  => NULL,
                                    );

    /**
     * Information about the cache tag that is being parsed.
     *
     * Its used to prevent widget initialisation code being printed when the content
     * is cached.
     *
     * @var array
     */
    private static $_cacheTagInfo = array();

    /**
     * If TRUE then the template that is being baked is used for re-caching.
     *
     * @var boolean
     */
    private static $_isReCacheTemplate = FALSE;

    /**
     * If TRUE then we are in the process of recaching a template.
     *
     * This is set and unset during the reCacheTemplate() function. It is not to be
     * confused with the above variable, which is used during bake time.
     *
     * @var boolean
     */
    private static $_isReCaching = FALSE;


    /**
     * Includes the template PHP file.
     *
     * Permission are handled by the templates themselves, so this
     * method is public.
     *
     * @param string $systemName Name of the system.
     * @param string $template   The template to load.
     *
     * @since  4.0.0
     * @return void
     *
     * @api            read
     * @api-permission public
     */
    public static function printTemplate($systemName, $template)
    {
        $dataPath = BaseSystem::getDataDir($systemName);
        $dest     = $dataPath.'/Templates/'.$template.'.php';

        // Start template output.
        if (self::$_isBaking === TRUE) {
            echo '<?php include_once "'.$dest.'"; ?>';
        } else {
            include_once $dest;
        }

    }//end printTemplate()


    /**
     * Loads the specified template file and returns it as DOMDocument.
     *
     * @param string $filePath The template file path.
     *
     * @since  4.0.0
     * @return DOMDocument
     */
    public static function loadTemplate($filePath)
    {
        if (file_exists($filePath) === FALSE) {
            return FALSE;
        }

        $fileCont = trim(file_get_contents($filePath));
        if (empty($fileCont) === TRUE) {
            return FALSE;
        }

        // Need to wrap content.
        // and declare &nbsp for templates containing a &nbsp.
        $declare   = '[ <!ENTITY nbsp "&#160;"> ]>';
        $contents  = self::$_docType;
        $contents  = str_replace('>', $declare, $contents);
        $contents .= '<gui_root_element>'.$fileCont.'</gui_root_element>';

        $tpl = new DOMDocument('1.0');
        $tpl->loadXML($contents);
        return $tpl;

    }//end loadTemplate()


    /**
     * Bakes all the templates in the Systems dir.
     *
     * @since  4.0.0
     * @return void
     */
    public static function bakeTemplates()
    {
        self::$_isBaking = TRUE;

        include_once 'Libs/FileSystem/FileSystem.inc';
        $root = BaseSystem::getSystemRootPath();
        $path = $root.'/Systems';

        // Get the tpl files.
        $templateDirs = FileSystem::findDirectories($path, 'Templates');

        if (empty($templateDirs) === TRUE) {
            return;
        }

        // Get All JS and CSS files in the system.
        $jsFiles  = self::_getJSFiles();
        $cssFiles = self::_getCSSFiles();

        // Convert them to URLs.
        foreach ($jsFiles as $index => $file) {
            $url = str_replace($root, '/__web', $file);
            $jsFiles[$index] = $url;
        }

        GUI::setJSIncludes($jsFiles);
        GUI::setCSSIncludes($cssFiles);

        sort($templateDirs);

        foreach ($templateDirs as $tplDir) {
            $tplFiles = FileSystem::listDirectory($tplDir, array('.tpl'));
            sort($tplFiles);

            $systemName = NULL;
            $syOvenPath = NULL;
            $bakeDest   = NULL;

            foreach ($tplFiles as $file) {
                // Get the system name for the template.
                if ($systemName === NULL) {
                    $systemName = GUI::getSystemNameFromTemplate($file);
                }

                $tplDoc = GUI::loadTemplate($file);
                if ($tplDoc !== NULL) {
                    $templateName = str_replace('.tpl', '', basename($file));
                    GUI::bakeTemplate($systemName, $templateName, $tplDoc);
                    $tplDoc = GUI::loadTemplate($file);
                    GUI::bakeTemplate($systemName, $templateName, $tplDoc, TRUE);
                }
            }
        }//end foreach

        self::_createWidgetSettingsFile();

        self::$_isBaking = FALSE;

    }//end bakeTemplates()


    /**
     * Returns array of JS files in the system.
     *
     * @return array
     */
    private static function _getJSFiles()
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $root        = BaseSystem::getSystemRootPath();
        $systemsPath = $root.'/Systems';
        $guiWebPath  = $systemsPath.'/GUI/Web';

        // First get the GUI and DfxJSLib, these need to come first.
        $guiJSFiles = FileSystem::listDirectory($guiWebPath, array('.js'));

        // Rest of the JS files.
        $jsFiles = FileSystem::listDirectory($systemsPath, array('.js'));

        $allFiles = array_merge($guiJSFiles, $jsFiles);
        $allFiles = array_unique($allFiles);

        // Remove the mysource.js not needed in Framework.
        foreach ($allFiles as $i => $file) {
            if (basename($file) === 'mysource.js') {
                unset($allFiles[$i]);
                break;
            }
        }

        return $allFiles;

    }//end _getJSFiles()


    /**
     * Returns array of CSS files in the system.
     *
     * @return array
     */
    private static function _getCSSFiles()
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $root = BaseSystem::getSystemRootPath();
        $path = $root.'/Systems';

        $cssFiles = FileSystem::listDirectory($path, array('.css'));

        $allCSSFiles = array('all' => array());
        foreach ($cssFiles as $index => $file) {
            $url = str_replace($root, '', $file);

            foreach (self::$_browserTypes as $bType => $parent) {
                if (strpos(basename($file), '.'.$bType.'.') !== FALSE) {
                    if (isset($allCSSFiles[$bType]) === FALSE) {
                        $allCSSFiles[$bType] = array();
                    }

                    $allCSSFiles[$bType][] = $url;
                    continue(2);
                }
            }

            $allCSSFiles['all'][] = $url;
        }

        return $allCSSFiles;

    }//end _getCSSFiles()


    /**
     * Bakes out the template's PHP file.
     *
     * @param string  $systemName        Name of the system that owns the template.
     * @param string  $templateName      Name of the template.
     * @param DOMNode $tplDoc            The HTML of the template as DOM object.
     * @param boolean $isReCacheTemplate If TRUE then the baked out template is used
     *                                   for re-caching templates, which does not
     *                                   include If conditions, exceptions etc.
     *
     * @since  4.0.0
     * @return void
     */
    public static function bakeTemplate($systemName, $templateName, DOMNode $tplDoc, $isReCacheTemplate=FALSE)
    {
        self::$_templateVariablesCode = '';
        self::$_isReCacheTemplate     = $isReCacheTemplate;
        self::$_currentTemplateKey    = '';

        $tplKey = $systemName.'::'.$templateName;

        // TODO: We may need to use a stack for this.
        self::$_currentTemplateKey = $tplKey;
        self::$_templateVarCounter = 0;

        $php  = '// Initialise template var.'."\n";
        $php .= 'if (isset($templateVars) === FALSE) $templateVars = array();'."\n";

        $tplVarName = $systemName.'_'.$templateName.'_prevTpl';
        $php       .= '// Set the current template key.'."\n";
        $php       .= '$'.$tplVarName.' = GUI::getCurrentTemplateKey();'."\n";
        $php       .= 'GUI::setCurrentTemplateKey('.var_export($tplKey, TRUE).");\n";

        $php .= self::_getVarCode().' = array();'."\n";

        // Parse the special tags in the template file.
        GUI::parseTemplate($tplDoc);

        $initCodes  = self::$_templateVariablesCode;
        $initCodes .= $php;

        $phpNode = $tplDoc->createProcessingInstruction('php', $initCodes);
        $tplDoc->insertBefore($phpNode, $tplDoc->firstChild);

        $resetCurrTemplateCode = 'GUI::setCurrentTemplateKey($'.$tplVarName.');'."\n";
        $phpNode = $tplDoc->createProcessingInstruction('php', $resetCurrTemplateCode);
        $tplDoc->appendChild($phpNode);

        $jsCode  = '';
        $cssCode = '';

        // Now add the dependent files to the HEAD tag, if it exists.
        $head = $tplDoc->getElementsByTagName('head');
        if ($head->length !== 0) {
            $head = $head->item(0);

            // Include JS files.
            $jsCode .= self::_getJSIncludeCode();

            // Include main CSS.
            $cssCode .= self::_getCSSIncludeCode('all');
            $cssCode .= "\n<"."?php \n";
            $cssCode .= "Channels::includeSystem('GUI'); \n";
            $cssCode .= '$_gui_browserType = GUI::getBrowserType();'."\n";
            $cssCode .= 'switch ($_gui_browserType) {'."\n";
            foreach (self::$_browserTypes as $browserType => $parentType) {
                $cssCode .= 'case \''.$browserType."':\n";
                $cssCode .= ' ?'.'>';
                $cssCode .= self::_getCSSIncludeCode($browserType)."\n";
                $cssCode .= '<'.'?php ';
                $cssCode .= "break;\n";
            }

            $cssCode .= "}\n?".'>';
        }//end if

        include_once 'Libs/XML/XML.inc';
        $xmlContent = $tplDoc->saveXML();
        // Remove the root element that was added when loading XML content from file.
        $xmlContent = str_replace('<gui_root_element>', '', $xmlContent);
        $xmlContent = str_replace('</gui_root_element>', '', $xmlContent);

        // Remove the &nbsp declaration from the DTD, as most browser do not implement this.
        $xmlContent = str_replace(" [\n<!ENTITY nbsp \"&#160;\">\n]>", '>', $xmlContent);

        $htmlCont = XML::xmlToXHTML($xmlContent);

        if ($head->length !== 0) {
            // Put the JS and CSS inclusion including browser specific CSS code.
            $htmlCont = str_replace('</head>', $cssCode.$jsCode.'</head>', $htmlCont);

            // The <head> element exists, insert API token hidden input field
            // for the interface to work.
            $replacement  = '<body>'."\n";
            $replacement .= '<?php '."\n";
            $replacement .= 'Channels::includeSystem(\'User\');'."\n";
            $replacement .= 'Channels::includeSystem(\'API\');'."\n";
            $replacement .= '$currUserid = User::getCurrentUserid();'."\n";
            $replacement .= 'if ($currUserid !== NULL) {'."\n";
            $replacement .= '   $currToken = API::getUserToken();'."\n";
            $replacement .= '   if ($currToken === NULL) {'."\n";
            $replacement .= '       $currToken = API::getUniqueToken();'."\n";
            $replacement .= '       API::setUserToken($currToken);'."\n";
            $replacement .= '   }'."\n";
            $replacement .= '   echo \'<input id="__api_token" type="hidden" value="\'.$currToken.\'" />\';'."\n";
            $replacement .= '}'."\n";
            $replacement .= '?'.">\n";

            $htmlCont = str_replace('<body>', $replacement, $htmlCont);

            // Since this is a main template add it to the template list.
            $htmlCont = str_replace('</head>', '<script>GUI.addTemplate(\''.$systemName.':'.$templateName.'\');</script></head>', $htmlCont);
        } else {
            // Replce the doc type for sub level templates.
            $htmlCont = str_replace(self::$_docType, '', $htmlCont);
        }//end if

        // Do some cleanup.
        $htmlCont = trim(str_replace('<?xml version="1.0"?>', '', $htmlCont));
        $htmlCont = str_replace('<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" ></meta>', '', $htmlCont);

        $htmlCont = str_replace('&lt;?php', '<?php', $htmlCont);
        $htmlCont = str_replace('?&gt;', '?>', $htmlCont);

        // Change all variable keywords (e.g. {varName}) with PHP code.
        $htmlCont = self::_replaceVarKeywords($htmlCont);

        include_once 'Libs/FileSystem/FileSystem.inc';
        $syDataPath = BaseSystem::getDataDir($systemName).'/Templates';
        if (file_exists($syDataPath) === FALSE) {
            FileSystem::mkdir($syDataPath, 0755, TRUE);
        }

        // Write to data file.
        $dest = '';
        if ($isReCacheTemplate !== TRUE) {
            $dest = $syDataPath.'/'.$templateName.'.tpl.php';
        } else {
            $dir = BaseSystem::getDataDir('GUI').'/rebakeTemplates';
            if (file_exists($dir) === FALSE) {
                mkdir($dir, 0755, TRUE);
            }

            $dest = $dir.'/'.$systemName.'-'.$templateName.'.tpl.php';
        }

        file_put_contents($dest, $htmlCont);

    }//end bakeTemplate()


    /**
     * Replace variable keywords in parsed HTML content.
     *
     * @param string  $content The HTML content to search.
     * @param boolean $echo    If TRUE, variable is echo'd. Otherwise var is printed.
     *
     * @return string
     */
    private static function _replaceVarKeywords($content, $echo=TRUE)
    {
        if ($echo === TRUE) {
            $callback = '_replaceVarKeywordsEchoCallback';
        } else {
            $callback = '_replaceVarKeywordsCallback';
        }

        // Replace all template variables with the actual PHP variables.
        $content = preg_replace_callback('/{([a-z0-9_:]+)}/i', array('self', $callback), $content);
        return $content;

    }//end _replaceVarKeywords()


    /**
     * Callback method for the _replaceVarKeywords preg_replace_callback function.
     *
     * @param array $matches Array of matches.
     *
     * @return string
     */
    private static function _replaceVarKeywordsCallback(array $matches)
    {
        return self::_getKeywordVariableCode($matches[1]);

    }//end _replaceVarKeywordsCallback()


    /**
     * Callback method for the _replaceVarKeywords preg_replace_callback function.
     *
     * @param array $matches Array of matches.
     *
     * @return string
     */
    private static function _replaceVarKeywordsEchoCallback(array $matches)
    {
        $code = self::_getKeywordVariableCode($matches[1]);
        $code = '<'.'?php echo '.$code.';?'.'>';

        return $code;

    }//end _replaceVarKeywordsEchoCallback()


    /**
     * Returns the actual PHP variable string from a given keyword string.
     *
     * @param string $keyword The variable keyword string.
     *
     * @return string
     */
    private static function _getKeywordVariableCode($keyword)
    {
        $keyword = trim($keyword, '}{');
        $code    = '';
        $indexes = explode(':', $keyword);
        if (count($indexes) > 1) {
            $varName = array_shift($indexes);
            $code    = '$_gui_'.$varName.'[\''.implode('\'][\'', $indexes).'\']';
        } else {
            $code = '$_gui_'.$keyword;
        }

        return $code;

    }//end _getKeywordVariableCode()


    /**
     * Returns the template key thats being parsed/painted.
     *
     * @return string
     */
    public static function getCurrentTemplateKey()
    {
        return self::$_currentTemplateKey;

    }//end getCurrentTemplateKey()


    /**
     * Set the template key thats being parsed/painted.
     *
     * @param string $tplKey The template key.
     *
     * @return void
     */
    public static function setCurrentTemplateKey($tplKey)
    {
        self::$_currentTemplateKey = $tplKey;

    }//end setCurrentTemplateKey()


    /**
     * Sets the template settings var.
     *
     * This method is used by baked out template code.
     *
     * @param array $settings The template settings array.
     *
     * @return void
     */
    public static function setTemplateSettingsVar(array $settings)
    {
        self::$_templateSettings = &$settings;

    }//end setTemplateSettingsVar()


    /**
     * Returns the settings for a specified widget in active template scope.
     *
     * @param string $widgetid Id of a widget in current template scope.
     *
     * @return array
     */
    public static function getWidgetSettings($widgetid)
    {
        $key = GUI::getCurrentTemplateKey();
        if (isset(self::$_settings[$key]) === TRUE
            && self::$_settings[$key]['widget'][$widgetid] === TRUE
        ) {
            return self::$_settings[$key]['widget'][$widgetid];
        }

        return NULL;

    }//end getWidgetSettings()


    /**
     * Parses the specified template, converting the special tags to HTML/PHP.
     *
     * @param DOMNode $tplDoc   The Template DOMDocument.
     * @param string  $assignTo Variable to assign the results to.
     *
     * @since  4.0.0
     * @return string
     */
    public static function parseTemplate(DOMNode $tplDoc, $assignTo=NULL)
    {
        include_once 'Libs/XML/XML.inc';
        $tagNames = array(
                     'var',
                     'content',
                     'widget',
                     'text',
                     'dataProvider',
                     'condition',
                     'cache',
                    );

        $php = '';
        foreach ($tagNames as $tagName) {
            $tags     = $tplDoc->getElementsByTagName($tagName);
            $topLevel = array();
            foreach ($tags as $tag) {
                if (XML::hasParentTag($tag, $tagNames) === FALSE) {
                    $topLevel[] = $tag;
                }
            }

            // Only the top level tags are parsed by this method, all the rest will
            // be parsed by the rest of the parseXYZ methods.
            foreach ($topLevel as $tag) {
                $php .= call_user_func(array('self', '_parse'.ucfirst($tagName).'Tag'), $tag, $assignTo);
            }
        }

        return $php;

    }//end parseTemplate()


    /**
     * Returns the PHP variable string for the current template and widget.
     *
     * @param string $type Type of the variable (e.g. widget).
     * @param string $id   If of the variable (e.g. a valid widgetid).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getVarCode($type=NULL, $id=NULL)
    {
        $php = '$templateVars["'.self::$_currentTemplateKey.'"]';

        if ($type !== NULL) {
            $php .= '["'.$type.'"]';
        }

        if ($id !== NULL) {
            $php .= '["'.$id.'"]';
        }

        return $php;

    }//end _getVarCode()


    /**
     * Returns the next variable name for the current template.
     *
     * Used during template baking to create valid variable names.
     *
     * @param string $prefix The prefix to use for the variable name.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getNextVarName($prefix='var')
    {
        self::$_templateVarCounter++;
        $key  = str_replace('::', '', self::$_currentTemplateKey);
        $name = $prefix.'_'.$key.'_'.self::$_templateVarCounter;
        return $name;

    }//end _getNextVarName()


    /**
     * Returns the PHP script of the given widget tag.
     *
     * @param DOMNode $widget   The tag to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']). if NULL
     *                          then value will be printed.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseWidgetTag(DOMNode $widget, $assignTo=NULL)
    {
        $widgetid         = $widget->getAttribute('id');
        $widgetType       = $widget->getAttribute('type');
        $widgetSystemName = GUI::getSystemNameFromWidgetType($widgetType);
        $php              = '';

        $settingsVar  = self::_getVarCode('widget', $widgetid).'["settings"]';
        $settingsCode = self::_getWidgetSettings($widget);

        $php .= $settingsCode;

        // Include widget system.
        $php .= 'Channels::includeSystem("'.$widgetSystemName.'");'."\n";

        if ($assignTo !== NULL) {
            // Need to assign the printed content to given variable so
            // use output buffering.
            $php .= "ob_start();\n";
        }

        $installCode = $php;

        // Add the widget paint() call.
        $php .= $settingsVar.' = '.$widgetSystemName.'::paint('.$settingsVar.');'."\n";

        try {
            // If a widget has an install method then call it with the settings array.
            if (self::$_isReCacheTemplate === FALSE && is_callable(array($widgetSystemName, 'install')) === TRUE) {
                $installCode .= $settingsVar.' = '.$widgetSystemName.'::install('.$settingsVar.');'."\n";
                ob_start();
                eval($installCode);
                ob_end_clean();
            }
        } catch (Exception $e) {
            // Ignore all exceptions for now..
        }

        // If this widget has a JS file then add it its JS init code.
        // Note that there is no need to include the JS file here since it should
        // have been included at the beginning.
        $jsFilePath  = Channels::getSystemsPath($widgetSystemName).'/Web/';
        $jsFilePath .= $widgetSystemName.'.js';
        if (file_exists($jsFilePath) === TRUE) {
            $php .= "echo '<script>'.GUI::getJSInitCode('".$widgetSystemName."', '".$widgetid."', ".$settingsVar.").'</script>';\n";
        }

        if ($assignTo !== NULL) {
            // End output buffering.
            $php .= $assignTo." = ob_get_clean();\n";
        } else {
            // Top level widget. Need to replace the widget tag with the PHP code.
             // and declare &nbsp for templates containing a &nbsp.
            $declare   = '[ <!ENTITY nbsp "&#160;"> ]>';
            $contents  = str_replace(self::$_docType, '', $contents);
            $contents .= self::$_docType;
            $contents  = str_replace('>', $declare, $contents);
            $contents .= '<phpCode><?php '.$php.' ?></phpCode>';

            $tmpDoc = DOMDocument::loadXML($contents);
            foreach ($tmpDoc->getElementsByTagName('phpCode')->item(0)->childNodes as $tmpNode) {
                $node = $widget->ownerDocument->importNode($tmpNode, TRUE);
                $widget->parentNode->insertBefore($node, $widget);
            }

            $widget->parentNode->removeChild($widget);
            $php = '';
        }//end if

        return $php;

    }//end _parseWidgetTag()


    /**
     * Returns the PHP script of settings code of the given widget tag.
     *
     * @param DOMNode $widget The widget tag to convert.
     *
     * @since  4.0.0
     * @return string
     * @throws ChannelException If a reserved tag name is used in the widget.
     */
    private static function _getWidgetSettings(DOMNode $widget)
    {
        $settings = array();
        $widgetid = $widget->getAttribute('id');
        $type     = $widget->getAttribute('type');

        $widgetSystem = GUI::getSystemNameFromWidgetType($type);
        list($templateSystem, $templateName) = explode('::', self::$_currentTemplateKey);

        $php  = self::_getVarCode('widget', $widgetid).'["settings"] = ';
        $php .= 'array(
                    "widget" => array(
                                 "id"   => "'.$widgetid.'",
                                 "type" => "'.$widgetSystem.'",
                                ),
                    "template" => array(
                                   "system" => "'.$templateSystem.'",
                                   "name"   => "'.$templateName.'",
                                  ),
                 );'."\n";

        // These setting names are reserved and cannot be used by templates.
        $reservedNames = array(
                          'widget',
                          'template',
                         );

        // Widget tags contain setting tags.
        foreach ($widget->childNodes as $node) {
            if ($node->nodeType !== XML_ELEMENT_NODE || $node->tagName !== 'setting') {
                continue;
            }

            $setting     = $node;
            $settingName = $setting->getAttribute('name');

            if (in_array($settingName, $reservedNames) === TRUE) {
                $msg = sprintf(
                    _('% is a reserved setting name. (Used by widget #%)'),
                    $settingName,
                    $widgetid
                );
                throw new ChannelException($msg);
            }

            if (self::_isValidWidgetSetting($widgetSystem, $settingName) === FALSE) {
                $msg = sprintf(
                    _('"%s" is not a valid setting for %s. (Found in %s template)'),
                    $settingName,
                    $widgetSystem,
                    self::$_currentTemplateKey
                );
                throw new ChannelException($msg);
            }

            $varName = self::_getVarCode('widget', $widgetid).'["settings"]["'.$settingName.'"]';
            $php    .= self::_parseSettingTag($setting, $varName);
        }//end foreach

        return $php;

    }//end _getWidgetSettings()


    /**
     * Returns TRUE if the specified setting for the specified widget type is valid.
     *
     * @param string  $widgetName  Type of the widget.
     * @param string  $settingName Name of the setting to check.
     * @param boolean $jsOnly      If TRUE then the setting must also be available
     *                             for JavaScript.
     *
     * @since  4.0.0
     * @return boolean
     */
    private static function _isValidWidgetSetting($widgetName, $settingName, $jsOnly=FALSE)
    {
        if ($settingName === 'widget'
            || $settingName === 'template'
            || strpos($settingName, '_') === 0
        ) {
            return TRUE;
        }

        $settings = self::_getAvaiableSettingsForWidget($widgetName);

        if (isset($settings[$settingName]) === TRUE) {
            if ($jsOnly === TRUE) {
                if ($settings[$settingName]['internal'] === FALSE) {
                    return TRUE;
                }
            } else {
                return TRUE;
            }
        }

        return FALSE;

    }//end _isValidWidgetSetting()


    /**
     * Returns the list of avialable settings for the specified widget type.
     *
     * @param string $widgetName The widget type.
     *
     * @return array
     */
    private static function _getAvaiableSettingsForWidget($widgetName)
    {
        if (self::$_isBaking === FALSE) {
            if (empty(self::$_widgetSettings[$widgetName]) === TRUE) {
                // At run time we need to include the widget settings file.
                $path = BaseSystem::getDataDir('GUI').'/widgetSettings.inc';

                include $path;
                self::$_widgetSettings = $settings;
            }
        }

        if (isset(self::$_widgetSettings[$widgetName]) === TRUE) {
            return self::$_widgetSettings[$widgetName];
        }

        $settings = array();
        Channels::includeSystem($widgetName);
        $ref   = new ReflectionClass($widgetName.'Actions');
        $props = $ref->getProperties(ReflectionProperty::IS_PROTECTED);

        foreach ($props as $prop) {
            if ($prop->isStatic() === FALSE) {
                continue;
            }

            $docComment = $prop->getDocComment();
            $internal   = FALSE;
            if (strpos($docComment, ' * @internal') !== FALSE) {
                $internal = TRUE;
            }

            $settings[$prop->getName()] = array('internal' => $internal);
        }//end foreach

        self::$_widgetSettings[$widgetName] = $settings;

        if (self::$_isBaking === FALSE) {
            // New widget type found during run time so store the new settings array.
            self::_createWidgetSettingsFile();
        }

        return $settings;

    }//end _getAvaiableSettingsForWidget()


    /**
     * Creates the widget settings file so that the list can be used during run time.
     *
     * @return void
     */
    private static function _createWidgetSettingsFile()
    {
        // Store the list of widget settings in a file so that they are accessible
        // at runtime.
        $widgetSettingsFile = BaseSystem::getDataDir('GUI').'/widgetSettings.inc';
        $settingsContent    = '<';
        $settingsContent   .= '?php $settings = ';
        $settingsContent   .= var_export(self::$_widgetSettings, TRUE);
        $settingsContent   .= '; ?';
        $settingsContent   .= '>';

        file_put_contents($widgetSettingsFile, $settingsContent);
        chmod($widgetSettingsFile, 0755);

    }//end _createWidgetSettingsFile()


    /**
     * Returns the PHP script of the setting tag.
     *
     * @param DOMNode $setting Tag to convert.
     * @param string  $varName Name of the variable to assign to.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseSettingTag(DOMNode $setting, $varName)
    {
        $php = '';

        include_once 'Libs/XML/XML.inc';
        if (XML::hasChildElements($setting) === FALSE) {
            $php .= $varName.' = '.self::_getPHPValue(trim($setting->nodeValue)).";\n";
            return $php;
        }

        foreach ($setting->childNodes as $node) {
            if ($node->nodeType === XML_TEXT_NODE) {
                continue;
            } else if ($node->nodeType === XML_ELEMENT_NODE) {
                $php .= self::_parseTag($node, $varName);
            }//end if
        }//end foreach

        return $php;

    }//end _parseSettingTag()


    /**
     * Returns the PHP code for the widget-settings tag.
     *
     * @param DOMNode $node     The tag to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']). if NULL
     *                          then value will be printed.
     *
     * @return string
     */
    private static function _parseWidgetSettingsTag(DOMNode $node, $assignTo)
    {
        $widgetid = $node->getAttribute('id');

        $php  = $assignTo.' = ';
        $php .= self::_getVarCode('widget', $widgetid).'["settings"];'."\n";

        return $php;

    }//end _parseWidgetSettingsTag()


    /**
     * Returns the PHP code of the given tag.
     *
     * @param DOMNode $node     The tag to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']). if NULL
     *                          then value will be printed.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseTag(DOMNode $node, $assignTo=NULL)
    {
        $php = '';
        if (empty($node->tagName) === TRUE) {
            return $php;
        }

        switch ($node->tagName) {
            case 'text':
                $php .= self::_parseTextTag($node, $assignTo);
            break;

            case 'widget':
                $php .= self::_parseWidgetTag($node, $assignTo);
            break;

            case 'dataProvider':
                $php .= self::_parseDataProviderTag($node, $assignTo);
            break;

            case 'widget-settings':
                $php .= self::_parseWidgetSettingsTag($node, $assignTo);
            break;

            case 'condition':
                $php .= self::_parseConditionTag($node, $assignTo);
            break;

            case 'exception':
                $php .= self::_parseExceptionTag($node, $assignTo);
            break;

            case 'var':
                $php .= self::_parseVarTag($node);
            break;

            case 'cache':
                $php .= self::_parseCacheTag($node);
            break;

            case 'content':
                $php .= self::_parseContentTag($node, $assignTo);
            break;

            default:
                include_once 'Libs/XML/XML.inc';
                if ($node->tagName === 'value') {
                    $tmpVarName = '$_idx_'.md5($assignTo);

                    $php .= 'if (isset('.$tmpVarName.') === FALSE) {'."\n\t";
                    $php .= $tmpVarName." = 0;\n} else {\n\t";
                    $php .= $tmpVarName."++;\n}\n";

                    $varName = $assignTo.'['.$tmpVarName.']';
                } else {
                    $varName = $assignTo.'["'.$node->tagName.'"]';
                }

                $php .= $varName.' = ';

                $counter = 0;
                if (XML::hasChildElements($node) === TRUE) {
                    $php .= 'array();'."\n";
                    foreach ($node->childNodes as $childTag) {
                        if ($childTag->nodeType !== XML_ELEMENT_NODE) {
                            continue;
                        }

                        $php .= self::_parseTag($childTag, $varName);
                    }
                } else {
                    $php .= self::_getPHPValue(trim($node->nodeValue)).";\n";
                }//end if
            break;
        }//end switch

        return $php;

    }//end _parseTag()


    /**
     * Converts a string to real PHP variable.
     *
     * @param string $value The string to convert.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _getPHPValue($value)
    {
        if (strtolower($value) === 'true') {
            $value = var_export(TRUE, TRUE);
        } else if (strtolower($value) === 'false') {
            $value = var_export(FALSE, TRUE);
        } else if (is_float($value) === TRUE) {
            $value = (float) $value;
        } else if (is_int($value) === TRUE) {
            $value = (int) $value;
        } else if (preg_match('/{([a-z0-9_:]+)}/i', $value) !== 0) {
            // Call replacement function.
            $value = self::_getKeywordVariableCode($value);
        } else {
            $value = '"'.addcslashes((string) $value, "\"\\\0").'"';
        }

        return $value;

    }//end _getPHPValue()


    /**
     * Returns the PHP script of the given dataProvider tag.
     *
     * @param DOMNode $dp       The dataProvider tag to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseDataProviderTag(DOMNode $dp, $assignTo=NULL)
    {
        include_once 'Libs/XML/XML.inc';

        $php = '';

        // Data provider can have a widget or an action.
        $action = $dp->getAttribute('action');
        if (empty($action) === TRUE) {
            // Must be a widget thats returning some data.
            $firstChild = XML::getFirstChildElement($dp);
            if ($firstChild !== NULL && $firstChild->tagName('widget') === TRUE) {
                self::_parseWidgetTag($firstChild, $assignTo);
            }
        } else {
            // Channel action.
            // First initialise the argument vars for this data provider action call.
            $argListCodes = self::_getArgumentListCodes($dp);

            $php .= $argListCodes['init'];

            // Include the system.
            list($systemName, $methodName) = explode('::', $action);
            $php .= 'Channels::includeSystem("'.$systemName.'");'."\n";

            if ($assignTo !== NULL) {
                $php .= $assignTo.' = ';
            } else {
                $php .= 'echo ';
            }

            $php .= $action.'('.$argListCodes['list'].');'."\n";
        }//end if

        if ($assignTo === NULL) {
            // Top level.
            $tmpDoc  = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $tmpNode = $tmpDoc->getElementsByTagName('phpCode')->item(0)->firstChild;
            $node    = $dp->ownerDocument->importNode($tmpNode, TRUE);
            $dp->parentNode->insertBefore($node, $dp);
            $dp->parentNode->removeChild($dp);
            $php = '';
        }

        return $php;

    }//end _parseDataProviderTag()


    /**
     * Returns the PHP code for argument initialisation and argument list.
     *
     * Example: array(
     *           'init' => "$exampleArg = 123;\n$exampleArg2 = 'example'",
     *           'list' => '$exampleArg, $exampleArg2',
     *          ).
     *
     * @param DOMNode $parent The parent node with child "arg" tags.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getArgumentListCodes(DOMNode $parent)
    {
        include_once 'Libs/XML/XML.inc';

        $php     = '';
        $argList = array();

        foreach ($parent->childNodes as $argNode) {
            if ($argNode->nodeType === XML_ELEMENT_NODE
                && $argNode->tagName === 'arg'
            ) {
                $arg       = XML::getFirstChildElement($argNode);
                $argName   = '$'.self::_getNextVarName('arg');
                $argList[] = $argName;

                if ($arg === NULL) {
                    // Must be a text node.
                    $php .= $argName.' = '.self::_getPHPValue(trim($argNode->nodeValue)).';';
                } else {
                    $php .= self::_parseTag($arg, $argName);
                }
            }
        }

        $code = array(
                 'init'   => $php,
                 'values' => $argList,
                 'list'   => implode(', ', $argList),
                );

        return $code;

    }//end _getArgumentListCodes()


    /**
     * Returns the PHP script of the given text tag.
     *
     * Text tags on their own are converted into plain localisable texts:
     *   <text>Localisable text</text>
     *
     * Text tags with arguments are converted into localisable texts which are then
     * run through sprintf():
     *   <text>
     *     <value>Localisable %s</value>
     *     <args>
     *       <arg>text</arg>
     *     </args>
     *   </text>
     *
     * Text tags with arguments can also accept a "plural" tag, which specifies that
     * this should be converted into a numerically dependent localisable text
     * (using ngettext() call). In this case, the first argument (cast to integer)
     * will determine the number to be placed into ngettext(). If you do not want
     * the first replacement in the string to determine this, use sprintf()
     * positional parameters (eg. "%1$s") to move the replacements around. If no
     * "args" tag, plural text is ignored.
     *
     * @param DOMNode $text     The text node to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']).
     *
     * @since  4.0.0
     * @return string
     * @throws ChannelException If the text content has a keyword.
     */
    private static function _parseTextTag(DOMNode $text, $assignTo=NULL)
    {
        $php     = "\n";
        $tagInfo = self::_getTextTagInfo($text);
        $args    = Util::getArrayIndex($tagInfo, 'argsCode', array());

        $value = $tagInfo['value'];
        if (preg_match('/{([a-z0-9_:]+)}/i', $value) !== 0) {
            throw new ChannelException(_('Cannot use variable keyword inside a text tag'));
        }

        if (empty($args) === TRUE) {
            if ($assignTo !== NULL) {
                $php .= $assignTo.' = ';
            } else {
                $php .= 'echo ';
            }

            $php  .= '_(\''.$value.'\');'."\n";
        } else {
            $plural = $tagInfo['plural'];

            // Init code for the arguments.
            $php .= $tagInfo['argsCode']['init'];

            if ($plural !== '') {
                $printArgCode = 'ngettext(\''.$value.'\', \''.$plural.'\', (int) ('.$tagInfo['argsCode']['values'][0].')), '.$tagInfo['argsCode']['list'];
            } else {
                $printArgCode = '_(\''.$value.'\'), '.$tagInfo['argsCode']['list'];
            }

            if ($assignTo !== NULL) {
                $php .= $assignTo.' = sprintf('.$printArgCode.');'."\n";
            } else {
                $php .= 'printf('.$printArgCode.');'."\n";
            }
        }

        if ($assignTo === NULL) {
            // Top level text. Need to replace the text tag with the PHP code.
            // Top level text. Need to replace the text tag with the PHP code.
            $tmpDoc      = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $phpCodeElem = $tmpDoc->getElementsByTagName('phpCode')->item(0);
            $childCount  = $phpCodeElem->childNodes->length;

            for ($i = 0; $i < $childCount; $i++) {
                $tmpNode = $phpCodeElem->childNodes->item($i);
                $newNode = $text->ownerDocument->importNode($tmpNode, TRUE);
                $text->parentNode->insertBefore($newNode, $text);
            }

            $text->parentNode->removeChild($text);
            $php = '';
        }

        return $php;

    }//end _parseTextTag()


    /**
     * Returns information about the text tag.
     *
     * @param DOMNode $text The text node to parse.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getTextTagInfo(DOMNode $text)
    {
        $tagInfo = array(
                    'value'    => '',
                    'plural'   => '',
                    'argsCode' => array(),
                   );

        foreach ($text->childNodes as $node) {
            if ($node->nodeType === XML_TEXT_NODE
                || $node->nodeType === XML_CDATA_SECTION_NODE
            ) {
                $tagInfo['value'] .= addcslashes($node->nodeValue, "\'\\\0");
            } else if ($node->nodeType === XML_ELEMENT_NODE) {
                if ($node->tagName === 'value') {
                    $tagInfo['value'] .= addcslashes($node->nodeValue, "\'\\\0");
                } else if ($node->tagName === 'plural') {
                    $tagInfo['plural'] .= addcslashes($node->nodeValue, "\'\\\0");
                } else if ($node->tagName === 'args') {
                    $tagInfo['argsCode'] = self::_getArgumentListCodes($node);

                    // Cannot have any more tags.
                    break;
                }
            }
        }

        $tagInfo['value'] = trim($tagInfo['value']);
        $tagInfo['plural'] = trim($tagInfo['plural']);

        return $tagInfo;

    }//end _getTextTagInfo()


    /**
     * Returns the PHP script of the given condition tag.
     *
     * @param DOMNode $node     The text node to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseConditionTag(DOMNode $node, $assignTo=NULL)
    {
        $tagTypes = array(
                     'if'      => 'if',
                     'if-not'  => 'if',
                     'else'    => 'else',
                     'if-null' => 'if',
                    );

        $php     = '';
        $varName = '$'.self::_getNextVarName('_conditionVar');

        foreach ($node->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE
                && isset($tagTypes[$child->tagName]) === TRUE
            ) {
                $condition = $child->getAttribute('condition');

                // Get the system name so we can include it.
                list($systemName, $method) = explode('::', $condition);
                if (empty($method) === FALSE) {
                    $php .= 'Channels::includeSystem(\''.$systemName.'\');'."\n";
                }

                // Replace kewords.
                $condition = self::_replaceVarKeywords($condition, FALSE);

                // Print the if, else block.
                $php .= $tagTypes[$child->tagName];

                switch ($child->tagName) {
                    case 'if':
                        $php .= ' (('.$condition.') === TRUE) {'."\n";
                    break;

                    case 'if-not':
                        $php .= ' (('.$condition.') === FALSE) {'."\n";
                    break;

                    case 'if-null':
                        $php .= ' (('.$condition.') === NULL) {'."\n";
                    break;

                    default:
                        $php .= " {\n";
                    break;
                }

                foreach ($child->childNodes as $condChild) {
                    $assignVarName = $varName;
                    if ($condChild->tagName === 'value') {
                        // The 'value' tags are used as array indexes and therefore
                        // if condition should not override the assignTo variable
                        // when calling parseTag.
                        $assignVarName = $assignTo;
                    }

                    $parseTagRes = self::_parseTag($condChild, $assignVarName);
                    if (empty($parseTagRes) === TRUE) {
                        continue;
                    }

                    $php .= $parseTagRes."\n";

                    if ($condChild->tagName !== 'value') {
                        if ($assignTo === NULL) {
                            $php .= 'echo (isset('.$assignVarName.') === TRUE ? '.$assignVarName." : '');\n";
                        } else {
                            $php .= $assignTo.' = (isset('.$assignVarName.') === TRUE ? '.$assignVarName." : '');\n";
                        }
                    }
                }

                $php .= "\n} ";
            }//end if
        }//end foreach

        $php .= "\n";

        if ($assignTo === NULL) {
            // Top level.
            $tmpDoc      = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $phpCodeElem = $tmpDoc->getElementsByTagName('phpCode')->item(0);
            $childCount  = $phpCodeElem->childNodes->length;

            for ($i = 0; $i < $childCount; $i++) {
                $tmpNode = $phpCodeElem->childNodes->item($i);
                $newNode = $node->ownerDocument->importNode($tmpNode, TRUE);
                $node->parentNode->insertBefore($newNode, $node);
            }

            $node->parentNode->removeChild($node);
            $php = '';
        }

        return $php;

    }//end _parseConditionTag()


    /**
     * Returns the PHP script of the given exception tag.
     *
     * @param DOMNode $node     The exception node to convert.
     * @param string  $assignTo The variable to assign to. This must be a valid PHP
     *                          variable name (e.g. $example['value']).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseExceptionTag(DOMNode $node, $assignTo=NULL)
    {
        $php = '';
        if (self::$_isReCacheTemplate === FALSE) {
            foreach ($node->childNodes as $child) {
                $php = self::_parseTag($child, '$_gui_exception');
            }

            $php .= "\n";
            $php .= 'throw new ChannelException($_gui_exception);';
        }

        if ($assignTo === NULL) {
            // Top level.
            $tmpDoc      = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $phpCodeElem = $tmpDoc->getElementsByTagName('phpCode')->item(0);
            $childCount  = $phpCodeElem->childNodes->length;

            for ($i = 0; $i < $childCount; $i++) {
                $tmpNode = $phpCodeElem->childNodes->item($i);
                $newNode = $node->ownerDocument->importNode($tmpNode, TRUE);
                $node->parentNode->insertBefore($newNode, $node);
            }

            $node->parentNode->removeChild($node);
            $php = '';
        }

        return $php;

    }//end _parseExceptionTag()


    /**
     * Converts a given var tag.
     *
     * The var tag is used to initialise PHP variables with specific name which can
     * be referenced in template using {varName}. These "keywords" will be replaced
     * with PHP code after the whole document is parsed.
     *
     * @param DOMNode $var The var tag to convert.
     *
     * @since  4.0.0
     * @return string
     * @throws ChannelException If the var tag does not have a name attribute.
     */
    private static function _parseVarTag(DOMNode $var)
    {
        include_once 'Libs/XML/XML.inc';

        $varName = $var->getAttribute('name');
        if (empty($varName) === TRUE) {
            throw new ChannelException(_('var tag must have a "name" attribute.'));
        }

        $varName = '$_gui_'.$varName;

        $firstChild = XML::getFirstChildElement($var);
        if ($firstChild !== NULL) {
            $php = self::_parseTag($firstChild, $varName);
        } else {
            // Just a text node.
            $textNode = $varName." = '".str_replace("'", "\'", $var->nodeValue)."';";
            return $textNode;
        }

        self::$_templateVariablesCode .= $php;

        // Remove the var tag.
        $var->parentNode->removeChild($var);

        return '';

    }//end _parseVarTag()


    /**
     * Returns the PHP script of the given content tag.
     *
     * @param DOMNode $contentTag The content node to convert.
     * @param string  $assignTo   The variable to assign to. This must be a valid PHP
     *                            variable name (e.g. $example['value']).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _parseContentTag(DOMNode $contentTag, $assignTo=NULL)
    {
        // Need to wrap content.
        $domContent  = self::$_docType;
        $domContent .= '<__tempContent></__tempContent>';

        $temptpl = new DOMDocument('1.0');
        $temptpl->loadXML($domContent);

        $tempContentEl = $temptpl->getElementsByTagName('__tempContent')->item(0);

        foreach ($contentTag->childNodes as $contentChild) {
            $importedNode = $temptpl->importNode($contentChild, TRUE);
            $tempContentEl->appendChild($importedNode);
        }

        $varName = self::_getNextVarName();
        GUI::parseTemplate($tempContentEl);
        $subTemplateStr = $temptpl->saveXML();

        // Just get the content we want.
        $subTemplateStr = str_replace(self::$_docType, '', $subTemplateStr);
        $subTemplateStr = trim(str_replace('<?xml version="1.0"?>', '', $subTemplateStr));
        $subTemplateStr = str_replace('<__tempContent>', '', $subTemplateStr);
        $subTemplateStr = str_replace('</__tempContent>', '', $subTemplateStr);
        $subTemplateStr = ' ?>'.$subTemplateStr.'<?php ';

        $php = '';
        if ($assignTo !== NULL) {
            $php .= 'ob_start();'."\n";
            $php .= $subTemplateStr;
            $php .= $assignTo.' = ob_get_clean();'."\n";
        } else {
            $tmpDoc  = DOMDocument::loadXML('<phpCode><?php '.$subTemplateStr.' ?></phpCode>');
            $tmpNode = $tmpDoc->getElementsByTagName('phpCode')->item(0)->firstChild;
            $node    = $contentTag->ownerDocument->importNode($tmpNode, TRUE);
            $contentTag->parentNode->insertBefore($node, $contentTag);
            $contentTag->parentNode->removeChild($contentTag);
            $php = '';
        }

        return $php;

    }//end _parseContentTag()


    /**
     * Converts a given cache tag.
     *
     * Cache tags are used to cache whole or part of a template for a period of time
     * or until an external system clears the cache.
     *
     * The cache tags cannot be nested and they require id and ttl attributes.
     *
     * @param DOMNode $node The cache tag to convert.
     *
     * @return string
     * @throws ChannelException If the cache tag is invalid.
     */
    private static function _parseCacheTag(DOMNode $node)
    {
        $cacheKey = trim($node->getAttribute('id'));
        $ttl      = trim($node->getAttribute('ttl'));

        self::$_cacheTagInfo = array(
                                'cacheKey' => $cacheKey,
                                'ttl'      => $ttl,
                               );

        // Need to wrap content.
        $domContent  = self::$_docType;
        $domContent .= '<__tempContent></__tempContent>';

        $temptpl = new DOMDocument('1.0');
        $temptpl->loadXML($domContent);

        $tempContentEl = $temptpl->getElementsByTagName('__tempContent')->item(0);

        foreach ($node->childNodes as $contentChild) {
            $importedNode = $temptpl->importNode($contentChild, TRUE);
            $tempContentEl->appendChild($importedNode);
        }

        GUI::parseTemplate($tempContentEl);
        $subTemplateStr = $temptpl->saveXML();

        self::$_cacheTagInfo = array();

        // Just get the content we want.
        $subTemplateStr = str_replace(self::$_docType, '', $subTemplateStr);
        $subTemplateStr = trim(str_replace('<?xml version="1.0"?>', '', $subTemplateStr));
        $subTemplateStr = str_replace('<__tempContent>', '', $subTemplateStr);
        $subTemplateStr = str_replace('</__tempContent>', '', $subTemplateStr);
        $subTemplateStr = ' ?>'.$subTemplateStr.'<?php ';

        if (strlen($cacheKey) === 0) {
            throw new ChannelException(_('Cache tag requires an "id" attribute'));
        } else if (strlen($ttl) === 0 || is_numeric($ttl) === FALSE || (int) $ttl < 0) {
            $msg = _('Cache tag requires a "ttl" (Time To Live) attribute and must be in minutes. Use 0 to keep cache forever');
            throw new ChannelException($msg);
        }

        $php = '';
        if (self::$_isReCacheTemplate === FALSE) {
            $php .= '$_gui_cachedContent = GUI::loadTemplateCache('.self::_printAttributeValue($cacheKey).', '.self::_printAttributeValue($ttl).');'."\n";
            $php .= 'if ($_gui_cachedContent !== NULL) {'."\n";
            $php .= '   echo $_gui_cachedContent;'."\n";
            $php .= '} else {'."\n";
        }

        $php .= "   // Create cache.\n";
        $php .= "   ob_start();\n";
        $php .= $subTemplateStr;
        $php .= '   $_gui_cachedContent = ob_get_contents();'."\n";
        $php .= '   GUI::saveTemplateCache('.self::_printAttributeValue($cacheKey).', $_gui_cachedContent);'."\n";
        $php .= "   ob_end_clean();\n";

        if (self::$_isReCacheTemplate === FALSE) {
            $php .= "   echo \$_gui_cachedContent;\n";
            $php .= '}'."\n";
        }

        $tmpDoc      = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
        $phpCodeElem = $tmpDoc->getElementsByTagName('phpCode')->item(0);
        $childCount  = $phpCodeElem->childNodes->length;

        for ($i = 0; $i < $childCount; $i++) {
            $tmpNode = $phpCodeElem->childNodes->item($i);
            $newNode = $node->ownerDocument->importNode($tmpNode, TRUE);
            $node->parentNode->insertBefore($newNode, $node);
        }

        $node->parentNode->removeChild($node);

        return $php;

    }//end _parseCacheTag()


    /**
     * Returns cached contents of a template.
     *
     * Returns NULL if there is no cached content or is expired.
     *
     * @param string  $cacheid Id of the cache file.
     * @param integer $ttl     Time to live value in minutes, 0 for no expire time.
     *
     * @return mixed
     */
    public static function loadTemplateCache($cacheid, $ttl)
    {
        if (GUI::templateCacheExists($cacheid, $ttl) === TRUE) {
            $cachePath = self::_getTemplateCachePath($cacheid);
            $content   = file_get_contents($cachePath);
            return $content;
        }

        return NULL;

    }//end loadTemplateCache()


    /**
     * Saved the contens of a template in to a cache file.
     *
     * @param string $cacheid      Id of the cache file.
     * @param string $cacheContent The content to cache.
     *
     * @return void
     */
    public static function saveTemplateCache($cacheid, $cacheContent)
    {
        $cachePath = self::_getTemplateCachePath($cacheid);
        $dir       = dirname($cachePath);

        if (file_exists($dir) === FALSE) {
            include_once 'Libs/FileSystem/FileSystem.inc';
            @FileSystem::mkdir($dir, 0755, TRUE);
        }

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($cachePath, $cacheContent);

    }//end saveTemplateCache()


    /**
     * Returns TRUE if the specified cache exists.
     *
     * @param string  $cacheid Id of the cache file.
     * @param integer $ttl     Time to live value in minutes, 0 for no expire time.
     *
     * @return boolean
     */
    public static function templateCacheExists($cacheid, $ttl)
    {
        $cachePath = self::_getTemplateCachePath($cacheid);
        if (file_exists($cachePath) === TRUE) {
            if ((int) $ttl !== 0) {
                // TTL in minutes.
                $filemTime   = filemtime($cachePath);
                $currentTime = time();

                $timeDiff = floor((($currentTime - $filemTime) / 60));
                if ($timeDiff < $ttl) {
                    return TRUE;
                }
            } else {
                // Zero TTL = no expiry time. If it exists, return it.
                return TRUE;
            }
        }

        return FALSE;

    }//end templateCacheExists()


    /**
     * Recaches the specified template.
     *
     * Other systems can call this method to re-cache templates, its up to the
     * calling method to set the required template variables.
     *
     * Note that this function may throw exception when the template file is included.
     *
     * @param string $systemName   Name of the system that owns the template.
     * @param string $templateName Name of the template (without the tpl extension).
     *
     * @return void
     */
    public static function reCacheTemplate($systemName, $templateName)
    {
        $dir  = BaseSystem::getDataDir('GUI').'/rebakeTemplates';
        $path = $dir.'/'.$systemName.'-'.$templateName.'.tpl.php';

        if (file_exists($path) === FALSE) {
            // No re-cache template file.
            return;
        }

        // Include the re-cache template file. These files contain calls to
        // GUI::saveTemplateCache().
        self::$_isReCaching = TRUE;

        ob_start();
            include $path;
        ob_end_clean();

        self::$_isReCaching = FALSE;

    }//end reCacheTemplate()


    /**
     * Returns TRUE if we are in the middle of a reCacheTemplate() call.
     *
     * @return boolean
     */
    public static function isReCachingTemplate()
    {
        return self::$_isReCaching;

    }//end isReCachingTemplate()


    /**
     * Returns the file path of the specified cache file.
     *
     * @param string $cacheid      Id of the cache file.
     * @param string $system       Name of the system that owns the template.
     * @param string $templateName Name of the template file.
     *
     * @return string
     */
    private static function _getTemplateCachePath($cacheid, $system=NULL, $templateName=NULL)
    {
        if ($system === NULL && $templateName === NULL) {
            $currentTplKey = GUI::getCurrentTemplateKey();
            $currentTplKey = str_replace('::', '-', $currentTplKey);
        } else {
            $currentTplKey = $systemName.'-'.$templateName;
        }

        $basePath  = BaseSystem::getDataDir('GUI').'/tplCache';
        $cachePath = $basePath.'/'.$currentTplKey.'_'.$cacheid.'.tplc';
        return $cachePath;

    }//end _getTemplateCachePath()


    /**
     * Returns the attribute value after processing it for printing in PHP code.
     *
     * @param string $value The attribute value.
     *
     * @return string
     */
    private static function _printAttributeValue($value)
    {
        if (is_numeric($value) === TRUE) {
            return $value;
        }

        $value = str_replace('{', '{{', $value);
        $value = str_replace('}', '}}', $value);

        $value = self::_replaceVarKeywords($value, FALSE);
        $value = '"'.$value.'"';

        return $value;

    }//end _printAttributeValue()


    /**
     * Creates a new widget and returns its contents as string.
     *
     * This method should be used to create widgets at run time (e.g. by data provider).
     *
     * @param string $id       The id to use for the widget.
     * @param string $type     The type of the widget.
     * @param array  $settings Settings for the widget.
     *
     * @return string
     * @throws ChannelException If an invalid setting is used.
     */
    public static function createWidget($id, $type, array $settings=array())
    {
        foreach ($settings as $settingName => $value) {
            if (self::_isValidWidgetSetting($type, $settingName) === FALSE) {
                $msg = sprintf(
                    _('"%s" is not a valid setting for %s.'),
                    $settingName,
                    $type
                );
                throw new ChannelException($msg);
            }
        }

        $settings['widget'] = array(
                               'id'   => $id,
                               'type' => $type,
                              );

        list($tplSystem, $tplName) = explode('::', GUI::getCurrentTemplateKey());
        $settings['template']      = array(
                                      'system' => $tplSystem,
                                      'name'   => $tplName,
                                     );

        Channels::includeSystem($type);

        $jsFilePath  = Channels::getSystemsPath($type).'/Web/';
        $jsFilePath .= $type.'.js';

        ob_start();
            $settings = call_user_func(array($type, 'paint'), $settings);

            // Add its JS code.
            if (file_exists($jsFilePath) === TRUE) {
                echo '<script>'.GUI::getJSInitCode($type, $id, $settings).'</script>';
            }

            $content = ob_get_contents();
        ob_end_clean();

        return $content;

    }//end createWidget()


    /**
     * Adds script tag to given HEAD elem that includes the specified JS file.
     *
     * @param string  $systemName Name of the system.
     * @param string  $jsFile     The JS file to include.
     * @param DOMNode $head       The head element.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addJS($systemName, $jsFile, DOMNode $head)
    {
        $url    = '/__web/'.$jsFile;
        $script = $head->ownerDocument->createElement('script');
        $script->setAttribute('type', 'text/javascript');
        $script->setAttribute('src', $url);
        $script->nodeValue = '';
        $head->appendChild($script);

    }//end addJS()


    /**
     * Adds link tag to given HEAD elem that includes the specified CSS file.
     *
     * @param string  $systemName Name of the system.
     * @param string  $cssFile    The CSS file to include.
     * @param DOMNode $head       The head element.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addCSS($systemName, $cssFile, DOMNode $head)
    {
        $url  = '/__web/'.$cssFile;
        $link = $head->ownerDocument->createElement('link');
        $link->setAttribute('rel', 'stylesheet');
        $link->setAttribute('type', 'text/css');
        $link->setAttribute('href', $url);
        $head->appendChild($link);

    }//end addCSS()


    /**
     * Set the JS files to be included by a main template.
     *
     * @param array $jsFiles List of JS files to include.
     *
     * @since  4.0.0
     * @return void
     */
    public static function setJSIncludes(array $jsFiles)
    {
        self::$_jsIncludes = $jsFiles;

    }//end setJSIncludes()


    /**
     * Set the CSS files to be included by a main template.
     *
     * @param array $cssFiles List of CSS files to include.
     *
     * @since  4.0.0
     * @return void
     */
    public static function setCSSIncludes(array $cssFiles)
    {
        self::$_cssIncludes = $cssFiles;

    }//end setCSSIncludes()


    /**
     * Returns HTML CSS include string.
     *
     * @param string $browserType Type of the browser.
     *
     * @return string
     */
    private static function _getCSSIncludeCode($browserType)
    {
        $css = '';
        if ($browserType === NULL
            || isset(self::$_cssIncludes[$browserType]) === FALSE
        ) {
            return $css;
        }

        // Get parent browser's CSS.
        $css .= self::_getCSSIncludeCode(self::$_browserTypes[$browserType]);

        $minifiedFilePath = BaseSystem::getDataDir('GUI').'/Web/squiz.'.$browserType.'.css';

        $css  = '<?php ';
        $css .= 'if (file_exists(\''.$minifiedFilePath.'\') === TRUE) {'."\n";
        $css .= '?>';
        $css .= '<link rel="stylesheet" type="text/css" href="/__web/squiz.'.$browserType.'.css" />';
        $css .= '<?php '."\n";
        $css .= "} else {\n ?>";

        // Current browser's CSS.
        $chunks = array_chunk(self::$_cssIncludes[$browserType], 30);
        foreach ($chunks as $chunk) {
            $css .= '<style type="text/css">';
            foreach ($chunk as $cssFile) {
                $url  = '/__web'.$cssFile;
                $css .= '@import url("'.$url.'");'."\n";
            }

            $css .= '</style>';
        }

        $css .= '<?php } ?>'."\n";

        return $css;

    }//end _getCSSIncludeCode()


    /**
     * Returns the JS Include code.
     *
     * @return string
     */
    private static function _getJSIncludeCode()
    {
        $minifiedFilePath = BaseSystem::getDataDir('GUI').'/Web/squiz.js';
        $gzipedFilePath   = BaseSystem::getDataDir('GUI').'/Web/squiz.jgz';

        $js  = '<?php ';
        $js .= '// JS Includes.'."\n";
        $js .= 'if (file_exists(\''.$gzipedFilePath.'\') === TRUE) {'."\n";
        $js .= '?>';
        $js .= '    <script src="/__web/squiz.jgz" type="text/javascript"></script>';
        $js .= '<?php '."\n";
        $js .= '} else if (file_exists(\''.$minifiedFilePath.'\') === TRUE) {'."\n";
        $js .= '?>'."\n";
        $js .= '    <script src="/__web/squiz.js" type="text/javascript"></script>';
        $js .= '<?php '."\n";
        $js .= '} else {'."\n";
        $js .= '?>';

        foreach (self::$_jsIncludes as $jsFile) {
            $js .= '<script src="'.$jsFile.'" type="text/javascript"></script>'."\n";
        }

        $js .= '<?php '."\n";
        $js .= '}//end Include JS'."\n";
        $js .= ' ?'.'>';

        return $js;

    }//end _getJSIncludeCode()


    /**
     * Returns the system name from a given template file path.
     *
     * @param string $tplFilePath File path of the template file.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSystemNameFromTemplate($tplFilePath)
    {
        $parts = explode('/', $tplFilePath);
        array_pop($parts);
        $parts = array_reverse($parts);

        $systemName = '';
        foreach ($parts as $name) {
            if ($name === 'Systems') {
                break;
            } else if ($name !== 'Widgets' && $name !== 'Templates') {
                $systemName = $name.$systemName;
            }
        }

        return $systemName;

    }//end getSystemNameFromTemplate()


    /**
     * Returns the system name from a given widget type.
     *
     * @param string $widgetType Type of a widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSystemNameFromWidgetType($widgetType)
    {
        $system = str_replace('/', '', $widgetType);
        return $system;

    }//end getSystemNameFromWidgetType()


    /**
     * Returns JS initialisation code to be included into a template.
     *
     * This is used to initialiase the JS Widget objects. These objects can be
     * retrieved by calling GUI.getWidget(id).
     *
     * @param string $systemName Name of the system.
     * @param string $id         Unique ID of the widget object.
     * @param array  $settings   Settings array of the widget object.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getJSInitCode($systemName, $id, array $settings=array())
    {
        // Need to remove all settings that are internal.
        foreach ($settings as $settingName => $value) {
            if (self::_isValidWidgetSetting($systemName, $settingName, TRUE) === FALSE) {
                unset($settings[$settingName]);
            }
        }

        $code = 'if (window["'.$systemName.'"]) {GUI.addWidget("'.$id.'", new '.$systemName.'("'.$id.'", '.json_encode($settings).'));}';
        return $code;

    }//end getJSInitCode()


    /**
     * Removes setting values that contain PHP tags.
     *
     * @param array $settings Settings array.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _removePHPSettings(array $settings)
    {
        foreach ($settings as $setting => $value) {
            if (is_array($value) === TRUE) {
                $settings[$setting] = self::_removePHPSettings($value);
            } else if (strpos($value, '<?php') !== FALSE) {
                unset($settings[$setting]);
            }
        }

        return $settings;

    }//end _removePHPSettings()


    /**
     * Used to act like PHP 5.3 static:: call.
     *
     * When widgets are calling methods in their own class they should use this
     * method so that child widgets can override those methods.
     *
     * @param string $widgetType Type of the widget.
     * @param string $methodName Name of the method to call.
     * @param array  $args       Array of arguments to pass to the method.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function callStatic($widgetType, $methodName, array $args=array())
    {
        $system = GUI::getSystemNameFromWidgetType($widgetType);

        $callback = array(
                     $system,
                     $methodName,
                    );

        Channels::includeSystem($system);
        $retVal = call_user_func_array(
            $callback,
            $args
        );

        return $retVal;

    }//end callStatic()


    /**
     * Returns the browser type that is known by the GUI system.
     *
     * @see    self::$_browserTypes
     * @return mixed
     */
    public static function getBrowserType()
    {
        if (isset($_SERVER['HTTP_USER_AGENT']) === TRUE) {
            $uAgent = str_replace(' ', '', strtolower($_SERVER['HTTP_USER_AGENT']));
            foreach (self::$_browserTypes as $type => $parent) {
                if (strpos($uAgent, $type) !== FALSE) {
                    return $type;
                }
            }
        }

        return NULL;

    }//end getBrowserType()


    /**
     * Saves the widget data for each specified template.
     *
     * This method is public, so the save functions for each template need
     * to implement their own permission checking.
     *
     * @param array $templateData List if templates and data for its widgets.
     *
     * @return mixed
     * @throws ChannelException If there was an error while processing save data.
     *
     * @api            write
     * @api-permission public
     */
    public static function saveTemplateData(array $templateData)
    {
        $success = array();
        $errors  = array();

        try {
            foreach ($templateData as $key => $data) {
                list($system, $templateName) = explode(':', $key);

                // Check if its an implementer.
                $channelInfo = Channels::getSystemDependencies($system, 'inheritance');
                if (empty($channelInfo) === FALSE) {
                    $system = $channelInfo[0]['to'];
                }

                Channels::includeSystem($system);
                $templateName = 'save'.$templateName;

                if (is_callable($system.'::'.$templateName) === FALSE) {
                    // No save method for this template.
                    continue;
                }

                $callback = array(
                             $system,
                             $templateName,
                            );
                $retVal   = call_user_func($callback, $data);

                if (is_array($retVal) === TRUE) {
                    if (isset($retVal['errors']) === TRUE) {
                        if (is_array($retVal['errors']) === FALSE) {
                            $retVal['errors'] = array($retVal['errors']);
                        }

                        $errors[$key] = $retVal['errors'];

                        // Do not continue and rollback changes.
                        DAL::rollBack();
                        break;
                    } else if (isset($retVal['success']) === TRUE) {
                        $success[$key] = $retVal['success'];
                        continue;
                    }
                } else if ($retVal === TRUE) {
                    $success[$key] = TRUE;
                    continue;
                }

                throw new ChannelException('Invalid return type. ('.$system.'::'.$templateName.')');
            }//end foreach
        } catch (ChannelException $e) {
            DAL::rollBack();
            throw new ChannelException($e->getMessage());
        }//end try

        $retVal = NULL;
        if (empty($errors) === TRUE) {
            if (empty($success) === FALSE) {
                // Commit all the changes.
                DAL::commit();
                $retVal = array('success' => $success);
            }
        } else {
            $retVal = array(
                       'errors'    => $errors,
                       'errorList' => GUI::getSaveErrorList($errors),
                      );
        }

        return $retVal;

    }//end saveTemplateData()


    /**
     * Returns the content for the error list.
     *
     * @param array $errors The list of errors to print.
     *
     * @return string
     */
    public static function getSaveErrorList(array $errors)
    {
        $content  = '<div class="GUI-overlay-errorList">';
        $content .= '<ul>';

        foreach ($errors as $templateErrors) {
            foreach ($templateErrors as $error) {
                $content .= '<li>'.$error.'</li>';
            }
        }

        $content .= '</ul>';

        $buttonOpts = array(
                       'value'  => _('Return to screen'),
                       'colour' => 'Dark',
                       'click'  => 'GUI.dequeueOverlay(\'warning\')',
                      );

        $content .= GUI::createWidget('GUI-save-returnButton', 'GUIButton', $buttonOpts);
        $content .= '</div>';

        return $content;

    }//end getSaveErrorList()


    /**
     * Returns the specified system's web path.
     *
     * @param string $systemName Name of the system.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSystemWebPath($systemName)
    {
        $path = str_replace(Channels::getSystemsPath(), '', Channels::getSystemsPath($systemName));
        $path = 'Systems'.$path.'/'.self::$_publicDirName;

        return $path;

    }//end getSystemWebPath()


    /**
     * Returns the Doctype used by GUI.
     *
     * @return string
     */
    public static function getDocType()
    {
        return self::$_docType;

    }//end getDocType()


    /**
     * Minifies all JS files.
     *
     * If both arguments are FALSE then the JS files will be placed in to one file
     * without any minification.
     *
     * Returns the path to the generated file.
     *
     * @param boolean $minify If TRUE then JS content will be minified.
     * @param boolean $gzip   If TRUE then file will be gzipped.
     *
     * @return string
     */
    public static function minifyGzipJSFiles($minify=TRUE, $gzip=TRUE)
    {
        $root      = BaseSystem::getSystemRootPath();
        $jsminPath = $root.'/Systems/GUI/Scripts/jsmin-1.1.1.php';

        if (file_exists($jsminPath) === FALSE) {
            return FALSE;
        }

        // Get the list of JS files.
        $jsFiles = self::_getJSFiles();

        $jsContent = '';
        foreach ($jsFiles as $file) {
            $jsContent .= file_get_contents($file);
        }

        if ($minify === TRUE) {
            include_once $jsminPath;

            // @codingStandardsIgnoreStart
            // JSMin is the 3rd party library file and the way we include is
            // different from MySource standards.
            $jsContent = JSMin::minify($jsContent);
            // @codingStandardsIgnoreEnd
        }

        $dataWebDir = BaseSystem::getDataDir('GUI').'/Web';
        if (file_exists($dataWebDir) === FALSE) {
            mkdir($dataWebDir, 0755, TRUE);
        }

        $newFile = $dataWebDir.'/squiz.js';
        file_put_contents($newFile, $jsContent);

        if ($gzip === TRUE) {
            $jgzFile     = $dataWebDir.'/squiz.jgz';
            $gzipCommand = 'gzip -c -n '.$newFile.' > '.$jgzFile;
            system($gzipCommand);

            $newFile = $jgzFile;
        }

        return $newFile;

    }//end minifyGzipJSFiles()


    /**
     * Minifies all CSS files in all Web folders.
     *
     * @return void
     */
    public static function minifyCSSFiles()
    {
        $root       = BaseSystem::getSystemRootPath();
        $cssMinPath = $root.'/Systems/GUI/Scripts/cssmin.php';

        if (file_exists($cssMinPath) === FALSE) {
            return;
        }

        include_once $cssMinPath;

        $cssFiles = self::_getCSSFiles();
        $path     = BaseSystem::getDataDir('GUI').'/Web';
        $root     = BaseSystem::getSystemRootPath();

        foreach ($cssFiles as $browserType => $cssFiles) {
            $css = '';
            foreach ($cssFiles as $file) {
                $fileContent = file_get_contents($root.$file);
                $fileContent = self::_replaceURLforCSSminification($root.$file, $fileContent);

                $css .= $fileContent;
            }

            $css = trim($css);
            if (empty($css) === FALSE) {
                // PHPCS exception for external library file.
                // @codingStandardsIgnoreStart
                $css = CSSMin::minify($css, 900);
                // @codingStandardsIgnoreEnd

                file_put_contents($path.'/squiz.'.$browserType.'.css', $css);
            }
        }

    }//end minifyCSSFiles()


    /**
     * Replace url() values in CSS for the minification.
     *
     * @param string $file    CSS file path.
     * @param string $content CSS content.
     *
     * @return string
     */
    private static function _replaceURLforCSSminification($file, $content)
    {
        $root    = BaseSystem::getSystemRootPath();
        $matches = array();
        preg_match_all('/url\(([^\)]+)\)/', $content, $matches);
        $matches = array_unique($matches[1]);
        $fileDir = dirname($file);

        foreach ($matches as $match) {
            $newPath = realpath($fileDir.'/'.$match);
            if ($newPath === FALSE) {
                continue;
            }

            $replacement = str_replace($root, '/__web', $newPath);
            $content     = str_replace('('.$match.')', '('.$replacement.')', $content);
        }

        return $content;

    }//end _replaceURLforCSSminification()


    /**
     * Serves any file on the file system, using X-Sendfile.
     *
     * @param string $filePath Path to the file.
     * @param string $mimeType Content type of the file.
     * @param array  $headers  Extra headers to send.
     *
     * @since  4.0.0
     * @return void
     */
    public static function serveFile(
        $filePath,
        $mimeType,
        array $headers=array()
    ) {
        // Set the content type header, X-Sendfile headers.
        $headers['Content-type']         = $mimeType;
        $headers['X-Sendfile']           = $filePath;
        $headers['x-lighttpd-send-file'] = $filePath;

        if (headers_sent() === FALSE) {
            foreach ($headers as $header => $value) {
                header($header.': '.$value);
            }
        }

        exit();

    }//end serveFile()


    /**
     * Serves the requested file.
     *
     * If the file is not in the __web directory, will return FALSE.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function serveWebFile()
    {
        // Get it from URL.
        include_once 'Libs/Web/Web.inc';
        $url = Web::getCurrentUrl(FALSE);
        $idx = strpos($url, '/__web/');
        if ($idx === FALSE) {
            return FALSE;
        }

        $guiFiles = array(
                     'squiz.js',
                     'squiz.jgz',
                     'squiz.all.css',
                     'squiz.firefox.css',
                     'squiz.msie.css',
                     'squiz.msie7.css',
                     'squiz.msie8.css',
                     'squiz.msie9.css',
                     'squiz.safari.css',
                    );

        $guiFile     = FALSE;
        $systemsPath = Channels::getSystemsPath();
        $filename    = basename($url);

        if (in_array($filename, $guiFiles) === TRUE) {
            $path    = BaseSystem::getDataDir('GUI').'/Web/'.$filename;
            $guiFile = TRUE;
        } else {
            $path = dirname($systemsPath).'/'.substr(str_replace('//', '/', $url), ($idx + 6));
            $path = realpath($path);
        }

        // File does not exist.
        if ($path === FALSE) {
            header($_SERVER['SERVER_PROTOCOL'].' 404 Not Found');
            exit;
        }

        if ($guiFile === FALSE) {
            // Must be in the Systems dir.
            if (substr($path, 0, strlen($systemsPath)) !== $systemsPath
                || strpos($url, '/Web/') === FALSE
            ) {
                header($_SERVER['SERVER_PROTOCOL'].' 404 Not Found');
                exit;
            }
        }

        // All good start sending headers.
        $modified = date('D, j M Y 00:00:00');
        if (isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) === TRUE) {
            // Send a not modified header if this request is being
            // made on the same day.
            if ($modified === $_SERVER['HTTP_IF_MODIFIED_SINCE']) {
                header('HTTP/1.1 304 Not Modified');
                exit();
            }
        }

        if ($filename === 'styles_moz.css'
            || $filename === 'styles_ie7.css'
            || $filename === 'styles_ie8.css'
        ) {
            ob_start('ob_gzhandler');

            header('Content-type: text/css; charset: UTF-8');
            header('Cache-Control: must-revalidate');

            $now     = time();
            $expires = date('D, j M Y 00:00:00', ($now + 86400));
            header('Expires: '.$expires);
            header('Last-Modified: '.$modified);

            echo file_get_contents(dirname(__FILE__).'/web/'.$filename);
        } else {
            header('X-Sendfile: '.$path);
            if ($filename === 'squiz.jgz') {
                header('Content-Encoding: gzip');
                header('Content-type: application/x-javascript');
            } else {
                include_once 'Libs/FileSystem/FileSystem.inc';
                $mimeType = FileSystem::getMimeType($filename);

                header('Content-type: '.$mimeType);
            }

            $now     = time();
            $expires = date('D, j M Y 00:00:00', ($now + 86400));
            header('Expires: '.$expires);
            header('Last-Modified: '.$modified);
        }//end if

        // Don't do anything else.
        exit;

    }//end serveWebFile()


}//end class

?>
