<?php
/**
 * Actions for the GUI Mode Switcher Widget System.
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
 * GUI Mode Switcher Widget Actions Class.
 *
 */
class GUIContentSwitcherActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        Channels::includeSystem('GUI');

        if (isset($settings['initialItem']) === FALSE) {
            // Use the first item.
            if (empty($settings['items']) === FALSE) {
                $settings['initialItem'] = $settings['items'][0]['id'];
            }
        }

        if ($settings['showButtons'] !== 'false') {
            $content = GUI::callStatic(
                $settings['type'],
                'getButtonsContent',
                array($settings)
            );
        } else {
            $content .= '<span />';
        }

        if (empty($settings['initialItem']) === FALSE
            || ($settings['loadType'] === 'static'
            && empty($settings['target']) === FALSE)
        ) {
            // Load type is static so add a post bake process to add the contents
            // of modes to specified target element.
            GUI::addPostBakeProcess(get_class(), $settings);
        }

        return $content;

    }//end getTemplateContent()


    /**
     * Returns the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
     * @return string
     */
    public static function getButtonsContent(array $settings=array())
    {
        $content = '<div id="%id%" class="%class%">';

        if (isset($settings['items']) === FALSE || count($settings['items']) === 0) {
            $content .= '</div>';
            return $content;
        }

        $content .= '<ul>';

        $total = count($settings['items']);
        $count = 1;
        $extraClass = ' positionFirst';
        foreach ($settings['items'] as $mode) {
            $content .= '<li id="%id%-'.$mode['id'].'" class="%class%-modeButton'.$extraClass.'">';
            $content .= '<a href="#" modeid="'.$mode['id'].'" system="'.$mode['system'].'">'.$mode['label'].'</a></li>';

            if (($total - 1) === $count) {
                $extraClass = ' positionLast';
            } else {
                $extraClass = '';
            }

            $count++;
        }

        $content .= '</ul>';
        $content .= '</div>';

        return $content;

    }//end getButtonsContent()


    /**
     * Returns the body content of the widget.
     *
     * @param array $settings The settings of the widget.
     *
     * @return string
     */
    public static function getBodyContent(array $settings=array())
    {
        Channels::includeSystem('GUI');
        $items = $settings['items'];

        if (empty($items) === TRUE) {
            throw new Exception('No items were specified for content switcher');
        }

        $contents = '<div id="'.$settings['id'].'-itemContents">';
        foreach ($items as $item) {
            $contents .= GUI::callStatic(
                $settings['type'],
                'getItemContent',
                array($settings, $item)
            );
        }//end foreach

        $contents .= '</div>';

        return $contents;

    }//end getBodyContent()


    /**
     * Returns the single item body content.
     *
     * @param array $settings The settings of the widget.
     * @param array $item     Information about the item.
     *
     * @return string
     */
    public static function getItemContent(array $settings, array $item)
    {
        if ($settings['loadType'] === 'dynamic'
            && $settings['initialItem'] !== $item['id']
        ) {
            // Load type is dynamic but we still need to load the initial item,
            // so skip items until initial item is found.
            return;
        }

        $contents .= '<div class="GUIContentSwitcher-itemContent';

        if ($settings['initialItem'] === $item['id']) {
            $contents .= ' visible';
        }

        // End class attribute.
        $contents .= '"';

        $contents .= ' id="'.$settings['id'].'-itemContent-'.$item['id'].'">';

        if (isset($item['content']) === TRUE) {
            $contents .= $item['content'];
        } else {
            ob_start();
                GUI::printTemplate($item['system'], $item['id'].'.tpl');
                $contents .= ob_get_contents();
            ob_end_clean();
        }

        $contents .= '</div>';

        return $contents;

    }//end getItemContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
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

        $content = GUI::callStatic(
            $settings['type'],
            'getBodyContent',
            array($settings)
        );

        $doc = new DOMDocument('1.0');
        $doc->loadXML($content);
        $newNode = $html->importNode($doc->getElementsByTagName('div')->item(0), TRUE);
        $elem->appendChild($newNode);

    }//end postBake()


    /**
     * Get the requested template for a system's screen.
     *
     * @param string $system The name of the system providing the screen.
     * @param string $modeid The ID of the mode.
     *
     * @return string
     * @api    read
     */
    public static function getDynamicItemContent($system, $modeid)
    {
        Channels::includeSystem('GUI');

        $contents    = '';
        $cssIncludes = array();
        $jsIncludes  = array();

        $modeFile = ucfirst($modeid).'.tpl';

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