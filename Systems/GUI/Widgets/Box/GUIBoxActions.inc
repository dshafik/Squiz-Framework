<?php
/**
 * Actions for the GUIBox System.
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
 * Box Actions Class.
 *
 */
class GUIBoxActions
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
        // Check border settings.
        $borderClass = '';

        include_once 'Libs/Util/Util.inc';
        $borderLeft  = Util::getArrayIndex($settings, 'borderLeft', 'true');
        $borderRight = Util::getArrayIndex($settings, 'borderRight', 'true');

        if ($borderLeft === 'false') {
            $borderClass = ' noLeftBorder';
        }

        if ($borderRight === 'false') {
            $borderClass .= ' noRightBorder';
        }

        $content  = '<div id="%id%" class="%class%'.$borderClass.'">';
        $content .= '<div class="%class%-header">';

        if (isset($settings['title']) === TRUE) {
            // Simple title text/widget.
            if (is_array($settings['title']) === FALSE) {
                if (empty($settings['title']) === FALSE) {
                    $content .= '<div class="%class%-title">'.$settings['title'].'</div>';
                }
            } else if (isset($settings['title']['dataProvider']) === TRUE) {
                // Dynamic title.
                $content .= self::_getContentFromDataProvider($settings['title']['dataProvider']);
            }
        }

        if (isset($settings['headerContent']) === TRUE) {
            $content .= '<div class="%class%-headerContent">';;
            // Simple extra header text or widget.
            if (is_array($settings['headerContent']) === FALSE) {
                if (empty($settings['headerContent']) === FALSE) {
                    $content .= '<div class="%class%-title">'.$settings['headerContent'].'</div>';
                }
            } else if (isset($settings['headerContent']['dataProvider']) === TRUE) {
                // Dynamic header content.
                $content .= self::_getContentFromDataProvider(
                    $settings['headerContent']['dataProvider']
                );
            }

            $content .= '</div>';
        }

        $content .= '</div>';
        $content .= '<div class="%class%-content">';

        // Content setting can be a single widget or a data provider.
        if (is_array($settings['content']) === FALSE
        ) {
            // Simple content.
            $content .= $settings['content'];
        } else if (isset($settings['content']['dataProvider']) === TRUE) {
            $content .= self::_getContentFromDataProvider(
                $settings['content']['dataProvider']
            );
        }

        $content .= '</div><!--end box content-->';
        $content .= '</div>';

        return $content;

    }//end getTemplateContent()


    private static function _getContentFromDataProvider($dataProvider)
    {
        list($providerSystem, $providerMethod) = explode('::', $dataProvider);
        $content  = '<?php '."\n";
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');'."\n";
        $content .= 'echo '.$dataProvider.'();'."\n";
        $content .= ' ?>';

        return $content;

    }//end _getContentFromDataProvider()


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


}//end class

?>
