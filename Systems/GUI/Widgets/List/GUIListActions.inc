<?php
/**
 * Actions for the GUI List System.
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
 * GUI List Actions Class.
 *
 * @since 4.0.0
 */
class GUIListActions
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
        $content  = '<div class="%class%-left" id="%id%-left">';
        $content .= '<div class="%class%-right" id="%id%-right">';
        $content .= '<ul class="%class%" id="%id%">';

        // Load the list items.
        list($providerSystem, $providerMethod) = explode('::', $settings['dataProvider']);
        $content .= '<?php ';
        $content .= 'Channels::includeSystem(\'GUIList\');';
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
        $content .= '$data = '.$settings['dataProvider'].'();';
        $content .= 'GUIList::paintItems($data, \''.(bool) $settings['sortable'].'\', \'%id%\', \'%class%\');';
        $content .= ' ?>';

        $content .= '</ul>';
        $content .= '</div>';
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
     * Paint the items of the list.
     *
     * @param array  $items    An array of items to print.
     * @param int    $sortable Should drag-handles be printed?
     * @param string $id       The ID used by the list.
     * @param string $class    The class used by the list.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintItems(array $items, $sortable, $id, $class)
    {
        $sortable = (int) $sortable;
        $title    = _('Click and drag to change order');

        foreach ($items as $item) {
            echo '<li>';
            if ($sortable === 1) {
                echo '<span class="'.$class.'-dragHandle" title="'.$title.'"></span>';
            }

            echo $item.'</li>';
        }

    }//end paintItems()


}//end class

?>