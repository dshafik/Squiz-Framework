<?php
/**
 * Actions for the GUIBox System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUIBox
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Box Actions Class.
 *
 * @since 4.0.0
 */
class GUIBoxActions
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
        // Check border settings.
        $borderClass = '';

        include_once 'Libs/Util/Util.inc';
        $borderLeft  = Util::getArrayIndex($settings, 'borderLeft', 'true');
        $borderRight = Util::getArrayIndex($settings, 'borderRight', 'true');

        if ($borderLeft === 'false') {
            $borderClass = ' noLeftBorder';
        }

        if ($borderRight === 'false') {
            $borderClass .= ' noRightBorder';
        }

        $content  = '<div id="%id%" class="%class%'.$borderClass.'">';
        $content .= '<div class="%class%-header">';

        if (isset($settings['title']) === TRUE) {
            // Simple title text/widget.
            if (is_array($settings['title']) === FALSE) {
                if (empty($settings['title']) === FALSE) {
                    $content .= '<div class="%class%-title">'.$settings['title'].'</div>';
                }
            } else if (isset($settings['title']['dataProvider']) === TRUE) {
                // Dynamic title.
                $content .= self::_getContentFromDataProvider($settings['title']['dataProvider']);
            }
        }

        if (isset($settings['headerContent']) === TRUE) {
            $content .= '<div class="%class%-headerContent">';;
            // Simple extra header text or widget.
            if (is_array($settings['headerContent']) === FALSE) {
                if (empty($settings['headerContent']) === FALSE) {
                    $content .= '<div class="%class%-title">'.$settings['headerContent'].'</div>';
                }
            } else if (isset($settings['headerContent']['dataProvider']) === TRUE) {
                // Dynamic header content.
                $content .= self::_getContentFromDataProvider(
                    $settings['headerContent']['dataProvider']
                );
            }

            $content .= '</div>';
        }

        $content .= '</div>';
        $content .= '<div class="%class%-content">';

        // Content setting can be a single widget or a data provider.
        if (is_array($settings['content']) === FALSE
        ) {
            // Simple content.
            $content .= $settings['content'];
        } else if (isset($settings['content']['dataProvider']) === TRUE) {
            $content .= self::_getContentFromDataProvider(
                $settings['content']['dataProvider']
            );
        }

        $content .= '</div><!--end box content-->';
        $content .= '</div>';

        return $content;

    }//end getTemplateContent()


    private static function _getContentFromDataProvider($dataProvider)
    {
        list($providerSystem, $providerMethod) = explode('::', $dataProvider);
        $content  = '<?php '."\n";
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');'."\n";
        $content .= 'echo '.$dataProvider.'();'."\n";
        $content .= ' ?>';

        return $content;

    }//end _getContentFromDataProvider()


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
