<?php
/**
 * Actions for the GUI Text Box System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Spinner
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI TextBox Actions Class.
 *
 * @since 4.0.0
 */
class GUITextBoxActions
{

    /**
     * Default value of the field.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_defaultValue = '';

    /**
     * Width of the field.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_width = '30';


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
        $content  = '<div class="%class% container" id="%id%">';
        $content .= '<?php ';
        if (isset($settings['dataProvider']) === TRUE) {
            list($providerSystem, $providerMethod) = explode(
                '::',
                $settings['dataProvider']
            );

            $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
            $content .= '$value = '.$settings['dataProvider'].'();';
        } else {
            $defaultValue = "'".self::$_defaultValue."'";
            $content     .= '$value = '.Util::getArrayIndex(
                $settings,
                'default',
                $defaultValue
            ).';';
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
        $content .= '<input class="%class% input"'.$name.' type="text" value="&lt;?php echo $value; ?&gt;" size="'.$width.'"'.$maxlength.' />';

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


}//end class

?>
