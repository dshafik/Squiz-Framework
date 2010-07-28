<?php
/**
 * Actions for the GUI System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUI
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Actions Class.
 *
 * @since 4.0.0
 */
class GUIActions
{

    /**
     * The docType to use for DOMDocument.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_docType = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">';

    /**
     * The widget settings required by a template, organised by template key.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_requiredWidgets = array();

    /**
     * Array of what templates require which other templates (notably, widgets).
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_dependants = array();

    /**
     * The JS files to include.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_jsIncludes = array();

    /**
     * The CSS files to include.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_cssIncludes = array();

    /**
     * The name of the publicly accessible directory for systems.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_publicDirName = 'Web';

    /**
     * List if systems and their settings that needs to do post bake processing.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_postBakeProcessors = array();

    /**
     * List of widgets to initialise in main template.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_tmpWidgetList = array();

    /**
     * List of widget settings for a template.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_tmpWidgetSettings = array();


    /**
     * Template stack.
     *
     * Only populated during getTemplateContent() calls.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_templateStack = array();


    /**
     * Includes the template PHP file.
     *
     * @param string $systemName Name of the system.
     * @param string $template   The template to load.
     *
     * @since  4.0.0
     * @return void
     */
    public static function printTemplate($systemName, $template)
    {
        $dataPath = BaseSystem::getDataDir($systemName);
        $dest     = $dataPath.'/Templates/'.$template.'.php';

        if (file_exists($dest) === FALSE) {
            echo '<div>ERROR: Template ('.$template.') file not found. ('.$dest.')</div>';
            return;
        }

        $deps = GUI::getTemplateDependentFiles($systemName, $template);

        foreach ($deps['widgets'] as $widgets) {
            foreach ($widgets as $widget) {
                self::$_tmpWidgetList[$widget['id']] = array(
                                                        'type'     => $widget['type'],
                                                        'settings' => $widget['settings'],
                                                       );
            }
        }

        include_once $dest;

    }//end printTemplate()


