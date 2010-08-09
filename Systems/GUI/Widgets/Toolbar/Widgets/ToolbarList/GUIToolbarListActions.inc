<?php
/**
 * Actions for the ToolbarList System.
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
 * ToolbarList Actions Class.
 *
 */
class GUIToolbarListActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
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