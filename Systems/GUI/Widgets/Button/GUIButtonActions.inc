<?php
/**
 * Actions for the Button System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Button
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Button Actions Class.
 *
 * @since 4.0.0
 */
class GUIButtonActions
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
        $val = $settings['value'];

        $content  = '<div class="%class%" id="%id%">';
        $content .= '<div class="%class%-left"></div>';
        $content .= '<input type="button" class="%class%-mid" value="&lt;?php echo _(\''.$val.'\'); ?&gt;" onclick="'.$settings['onclick'].'" />';
        $content .= '<div class="%class%-right"></div>';
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