    /**
     * Includes the PHP file containing the dependencies of the template.
     *
     * The template dependencies file contains:
     * - the JavaScript and CSS files that are loaded in this template,
     * - the widgets that exist in this template.
     *
     * The intention is to provide enough information for such files and widgets to
     * be dynamically loaded if necessary, instead of just being hard-coded into a
     * template.
     *
     * @param string $systemName Name of the system.
     * @param string $template   The template to load.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getTemplateDependentFiles($systemName, $template)
    {
        $dataPath = BaseSystem::getDataDir($systemName);
        $depFile  = $dataPath.'/Templates/'.$template.'.deps.php';
        if (file_exists($depFile) === FALSE) {
            return array();
        }

        include $depFile;

        return $dependentFiles;

    }//end getTemplateDependentFiles()


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
        $contents .= $fileCont;

        $tpl = new DOMDocument('1.0');
        $tpl->loadXML($contents);
        return $tpl;

    }//end loadTemplate()


    /**
     * Returns the template contents as DOMNode.
     *
     * @param string $systemName Name of the system.
     * @param array  $settings   List of settings.
     * @param string $tplFile    The template file to load.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function getTemplateContent(
        $systemName,
        array $settings=array(),
        $tplFile=NULL
    ) {
        Channels::includeSystem('GUI');

        // Build required template key, something like 'systemName:templateName'.
        // Default templateName's (tplFile === NULL) have the same name as the
        // system name.
        $templateKey = self::_getTemplateKey($systemName, $tplFile);

        // Add the template to the stack.
        array_unshift(self::$_templateStack, explode(':', $templateKey, 2));

        // Get the template content from the system as a string.
        if ($systemName === 'GUI') {
            return NULL;
        } else {
            Channels::includeSystem($systemName);
            $content = trim(call_user_func(array($systemName, 'getTemplateContent'), $settings, $tplFile));
        }

        if (empty($content) === TRUE) {
            $content = '<div>(ERROR) Empty Template file in '.$systemName.': '.$tplFile.'</div>';
        }

        $content = self::_replaceKeywords($content, $systemName, $settings);

        // Add it to dom object.
        $doc = new DOMDocument('1.0');
        $doc->loadXML($content);

        // If this a top level template then reset vars.
        $head = $doc->getElementsByTagName('head');
        if ($head->length === 1) {
            self::$_tmpWidgetList     = array();
            self::$_tmpWidgetSettings = array();
        }

        // Find what widgets we have, and store them by widget type.
        $includedWidgets = array();
        $widgetNodes     = $doc->documentElement->getElementsByTagName('widget');

        foreach ($widgetNodes as $widgetNode) {
            $widgetId       = (string) $widgetNode->getAttribute('id');
            $widgetName     = (string) $widgetNode->getAttribute('type');
            $widgetName     = str_replace('/', '', $widgetName);
            $widgetSettings = self::_getSettingsForWidget($widgetNode);

            // Widgets always use the default template.
            $widgetTemplateKey = self::_getTemplateKey($widgetName, NULL);

            $thisWidget = array(
                           'id'       => $widgetId,
                           'type'     => $widgetName,
                           'settings' => $widgetSettings,
                          );

            self::$_tmpWidgetList[$widgetId] = array(
                                                'type'     => $widgetName,
                                                'settings' => $widgetSettings,
                                               );

            $includedWidgets[$widgetTemplateKey][] = $thisWidget;
        }//end foreach

        self::$_requiredWidgets[$templateKey] = $includedWidgets;

        // Convert all widget tags.
        GUI::convertWidgetTags($doc->documentElement);

        // Work out which templates require which.
        self::$_dependants[$templateKey] = array();

        foreach ($includedWidgets as $widgetTemplateKey => $typeWidgets) {
            foreach ($typeWidgets as $includedWidget) {
                $widgetId = $includedWidget['id'];

                self::$_dependants[$templateKey] = array_merge(
                    self::$_dependants[$templateKey],
                    Util::getArrayIndex(
                        self::$_dependants,
                        $widgetTemplateKey,
                        array()
                    ),
                    array($widgetTemplateKey)
                );
            }//end foreach
        }//end foreach

        self::$_dependants[$templateKey] = array_unique(
            self::$_dependants[$templateKey]
        );

        // Call prepareBake.
        call_user_func(array($systemName, 'prepareBake'), $doc->documentElement, $settings);

        // Remove the template from the stack.
        array_shift(self::$_templateStack);

        return $doc->documentElement;

    }//end getTemplateContent()


    /**
    * Get the details of the template including the current widget during bake time.
    *
    * Only gives meaningful information when getTemplateContent() is being called,
    * and we are getting the contents of a widget (more correctly, we are not working
    * on a top-level template). Otherwise, returns NULL.
    *
    * We get the SECOND template on the stack, since the first one will be the
    * currently printed template.
    *
    * @since  4.0.0
    * @return array
    */
    public static function getBakeTimeIncludingTemplate()
    {
        if (count(self::$_templateStack) > 1) {
            $retval = array(
                       'system'   => self::$_templateStack[1][0],
                       'template' => self::$_templateStack[1][1],
                      );
        } else {
            $retval = NULL;
        }

        return $retval;

    }//end getParentTemplate()


    /**
     * Converts widget tags to their HTML.
     *
     * @param DOMNode $parent The parent tag.
     *
     * @since  4.0.0
     * @return void
     */
    public static function convertWidgetTags(DOMNode $parent)
    {
        $warray  = array();
        $widgets = $parent->getElementsByTagName('widget');
        foreach ($widgets as $widget) {
            $warray[] = $widget;
        }

        Channels::includeSystem('GUI');
        foreach ($warray as $widget) {
            GUI::convertWidgetToHTML($widget);
        }

    }//end convertWidgetTags()


