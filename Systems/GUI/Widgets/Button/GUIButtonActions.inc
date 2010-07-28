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
        $value = $settings['value'];
        $color = $settings['color'];

        if (empty($color) === TRUE) {
            $color = 'Default';
        }

        // Handle click setting.
        $onClick = '';
        if (isset($settings['click']) === TRUE) {
            if (isset($settings['click']['function']) === TRUE) {
                $onClick .= ' onclick="';

                $args = '';
                if (isset($settings['click']['arguments']) === TRUE
                    && empty($settings['click']['arguments']) === FALSE
                ) {
                    $args .= '\''.implode('\', \'', $settings['click']['arguments']).'\'';
                }

                if (isset($settings['click']['targetWidget']) === TRUE) {
                    $onClick .= 'GUI.getWidget(\''.$settings['click']['targetWidget'].'\').';
                }

                $onClick .= $settings['click']['function'].'('.$args.');"';
            }
        }

        $content  = '<button class="%class% button'.$color.'" type="button"'.$onClick.'>';
        $content .= '<span>'.$value.'</span>';
        $content .= '</button>';

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