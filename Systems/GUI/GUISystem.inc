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
     * @since  4.0.0
     * @return void
     */
    public function bakeTemplates()
    {
        include_once 'Libs/FileSystem/FileSystem.inc';
        $path = BaseSystem::getSystemRootPath().'/Systems';

        // Get the tpl files.
        $templateDirs = FileSystem::findDirectories($path, 'Templates');

        if (empty($templateDirs) === TRUE) {
            return;
        }

        foreach ($templateDirs as $tplDir) {
            $tplFiles = FileSystem::listDirectory($tplDir, array('.tpl'));

            $systemName = NULL;
            $syOvenPath = NULL;
            $bakeDest   = NULL;

            Channels::includeSystem('GUI');
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

        GUI::copyWebFiles();

    }//end bakeTemplates()


}//end class

?>