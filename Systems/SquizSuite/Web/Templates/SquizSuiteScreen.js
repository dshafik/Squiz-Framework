/**
 * JS Class for the SquizSuite Screen.
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
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

var SquizSuiteScreen = new function()
{
    var _currentProduct       = null;
    var _connectedProducts    = [];
    var _currentTableDOM      = null;
    var _connectedTableDOM    = null;
    var _connectedProductRows = null;

    var _newRow       = null;
    var _newDetailRow = null;

    this.initScreen = function(data) {
        var self = this;

        _currentProduct       = data.currentProduct;
        _connectedProducts    = data.connectedProducts;
        _currentTableDOM      = dfx.getId('currProductTable');
        _connectedTableDOM    = dfx.getId('connProductsTable');
        _connectedProductRows = dfx.getTag('tr', dfx.getTag('tbody', _connectedTableDOM));

        // Hide all the product detail rows.
        dfx.foreach(_connectedProductRows, function(idx) {
            if (idx === 0) {
                return true;
            }

            if (dfx.hasClass(_connectedProductRows[idx], 'rowEven') === true) {
                dfx.addClass(_connectedProductRows[idx], 'hidden');
            }

            return true;
        });

        // Attach expander event.
        var expanders = dfx.getClass('SquizSuiteScreen-productExpander', _connectedTableDOM);
        dfx.foreach(expanders, function(idx) {
            var span = expanders[idx];
            dfx.addEvent(span, 'click', function() {
                self.toggleProductDetails(span);
            });

            return true;
        });

        // Hide new product row.
        _newRow       = _connectedProductRows[(_connectedProductRows.length - 3)];
        _newDetailRow = _connectedProductRows[(_connectedProductRows.length - 1)];
        dfx.hideElement(dfx.getClass('GUI-delete', _newDetailRow)[0]);
        this.toggleNewRow();
    },

    this.toggleProductDetails = function(span) {
        var rowid = parseInt(span.getAttribute('rowid'));
        var row   = _connectedProductRows[(rowid * 2) + 1];
        if (dfx.hasClass(row, 'hidden') === true) {
            dfx.removeClass(row, 'hidden');
            dfx.addClass(span, 'expanded');
        } else {
            dfx.addClass(row, 'hidden');
            dfx.removeClass(span, 'expanded');
        }
    },

    this.addProduct = function() {
        if (dfx.hasClass(_newRow, 'hidden') === true) {
            this.toggleNewRow();
        }
    },

    this.toggleNewRow = function() {
        if (dfx.hasClass(_newRow, 'hidden') === true) {
            dfx.removeClass(_newRow, 'hidden');
            dfx.removeClass(_newDetailRow, 'hidden');
        } else {
            dfx.addClass(_newRow, 'hidden');
            dfx.addClass(_newDetailRow, 'hidden');
        }
    }

}
