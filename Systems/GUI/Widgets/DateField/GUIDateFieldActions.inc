<?php
/**
 * Actions for the GUI Date Field Widget System.
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
 * GUI Date Field Widget Actions Class.
 */
class GUIDateFieldActions
{

    /**
     * Determines whether the field should be inlined.
     *
     * If FALSE, it is placed in a DIV of its own.
     * If TRUE, it will be placed in a SPAN instead. An enclosing tag is required
     * so the select widgets created can be styled.
     *
     * @var boolean
     */
    protected static $inline = FALSE;

    /**
     * Initial date.
     *
     * Accepts both a UNIX timestamp (from time() for instance) or a 'Y-m-d'
     * string format.
     *
     * @var mixed
     */
    protected static $initialDate = NULL;

    /**
     * Range of years to be displayed.
     *
     * @var array
     */
    protected static $yearRange = NULL;


	/**
	 * Returns this widget's content.
	 *
	 * @param array $settings The list of settings for this widget.
	 *
	 * @return void
	 */
	public static function paint(array $settings=array())
	{
	    $defaultYears = array(
	                     'min' => 1900,
	                     'max' => 2100,
	                    );

	    $widgetPrefix = $settings['widget']['id'];
	    $widgetClass  = $settings['widget']['type'];
	    $yearRange    = Util::getArrayIndex($settings, 'yearRange', $defaultYears);
	    $initialDate  = Util::getArrayIndex($settings, 'initialDate', date('Y-m-d'));
	    $inline       = Util::getArrayIndex($settings, 'inline', FALSE);

	    if ($inline === TRUE) {
	        $enclosingTag = 'span';
	    } else {
	        $enclosingTag = 'div';
	    }

	    // If we were given a numeric initial date, then turn it into
	    // an ISO8601 date.
	    if (is_numeric($initialDate) === TRUE) {
	        $initialDate = date('Y-m-d', $initialDate);
	    }

	    $dateParts = explode('-', substr($initialDate, 0, 10));

	    echo '<'.$enclosingTag.' id="'.$widgetPrefix.'" class="'.$widgetClass.'">';

	    $dayOptions = array();
	    for ($i = 1; $i <= 31; $i++) {
	        $dayOptions[$i] = $i;
	    }

	    $selectSettings = array(
	                       'options'  => $dayOptions,
	                       'selected' => (int) $dateParts[2],
	                      );
	    echo GUI::createWidget($widgetPrefix.'-day', 'GUISelect', $selectSettings);
	    echo ' ';

	    $monthOptions = array(
                         1  => _('Jan'),
                         2  => _('Feb'),
                         3  => _('Mar'),
                         4  => _('Apr'),
                         5  => _('May'),
                         6  => _('Jun'),
                         7  => _('Jul'),
                         8  => _('Aug'),
                         9  => _('Sep'),
                         10 => _('Oct'),
                         11 => _('Nov'),
                         12 => _('Dec'),
                        );

	    $selectSettings = array(
	                       'options'  => $monthOptions,
	                       'selected' => (int) $dateParts[1],
	                      );
	    echo GUI::createWidget($widgetPrefix.'-month', 'GUISelect', $selectSettings);
	    echo ' ';

	    foreach ($yearRange as $yearRangeIndex => &$yearRangePart) {
	        if (substr($yearRangePart, 0, 1) === '+') {
	            // Adding years from the current year if the first character is '+'.
	            $yearDiff = substr($yearRangePart, 1);
	            if (is_numeric($yearDiff) === TRUE) {
	                $yearRangePart = date('Y') + $yearDiff;
	            } else {
	                $yearRangePart = $defaultYears[$yearRangeIndex];
	            }
	        } else if (substr($yearRangePart, 0, 1) === '-') {
	            // Subtracting years from the current year if the first character is '-'.
	            $yearDiff = substr($yearRangePart, 1);
	            if (is_numeric($yearDiff) === TRUE) {
	                $yearRangePart = date('Y') - $yearDiff;
	            } else {
	                $yearRangePart = $defaultYears[$yearRangeIndex];
	            }
	        } else if (is_numeric($yearRangePart) === FALSE) {
	            // If not numeric, fail it.
	            $yearRangePart = $defaultYears[$yearRangeIndex];
	        }//end if
	    }//end foreach

	    unset($yearRangePart);

	    $yearOptions = array();
	    for ($i = $yearRange['min']; $i <= $yearRange['max']; $i++) {
	        $yearOptions[$i] = $i;
	    }

	    $selectSettings = array(
	                       'options'  => $yearOptions,
	                       'selected' => (int) $dateParts[0],
	                      );
	    echo GUI::createWidget($widgetPrefix.'-year', 'GUISelect', $selectSettings);

	    echo '</'.$enclosingTag.'>';

	    // Replace the initial date and year range with the sanitised versions
	    // for the JavaScript to munch on.
	    $settings['yearRange']   = $yearRange;
	    $settings['initialDate'] = $initialDate;

		return $settings;

	}//end paint()


}//end class

?>