    /**
     * Replaces the specified widget tag with its HTML tags.
     *
     * @param DOMNode $widget The widget tag to replace.
     *
     * @since  4.0.0
     * @return void
     */
    public static function convertWidgetToHTML(DOMNode $widget)
    {
        Channels::includeSystem('GUI');
        $type     = $widget->getAttribute('type');
        $system   = GUI::getSystemNameFromWidgetType($type);
        $settings = self::_getSettingsForWidget($widget);

        // Get the widget contents as DOMNode.
        $content = GUI::getTemplateContent($system, $settings);

        if (is_string($content) === TRUE) {
            $content = DOMDocument::loadXML($content);
        }

        // Replace widget tag with the html.
        $node = $widget->ownerDocument->importNode($content, TRUE);
        $widget->parentNode->insertBefore($node, $widget);
        $widget->parentNode->removeChild($widget);

    }//end convertWidgetToHTML()


    /**
     * Returns the HTML content of the specified widget.
     *
     * @param DOMNode $widget The widget tag to replace.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getWidgetHTML(DOMNode $widget)
    {
        Channels::includeSystem('GUI');
        $type     = $widget->getAttribute('type');
        $system   = GUI::getSystemNameFromWidgetType($type);
        $settings = self::_getSettingsForWidget($widget);

        // Get the widget contents as DOMNode.
        $content = GUI::getTemplateContent($system, $settings);

        if (is_string($content) === TRUE) {
            $content = DOMDocument::loadXML($content);
        }

        $content = $content->ownerDocument->saveXML();
        $content = trim(str_replace('<?xml version="1.0"?>', '', $content));
        return $content;

    }//end getWidgetHTML()


    /**
     * Returns the list of settings for a widget tag.
     *
     * Settings array will contain all the attributes of widget tag and its
     * child setting tags which have name and value attributes.
     *
     * @param DOMNode $widget The widget tag with attributes and setting nodes.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getSettingsForWidget(DOMNode $widget)
    {
        $widgetid = $widget->getAttribute('id');
        if (isset(self::$_tmpWidgetSettings[$widgetid]) === TRUE) {
            return self::$_tmpWidgetSettings[$widgetid];
        }

        $settings = array();

        // Get the widget attributes.
        foreach ($widget->attributes as $attr) {
            $settings[$attr->name] = $attr->value;
        }

        foreach ($widget->childNodes as $node) {
            if ($node->nodeType !== XML_ELEMENT_NODE
                || $node->tagName !== 'setting'
            ) {
                continue;
            }

            // Found a setting node.
            $setting = $node->getAttribute('name');
            $value   = self::_parseSettingValue($node);

            // TODO: Make this work with array setting values.
            if (strpos($value, '#') === 0) {
                $value = self::_replaceWidgetIds($value);
            }

            $settings[$setting] = $value;
        }//end foreach

        self::$_tmpWidgetSettings[$widgetid] = $settings;

        return $settings;

    }//end _getSettingsForWidget()


    /**
     * Parses specified widget setting node.
     *
     * @param DOMNode $settingNode A single setting for a widget.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _parseSettingValue(DOMNode $settingNode)
    {
        if ($settingNode->hasAttribute('value') === TRUE) {
            return $settingNode->getAttribute('value');
        }

        include_once 'Libs/XML/XML.inc';

        if (XML::hasChildElements($settingNode) === FALSE) {
            return $settingNode->nodeValue;
        }

        $value = array();

        foreach ($settingNode->childNodes as $childNode) {
            if ($childNode->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            switch ($childNode->tagName) {
                case 'widget':
                    $value = GUI::getWidgetHTML($childNode);
                break;

                case 'value':
                    $value[] = self::_parseSettingValue($childNode);
                break;

                default:
                    $value[$childNode->tagName] = self::_parseSettingValue($childNode);
                break;
            }//end switch
        }//end foreach

        return $value;

    }//end _parseSettingValue()


    /**
     * Bakes out the templates PHP file.
     *
     * @param string  $systemName Name of the system that owns the template.
     * @param string  $tplName    Name of the template file.
     * @param DOMNode $html       The HTML of the template.
     *
     * @since  4.0.0
     * @return string
     */
    public static function bakeTemplate($systemName, $tplName, DOMNode $html)
    {
        Channels::includeSystem('GUI');

        // Run post bake processes.
        self::_runPostbakeFunctions($html->ownerDocument);

        // Reset post bake processors.
        self::$_postBakeProcessors = array();

        // Now work out exactly which ones we need for this template.
        $templateKey = self::_getTemplateKey($systemName, $tplName);
        $widgets     = Util::getArrayIndex(self::$_requiredWidgets, $templateKey, array());

        // Write out the list of dependents.
        $saveArray = array(
                      'widgets' => self::$_requiredWidgets[$templateKey],
                     );

        include_once 'Libs/FileSystem/FileSystem.inc';
        $syDataPath = BaseSystem::getDataDir($systemName).'/Templates';
        if (file_exists($syDataPath) === FALSE) {
            FileSystem::mkdir($syDataPath, 0755, TRUE);
        }

        $dest = $syDataPath.'/'.$tplName.'.deps.php';
        FileSystem::arrayToFile($saveArray, 'dependentFiles', $dest);

        // Now add the dependent files to the HEAD tag, if it exists.
        $head = $html->getElementsByTagName('head');

        if ($head->length !== 0) {
            $head = $head->item(0);

            foreach (self::$_jsIncludes as $jsInclude) {
                GUI::addJS(NULL, $jsInclude, $head);
            }

            foreach (self::$_cssIncludes as $cssInclude) {
                GUI::addCSS(NULL, $cssInclude, $head);
            }

            $js = array();

            foreach (self::$_tmpWidgetList as $widgetid => $widget) {
                $js[] = GUI::getJSInitCode($widget['type'], $widgetid, $widget['settings']);
            }

            // Wrote out all the widgets in this template, clear widget list.
            self::$_tmpWidgetList     = array();
            self::$_tmpWidgetSettings = array();

            // Create the script tag that contains JS init codes.
            $jsElem = $head->ownerDocument->createElement('script');
            $js     = implode("\n", $js);

            // All these needs to be wrapped with onLoad.
            $js = "dfx.addLoadEvent(function() {\n\t".$js."\n});";

            $jsElem->nodeValue = $js;
            $head->appendChild($jsElem);
        }//end if

        // Get the HTML string.
        include_once 'Libs/XML/XML.inc';
        $htmlCont = $html->ownerDocument->saveXML();
        $htmlCont = XML::xmlToXHTML($htmlCont);

        // If <head> element exists, insert API token hidden input field
        // for the interface to work.
        if ($head->length !== 0) {
            $replacement  = '<?php '."\n";
            $replacement .= 'Channels::includeSystem(\'User\');'."\n";
            $replacement .= 'Channels::includeSystem(\'API\');'."\n";
            $replacement .= '$currUserid = User::getCurrentUserid();'."\n";
            $replacement .= 'if ($currUserid !== NULL) {'."\n";
            $replacement .= '   Channels::includeSystem(\'Session\');'."\n";
            $replacement .= '   $currToken = Session::getSessionData(\'API\', \'userToken:\'.$currUserid);'."\n";
            $replacement .= '   if ($currToken === NULL) {'."\n";
            $replacement .= '       $currToken = API::getUniqueToken();'."\n";
            $replacement .= '       Session::setSessionData(\'API\', \'userToken:\'.$currUserid, $currToken);'."\n";
            $replacement .= '   }'."\n";
            $replacement .= '   echo \'<input id="__api_token" type="hidden" value="\'.$currToken.\'" />\';'."\n";
            $replacement .= '}'."\n";
            $replacement .= '?'.'>'."\n";
            $replacement .= '</body>'."\n";

            $htmlCont = str_replace('</body>', $replacement, $htmlCont);
        }//end if

        // Do some cleanup.
        $htmlCont = str_replace(self::$_docType, '', $htmlCont);
        $htmlCont = trim(str_replace('<?xml version="1.0"?>', '', $htmlCont));

        $htmlCont = str_replace('&lt;?php', '<'.'?php', $htmlCont);
        $htmlCont = str_replace('?&gt;', '?'.'>', $htmlCont);

        // Write to data file.
        $dest = $syDataPath.'/'.$tplName.'.php';
        file_put_contents($dest, $htmlCont);

        return $htmlCont;

    }//end bakeTemplate()


