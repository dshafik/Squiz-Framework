<?php
/**
 * GUI Date Switcher Widget Actions Class.
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
 * GUI Date Switcher Widget Actions Class.
 *
 * The Date Switcher widget is intended to be placed in the toolbar, and to be used
 * where there is a time dimension to data.
 *
 * The Date Switcher widget will allow you to select from the following time periods:
 * - Monthly,
 * - Quarterly (on a calendar scale - Jan-Mar, Apr-Jun, Jul-Sep, Oct-Dec),
 * - Half-Year periods (Jan-Jun, Jul-Dec),
 * - Special periods (eg. last 30 days).
 *
 * The "value" of the widget, as returned in JavaScript at least, consists of the
 * following:
 * - Date period selected.
 * - Start date.
 * - Calculated end date.
 */
class GUIDateSwitcherActions
{


    /**
     * Start date.
     *
     * If this is NULL, the "end date" will be set to the date before today (this is
     * a Google Analytics standard since it is the last *completed* day), and the
     * start date will be set accordingly.
     *
     * @var string
     */
    protected static $startDate = NULL;


    /**
     * Date period in months.
     *
     * @var integer
     */
    protected static $months = 1;


    /**
     * Paints the widget contents.
     *
     * Returns any modified settings.
     *
     * @param array $settings The passed set of settings.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        // Fill default values.
        $settings['startDate'] = Util::getArrayIndex($settings, 'startDate', self::$startDate);
        $settings['months']    = (int) Util::getArrayIndex($settings, 'months', self::$months);

        $endDate = date('Y-m-d', strtotime($settings['startDate'].' +'.$settings['months'].' months -1 day'));

        $forClass = 'GUIDateSwitcher-for-'.$settings['widget']['id'];

        echo '<div class="'.$settings['widget']['type'].'" id="'.$settings['widget']['id'].'">';
        echo '<ul class="switcher-list">';
        echo '<li class="previous-period '.$forClass.'-previous"><a href=""><span></span></a></li>';
        echo '<li class="current-period '.$forClass.'-current"><a href="">';
        echo sprintf(ngettext('%s month: %s - %s', '%s months: %s - %s', $settings['months']), $settings['months'], date('j M', strtotime($settings['startDate'])), date('j M', strtotime($endDate)));
        echo '</a>';
        echo '<span class="static-text last-1-month">'._('Last month').'</span>';
        echo '<span class="static-text last-3-months">'._('Last 3 months').'</span>';
        echo '<span class="static-text last-6-months">'._('Last 6 months').'</span>';
        echo '</li>';
        echo '<li class="next-period '.$forClass.'-next disabled"><a href=""><span></span></a></li>';
        echo '</ul>';
        echo '<div class="date-popup">';

        for ($month = 0; $month <= 11; $month++) {
            echo '<span class="month-block">';
            echo date('M', strtotime('2010-01-01 +'.$month.' months'));
            echo '</span>';
        }//end for

        echo '</div>';
        echo '</div>';

        return $settings;

    }//end paint()


    public static function paintPeriodText()
    {
        if ($startDate === NULL) {

        }
    }


}//end class

?>
