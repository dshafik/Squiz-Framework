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
 */
class GUIContentSwitcherActions
{

    /**
     * List of items for the content switcher.
     *
     * @var array
     * @internal
     */
    protected static $items = array();

    /**
     * The initial item.
     *
     * @var string
     */
    protected static $initialItem = NULL;

    /**
     * If FALSE then buttons for the switcher will not be shown.
     *
     * @var boolean
     */
    protected static $showButtons = TRUE;

    /**
     * The load type of the switcher (dynamic | static).
     *
     * @var string
     */
    protected static $loadType = 'dynamic';

    /**
     * If TRUE then all switcher buttons will be displayed.
     *
     * @var boolean
     */
    protected static $showAll = TRUE;

    /**
     * The target element id where the content will be shown.
     *
     * @var string
     */
    protected static $target;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        // May need to override initial item.
        include_once 'Libs/Web/Web.inc';
        include_once 'Libs/Security/Security.inc';
        $url = Web::getCurrentUrl();
        $queryString = Web::getQueryStringParams($url);

        if (isset($queryString['sq_'.$id]) === TRUE) {
            $settings['initialItem'] = $queryString['sq_'.$id];
        }

        $screens = array();
        if (isset($settings['items']) === TRUE) {
            $screens = $settings['items'];
        }

        Channels::includeSystem('GUI');
        $initialItem = Util::getArrayIndex($settings, 'initialItem', NULL);
        if (empty($initialItem) === TRUE) {
            // Use the first item.
            if (empty($settings['items']) === FALSE) {
                $current = reset($settings['items']);
                $settings['initialItem'] = $current['id'];
            }
        } else if (empty($screens) === FALSE) {
            // Check if initial item is in the items list, if not use the first
            // item as the initial item.
            $found = FALSE;
            foreach ($screens as $screen) {
                if ($settings['initialItem'] === $screen['id']) {
                    $found = TRUE;
                    break;
                }
            }

            if ($found === FALSE) {
                $current = reset($screens);
                $settings['initialItem'] = $current['id'];
            }
        }

        $showButtons = Util::getArrayIndex($settings, 'showButtons', 'true');
        if ($showButtons !== 'false') {
            GUI::callStatic(
                $settings['widget']['type'],
                'printButtonsContent',
                array($settings)
            );
        } else {
            echo '<span />';
        }

        $settings['_items'] = array();
        foreach ($screens as $item) {
            unset($item['itemContent']);
            $settings['_items'][] = $item;
        }

        return $settings;

    }//end paint()


    /**
     * Prints the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
     * @return mixed
     */
    public static function printButtonsContent(array $settings)
    {
        $id        = $settings['widget']['id'];
        $className = $settings['widget']['type'];

        $initItem = Util::getArrayIndex($settings, 'initialItem', NULL);

        echo '<div id="'.$id.'" class="'.$className.'">';

        if (isset($settings['items']) === FALSE || count($settings['items']) === 0) {
            echo '</div>';
            return;
        }

        echo '<ul class="SwitcherList">';

        $total      = count($settings['items']);
        $count      = 1;
        $extraClass = ' positionFirst';

        foreach ($settings['items'] as $item) {
            $itemSystem = Util::getArrayIndex($item, 'system', '');

            if ($initItem === NULL) {
                $initItem = $item['id'];
            }

            $selected = '';
            if ($item['id'] === $initItem) {
                $selected = ' selected';
            }

            echo '<li id="'.$id.'-'.$item['id'].'" class="'.$className.'-modeButton'.$extraClass.$selected.'">';
            echo '<a id="'.$id.'-link-'.$item['id'].'" href="#" modeid="'.$item['id'].'" system="'.$itemSystem.'">'.$item['label'].'</a></li>';

            if (($total - 1) === $count) {
                $extraClass = ' positionLast';
            } else {
                $extraClass = '';
            }

            $count++;
        }//end foreach

        echo '</ul>';
        echo '</div>';

    }//end printButtonsContent()


    /**
     * Returns the body content of the widget.
     *
     * @param array $settings The settings of the widget.
     *
     * @return string
     * @throws ChannelException If no items are specified for the content switcher.
     */
    public static function getBodyContent(array $settings=array())
    {
        $content = '';
        Channels::includeSystem('GUI');
        $items = Util::getArrayIndex($settings, 'items', array());

        $content .= '<div id="'.$settings['widget']['id'].'-itemContents">';
        foreach ($items as $item) {
            $content .= GUI::callStatic(
                $settings['widget']['type'],
                'getItemContent',
                array(
                 $settings,
                 $item,
                )
            );
        }//end foreach

        $content .= '</div>';

        return $content;

    }//end getBodyContent()


    /**
     * Returns the single item body content.
     *
     * @param array $settings The settings of the widget.
     * @param mixed $item     Information about the item.
     *
     * @return string
     */
    public static function getItemContent(array $settings, $item)
    {
        if ($settings['loadType'] === 'dynamic'
            && $settings['initialItem'] !== $item['id']
        ) {
            // Load type is dynamic but we still need to load the initial item,
            // so skip items until initial item is found.
            return '';
        }

        $content = '<div class="'.$settings['widget']['type'].'-itemContent';

        if (Util::getArrayIndex($settings, 'initialItem') === $item['id']) {
            $content .= ' visible';
        }

        // End class attribute.
        $content .= '"';

        $content .= ' id="'.$settings['widget']['id'].'-itemContent-'.$item['id'].'">';

        if (isset($item['itemContent']) === TRUE) {
            $content .= $item['itemContent'];
        } else {
            Channels::includeSystem('GUI');

            // If we are recaching, don't print the template.
            if (GUI::isReCachingTemplate() === FALSE) {
                ob_start();
                    GUI::printTemplate($item['system'], $item['id'].'.tpl');
                    $content .= ob_get_contents();
                ob_end_clean();
            }
        }

        $content .= '</div>';

        return $content;

    }//end getItemContent()


    /**
     * Get the requested template for a system's screen.
     *
     * Permissions are handled by the templates themselves, so this method
     * is considered public in the API.
     *
     * @param string $system The name of the system providing the screen.
     * @param string $modeid The ID of the mode.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
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

        return $contents;

    }//end getDynamicItemContent()


    /**
     * Install this widget.
     *
     * Adds templates used in switchers to Help systems template lineage list.
     *
     * @param array $settings List of settings for this widget.
     *
     * @return void
     */
    public static function install(array $settings)
    {
        if (Channels::systemExists('Help') === FALSE) {
            return;
        }

        if (isset($settings['items']) === TRUE) {
            Channels::includeSystem('Help');
            foreach ($settings['items'] as $item) {
                if (empty($item['system']) === TRUE) {
                    continue;
                }

                Help::addTemplateToLineage(
                    $item['system'].':'.$item['id'],
                    $settings['template']['system'].':'.$settings['template']['name'],
                    $settings['widget']['id'],
                    $item['label']
                );
            }
        }

    }//end install()


}//end class

?>