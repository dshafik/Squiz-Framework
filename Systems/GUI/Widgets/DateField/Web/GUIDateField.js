/**
 * JS Class for the Date Field Widget.
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

function GUIDateField(id, settings)
{
	this.id       = id;
	this.settings = settings;

	this._currentValue = this.settings.initialDate;
	this._dayWidget    = GUI.getWidget(this.id + '-day');
	this._monthWidget  = GUI.getWidget(this.id + '-month');
	this._yearWidget   = GUI.getWidget(this.id + '-year');

	this.changeEnabled = false;

	// Add a widget for the date changing.
	GUI.addWidgetEvent(this, 'change');

	this.init();
}

GUIDateField.prototype = {
	/**
	 * Initialises the Date Field widget.
	 */
	init: function()
	{
	    var self = this;
	    this.validateDate();

	    this._dayWidget.addChangeCallback(function() {
            if (self.changeEnabled === true) {
                self.dateChanged.call(self);
            }
	    });

	    this._monthWidget.addChangeCallback(function() {
            if (self.changeEnabled === true) {
                self.dateChanged.call(self);
	        }
	    });

	    this._yearWidget.addChangeCallback(function() {
            if (self.changeEnabled === true) {
                self.dateChanged.call(self);
            }
	    });

	    this.changeEnabled = true;
	},

	validateDate: function()
	{
	    var day   = this._dayWidget.getValue()[0];
	    var month = this._monthWidget.getValue()[0];
	    var year  = this._yearWidget.getValue()[0];

	    var leapYear = false;
	    if (year % 100 === 0) {
	        if (year % 400 === 0) {
	            leapYear = true;
	        }
	    } else {
	        if (year % 4 === 0) {
	            leapYear = true;
	        }
	    }//end if

	    // Calculate how many days there should be in the month.
	    var maxDay = 31;
	    if (month === '02') {
	        if (leapYear === true) {
	            maxDay = 29;
	        } else {
	            maxDay = 28;
	        }
	    } else {
	        var monthsWith30Days = ['04', '06', '09', '11'];
	        if (monthsWith30Days.inArray(month) === true) {
	            maxDay = 30;
	        }
	    }//end if

	    // If a month/year change has caused the day to fall out of range,
	    // change it. Don't fire an additional dateChanged event for this.
	    if (day > maxDay) {
	        var oldCE = this.changeEnabled;
	        this.changeEnabled = false;
	        this._dayWidget.setValue([maxDay.toString()]);
	        this.changeEnabled = oldCE;
	    }

	    // Disable any days in the month that are not valid for this month/year
	    // selection.
	    var daySelect = dfx.getId(this._dayWidget.id);
	    for (var i = 0; i < daySelect.options.length; i++) {
	        var option = daySelect.options[i];
	        if (parseInt(option.value, 10) > maxDay) {
	            option.disabled = true;
	        } else {
	            option.disabled = false;
	        }
	    }
	},

	dateChanged: function()
	{
        this.validateDate();
        var newValue = this.getValue();

        // Invalid date. Do not change our current value.
        if (newValue === null) {
            newValue = this.currentValue;
        }

        if (this.currentValue !== newValue) {
            this.currentValue = newValue;
            this.fireChangeCallbacks(newValue);
        }
	},

	getValue: function()
	{
	    var ok = true;

	    if (this._dayWidget.getValue().length === 0) {
	        ok = false;
	    }

	    if (this._monthWidget.getValue().length === 0) {
	        ok = false;
	    }

	    if (this._yearWidget.getValue().length === 0) {
	        ok = false;
	    }

	    if (ok === true) {
            var day   = this._dayWidget.getValue()[0];
            var month = this._monthWidget.getValue()[0];
            var year  = this._yearWidget.getValue()[0];

            if (day < 10) {
                day = '0' + day;
            }

            if (month < 10) {
                month = '0' + month;
            }

            console.info(year + '-' + month + '-' + day);
            return (year + '-' + month + '-' + day);
        } else {
            return null;
        }
	},

	setValue: function(newDate)
	{
	    var dateParts = newDate.split(/-/);
	    var oldCE = this.changeEnabled;

	    this.changeEnabled = false;

	    this._yearWidget.setValue([parseInt(dateParts[0], 10)]);
	    this._monthWidget.setValue([parseInt(dateParts[1], 10)]);
	    this._dayWidget.setValue([parseInt(dateParts[2], 10)]);

	    // Now fire the event in one go.
	    this.changeEnabled = oldCE;
	    this.dateChanged();
	},

	getDateObject: function()
	{
	    return new Date(this.getValue());
	},
};