    /**
     * Adds a new post bake process.
     *
     * @param string $system   Name of the system that will processes the settings.
     * @param array  $settings Settings for the post bake process or widget.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addPostBakeProcess($system, array $settings)
    {
        self::$_postBakeProcessors[$settings['id']] = array(
                                                       'system'   => $system,
                                                       'settings' => $settings,
                                                      );

    }//end addPostBakeProcess()


    /**
     * Runs the post bake functions.
     *
     * @param DOMNode $html The whole HTML node.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _runPostbakeFunctions(DOMNode $html)
    {
        foreach (self::$_postBakeProcessors as $p) {
            call_user_func(array($p['system'], 'postBake'), $html, $p['settings']);
        }

    }//end _runPostbakeFunctions()


    /**
     * Adds JS includes.
     *
     * @param string $systemName Name of the system.
     * @param array  $jsFiles    List of JS files to be included for the system.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addJSInclude($systemName, array $jsFiles=array())
    {
        if (isset(self::$_jsIncludes[$systemName]) === FALSE) {
            self::$_jsIncludes[$systemName] = array();
        }

        self::$_jsIncludes[$systemName] = array_merge(
            self::$_jsIncludes[$systemName],
            $jsFiles
        );

    }//end addJSInclude()


    /**
     * Returns list of JS files to include.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getJSIncludes()
    {
        return self::$_jsIncludes;

    }//end _getJSIncludes()


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
     * Returns list of CSS files to include.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getCSSIncludes()
    {
        return self::$_cssIncludes;

    }//end _getCSSIncludes()


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
        $name = basename(dirname(dirname($tplFilePath)));
        return $name;

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
     * Wraps the given DOMNode with specified IF conditions.
     *
     * @param DOMNode $elem The element to wrap.
     * @param string  $cond Valid PHP if conditions.
     *
     * @since  4.0.0
     * @return void
     */
    public static function wrapConditionAroundElement(DOMNode $elem, $cond)
    {
        $doc = $elem->ownerDocument;

        // First all systems used in the conditions must be included.
        $matches = array();
        preg_match_all('/([a-zA-Z]+)::/', $cond, $matches);

        $includes = '';
        if (empty($matches) === FALSE && isset($matches[1]) === TRUE) {
            $ignoreSystems = array(
                              'Channels',
                              'BaseSystem',
                              'DAL',
                             );

            foreach ($matches[1] as $systemName) {
                if (in_array($systemName, $ignoreSystems) === FALSE) {
                    $includes       .= 'Channels::includeSystem(\''.$systemName.'\');'."\n";
                    $ignoreSystems[] = $systemName;
                }
            }
        }

        $cond   = $includes.'if ('.$cond.') {';
        $before = $doc->createProcessingInstruction('php', $cond);
        $after  = $doc->createProcessingInstruction('php', '}');

        $elem->parentNode->insertBefore($before, $elem);

        if ($elem->nextSibling !== NULL) {
            $elem->parentNode->insertBefore($after, $elem->nextSibling);
        } else {
            $elem->parentNode->appendChild($after);
        }

    }//end wrapConditionAroundElement()


