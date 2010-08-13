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
        $selected = Util::getArrayIndex($settings, 'selected');
        $options  = Util::getArrayIndex($settings, 'options', array());
        $depths   = array();

        while (count($options) > 0) {
            reset($options);
            $optionKey = key($options);
            $option    = array_shift($options);

            if (count($depths) === 0) {
                $depth = 0;
            } else {
                $depthEl = array_pop($depths);
                $depth   = $depthEl[0];

                if ($depthEl[1] > 1) {
                    $depthEl[1]--;
                    $depths[] = $depthEl;
                }
            }//end if

            // If this is an array, store the depths of the next elements so we
            // know when to switch back to the next lower level.
            // Then add the sub elements to the start of the options so we know
            // to work with them first. Ignore the key provided.
            if (is_array($option) === TRUE) {
                $newLength = (count($depths) + count($option));
                $depths[]  = array(
                              ($depth + 1),
                              count($option),
                             );
                $options   = ($option + $options);
            } else {
                echo '<option value="'.$optionKey.'"';

                if ($selected === $optionKey) {
                    echo ' selected="selected"';
                }

                echo '>';

                if ($depth > 0) {
                    echo str_repeat('&nbsp;', (($depth - 1) * 4 + 1));
                    echo '--&nbsp;';
                }

                echo $option.'</option>';
            }//end if
        }//end while

    }//end paintItems()


}//end class

?>
