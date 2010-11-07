<?php
/**
 * Actions for the GUI TabPane System.
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
 * GUI TabPane Actions Class.
 */
class GUITabPaneActions
{

    /**
     * Tabs for the pane.
     *
     * @var array
     */
    protected static $tabs = array();

    /**
     * Static content shown for all tabs.
     *
     * @var string
     */
    protected static $staticContent = '';

    /**
     * The initial tabid.
     *
     * Default is the first tab.
     *
     * @var string
     */
    protected static $initialItem = '';


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];
        echo '<div id="'.$id.'" class="'.$class.'">';

        $tabButtons  = '';
        $tabContents = '';
        $tabs        = Util::getArrayIndex($settings, 'tabs', self::$tabs);

        $initialTab = '';
        if (empty($tabs) === FALSE) {
            $initialTab = Util::getArrayIndex($settings, 'initialItem', $tabs[0]['id']);
        }

        $firstTabClass = ' first';
        foreach ($tabs as $tab) {
            $clickAct = 'GUI.getWidget(\''.$id.'\').switchTo(\''.$tab['id'].'\');';
            $tabTitle = sprintf(_('Switch to %s tab'), $tab['label']);

            if ($tab['id'] === $initialTab) {
                $initialTabClass = ' active';
            }

            $tabButtons .= '<li class="'.$class.'-tab tabButton-'.$tab['id'].$firstTabClass.$initialTabClass.'">';
            $tabButtons .= '<a href="javascript:'.$clickAct.'" title="'.$tabTitle.'" onclick="'.$clickAct.' return false;">'.$tab['label'].'</a>';
            $tabButtons .= '</li>';

            $tabContents .= '<div class="'.$class.'-tabContent tabContent-'.$tab['id'].$initialTabClass.'">';
            $tabContents .= $tab['itemContent'].'</div>';

            $firstTabClass   = '';
            $initialTabClass = '';
        }

        echo '<ul class="'.$class.'-top">';
        echo $tabButtons;
        echo '</ul>';

        // Static content is displayed on top of all tabs.
        $staticContent = Util::getArrayIndex($settings, 'staticContent', self::$staticContent);
        if (empty($staticContent) === FALSE) {
            echo '<div class="'.$class.'-static">';
            echo $staticContent;
            echo '</div>';
        }

        echo '<div class="'.$class.'-contents">';
        echo $tabContents;
        echo '</div>';

        echo '</div>';

        return $settings;

    }//end paint()


}//end class

?>
