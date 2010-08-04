<?php
/**
 * Actions for the GUI Toggle Button System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage ToggleButton
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Toggle Button Actions Class.
 *
 * @since 4.0.0
 */
class GUIToggleButtonActions
{

    private static $defaultValue = FALSE;


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        $content = '<div class="%class%" id="%id%">';

        $content .= '<?php ';
        $content .= 'Channels::includeSystem(\'GUIToggleButton\');';
        if (isset($settings['dataProvider']) === TRUE) {
            list($providerSystem, $providerMethod) = explode('::', $settings['dataProvider']);
            $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
            $content .= '$selected = '.$settings['dataProvider'].'();';
        } else {
            $strVal = 'FALSE';
            if (Util::getArrayIndex($settings, 'default', self::$defaultValue) === TRUE) {
                $strVal = 'TRUE';
            }

            $content .= '$selected = '.$strVal.';';
        }

        $content .= '$options = '.var_export(self::_getToggleOptions($settings), TRUE).';';
        $content .= 'echo GUIToggleButton::paint(\'%id%\', \'%class%\', $options, (bool) $selected);';
        $content .= ' ?>';
        $content .= '</div>';

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Paints the toggle button based on the data provided.
     *
     * @param string  $id       The id of the widget.
     * @param string  $class    The class of the widget.
     * @param array   $options  The options to paint this widget.
     * @param boolean $selected The selected value of this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function paint($id, $class, $options, $selected)
    {
        // Defaults etc.
        $optionY     = Util::getArrayIndex($options[0], 'name', 'Yes');
        $optionN     = Util::getArrayIndex($options[1], 'name', 'No');
        $classY      = $class.' option yes'.((strlen($optionY) === 2) ? ' smallYes' : '').(($selected === TRUE) ? ' selected' : '');
        $classN      = $class.' option no'.((strlen($optionN) === 2) ? ' smallNo' : '').(($selected === FALSE) ? ' selected' : '');
        $classToggle = $class.' toggle '.(($selected === TRUE) ? 'toggleY' : 'toggleN');

        // Toggle Icon.
        $content = '<div class="'.$classToggle.'">&nbsp;</div>';

        // Left container.
        $content .= '<div class="'.$class.' left">&nbsp;</div>';

        // Options.
        $content .= '<div class="'.$class.' middle">';
        $content .= '<div class="'.$classY.'">'._($optionY).'</div>';
        $content .= '<div class="'.$classN.'">'._($optionN).'</div>';
        $content .= '</div>';

        // Right container.
        $content .= '<div class="'.$class.' right">&nbsp;</div>';

        return $content;

    }//end paint()


    /**
     * Gets the options for this toggle widget.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getToggleOptions(array $settings=array())
    {
        // Set a default options value, in case someone is lazy.
        $default = array(
                    array('name' => 'Yes'),
                    array('name' => 'No'),
                   );

        $options = Util::getArrayIndex($settings, 'options', $default);

        return $options;

    }//end _getToggleOptions()


}//end class

?>