    /**
     * Returns a template's associated JS file path.
     *
     * For compiling widgets, the template file will be NULL. In that case, we will
     * use the same name as the system.
     *
     * @param string $systemName System name from which the template is drawn.
     * @param string $tplFile    Name of the template file.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateJSFilePath($systemName, $tplFile=NULL)
    {
        if ($tplFile === NULL) {
            $tplFile = $systemName;
        } else {
            $tplFile = basename($tplFile, '.tpl');
        }

        $path  = GUI::getSystemWebPath($systemName);
        $path .= '/'.$tplFile.'.js';

        $fullPath = dirname(Channels::getSystemsPath()).'/'.$path;

        if (file_exists($path) === TRUE) {
            return $path;
        }

        return NULL;

    }//end getTemplateJSFilePath()


    /**
     * Returns a template's associated CSS file path.
     *
     * For compiling widgets, the template file will be NULL. In that case, we will
     * use the same name as the system.
     *
     * @param string $systemName System name from which the template is drawn.
     * @param string $tplFile    Name of the template file.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateCSSFilePath($systemName, $tplFile=NULL)
    {
        if ($tplFile === NULL) {
            $tplFile = $systemName;
        } else {
            $tplFile = basename($tplFile, '.tpl');
        }

        $path  = GUI::getSystemWebPath($systemName);
        $path .= '/'.$tplFile.'.css';

        $fullPath = dirname(Channels::getSystemsPath()).'/'.$path;

        if (file_exists($path) === TRUE) {
            return $path;
        }

        return NULL;

    }//end getTemplateCSSFilePath()


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
        // Need to remove all settings that have widget in them.
        $settings = self::_removePHPSettings($settings);

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
     * Adds CSS includes.
     *
     * @param string $systemName Name of the system.
     * @param array  $cssFiles   List of CSS files to be included for the system.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addCSSInclude($systemName, array $cssFiles=array())
    {
        if (isset(self::$_cssIncludes[$systemName]) === FALSE) {
            self::$_cssIncludes[$systemName] = array();
        }

        self::$_cssIncludes[$systemName] = array_merge(
            self::$_cssIncludes[$systemName],
            $cssFiles
        );

    }//end addCSSInclude()


    /**
     * Replaces the GUI related keywords using the given settings array.
     *
     * @param string $content    Content to search.
     * @param string $systemName Name of the system.
     * @param array  $settings   Settings for the content (widget).
     *
     * @since  4.0.0
     * @return string
     */
    private static function _replaceKeywords($content, $systemName, array $settings)
    {
        $content = str_replace('%class%', $systemName, $content);

        include_once 'Libs/Util/Util.inc';
        $id      = Util::getArrayIndex($settings, 'id', '');
        $content = str_replace('%id%', $id, $content);

        return $content;

    }//end _replaceKeywords()


    /**
     * Replaces the widgetids (#widgetid) in template strings.
     *
     * @param string $str The template content.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _replaceWidgetIds($str)
    {
        $matches = array();
        preg_match_all('/^#([A-Za-z0-9]+)/', $str, $matches);

        if (isset($matches[1]) === TRUE) {
            foreach ($matches[1] as $match) {
                $str = str_replace('#'.$match, 'GUI.getWidget(\''.$match.'\')', $str);
            }
        }

        return $str;

    }//end _replaceWidgetIds()


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
     * Build a template key from the system name and template file.
     *
     * @param string $systemName The name of the system.
     * @param string $tplFile    Filename of the template.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getTemplateKey($systemName, $tplFile=NULL)
    {
        if ($tplFile === NULL) {
            $tplFile = $systemName;
        } else {
            $tplFile = basename($tplFile, '.tpl');
        }

        return $systemName.':'.$tplFile;

    }//end _getTemplateKey()


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

        Channels::includeSystem($system);
        $retVal = call_user_func_array(
            array($system, $methodName),
            $args
        );

        return $retVal;

    }//end callStatic()


}//end class

?>