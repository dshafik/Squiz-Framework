<?php
/**
 * Actions for the Button System.
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
 * Button Actions Class.
 *
 */
class GUIButtonActions
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
        $value = $settings['value'];
        $color = $settings['color'];

        if (empty($color) === TRUE) {
            $color = 'Default';
        }

        // Handle click setting.
        $onClick = '';
        if (isset($settings['click']) === TRUE) {
            if (isset($settings['click']['function']) === TRUE) {
                $onClick .= ' onclick="';

                $args = '';
                if (isset($settings['click']['arguments']) === TRUE
                    && empty($settings['click']['arguments']) === FALSE
                ) {
                    $args .= '\''.implode('\', \'', $settings['click']['arguments']).'\'';
                }

                if (isset($settings['click']['targetWidget']) === TRUE) {
                    $onClick .= 'GUI.getWidget(\''.$settings['click']['targetWidget'].'\').';
                }

                $onClick .= $settings['click']['function'].'('.$args.');"';
            }
        }

        $content  = '<button class="%class% button'.$color.'" type="button"'.$onClick.'>';
        $content .= '<span>'.$value.'</span>';
        $content .= '</button>';

        return $content;

    }//end getTemplateContent()


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