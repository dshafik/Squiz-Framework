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
 * GUI TextBox Actions Class.
 */
class GUITextBoxActions
{

    /**
     * Size attribute of the text field.
     *
     * @var string
     */
    protected static $size = '30';

    /**
     * Max length attribute of the text field.
     *
     * @var string
     */
    protected static $maxlength = '30';

    /**
     * Name attribute of the text field.
     *
     * @var string
     */
    protected static $name = 'GUITextBox';

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
     * The type of the text box (e.g. text, password).
     *
     * @var string
     */
    protected static $type = 'text';

    /**
     * Disabled field for 'checkbox' type.
     *
     * @var string
     */
    protected static $disabled = FALSE;

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
     * If specified then this value will be shown if no value is set.
     *
     * If the textbox is left empty then this will be set as the value of the field.
     *
     * @var string
     */
    protected static $defaultValue = '';

    /**
     * Text shown as a message in the field.
     *
     * Not available for 'password' type.
     *
     * @var string
     */
    protected static $hint = '';

    /**
     * Custom class name to be added to input field.
     *
     * @var string
     */
    protected static $customClass = '';

    /**
     * Use hint as default value.
     *
     * @var boolean
     */
    protected static $hintAsDefault = FALSE;

    /**
     * If FALSE the Save button will not be enabled when widget value is changed.
     *
     * @var boolean
     */
    protected static $enablesSaveButton = TRUE;

    /**
     * If FALSE the GUI.setModified method will not be called when value is changed.
     *
     * @var boolean
     */
    protected static $requiresSave = TRUE;


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
            self::$value
        );

        $hint      = Util::getArrayIndex($settings, 'hint', self::$hint);
        $usingHint = FALSE;
        $value     = trim($value);
        if ($value === self::$defaultValue) {
            $value = Util::getArrayIndex(
                $settings,
                'defaultValue',
                self::$defaultValue
            );

            if ($value === self::$defaultValue && empty($hint) === FALSE) {
                $value     = $hint;
                $usingHint = TRUE;
            }
        }

        $size          = Util::getArrayIndex($settings, 'size', self::$size);
        $maxLengthProp = Util::getArrayIndex($settings, 'maxlength', 0);
        $nameProp      = Util::getArrayIndex($settings, 'name', '');

        // Add in the name property if needed.
        $name = '';
        if (empty($nameProp) === FALSE) {
            $name = ' name="'.$nameProp.'"';
        }

        // Add in the maxlength property if needed.
        $maxlength = '';
        if ($maxLengthProp !== 0) {
            $maxlength = ' maxlength="'.$maxLengthProp.'"';
        }

        $type = Util::getArrayIndex($settings, 'type', self::$type);

        if ($usingHint === TRUE) {
            $class .= ' usingHint';
        }

        // The input field.
        echo '<input id="'.$id.'-input" class="'.$class.' input';
        $customClass = Util::getArrayIndex($settings, 'customClass', self::$customClass);
        if (empty($customClass) === FALSE) {
            echo ' '.$customClass;
        }

        echo '" '.$name.' type="'.$type.'" value="'.$value.'" size="'.$size.'"'.$maxlength;

        $disabled = Util::getArrayIndex($settings, 'disabled', self::$disabled);
        if ($disabled === TRUE) {
            echo ' disabled="disabled"';
        }

        echo ' />';
        echo '</div>';

        // If focus set to true then add JS code.
        if (Util::getArrayIndex($settings, 'focus', self::$focus) === TRUE) {
             echo '<script>dfx.addLoadEvent(function() {document.getElementById("'.$id.'-input").focus();});</script>';
        }

        // Set default settings for requiresSave and enablesSaveButton.
        $settings['requiresSave']      = Util::getArrayIndex($settings, 'requiresSave', self::$requiresSave);
        $settings['enablesSaveButton'] = Util::getArrayIndex($settings, 'enablesSaveButton', self::$enablesSaveButton);

        return $settings;

    }//end paint()


}//end class

?>
