<?php
/**
 * Actions for the GUI Text Box System.
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
 * GUI CheckBox Actions Class.
 */
class GUICheckBoxActions
{

    /**
     * Name attribute of the text field.
     *
     * @var string
     */
    protected static $name = 'GUICheckBox';

    /**
     * Value attribute of the text field.
     *
     * @var string
     */
    protected static $value = '';

    /**
     * If TRUE then the container will have the style display:inline.
     *
     * @var boolean
     * @internal
     */
    protected static $inline = TRUE;

    /**
     * Checked field for 'checkbox' type.
     *
     * @var string
     */
    protected static $checked = FALSE;

    /**
     * Label for the checkbox type.
     *
     * @var string
     */
    protected static $label = FALSE;


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings=array())
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div class="'.$class.' container';

        $inline = Util::getArrayIndex($settings, 'inline', self::$inline);
        if ($inline === TRUE || strtolower($inline) === 'true') {
            echo ' inline';
        }

        echo '" id="'.$id.'">';

        $value = Util::getArrayIndex(
            $settings,
            'value',
            ''
        );

        // The input field.
        echo '<input id="'.$id.'-checkbox" class="'.$class.' checkbox" type="checkbox" value="'.$value.'"';

        $checked = Util::getArrayIndex($settings, 'checked', self::$checked);
        if ($checked === TRUE) {
            echo ' checked';
        }

        echo ' />';

        if (isset($settings['label']) === TRUE && empty($settings['label']) === FALSE) {
            echo '<label for="'.$id.'-checkbox" class="'.$class.' label">'.$settings['label'].'</label>';
        }

        echo '</div>';

        return $settings;

    }//end paint()


}//end class

?>
