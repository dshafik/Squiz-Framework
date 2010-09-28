<?php
/**
 * Actions for the Select GUI Widget System.
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
 * Select GUI Widget Actions Class.
 */
class GUISelectActions
{

    /**
     * Id of the selected option.
     *
     * @var string
     */
    protected static $selected;

    /**
     * List of options.
     *
     * @var array
     */
    protected static $options;


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings=array())
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        $minWidth     = Util::getArrayIndex($settings, 'minWidth');
        $inlineStyles = array();

        if ($minWidth !== NULL) {
            $inlineStyles[] = 'min-width: '.$minWidth.';';
        }

        echo '<select id="'.$id.'" class="'.$class.'"';

        if (count($inlineStyles) > 0) {
            echo ' style="'.implode(' ', $inlineStyles).'"';
        }

        echo '>';

        GUISelect::paintItems($settings);

        echo '</select>';

        return $settings;

    }//end paint()


    /**
     * Paint the items of the select field.
     *
     * @param array $settings List of settings for the widget.
     *
     * @return void
     */
    public static function paintItems(array $settings)
    {
        $id       = $settings['widget']['id'];
        $class    = $settings['widget']['type'];
        $selected = Util::getArrayIndex($settings, 'selected', NULL);
        $options  = Util::getArrayIndex($settings, 'options', array());
        self::_paintItems($options, $selected);

    }//end paintItems()


    /**
     * Recursively paint option items.
     *
     * @param array   $options  List of options to paint.
     * @param mixed   $selected Selected option value.
     * @param integer $depth    Recursion depth.
     *
     * @return void
     */
    private static function _paintItems($options, $selected, $depth=0)
    {
        foreach ($options as $key => $option) {
            if (is_array($option) === TRUE) {
                self::_paintItems($option, $selected, ($depth + 1));
            } else {
                echo '<option value="'.$key.'"';

                if ($selected === $key) {
                    echo ' selected="selected"';
                }

                echo '>';

                if ($depth > 0) {
                    echo str_repeat('&nbsp;', (($depth - 1) * 4 + 1));
                    echo '--&nbsp;';
                }

                echo $option.'</option>';
            }
        }//end foreach

    }//end _paintItems()


}//end class

?>
