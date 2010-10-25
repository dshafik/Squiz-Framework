/**
 * JavaScript class for the Date Switcher GUI widget.
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

function GUIDateSwitcher(id, settings)
{
    this.id        = id;
    this.settings  = settings;
    this.className = 'GUIDateSwitcher';

    this.months    = null;
    this.startDate = null;
    this.endDate   = null;

    GUI.addWidgetEvent(this, 'dateChanged');

    this.init();
}

GUIDateSwitcher.prototype = {
    init: function() {
        if (this.settings.startDate === null) {
            this.startDate = null;
        } else {
            this.startDate = new Date(this.settings.startDate);
        }

        this.months = this.settings.months;
        this.addEvents();
    },

    addEvents: function() {
        var self = this;

        var widget     = dfx.getId(this.id);
        var currentBtn = dfx.getClass('current-period', widget)[0];
        var currentA   = dfx.getTag('A', currentBtn)[0];
        dfx.addEvent(currentA, 'click', function() {
            var widget    = dfx.getId(self.id);
            var currentBtn = dfx.getClass('current-period', widget)[0];
            var datePopup = dfx.getClass('date-popup', widget)[0];
            dfx.toggleClass(datePopup, 'expanded');
            dfx.toggleClass(currentBtn, 'selected');
            return false;
        });
    },

    setMonths: function(months) {
        this.months = months;
    },

    setStartDate: function(date) {
        this.startDate = date;
    },

    getValue: function() {
        var retval = {
            endDate: new Date(),
            startDate: new Date(),
        };

        if (this.startDate === null) {
            // We are set to "last X months".
            // Get today's date and time, and zero out the time.
            var today = new Date();
            today.setHours(0);
            today.setMinutes(0);
            today.setSeconds(0);
            today.setMilliseconds(0);

            // Calculate the end date to be the last complete day, by subtracting
            // the number of milliseconds in a day. (JS dates are stored in
            // millisecond format, not seconds as in PHP.)
            retval.endDate.setDate(retval.endDate.getDate() - 1);

            retval.startDate.setTime(retval.endDate.getTime());
            retval.startDate.setMonth((retval.endDate.getMonth() - this.months));

            // We now have date exactly X months ago, however the period actually
            // starts on the next day.
            retval.startDate.setDate(retval.startDate.getDate() + 1);
        } else {
            // We are set to a specific time period.
            retval.startDate.setTime(this.startDate.getTime());
            retval.endDate.setMonth((retval.startDate.getMonth() + this.months));

            // We now have date exactly X months later, however the period actually
            // concludes on the previous day.
            retval.endDate.setDate(retval.endDate.getDate() + 1);
        }//end if

        return retval;
    }
}
