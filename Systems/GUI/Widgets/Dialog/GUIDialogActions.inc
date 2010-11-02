<?php
/**
 * Actions for the GUI Dialog Widget.
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
 * GUI Dialog Widget Actions Class.
 */
class GUIDialogActions
{

    /**
     * Main title of the dialog.
     *
     * @var string
     * @internal
     */
    protected static $title;

    /**
     * Sub title of the dialog.
     *
     * @var string
     * @internal
     */
    protected static $subTitle = '';

    /**
     * Content of the dialog.
     *
     * @var string
     * @internal
     */
    protected static $content = '';

    /**
     * Content for the bottom of the dialog.
     *
     * @var string
     * @internal
     */
    protected static $bottomContent = '';

    /**
     * If TRUE then the dialog will cover almost the whole screen.
     *
     * @var boolean
     */
    protected static $fullScreen = FALSE;

    /**
     * Minimum width of the dialog.
     *
     * @var integer
     */
    protected static $minWidth = 0;

    /**
     * Minimum height of the dialog.
     *
     * @var integer
     */
    protected static $minHeight = 0;

    /**
     * Maximum width of the dialog.
     *
     * @var integer
     */
    protected static $maxWidth = NULL;

    /**
     * Maximum height of the dialog.
     *
     * @var integer
     */
    protected static $maxHeight = NULL;

    /**
     * Initial width of the dialog.
     *
     * @var integer
     */
    protected static $width = NULL;

    /**
     * Initial height of the dialog.
     *
     * @var integer
     */
    protected static $height = NULL;

    /**
     * If TRUE then the dialog will be resizable.
     *
     * @var boolean
     */
    protected static $resizable = TRUE;

    /**
     * If TRUE then the dialog will be draggable.
     *
     * @var boolean
     */
    protected static $draggable = TRUE;

    /**
     * The positioning type.
     *
     * Valid values: fixed, absolute.
     * Default is absolute.
     *
     * @var string
     */
    protected static $position = 'absolute';

    /**
     * The left position of the dialog.
     *
     * E.g. 50%, 10%, 10px.
     *
     * @var string
     */
    protected static $posLeft = '50%';

    /**
     * The top position of the dialog.
     *
     * E.g. 50%, 10%, 10px.
     *
     * @var string
     */
    protected static $posTop = '50%';

    /**
     * The left position of the dialog.
     *
     * Note that if posLeft is set then this value will be ignored.
     *
     * @var string
     */
    protected static $posRight = NULL;

    /**
     * The bottom position of the dialog.
     *
     * Note that if posTop is set then this value will be ignored.
     *
     * @var string
     */
    protected static $posBottom = NULL;


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

        // We need this info in JS so make sure they are in the settings array.
        $settings['resizable'] = Util::getArrayIndex($settings, 'resizable', TRUE);
        $settings['draggable'] = Util::getArrayIndex($settings, 'draggable', TRUE);

        $positionClass = ' '.$class.'-position-'.Util::getArrayIndex($settings, 'position', self::$position);

        // Top section.
        echo '<div class="'.$class.$positionClass.'" id="'.$id.'">';
        echo '<div class="'.$class.'-top">';
        echo '<div class="'.$class.'-title">'.Util::getArrayIndex($settings, 'title', '').'</div>';
        echo '<div class="'.$class.'-subTitle">'.Util::getArrayIndex($settings, 'subTitle', '').'</div>';
        echo '<div class="'.$class.'-closeIcon" onclick="GUI.getWidget(\''.$id.'\').close(this);"></div>';
        echo '</div>';

        // Middle section.
        echo '<div class="'.$class.'-middle">'.Util::getArrayIndex($settings, 'content', '').'</div>';

        // Bottom section.
        echo '<div class="'.$class.'-bottom">';
        echo Util::getArrayIndex($settings, 'bottomContent', '');
        echo '</div>';

        // Make sure pos settings are correct.
        $posLeft = Util::getArrayIndex($settings, 'posLeft', NULL);
        $posTop  = Util::getArrayIndex($settings, 'posLeft', NULL);

        if ($posLeft !== NULL) {
            $settings['posRight'] = NULL;
        }

        if ($posTop !== NULL) {
            $settings['posBottom'] = NULL;
        }

        return $settings;

    }//end paint()


}//end class

?>
