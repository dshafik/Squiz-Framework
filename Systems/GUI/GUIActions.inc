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
     * Templates initialisation PHP code.
     *
     * @var string
     */
    private static $_templateInitCode = '';

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
     * Includes the template PHP file.
     *
     * @param string $systemName Name of the system.
     * @param string $template   The template to load.
     *
     * @since  4.0.0
     * @return void
     *
     * @api read
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
        $contents  = self::$_docType;
        $contents .= '<gui_root_element>'.$fileCont.'</gui_root_element>';

        $tpl = new DOMDocument('1.0');
        $tpl->loadXML($contents);
        return $tpl;

    }//end loadTemplate()


    /**
     * Bakes all the templates in the Systems dir.
     *
     * @param boolean $minified If TRUE then JS and CSS will be minified.
     *
     * @since  4.0.0
     * @return void
     */
    public static function bakeTemplates($minified=FALSE)
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
        $jsFiles  = FileSystem::listDirectory($path, array('.js'));
        $cssFiles = FileSystem::listDirectory($path, array('.css'));

        $ignoreJSFiles = array('SquizAPI_demo.js');

        // Convert them to URLs.
        foreach ($jsFiles as $index => $file) {
            if (strpos($file, 'DfxJSLib') !== FALSE
                || in_array(basename($file), $ignoreJSFiles) === TRUE
            ) {
                unset($jsFiles[$index]);
            } else {
                $url = str_replace($root, '', $file);
                $jsFiles[$index] = $url;
            }
        }

        $guiPath = GUI::getSystemWebPath('GUI');
        array_unshift($jsFiles, $guiPath.'/DfxJSLib/mysource.js');
        $url        = str_replace($root, '', $guiPath.'/DfxJSLib/mysource.js');
        $jsFiles[0] = '/'.$url;

        foreach ($cssFiles as $index => $file) {
            $url = str_replace($root, '', $file);
            $cssFiles[$index] = $url;
        }

        if ($minified === FALSE) {
            GUI::setJSIncludes($jsFiles);
            GUI::setCSSIncludes($cssFiles);
        }

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
                    GUI::bakeTemplate($systemName, str_replace('.tpl', '', basename($file)), $tplDoc);
                }
            }
        }//end foreach

        self::_createWidgetSettingsFile();

        self::$_isBaking = FALSE;

    }//end bakeTemplates()


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
     * Bakes out the template's PHP file.
     *
     * @param string  $systemName   Name of the system that owns the template.
     * @param string  $templateName Name of the template.
     * @param DOMNode $tplDoc       The HTML of the template as DOM object.
     *
     * @since  4.0.0
     * @return void
     */
    public static function bakeTemplate($systemName, $templateName, DOMNode $tplDoc)
    {
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

        // Add the PHP code to member var so that during parsing it can be updated.
        self::$_templateInitCode      = $php;
        self::$_templateVariablesCode = '';

        // Parse the special tags in the template file.
        GUI::parseTemplate($tplDoc);

        $initCodes  = self::$_templateVariablesCode;
        $initCodes .= self::$_templateInitCode;

        $phpNode = $tplDoc->createProcessingInstruction('php', $initCodes);
        $tplDoc->insertBefore($phpNode, $tplDoc->firstChild);

        $resetCurrTemplateCode = 'GUI::setCurrentTemplateKey($'.$tplVarName.');'."\n";
        $phpNode = $tplDoc->createProcessingInstruction('php', $resetCurrTemplateCode);
        $tplDoc->appendChild($phpNode);

        // Now add the dependent files to the HEAD tag, if it exists.
        $head = $tplDoc->getElementsByTagName('head');
        if ($head->length !== 0) {
            $head = $head->item(0);

            foreach (self::$_jsIncludes as $jsInclude) {
                GUI::addJS(NULL, $jsInclude, $head);
            }

            foreach (self::$_cssIncludes as $cssInclude) {
                GUI::addCSS(NULL, $cssInclude, $head);
            }
        }

        include_once 'Libs/XML/XML.inc';
        $xmlContent = $tplDoc->saveXML();
        // Remove the root element that was added when loading XML content from file.
        $xmlContent = str_replace('<gui_root_element>', '', $xmlContent);
        $xmlContent = str_replace('</gui_root_element>', '', $xmlContent);

        $htmlCont = XML::xmlToXHTML($xmlContent);

        // If <head> element exists, insert API token hidden input field
        // for the interface to work.
        if ($head->length !== 0) {
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
        $dest = $syDataPath.'/'.$templateName.'.tpl.php';
        file_put_contents($dest, $htmlCont);

    }//end bakeTemplate()


    /**
     * Replace variable keywords in parsed HTML content.
     *
     * @param string $content The HTML content to search.
     *
     * @return string
     */
    private static function _replaceVarKeywords($content)
    {
        // Replace all template variables with the actual PHP variables.
        $content = preg_replace_callback('/{([a-z0-9_:]+)}/i', array('self', '_replaceVarKeywordsCallback'), $content);
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
        $code = self::_getKeywordVariableCode($matches[1]);
        $code = '<'.'?php echo '.$code.';?'.'>';

        return $code;

    }//end _replaceVarKeywordsCallback()


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
     * @param DOMNode $tplDoc The Template DOMDocument.
     *
     * @since  4.0.0
     * @return void
     */
    public static function parseTemplate(DOMNode $tplDoc)
    {
        include_once 'Libs/XML/XML.inc';
        $tagNames = array(
                     'var',
                     'widget',
                     'text',
                     'dataProvider',
                     'condition',
                    );

        foreach ($tagNames as $tagName) {
            $tags     = $tplDoc->getElementsByTagName($tagName);
            $topLevel = array();
            foreach ($tags as $tag) {
                if (XML::hasParentTag($tag, $tagNames) === FALSE) {
                    $topLevel[] = $tag;
                }
            }

            foreach ($topLevel as $tag) {
                call_user_func(array('self', '_parse'.ucfirst($tagName).'Tag'), $tag);
            }
        }

    }//end parseTemplate()


    /**
     * Converts widget tags to HTML/PHP.
     *
     * @param DOMNode $parent The parent tag.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _convertWidgetTags(DOMNode $parent)
    {
        $php .= self::_getVarCode('widget').' = array();'."\n";

        while (TRUE) {
            $widgets = $parent->getElementsByTagName('widget');
            if ($widgets->length === 0) {
                break;
            }

            $php .= self::_parseWidgetTag($widgets->item(0));
        }

        return $php;

    }//end _convertWidgetTags()


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

        if ($assignTo === NULL) {
            self::$_templateInitCode .= $settingsCode;
        } else {
            $php .= $settingsCode;
        }

        // Include widget system.
        $php .= 'Channels::includeSystem("'.$widgetSystemName.'");'."\n";

        if ($assignTo !== NULL) {
            // Need to assign the printed content to given variable so
            // use output buffering.
            $php .= "ob_start();\n";
        }

        // Add the widget paint() call.
        $php .= $settingsVar.' = '.$widgetSystemName.'::paint('.$settingsVar.');'."\n";

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
            $tmpDoc  = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $tmpNode = $tmpDoc->getElementsByTagName('phpCode')->item(0)->firstChild;
            $node    = $widget->ownerDocument->importNode($tmpNode, TRUE);
            $widget->parentNode->insertBefore($node, $widget);
            $widget->parentNode->removeChild($widget);
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

        $indexVarName = NULL;
        foreach ($setting->childNodes as $node) {
            if ($node->nodeType === XML_TEXT_NODE) {
                continue;
            } else if ($node->nodeType === XML_ELEMENT_NODE) {
                // There are a few special/reserved tags.
                switch ($node->tagName) {
                    case 'text':
                        $php .= self::_parseTextTag($node, $varName);
                    break;

                    case 'widget':
                        $php .= self::_parseWidgetTag($node, $varName);
                    break;

                    case 'dataProvider':
                        $php .= self::_parseDataProviderTag($node, $varName);
                    break;

                    case 'widget-settings':
                        $php .= self::_parseWidgetSettingsTag($node, $varName);
                    break;

                    case 'condition':
                        $php .= self::_parseConditionTag($node, $varName);
                    break;

                    default:
                        $php .= self::_parseTag($node, $varName, $indexVarName);
                    break;
                }//end switch
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
                $php .= self::_parseConditionTag($node, $varName);
            break;

            case 'content':
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

                // Just get the content we want.
                $subTemplateStr = str_replace(self::$_docType, '', $subTemplateStr);
                $subTemplateStr = trim(str_replace('<?xml version="1.0"?>', '', $subTemplateStr));
                $subTemplateStr = str_replace('<__tempContent>', '', $subTemplateStr);
                $subTemplateStr = str_replace('</__tempContent>', '', $subTemplateStr);
                $subTemplateStr = ' ?>'.$subTemplateStr.'<? ';

                $php .= 'ob_start();'."\n";
                $php .= $subTemplateStr;
                $php .= $assignTo.' = ob_get_clean();'."\n";
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
                 'init' => $php,
                 'list' => implode(', ', $argList),
                );

        return $code;

    }//end _getArgumentListCodes()


    /**
     * Returns the PHP script of the given text tag.
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

            $php .= '_("'.$value.'");'."\n";
        } else {
            // Init code for the arguments.
            $php .= $tagInfo['argsCode']['init'];

            $printArgCode = '_("'.$value.'"), '.$tagInfo['argsCode']['list'];

            if ($assignTo !== NULL) {
                $php .= $assignTo.' = sprintf('.$printArgCode.');'."\n";
            } else {
                $php .= 'printf('.$printArgCode.');'."\n";
            }
        }

        if ($assignTo === NULL) {
            // Top level text. Need to replace the text tag with the PHP code.
            $tmpDoc  = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $tmpNode = $tmpDoc->getElementsByTagName('phpCode')->item(0)->firstChild;
            $node    = $text->ownerDocument->importNode($tmpNode, TRUE);
            $text->parentNode->insertBefore($node, $text);
            $text->parentNode->removeChild($text);
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
                    'argsCode' => array(),
                   );

        foreach ($text->childNodes as $node) {
            if ($node->nodeType === XML_TEXT_NODE
                || $node->nodeType === XML_CDATA_SECTION_NODE
            ) {
                $tagInfo['value'] .= addcslashes($node->nodeValue, "\"\\\0");
            } else if ($node->nodeType === XML_ELEMENT_NODE) {
                if ($node->tagName === 'value') {
                    $tagInfo['value'] .= addcslashes($node->nodeValue, "\"\\\0");
                } else if ($node->tagName === 'args') {
                    $tagInfo['argsCode'] = self::_getArgumentListCodes($node);

                    // Cannot have any more tags.
                    break;
                }
            }
        }

        $tagInfo['value'] = trim($tagInfo['value']);

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
                     'if'   => 'if',
                     'else' => 'else',
                    );

        $php = '';
        foreach ($node->childNodes as $child) {
            if ($child->nodeType === XML_ELEMENT_NODE
                && isset($tagTypes[$child->tagName]) === TRUE
            ) {
                $condition = $child->getAttribute('condition');

                // Get the system name so we can include it.
                list($systemName, $method) = explode('::', $condition);
                $php .= 'Channels::includeSystem(\''.$systemName.'\');'."\n";

                // Print the if, else block.
                $php .= $tagTypes[$child->tagName];

                if ($child->tagName !== 'else') {
                    $php .= ' ('.$condition.' === TRUE) {'."\n";
                }

                foreach ($child->childNodes as $condChild) {
                    $php .= self::_parseTag($condChild, $assignTo);
                }

                $php .= "\n}\n";
            }//end if
        }//end foreach

        if ($assignTo === NULL) {
            // Top level.
            $tmpDoc  = DOMDocument::loadXML('<phpCode><?php '.$php.' ?></phpCode>');
            $tmpNode = $tmpDoc->getElementsByTagName('phpCode')->item(0)->firstChild;
            $newNode = $node->ownerDocument->importNode($tmpNode, TRUE);
            $node->parentNode->insertBefore($newNode, $node);
            $node->parentNode->removeChild($node);
        }

        return $php;

    }//end _parseConditionTag()


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
        $php       .= self::_parseTag($firstChild, $varName);

        self::$_templateVariablesCode .= $php;

        // Remove the var tag.
        $var->parentNode->removeChild($var);

        return '';

    }//end _parseVarTag()


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
     * Saves the widget data for each specified template.
     *
     * @param array $templateData List if templates and data for its widgets.
     *
     * @return mixed
     * @throws ChannelException If there was an error while processing save data.
     *
     * @api write
     */
    public static function saveTemplateData(array $templateData)
    {
        $success = array();
        $errors  = array();

        try {
            foreach ($templateData as $key => $data) {
                list($system, $templateName) = explode(':', $key);
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
                       'errorList' => self::_getSaveErrorList($errors),
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
    private static function _getSaveErrorList(array $errors)
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
                       'click'  => 'GUI.hideOverlay()',
                      );

        $content .= GUI::createWidget('GUI-save-returnButton', 'GUIButton', $buttonOpts);
        $content .= '</div>';

        return $content;

    }//end _getSaveErrorList()


    /**
     * Copies the web files from Systems dir to web dir.
     *
     * @since  4.0.0
     * @return void
     */
    public static function copyWebFiles()
    {
        include_once 'Libs/FileSystem/FileSystem.inc';

        $paths = array(Init::ROOT_DIR.'/Systems' => 'Systems');
        foreach ($paths as $systemsPath => $webName) {
            $webDirds = FileSystem::findDirectories($systemsPath, 'web');
            $webDir   = Init::WEB_DIR.'/'.$webName;

            // Need to keep the directory structure.
            $systemsPath = Channels::getSystemsPath();
            foreach ($webDirds as $dir) {
                $path = str_replace($systemsPath, '', $dir);
                $path = dirname(str_replace('/Widgets', '', $path));

                @FileSystem::mkdir($webDir.$path, 0755, TRUE);

                // Copy the contents.
                FileSystem::copyDirectory($dir, $webDir.$path);
            }
        }

    }//end copyWebFiles()


    /**
     * Returns the specified systems lineage/path.
     *
     * @param string $systemName Name of the system.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getSystemLineage($systemName)
    {
        $systemsPath = Channels::getSystemsPath();
        $path        = Channels::getSystemsPath($systemName);
        $path        = str_replace($systemsPath, '', $path);
        $path        = str_replace('/Widgets', '', $path);

        return $path;

    }//end _getSystemLineage()


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

        $systemsPath = Channels::getSystemsPath();

        $path = dirname(Channels::getSystemsPath()).'/'.substr(str_replace('//', '/', $url), ($idx + 6));
        $path = realpath($path);

        // File does not exist.
        if ($path === FALSE) {
            header($_SERVER['SERVER_PROTOCOL'].' 404 Not Found');
            exit;
        }

        // Must be in the Systems dir.
        if (substr($path, 0, strlen($systemsPath)) !== $systemsPath
            || strpos($url, '/Web/') === FALSE
        ) {
            header($_SERVER['SERVER_PROTOCOL'].' 404 Not Found');
            exit;
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

        include_once 'Libs/FileSystem/FileSystem.inc';
        $filename = basename($path);
        $mimeType = FileSystem::getMimeType($filename);

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
            if ($filename === 'mysource.jgz' || $filename === 'dfx.jgz') {
                header('Content-Encoding: gzip');
                header('Content-type: application/x-javascript');
            } else {
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