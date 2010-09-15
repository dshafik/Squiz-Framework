<?php
/**
 * Actions for the GUI List System.
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
 * GUI List Actions Class.
 */
class GUIFieldListActions
{


	/**
	 * List of initial items to be painted.
	 *
	 * @var array
	 */
	protected static $items = array();


	/**
	 * Controls the appearance of sorting handles next to items.
	 *
	 * If TRUE, sorting handles will appear next to the items, which can be dragged
	 * up and down into new positions, and saved.
	 *
	 * @var boolean
	 */
	protected static $sortable = FALSE;


	/**
	 * Controls whether the items should be editable.
	 *
	 * @var boolean
	 */
	protected static $editable = FALSE;


	/**
	 * Controls whether addition of new items to the list is allowed.
	 *
	 * Ignored when a list is not editable.
	 *
	 * @var boolean
	 */
	protected static $allowAdd = FALSE;


	/**
	 * Controls whether marking items for deletion is allowed.
	 *
	 * Ignored when a list is not editable.
	 *
	 * This widget never deletes any rows from the interface. This setting provides
	 * a "delete" icon to the right of the item, which marks the option as deleted.
	 * This will be sent to the saving function with data marking it as deleted.
	 *
	 * @var boolean
	 */
	protected static $allowDelete = FALSE;


	/**
	 * Sets the default, greyed text to appear on the "add new item" text box.
	 *
	 * Ignored when a list is not editable, or if adding is prohibited.
	 *
	 * @var boolean
	 */
	protected static $addText = '';


	/**
	 * Returns this widgets content.
	 *
	 * @param array $settings The list of settings for this widget.
	 *
	 * @return void
	 */
	public static function paint(array $settings=array())
	{
		$id          = $settings['widget']['id'];
		$class       = $settings['widget']['type'];
		$sortable    = Util::getArrayIndex($settings, 'sortable', self::$sortable);
		$editable    = Util::getArrayIndex($settings, 'editable', self::$editable);
		$allowAdd    = Util::getArrayIndex($settings, 'allowAdd', self::$allowAdd);
		$allowDelete = Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete);
		$addText     = Util::getArrayIndex($settings, 'addText', _('Add new row...'));

		echo '<div class="'.$class.'-left" id="'.$id.'-left">';
		echo '<div class="'.$class.'-right" id="'.$id.'-right">';
		echo '<ul class="'.$class.'" id="'.$id.'">';

		$sortTitle   = _('Click and drag to change order');
		$deleteTitle = _('Click to mark item as deleted. Click again to unmark.');

		$extraClasses = '';

		if ($sortable === FALSE) {
			$extraClasses .= ' noSort';
		}

		if ($allowDelete === FALSE) {
			$extraClasses .= ' noDelete';
		}

		if ($allowAdd === TRUE) {
			$settings['items']['__new'] = '';
		}

		foreach ($settings['items'] as $itemIndex => $item) {
			if ($itemIndex === '__new') {
				$extraClasses .= ' extraRow';
			}

			echo '<li class="existingRow'.$extraClasses.'" id="'.$id.'-'.$itemIndex.'">';
			if ($sortable === TRUE) {
				echo '<span class="'.$class.' dragHandle" title="'.$sortTitle.'"></span>';
			}

			if ($allowDelete === TRUE) {
				echo '<span class="'.$class.' deleteIcon" title="'.$deleteTitle.'"></span>';
			}

			if ($editable === TRUE) {
				$fieldName = $id.'-'.$itemIndex.'-value';
				echo '<div class="editableItem">';
				echo '<input type="text" id="'.$fieldName.'" value="'.htmlentities($item).'" />';
				echo '</div>';
			} else {
				echo htmlentities($item);
			}//end if

			echo '</li>';

		}//end foreach

		echo '</ul>';

		// If we're allowed to add a row, give an extra row.
		if (($editable === TRUE) && ($allowAdd === TRUE)) {
			echo '<div class="'.$class.'-newRow'.$extraClasses.'">';
			if ($sortable === TRUE) {
				echo '<span class="'.$class.' dragHandle" title="'.$sortTitle.'"></span>';
			}

			if ($allowDelete === TRUE) {
				echo '<span class="'.$class.' deleteIcon" title="'.$deleteTitle.'"></span>';
			}

			$fieldName = $id.'-list-item-add';
			echo '<div class="editableItem">';
			echo '<input type="text" id="'.$fieldName.'" value="" />';
			echo '<div class="newRowName">'.htmlentities($addText).'</div>';
			echo '</div>';

			echo '</div>';
		}//end if

		echo '</div>';
		echo '</div>';

		return $settings;

	}//end paint()


}//end class

?>