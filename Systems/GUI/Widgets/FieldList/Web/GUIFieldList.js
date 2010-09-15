/**
 * JS Class for the List Widget.
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

function GUIFieldList(id, settings)
{
	this.id       = id;
	this.settings = settings;
	this.init();

}

GUIFieldList.prototype = {
	/**
	 * Initialises the List widget.
	 *
	 * The handler functions are stored as closure variables for clean access to the
	 * widget object through "self". Otherwise a more obtuse method, like using
	 * jQuery's data parameter, would be necessary.
	 *
	 */
	init: function()
	{
		var self = this;
		var listObj = dfx.getId(this.id);

		// Setup sorting.
		if (this.settings.sortable === true) {
			this.enableSorting();
		}

		// Setup sorting.
		if (this.settings.editable === true) {
			// Name of the "add new row" textbox.
			var newRowName = this.id + '-list-item-add';
			var newRowTb   = dfx.getId(newRowName);

			/**
			 * Handle typing in an existing row.
			 *
			 * @return {Boolean}
			 */
			var handleExistingRowKeypress = function() {
				GUI.setModified(self, true);
				return true;
			};

			/**
			 * Handle toggling the deleted state of a row.
			 *
			 * @return {Boolean}
			 */
			var handleToggleDelete = function() {
				var row = dfx.getParents(this)[0];
				dfx.toggleClass(row, 'deleted');
				GUI.setModified(self, true);

				return true;
			};

			/**
			 * Handle typing in a new row.
			 *
			 * The crucial for handling this in a keypress state is that the existing
			 * "new row" text box must become the "existing row" text box of the newly
			 * created row. (We cannot just plain copy the data, since the data is not
			 * available yet at the keypress stage.)
			 *
			 * What this does, then, is:
			 * - clone the last existing row to create the new row.
			 * - replace the text box in the new row with the "Add new row" text box.
			 * - create a new text box to put into the "Add new row" row.
			 * - re-assign the events so the new text box runs this when necessary, and
			 *   the newly-created row's text box now runs the existing row handler.
			 *
			 * @return {Boolean}
			 */
			var handleNewRowKeypress = function() {
				var listObj = dfx.getId(self.id);
				var newRow  = dfx.getParents(this)[0];

				var rows     = dfx.getClass('extraRow', listObj);
				var lastRow  = rows[0];

				var newRowParent = dfx.getParents(this)[0];

				// Clone the row, and remove any "deleted" attribute from it so it
				// begins "active".
				var rowClone = dfx.cloneNode(lastRow)[0];
				dfx.removeClass(rowClone, 'extraRow');

				// Move the "new row" input box into the cloned row, then remove its
				// existing input box.
				var clonedRowTb = dfx.getTag('input', rowClone)[0];
				dfx.insertAfter(clonedRowTb, this);
				dfx.remove(clonedRowTb);

				// Set the appropriate random ID of the new row.
				var cloneId = self.id + '-new_' + dfx.getUniqueId();
				this.id     = cloneId + '-value';

				// Add the new row to the list, and set the appropriate ID.
				// Then (re-)give the focus to the textbox so the user can
				// continue typing.
				dfx.append(listObj, rowClone);
				rowClone.id = cloneId;
				this.focus();

				// Now make a new "add new row" text box.
				var newTb   = dfx.cloneNode(this, false)[0];
				newTb.value = '';
				newTb.id    = newRowName;

				// Pass this event handler onto the new, new row textbox.
				// Now give this event to the new "add new row" box, and change this to
				// use the existing row function instead.
				dfx.addEvent(newTb, 'keypress', handleNewRowKeypress);
				dfx.removeEvent(this, 'keypress', handleNewRowKeypress);
				dfx.addEvent(this, 'keypress', handleExistingRowKeypress);

				// And add it.
				dfx.append(newRowParent, newTb);

				// Finally, set the dirty flag for the widget.
				GUI.setModified(self, true);

				return true;
			};

			dfx.addEvent(newRowTb, 'keypress', handleNewRowKeypress);

			var inputs = dfx.getTag('input', listObj);
			dfx.addEvent(inputs, 'keypress', handleExistingRowKeypress);
		}

		// Setup deletion events.
		if (this.settings.allowDelete === true) {
			var deleteIcons = dfx.getClass('deleteIcon', listObj);
			dfx.addEvent(deleteIcons, 'click', handleToggleDelete);
		}
	},

	/**
	 * Make the list widget sortable.
	 *
	 * Invokes the jQuery UI sortable pattern on the list, allowing the handles to
	 * be dragged.
	 *
	 * Dragging into the new row is prohibited. Because "sortable" doesn't support
	 * sorting part of a list, the "add new row" row is defined outside it so it
	 * doesn't get reordered.
	 *
	 */
	enableSorting: function()
	{
		var self = this;

		jQuery('#' + this.id).sortable({
			handle: 'span.dragHandle',
			axis: 'y',
			change: function(event, ui) {
				GUI.setModified(self, true);
			}
		});

	},

	/**
	 * Gets the value of the widget.
	 *
	 * Returns a multi-dimensional associative array:
	 * - 'values'  => The new values of the rows sent into the list.
	 *             => New rows will be given row-names beginning with "new-" and
	 *                a unique ID.
	 * - 'order'   => The new order of the rows.
	 * - 'deleted' => A list of the rows that have been marked for deletion.
	 *
	 * Order and deleted values, and values keys, are the row IDs passed into the
	 * template.
	 *
	 * @return {Object}
	 */
	getValue: function()
	{
		var elem = dfx.getId(this.id);
		if (!elem) {
			return null;
		}

		var listItems = dfx.getTag('li', elem);
		var ln        = listItems.length;

		var value = {
						'values': {},
						'order': [],
						'deleted': []
					};

		for (var i = 0; i < ln; i++) {
			var li = listItems[i];
			var rowId = listItems[i].id.replace(this.id + '-', '');

			if (rowId !== '__new') {
				value['order'].push(rowId);

				// If we allow deletion of rows, check for whether row
				// is showing as marked for deletion.
				if (this.settings.allowDelete === true) {
					if (dfx.hasClass(li, 'deleted') === true) {
						value['deleted'].push(rowId);
					}
				}

				var tb = dfx.getId(listItems[i].id + '-value');
				value['values'][rowId] = tb.value;
			}
		}

		return value;
	},



};
