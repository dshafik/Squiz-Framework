<?php
/**
 * Actions for the GUI Intervention Widget.
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
 * GUI Intervention Widget Actions Class.
 */
class GUIInterventionActions
{

    /**
     * The ID of the DOM element to point to.
     *
     * @var string
     */
    protected static $targetElementid = NULL;

    /**
     * Intervention popup contents.
     *
     * @var string
     * @internal
     */
    protected static $elementContents = '';

    /**
     * Specified method will be called when the popup is closed.
     *
     * Data that was passed to JS show() method will be passed to this function.
     *
     * @var string
     */
    protected static $onclose = '';

    /**
     * If TRUE then the popup will use 'fixed' positioning instead of 'absolute'.
     *
     * @var boolean
     */
    protected static $fixedPositioning = FALSE;

    /**
     * If TRUE then the intervention popup will stay open even if the template/screen is changed.
     *
     * @var string
     */
    protected static $persistent = FALSE;


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

        $extraClass = '';
        if (Util::getArrayIndex($settings, 'fixedPositioning', self::$fixedPositioning) === TRUE) {
            $extraClass .= ' fixedPositioning';
        }

        echo '<div id="'.$id.'" class="'.$class.$extraClass.'">';
        echo '<div class="'.$class.'-arrow"></div>';

        echo '<div class="'.$class.'-content">';
        echo Util::getArrayIndex(
            $settings,
            'elementContents',
            self::$elementContents
        );
        echo '</div>';

        echo '</div>';

        return $settings;

    }//end paint()


}//end class

?>
