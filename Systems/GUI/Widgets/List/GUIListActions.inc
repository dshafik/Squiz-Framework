<?php
/**
 * Actions for the GUIList widget.
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
 * List Actions Class.
 */
class GUIListActions
{

    /**
     * List of items.
     *
     * @var array
     * @internal
     */
    protected static $items = array();

    /**
     * If TRUE then items will be sortable.
     *
     * @var boolean
     */
    protected static $sortable = FALSE;

    /**
     * If TRUE then items will be deletable.
     *
     * @var boolean
     */
    protected static $allowDelete = FALSE;

    /**
     * Custom Delete class.
     *
     * @var string
     */
    protected static $customDeleteClass = '';

    /**
     * The items generator channel.
     *
     * @var array
     */
    protected static $itemsGenerator = NULL;

    /**
     * Message that is displayed when there are no items in the list.
     *
     * @var string
     */
    protected static $noItemsMsg = '';

    /**
     * If FALSE the Save button will not be enabled when widget value is changed.
     *
     * @var boolean
     */
    protected static $enablesSaveButton = TRUE;

    /**
     * If FALSE the GUI.setModified method will not be called when value is changed.
     *
     * @var boolean
     */
    protected static $requiresSave = TRUE;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        $sortingClass  = '';
        $deletingClass = '';

        if (Util::getArrayIndex($settings, 'sortable', self::$sortable) === TRUE) {
            $sortingClass = ' sortable';
        }

        if (Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete) === TRUE) {
            $deletingClass = ' deletable';
        }

        $items = Util::getArrayIndex($settings, 'items', self::$items);

        $noItemsClass = '';
        if (empty($items) === TRUE) {
            $noItemsClass = ' noItems';
        }

        echo '<ul id="'.$id.'" class="'.$class.$sortingClass.$deletingClass.$noItemsClass.'">';

        foreach ($items as $itemid => $item) {
            self::paintItem($itemid, $item, $settings);
        }

        echo '</ul>';

        $noItemsMsg = Util::getArrayIndex($settings, 'noItemsMsg', self::$noItemsMsg);
        if (empty($items) === TRUE && empty($noItemsMsg) === FALSE) {
            echo '<div id="'.$id.'-noItemsMsg" class="'.$class.'-noItemsMsg '.$noItemsClass.'">';
            echo $noItemsMsg;
            echo '</div>';
        }

        return $settings;

    }//end paint()


    /**
     * Prints a single list item.
     *
     * @param string $itemid   Id of the item.
     * @param array  $item     Item infromation.
     * @param array  $settings Widget settings.
     *
     * @return void
     */
    protected static function paintItem($itemid, array $item, array $settings)
    {
        $id                = $settings['widget']['id'];
        $class             = $settings['widget']['type'];
        $deleteClass       = 'GUI-delete '.$id.'-deleteIcon';
        $customDeleteClass = Util::getArrayIndex($settings, 'customDeleteClass', '');
        if (empty($customDeleteClass) === FALSE) {
            $deleteClass .= ' '.$customDeleteClass;
        }

        echo '<li itemid="'.$itemid.'">';

        echo '<div class="'.$class.'-dragHandle"></div>';
        echo '<div class="'.$class.'-itemContent">'.$item['itemContent'].'</div>';
        echo '<div class="'.$deleteClass.'"><span class="GUI-deleteIcon" onclick="GUI.getWidget(\''.$id.'\').toggleDelete(this.parentNode.parentNode);"/></div>';

        echo '</li>';

    }//end paintItem()


    /**
     * Generated new items using specified data and channel.
     *
     * @param array $itemsData The row data to send to the specified channel function.
     * @param array $channel   The channel to call which generates the column contents
     *                         using the given rowData.
     * @param array $settings  The widget settings.
     *
     * @return string
     * @throws ChannelException If invalid API channel is specified.
     *
     * @api            read
     * @api-permission public
     */
    public static function generateItems(array $itemsData, array $channel, array $settings)
    {
        // Check if channel is callable.
        Channels::includeSystem('API');
        $api = API::getAPI($channel['system'], $channel['action']);
        if ($api === FALSE) {
            throw new ChannelException(_('Specified channel is invalid'));
        }

        // This is an API request therefore we need to set the current template, it
        // should be the same as the table's template.
        Channels::includeSystem('GUI');
        $template = Util::getArrayIndex($settings, 'template', array());
        GUI::setCurrentTemplateKey(implode('::', $template));

        Channels::includeSystem($channel['system']);
        $columnNames = Util::getArrayIndex($settings, 'columns', array());

        $rows = array();

        ob_start();
            foreach ($itemsData as $itemid => $data) {
                $item = API::executeRequest($channel['system'], $channel['action'], array($itemsData[$itemid]), $api['options']);
                self::paintItem($itemid, $item, $settings);
            }

            $contents = ob_get_contents();
        ob_end_clean();

        return $contents;

    }//end generateItems()


}//end class

?>
