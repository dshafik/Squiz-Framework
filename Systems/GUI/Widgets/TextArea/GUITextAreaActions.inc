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
 * GUI TextArea Actions Class.
 */
class GUITextAreaActions
{

    /**
     * Size attribute of the text field.
     *
     * @var string
     */
    protected static $cols = '15';

    /**
     * Max length attribute of the text field.
     *
     * @var string
     */
    protected static $rows = '3';

    /**
     * Name attribute of the text field.
     *
     * @var string
     */
    protected static $name = 'GUITextArea';

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
    protected static $inline = FALSE;

    /**
     * If TRUE then the input box will have the focus.
     *
     * It is possible that this input box may not have the focus if there are
     * elements on the screen that also have focus set to TRUE.
     *
     * @var boolean
     * @internal
     */
    protected static $focus = FALSE;


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

        $inline = Util::getArrayIndex($settings, 'inline', FALSE);
        if ($inline === TRUE || strtolower($inline) === 'true') {
            echo ' inline';
        }

        echo '" id="'.$id.'">';

        $value = Util::getArrayIndex(
            $settings,
            'value',
            ''
        );

        $colsProp = Util::getArrayIndex($settings, 'cols', self::$cols);
        $rowsProp = Util::getArrayIndex($settings, 'rows', self::$rows);
        $nameProp = Util::getArrayIndex($settings, 'name', self::$name);

        // Add in the name property if needed.
        $name = '';
        if (empty($nameProp) === FALSE) {
            $name = ' name="'.$nameProp.'"';
        }

        // The input field.
        echo '<textarea id="'.$id.'-textarea" class="'.$class.' textarea"';
        echo ' rows="'.$rowsProp.'" cols="'.$colsProp.'">';
        echo $value.'hell world';
        echo '</textarea>';
        echo '</div>';

        // If focus set to true then add JS code.
        if (Util::getArrayIndex($settings, 'focus', self::$focus) === TRUE) {
             echo '<script>dfx.addLoadEvent(function() {document.getElementById("'.$id.'-input").focus();});</script>';
        }

        return $settings;

    }//end paint()


}//end class

?>
