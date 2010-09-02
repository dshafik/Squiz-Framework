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
     * TRUE hides headers.
     *
     * @var boolean
     */
    protected static $hideHeader = FALSE;

    /**
     * If TRUE then the remove row icon will be shown at the end of the row.
     *
     * @var boolean
     */
    protected static $allowDelete = FALSE;

    /**
     * Alternate row or column colours.
     *
     * Valid values are: rows, cols.
     *
     * @var boolean
     */
    protected static $alternate;

    /**
     * Channel to call to generate new row for the table.
     *
     * Array must specify a valid API method:
     * array(
     *  'system' => 'System',
     *  'action' => 'action',
     * ).
     *
     * @var array
     */
    protected static $rowGenerator = NULL;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     * @throws ChannelException If no table columns specified.
     */
    public static function paint(array $settings)
    {
        $settings['rows'] = Util::getArrayIndex($settings, 'rows', array());
        $tableClass       = $settings['widget']['type'];

        include_once 'Libs/Util/Util.inc';
        $columns = Util::getArrayIndex($settings, 'columns', array());
        if (empty($columns) === TRUE) {
            throw new ChannelException(_('Table widget must have at least one column'));
        }

        if (Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete) === TRUE) {
            $columns['_remove'] = array(
                                   'name'  => '',
                                   'align' => 'right',
                                  );

            // Update the settings array.
            $settings['columns'] = $columns;
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
            || $settings['hideHeader'] !== TRUE
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

        return $settings;

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

            if (Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete) === TRUE) {
                if (isset($row['_remove']) === FALSE) {
                    $row['_remove'] = '&nbsp;<div class="GUI-delete"><span class="GUI-deleteIcon"/></div>';
                }
            }

            $colIdx = 1;
            foreach ($colids as $colid) {
                $col = Util::getArrayIndex($row, $colid, '');

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
                echo '<div class="'.$class.'-colWrapper">';
                echo $col;
                echo '<div class="'.$class.'-colRemovedOverlay"></div>';
                echo '</div></td>';

                $colIdx++;
            }//end foreach

            echo '</tr>';

            $idx++;
        }//end foreach

    }//end paintRows()


    /**
     * Generated a new table row using specified data and channel.
     *
     * @param array $rowsData The row data to send to the specified channel function.
     * @param array $channel  The channel to call which generates the column contents
     *                        using the given rowData.
     * @param array $settings The widget settings.
     *
     * @return void
     * @throws ChannelException If invalid API channel is specified.
     *
     * @api read
     */
    public static function generateRows(array $rowsData, array $channel, array $settings)
    {
        // Check if channel is callable.
        Channels::includeSystem('API');
        if (API::getAPI($channel['system'], $channel['action']) === FALSE) {
            throw new ChannelException(_('Specified channel is invalid.'));
        }

        // This is an API request therefore we need to set the current template, it
        // should be the same as the table's template.
        Channels::includeSystem('GUI');
        $template = Util::getArrayIndex($settings, 'template', array());
        GUI::setCurrentTemplateKey(implode('::', $template));

        Channels::includeSystem($channel['system']);
        $columnNames = Util::getArrayIndex($settings, 'columns', array());

        $rows = array();
        foreach ($rowsData as $rowid => $columns) {
            $rows[$rowid] = call_user_func(array_values($channel), $rowsData[$rowid]);
        }

        GUITable::paintRows($rows, $columnNames, $settings);

    }//end generateRows()


}//end class

?>