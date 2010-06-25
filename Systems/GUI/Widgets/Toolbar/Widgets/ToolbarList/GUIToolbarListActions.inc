<?php
/**
 * Actions for the ToolbarList System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage ToolbarList
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * ToolbarList Actions Class.
 *
 * @since 4.0.0
 */
class GUIToolbarListActions
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
        $content = '<div id="'.$settings['id'].'" class="List"><ul></ul></div>';
        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl  The HTML content of the widget.
     * @param array   $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl, array $settings=array())
    {
        $uls = $htmlTpl->getElementsByTagName('ul');
        $ul  = $uls->item(0);

        return $htmlTpl;

    }//end prepareBake()


    /**
     * Adds a list item.
     *
     * @param DOMNode $list      The list element to add to.
     * @param string  $id        The id of the list item.
     * @param string  $title     The title of the list item.
     * @param integer $position  The position of the item in the list.
     * @param string  $printCond PHP Condition to check before painting the item.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function additem(DOMNode $list, $id, $title, $position, $printCond=NULL)
    {
        $uls = $list->getElementsByTagName('ul');
        $ul  = $uls->item(0);
        $doc = new DOMDocument();
        $doc->loadXML('<li id="'.$id.'">'._($title).'</li>');

        $listItemNode = $list->ownerDocument->importNode($doc->documentElement, TRUE);
        $ul->appendChild($listItemNode);

        if ($printCond !== NULL) {
            Channels::includeSystem('GUI');
            GUI::wrapConditionAroundElement($listItemNode, $printCond);
        }

        return $list;

    }//end additem()


}//end class

?>