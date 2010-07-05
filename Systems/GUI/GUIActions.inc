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
     * The JS content.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_js = array();

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

        include_once $dest;

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
    public static function getTemplateContent($systemName, array $settings=array(), $tplFile=NULL)
    {
        Channels::includeSystem('GUI');

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

        // Add any JS and CSS files that this widget has.
        $jsPath = GUI::getWidgetJSFilePath($systemName);
        if ($jsPath !== NULL) {
            GUI::addJSInclude($systemName, array($jsPath));
            GUI::addJSInitCode($systemName, $settings['id'], $settings);
        }

        // Get the CSS path.
        $cssPath = GUI::getWidgetCSSFilePath($systemName);
        if ($cssPath !== NULL) {
            GUI::addCSSInclude($systemName, array($cssPath));
        }

        $content = self::_replaceKeywords($content, $systemName, $settings);

        // Add it to dom object.
        $doc = new DOMDocument('1.0');
        $doc->loadXML($content);

        // Convert all widget tags.
        GUI::convertWidgetTags($doc->documentElement);

        // Call prepareBake.
        call_user_func(array($systemName, 'prepareBake'), $doc->documentElement, $settings);

        return $doc->documentElement;

    }//end getTemplateContent()


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
            GUI::getWidgetHTML($widget);
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

        // Replace widget tag with the html.
        $node = $widget->ownerDocument->importNode($content, TRUE);
        $widget->parentNode->insertBefore($node, $widget);
        $widget->parentNode->removeChild($widget);

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

            if ($node->childNodes->length !== 0) {
                include_once 'Libs/XML/XML.inc';
                $xml   = simplexml_import_dom($node);
                $value = array();
                foreach ($xml->value as $val) {
                    $value[] = (array) $val;
                }
            } else {
                $value  = $node->getAttribute('value');
            }

            // TODO: Make this work with array setting values.
            if (strpos($value, '#') === 0) {
                $value = self::_replaceWidgetIds($value);
            }

            $settings[$setting] = $value;
        }//end foreach

        return $settings;

    }//end _getSettingsForWidget()


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

        // Before converting to HTML we need to add the JS and CSS files.
        // But first lets check if we can add them to HEAD tag.
        $head = $html->getElementsByTagName('head');
        if ($head->length !== 0) {
            $head = $head->item(0);

            // Add the GUI systems JS files. These are required.
            $path = GUI::getSystemWebPath('GUI');
            GUI::addJSInclude('GUI', array($path.'/GUI.js', $path.'/DfxJSLib/mysource.js'));

            // Add required JS files.
            $jsIncludes = self::_getJSIncludes();
            foreach ($jsIncludes as $jsSysName => $sysJsIncs) {
                $sysJsIncs = array_unique($sysJsIncs);
                foreach ($sysJsIncs as $jsInclude) {
                    GUI::addJS($jsSysName, $jsInclude, $head);
                }
            }

            // Create the script tag that contains JS init codes.
            $jsElem = $head->ownerDocument->createElement('script');
            $js     = implode("\n", self::$_js);

            // All these needs to be wrapped with onLoad.
            $js = "dfx.addLoadEvent(function() {\n\t".$js."\n});";

            $jsElem->nodeValue = $js;
            $head->appendChild($jsElem);

            // Now get the CSS.
            $cssIncludes = self::_getCSSIncludes();
            foreach ($cssIncludes as $cssSysName => $sysCssIncs) {
                $sysCssIncs = array_unique($sysCssIncs);
                foreach ($sysCssIncs as $cssInclude) {
                    GUI::addCSS($cssSysName, $cssInclude, $head);
                }
            }

        }//end if

        // Get the HTML string.
        include_once 'Libs/XML/XML.inc';
        $htmlCont = $html->ownerDocument->saveXML();
        $htmlCont = XML::xmlToXHTML($htmlCont);

        // Do some cleanup.
        $htmlCont = str_replace(self::$_docType, '', $htmlCont);
        $htmlCont = trim(str_replace('<?xml version="1.0"?>', '', $htmlCont));

        $htmlCont = str_replace('&lt;?php', '<'.'?php', $htmlCont);
        $htmlCont = str_replace('?&gt;', '?'.'>', $htmlCont);

        // Write to data file.
        $syDataPath = BaseSystem::getDataDir($systemName).'/Templates';
        if (file_exists($syDataPath) === FALSE) {
            FileSystem::mkdir($syDataPath, 0755, TRUE);
        }

        $dest = $syDataPath.'/'.$tplName.'.php';
        file_put_contents($dest, $htmlCont);

        return $htmlCont;

    }//end bakeTemplate()


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
     * Returns the widget's JS file path.
     *
     * @param string $widgetType Type of the widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getWidgetJSFilePath($widgetType)
    {
        $path  = GUI::getSystemWebPath($widgetType);
        $path .= '/'.$widgetType.'.js';

        $fullPath = dirname(Channels::getSystemsPath()).'/'.$path;

        if (file_exists($path) === TRUE) {
            return $path;
        }

        return NULL;

    }//end getWidgetJSFilePath()


    /**
     * Returns the widget's CSS file path.
     *
     * @param string $widgetType Type of the widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getWidgetCSSFilePath($widgetType)
    {
        $path  = GUI::getSystemWebPath($widgetType);
        $path .= '/'.$widgetType.'.css';

        $fullPath = dirname(Channels::getSystemsPath()).'/'.$path;

        if (file_exists($path) === TRUE) {
            return $path;
        }

        return NULL;

    }//end getWidgetCSSFilePath()


    /**
     * Adds JS initialisation code.
     *
     * This is used to initialiase the JS Widget objects. These objects can be
     * retrieved by calling GUI.getWidget(id).
     *
     * @param string $systemName Name of the system.
     * @param string $id         Unique ID of the widget object.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addJSInitCode($systemName, $id, array $settings=array())
    {
        $code = 'GUI.addWidget("'.$id.'", new '.$systemName.'("'.$id.'", '.json_encode($settings).'));';

        self::$_js[$id] = $code;

    }//end addJSInitCode()


    /**
     * Adds CSS includes.
     *
     * @param string $systemName Name of the system.
     * @param array  $jsFiles    List of CSS files to be included for the system.
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
     * @since  4.0.0
     * @return void
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
            header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found");
            exit;
        }

        // Must be in the Systems dir.
        if (substr($path, 0, strlen($systemsPath)) !== $systemsPath
            || strpos($url, '/Web/') === FALSE
        ) {
            header($_SERVER["SERVER_PROTOCOL"]." 404 Not Found");
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