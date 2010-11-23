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
 */
class GUIButtonActions
{

    /**
     * Value of button.
     *
     * @var string
     * @internal
     */
    protected static $value = '';

    /**
     * The onlcick action for the button.
     *
     * @var string
     * @internal
     */
    protected static $click = '';

    /**
     * The type of the button (e.g. submit).
     *
     * @var string
     * @internal
     */
    protected static $type = 'button';

    /**
     * TRUE if the button is disabled.
     *
     * @var boolean
     */
    protected static $disabled = '';

    /**
     * The colour of the button.
     *
     * @var string
     * @internal
     */
    protected static $colour = 'Default';

    /**
     * Access key for the button.
     *
     * @var string
     * @internal
     */
    protected static $accessKey = NULL;

    /**
     * Custom class name to be added to button.
     *
     * @var string
     */
    protected static $customClass = '';


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $value    = Util::getArrayIndex($settings, 'value', '');
        $colour   = Util::getArrayIndex($settings, 'colour', self::$colour);
        $disabled = Util::getArrayIndex($settings, 'disabled', '');

        // Handle click setting.
        $onClick = '';
        $click   = Util::getArrayIndex($settings, 'click', NULL);
        if ($click !== NULL) {
            $function = Util::getArrayIndex($click, 'function', NULL);
            $onClick .= ' onclick="';
            if ($function !== NULL) {
                $funcArguments = Util::getArrayIndex($click, 'arguments', array());
                if (is_array($funcArguments) === FALSE) {
                    $funcArguments = array();
                }

                $quoteArgs = Util::getArrayIndex($click, 'quoteArgs', TRUE);

                if ($quoteArgs === TRUE) {
                    $args = '\''.implode('\', \'', $funcArguments).'\'';
                } else {
                    $args = implode(', ', $funcArguments);
                }

                $targetWidget = Util::getArrayIndex($click, 'targetWidget', NULL);
                if ($targetWidget !== NULL) {
                    $onClick .= 'GUI.getWidget(\''.$settings['click']['targetWidget'].'\').';
                }

                $onClick .= $function.'('.$args.')';
            } else {
                $onClick .= $click;
            }//end if

            $onClick .= ';"';
        }//end if

        echo '<button id="'.$settings['widget']['id'].'"';

        echo ' type="'.Util::getArrayIndex($settings, 'type', self::$type).'"';

        if ($disabled === 'disabled') {
            echo ' disabled="disabled"';
        }

        $accessKey = Util::getArrayIndex($settings, 'accesskey', self::$accessKey);
        if ($accessKey !== NULL) {
            echo ' accesskey="'.$accessKey.'"';
        }

        echo ' class="'.$settings['widget']['type'].' button'.$colour;
        $customClass = Util::getArrayIndex($settings, 'customClass', self::$customClass);
        if (empty($customClass) === FALSE) {
            echo ' '.$customClass;
        }

        echo '"'.$onClick.'>';

        echo '<span class="'.$settings['widget']['type'].'Label">'.$value.'</span>';
        echo '</button>';

        return $settings;

    }//end paint()


}//end class

?>