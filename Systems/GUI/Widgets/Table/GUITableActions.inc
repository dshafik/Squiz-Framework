<?php
/**
 * Actions for the GUI Table System.
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
 * GUI Table Actions Class.
 */
class GUITableActions
{

    /**
     * Show/hide table borders.
     *
     * @var boolean
     */
    protected static $showBorders = TRUE;

    /**
     * If TRUE then table rows are selectable.
     *
     * @var boolean
     */
    protected static $selectable = TRUE;

    /**
     * List of table columns.
     *
     * Expected array structure is array('columnid' => 'Column Title').
     *
     * @var array
     */
    protected static $columns = array();

    /**
     * List of rows.
     *
     * Expected array structure is array('rowid' => array('colid' => 'value')).
     *
     * @var array
     */
    protected static $rows = array();


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     * @throws ChannelException If no table columns specified.
     */
    public static function paint(array $settings)
    {
        $tableClass = $settings['widget']['type'];

        include_once 'Libs/Util/Util.inc';
        $columns = Util::getArrayIndex($settings, 'columns', array());
        if (empty($columns) === TRUE) {
            throw new ChannelException(_('Table widget must have at least one column'));
        }

        $showTableBorders = Util::getArrayIndex($settings, 'showBorders', 'false');
        if ($showTableBorders === 'false') {
            $tableClass .= ' noBorders';
        }

        $alternateType = Util::getArrayIndex($settings, 'alternate', NULL);
        if ($alternateType === 'rows') {
            $tableClass .= ' alternateRows';
        } else if ($alternateType === 'cols') {
            $tableClass .= ' alternateCols';
        }

        $selectable = Util::getArrayIndex($settings, 'selectable', FALSE);
        if ($selectable === 'true') {
            $tableClass .= ' selectable';
        }

        echo '<table class="'.$tableClass.'" id="'.$settings['widget']['id'].'" cellspacing="0" cellpadding="0">';

        if (isset($settings['hideHeader']) === FALSE
            || $settings['hideHeader'] !== 'true'
        ) {
            echo '<thead><tr>';
            $colids     = array_keys($columns);
            $lastColid  = $colids[(count($colids) - 1)];
            $firstColid = $colids[0];
            foreach ($columns as $colid => $column) {
                if ($colid === $firstColid) {
                    $colClass = 'first';
                } else if ($colid === $lastColid) {
                    $colClass = 'last';
                } else {
                    $colClass = 'middle';
                }

                echo '<th align="'.$column['align'].'" class="'.$settings['widget']['type'].'-'.$colClass.'Col">'.$column['name'].'</th>';
            }

            echo '</tr></thead>';
        }//end if

        echo '<tbody>';

        GUITable::paintRows($settings['rows'], $columns, $settings);

        echo '</tbody></table>';

    }//end paint()


    /**
     * Paint the rows of the table.
     *
     * @param array $rows     An array of arrays that contain row data.
     * @param array $columns  An array of column names and information.
     * @param array $settings Settings for the widget.
     *
     * @return void
     */
    public static function paintRows(array $rows, array $columns, array $settings)
    {
        $id         = $settings['widget']['id'];
        $class      = 'GUITable';
        $colids     = array_keys($columns);
        $lastColid  = $colids[(count($colids) - 1)];
        $firstColid = $colids[0];
        $rowCount   = count($rows);
        $rowClass   = ' firstRow';

        $idx = 1;
        foreach ($rows as $rowid => $row) {
            if ($idx === $rowCount) {
                $rowClass = ' lastRow';
            } else if ($idx !== 1) {
                $rowClass = ' midRow';
            }

            echo '<tr rowid="'.$rowid.'" class="';

            if (($idx % 2) === 0) {
                echo 'rowEven';
            } else {
                echo 'rowOdd';
            }

            echo '">';

            $colIdx = 1;
            foreach ($row as $colid => $col) {
                $colClass = 'middleCol';
                if ($colid === $firstColid) {
                    $colClass = 'firstCol';
                } else if ($colid === $lastColid) {
                    $colClass = 'lastCol';
                }

                if (($colIdx % 2) === 0) {
                    $colClass .= ' colEven';
                } else {
                    $colClass .= ' colOdd';
                }

                echo '<td colid="'.$id.'-'.$rowid.'-'.$colid.'" align="'.$columns[$colid]['align'].'" class="'.$class.'-'.$colClass.$rowClass.'">';
                echo $col;
                echo '</td>';

                $colIdx++;
            }//end foreach

            echo '</tr>';

            $idx++;
        }//end foreach

    }//end paintRows()


}//end class

?>