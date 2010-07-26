<?php
/**
 * GUI System Class.
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
 * GUI System Class.
 *
 * @since 4.0.0
 */
class GUISystem extends BaseSystem
{


    /**
     * Install this system.
     *
     * @since  4.0.0
     * @return void
     */
    public function install()
    {
        parent::install();

        $this->bakeTemplates();

    }//end install()


    /**
     * Bakes all the templates in the Systems dir.
     *
     * @param boolean $minified If TRUE then JS and CSS will be minified.
     *
     * @since  4.0.0
     * @return void
     */
    public function bakeTemplates($minified=FALSE)
    {
        Channels::includeSystem('GUI');
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

        foreach ($templateDirs as $tplDir) {
            $tplFiles = FileSystem::listDirectory($tplDir, array('.tpl'));

            $systemName = NULL;
            $syOvenPath = NULL;
            $bakeDest   = NULL;

            foreach ($tplFiles as $file) {
                // Get the system name for the template.
                if ($systemName === NULL) {
                    $systemName = GUI::getSystemNameFromTemplate($file);
                    Channels::includeSystem($systemName);
                }

                $tplDoc = GUI::getTemplateContent($systemName, array(), $file);
                if ($tplDoc !== NULL) {
                    GUI::bakeTemplate($systemName, basename($file), $tplDoc);
                }
            }
        }//end foreach

    }//end bakeTemplates()


}//end class

?>