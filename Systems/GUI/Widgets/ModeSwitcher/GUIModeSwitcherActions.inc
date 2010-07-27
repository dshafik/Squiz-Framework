<?php
/**
 * Actions for the GUI Mode Switcher Widget System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Button
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Mode Switcher Widget Actions Class.
 *
 * @since 4.0.0
 */
class GUIModeSwitcherActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        Channels::includeSystem('GUI');

        $content = '<div id="%id%" class="%class%">';

        if (isset($settings['modes']) === FALSE || count($settings['modes']) === 0) {
            $content .= '</div>';
            return $content;
        }

        $content .= '<ul>';

        foreach ($settings['modes'] as $mode) {
            $content .= '<li id="%id%" class="%class%-modeButton" modeid="';
            $content .= $mode['id'].'" system="'.$mode['system'].'">'.$mode['label'].'</li>';
        }

        $content .= '</ul>';

        if (empty($settings['initialMode']) === FALSE
            || ($settings['loadType'] === 'static'
            && empty($settings['target']) === FALSE)
        ) {
            // Load type is static so add a post bake process to add the contents
            // of modes to specified target element.
            GUI::addPostBakeProcess(get_class(), $settings);
        }

        $content .= '</div>';

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Post bake processing.
     *
     * Adds content of modes in to specified target element.
     *
     * @param DOMNode $html     The whole HTML.
     * @param array   $settings Settings for this widget.
     *
     * @since  4.0.0
     * @return void
     */
    public static function postBake(DOMNode $html, array $settings)
    {
        $xpath = new DOMXPath($html);
        $elems = $xpath->query('//div[@id="'.$settings['target'].'"]');

        if ($elems->length !== 1) {
            return;
        }

        $elem = $elems->item(0);

        $content = GUIModeSwitcher::getAllModeContents($settings);

        $doc = new DOMDocument('1.0');
        $doc->loadXML($content);
        $newNode = $html->importNode($doc->getElementsByTagName('div')->item(0), TRUE);
        $elem->appendChild($newNode);

    }//end postBake()


    /**
     * Returns the contents of every mode in given settings.
     *
     * @param array $settings Settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getAllModeContents(array $settings)
    {
        Channels::includeSystem('GUI');
        $modes = $settings['modes'];

        $contents = '<div id="'.$settings['id'].'-modeContents">';
        foreach ($modes as $mode) {
            if ($settings['loadType'] === 'dynamic'
                && $settings['initialMode'] !== $mode['id']
            ) {
                // Load type is dynamic but we still need to load the initial mode,
                // so skip modes until initial mode is found.
                continue;
            }

            $contents .= '<div class="GUIModeSwitcher-modeContent';

            if ($settings['initialMode'] === $mode['id']) {
                $contents .= ' visible';
            }

            // End class attribute.
            $contents .= '"';

            $contents .= ' id="'.$settings['id'].'-modeContent-'.$mode['id'].'">';

            if (isset($mode['content']) === TRUE) {
                $contents .= $mode['content'];
            } else {
                ob_start();
                    GUI::printTemplate($mode['system'], ucfirst($mode['id']).'Mode.tpl');
                    $contents .= ob_get_contents();
                ob_end_clean();
            }

            $contents .= '</div>';
        }//end foreach

        $contents .= '</div>';

        return $contents;

    }//end getAllModeContents()


    /**
     * Get the requested template for a system's screen.
     *
     * @param string $system The name of the system providing the screen.
     * @param string $modeid The ID of the mode.
     *
     * @since  4.0.0
     * @return string
     * @api    read
     */
    public static function getModeContents($system, $modeid)
    {
        Channels::includeSystem('GUI');

        $contents    = '';
        $cssIncludes = array();
        $jsIncludes  = array();

        $modeFile = ucfirst($modeid).'Mode.tpl';

        ob_start();
            GUI::printTemplate($system, $modeFile);
            $contents = ob_get_contents();
        ob_end_clean();

        // Get the list of dependent files we need.
        $depFiles = GUI::getTemplateDependentFiles($system, $modeFile);

        $widgets = array();

        if (isset($depFiles['widgets']) === TRUE) {
            $widgets = $depFiles['widgets'];
        }

        $retval = array(
                   'contents' => $contents,
                   'widgets'  => $widgets,
                  );

        return $retval;

    }//end getModeContents()


}//end class

?>