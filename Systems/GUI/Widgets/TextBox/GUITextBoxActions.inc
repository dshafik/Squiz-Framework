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
 *
 */
class GUITextBoxActions
{

    /**
     * Default value of the field.
     *
     * @var   string
     */
    private static $_defaultValue = '';

    /**
     * Width of the field.
     *
     * @var   string
     */
    private static $_width = '30';


    public static function paint(array $settings)
    {
        echo '<input type="textbox" />';

    }//end paint()


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        $content = '<div class="%class% container';

        if (Util::getArrayIndex($settings, 'inline', FALSE) === TRUE) {
            $content .= ' inline';
        }

        $content .= '" id="%id%">';

        $content .= '<?php ';

        $valueCont = '';
        if (isset($settings['dataProvider']) === TRUE) {
            list($providerSystem, $providerMethod) = explode(
                '::',
                $settings['dataProvider']
            );

            $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
            $content .= '$value = '.$settings['dataProvider'].'();';

            $valueCont = '&lt;?php echo $value; ?&gt;';
        } else {
            $defaultValue = "'".self::$_defaultValue."'";
            $valueCont    = Util::getArrayIndex(
                $settings,
                'default',
                $defaultValue
            );
        }

        $content .= ' ?>';

        $width         = Util::getArrayIndex($settings, 'width', self::$_width);
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

        // The input field.
        $content .= '<input class="%class% input"'.$name.' type="text" value="'.$valueCont.'" size="'.$width.'"'.$maxlength.' />';

        $content .= '</div>';

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
