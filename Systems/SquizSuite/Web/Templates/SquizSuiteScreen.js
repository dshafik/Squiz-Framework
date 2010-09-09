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
    var _liveProductsIdx      = [];

    var _newRow       = null;
    var _newDetailRow = null;

    var _deletedProducts = {};

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

        // Attach delete event.
        _deletedProducts = {}

        var delBtns = dfx.getClass('SquizSuiteScreen-deleteColBtn', _connectedTableDOM);
        dfx.foreach(delBtns, function(idx) {
            var delBtn = delBtns[idx];
            dfx.addEvent(delBtn, 'click', function() {
                self.toggleProductDelete(delBtn);
            });

            return true;
        });

        // Hide new product row.
        _newRow       = _connectedProductRows[(_connectedProductRows.length - 3)];
        _newDetailRow = _connectedProductRows[(_connectedProductRows.length - 1)];
        dfx.hideElement(dfx.getClass('GUI-delete', _newDetailRow)[0]);
        this.toggleNewRow();

        // Collect the list of live products and
        // update the summary information.
        _liveProductsIdx = [];
        dfx.foreach(_connectedProducts, function(idx) {
            if (_connectedProducts[idx].status === 'live') {
                _liveProductsIdx.push(idx);
            }

            return true;
        });

        if (_liveProductsIdx.length > 0) {
            self._initLiveProductStatus(0, null);
        }

        GUI.setModified(this, true);
    };

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
    };

    this.toggleProductDelete = function(delBtn) {
        var trDom    = dfx.getParents(delBtn, 'tr', _connectedTableDOM)[0];
        var rowid    = parseInt(trDom.getAttribute('rowid'));
        var row      = _connectedProductRows[(rowid * 2) + 1];
        var systemid = delBtn.getAttribute('systemid');
        if (dfx.hasClass(trDom, 'deleted') === true) {
            dfx.removeClass(trDom, 'deleted');
            dfx.removeClass(delBtn, 'recover');
            dfx.removeClass(row, 'deleted');
            _deletedProducts[systemid] = false;
        } else {
            dfx.addClass(trDom, 'deleted');
            dfx.addClass(delBtn, 'recover');
            dfx.addClass(row, 'deleted');
            _deletedProducts[systemid] = true;
        }
    };

    this.addProduct = function() {
        if (dfx.hasClass(_newRow, 'hidden') === true) {
            this.toggleNewRow();
        }
    };

    this.toggleNewRow = function() {
        if (dfx.hasClass(_newRow, 'hidden') === true) {
            dfx.removeClass(_newRow, 'hidden');
            dfx.removeClass(_newDetailRow, 'hidden');
        } else {
            dfx.addClass(_newRow, 'hidden');
            dfx.addClass(_newDetailRow, 'hidden');
        }
    };

    this.approveConnection = function(btn) {
        var screenid = 'SquizSuite:SquizSuiteScreen';
        var systemid = btn.id.replace('squizSuite-', '');
        systemid     = systemid.replace('-approve-btn', '');

        var data       = {};
        data[screenid] = {
            type: 'approveConnection',
            systemid: systemid
        };

        var self   = this;
        var params = {
            templateData: dfx.jsonEncode(data)
        };

        GUI.sendRequest('GUI', 'saveTemplateData', params, function(data) {
            if (data.result.success) {
                self._requestProductSummary(systemid, function() {});
            }
        });
    };

    this._initLiveProductStatus = function(idx, callback) {
        var self     = this;
        var systemid = _connectedProducts[_liveProductsIdx[idx]].systemid;

        this._requestProductSummary(systemid, function() {
            if (idx < (_liveProductsIdx.length - 1)) {
                self._initLiveProductStatus((idx + 1), callback);
            } else {
                if (callback) {
                    callback.call(self);
                }
            }
        });
    };

    this._requestProductSummary = function(systemid, callback) {
        var self = this;

        var data       = {};
        var screenid   = 'SquizSuite:SquizSuiteScreen';
        data[screenid] = {
            type: 'getProductSummary',
            systemid: systemid
        };

        var params = {
            templateData: dfx.jsonEncode(data)
        };

        GUI.sendRequest('GUI', 'saveTemplateData', params, function(data) {
            var statusDiv  = dfx.getId('squizSuite-' + systemid + '-statusWrap');
            var summaryDiv = dfx.getId('squizSuite-' + systemid + '-summary');
            if (data.result.success) {
                var summaryInfo = data.result.success[screenid];
                if (dfx.hasClass(statusDiv, 'pending') === true) {
                    dfx.swapClass(statusDiv, 'pending', 'live');
                } else {
                    dfx.swapClass(statusDiv, 'loading', 'live');
                }

                var c     = '<span class="SquizSuiteScreen-summaryLabel">No summary information</span>';
                var first = true;
                dfx.foreach(summaryInfo, function(idx) {
                    if (first === true) {
                        c     = '';
                        first = false;
                    }

                    c += '<span class="SquizSuiteScreen-summaryLabel">' + summaryInfo[idx].label + '</span>';
                    c += '<span class="SquizSuiteScreen-summaryValue">' + summaryInfo[idx].value + '</span>';

                    return true;
                });

                dfx.setHtml(summaryDiv, c);
            } else if (data.result.errors) {
                var summaryInfo = data.result.errors[screenid];
                dfx.swapClass(statusDiv, 'loading', 'error');
                var c = '<span class="SquizSuiteScreen-summaryLabel">';
                c    += 'Failed to get the product summary ...</span>';
                c    += '</span>';
                dfx.setHtml(summaryDiv, c);
            }//end if

            if (callback) {
                callback.call(self);
            }
        });
    };

    this.getValue = function() {
        var data = {};
        data.currProductName = GUI.getWidget('squizSuite-currProductName').getValue();
        data.deletedProducts = _deletedProducts;
        return data;
    };

}
