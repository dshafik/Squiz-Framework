<?php
/**
 * Actions for the Select GUI Widget System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUISelect
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Select GUI Widget Actions Class.
 *
 * @since 4.0.0
 */
class GUISelectActions
{


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
        $minWidth     = Util::getArrayIndex($settings, 'minWidth');
        $inlineStyles = array();

        if ($minWidth !== NULL) {
            $inlineStyles[] = 'min-width: '.$minWidth.';';
        }

        // Load the list items.
        list($providerSystem, $providerMethod) = explode(
            '::',
            $settings['dataProvider']
        );

        $content = '<select id="%id%" class="%class%"';

        if (count($inlineStyles) > 0) {
            $content .= ' style="'.implode(' ', $inlineStyles).'"';
        }

        $content .= '>';

        $content .= '<?php ';
        $content .= 'Channels::includeSystem(\'GUISelect\');';
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
        $content .= '$data = '.$settings['dataProvider'].'();';
        $content .= 'GUISelect::paintItems($data, \'%id%\', \'%class%\');';
        $content .= ' ?>';

        $content .= '</select>';

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
     * Paint the items of the select field.
     *
     * @param array  $data  Data provided by the specified data provider.
     * @param string $id    The ID used by the list.
     * @param string $class The class used by the list.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintItems(array $data, $id, $class)
    {
        $selected = Util::getArrayIndex($data, 'selected');
        $options  = $data['options'];

        foreach ($options as $optionKey => $option) {
            echo '<option value="'.$optionKey.'"';

            if ($selected === $optionKey) {
                echo ' selected="selected"';
            }

            echo '>'.$option.'</option>';
        }//end foreach item

    }//end paintItems()


}//end class

?>
