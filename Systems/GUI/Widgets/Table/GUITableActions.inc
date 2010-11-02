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
     * Expected array structure is:
     * array(
     *  'rowid1' => array(
     *               'colid1' => 'value',
     *               'colid2' => 'value',
     *               'colid3' => 'value',
     *              ),
     *  'rowid2' => array(
     *               'colid1' => 'value',
     *               'colid2' => array(
     *                            'colspan'    => 2,
     *                            'conContent' => 'value',
     *                           ),
     *              ),
     *  'rowid3' => array(
     *               'colid1' => 'value',
     *               'colid2' => 'value',
     *               'colid3' => 'value',
     *              ),
     * ).
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
     * Custom class for the delete column.
     *
     * @var string
     */
    protected static $customDeleteClass = '';

    /**
     * If TRUE then the rows will be numbered at the start of each row.
     *
     * @var boolean
     */
    protected static $rowNumbering = FALSE;

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
     * Message that is displayed when there are no items in the table.
     *
     * @var string
     */
    protected static $noItemsMsg = '';


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
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

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
                                   'width' => '1px',
                                  );

            // Update the settings array.
            $settings['columns'] = $columns;
        }

        if (Util::getArrayIndex($settings, 'rowNumbering', self::$rowNumbering) === TRUE) {
            $columns            = array_reverse($columns, TRUE);
            $columns['_number'] = array(
                                   'name'  => '',
                                   'align' => 'left',
                                   'width' => '10px',
                                  );
            $columns            = array_reverse($columns, TRUE);

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

        $noItemsMsg = Util::getArrayIndex($settings, 'noItemsMsg', self::$noItemsMsg);
        if (empty($settings['rows']) === TRUE && empty($noItemsMsg) === FALSE) {
            $tableClass .= ' noItems';
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

                echo '<th class="'.$settings['widget']['type'].'-'.$colClass.'Col th-'.$colid.'" ';

                echo 'style="text-align: '.$column['align'].';';
                if (isset($column['width']) === TRUE) {
                    echo ' width: '.$column['width'].';';
                }

                echo '">'.$column['name'].'</th>';
            }

            echo '</tr></thead>';
        }//end if

        echo '<tbody>';

        GUITable::paintRows($settings['rows'], $columns, $settings);

        echo '</tbody></table>';

        if (empty($settings['rows']) === TRUE && empty($noItemsMsg) === FALSE) {
            echo '<div id="'.$id.'-noItemsMsg" class="'.$class.'-noItemsMsg noItems">';
            echo $noItemsMsg;
            echo '</div>';
        }

        return $settings;

    }//end paint()


    /**
     * Paint the rows of the table.
     *
     * @param array   $rows         An array of arrays that contain row data.
     * @param array   $columns      An array of column names and information.
     * @param array   $settings     Settings for the widget.
     * @param boolean $actualRowids If TRUE, then the rowids passed are correct.
     *                              (as opposed to just being an array starting at 0).
     *
     * @return void
     */
    public static function paintRows(array $rows, array $columns, array $settings, $actualRowids=FALSE)
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
            // If we have a "rowContent" field, then this will be the contents, and
            // the rest will be options.
            $customRowClass = '';
            if (array_key_exists('rowContent', $row) === TRUE) {
                $rowOptions = $row;
                $row        = $row['rowContent'];

                if (array_key_exists('rowClass', $rowOptions) === TRUE) {
                    $customRowClass = ' '.$rowOptions['rowClass'];
                }
            }

            // Set the class id.
            $classid = $rowid;
            if (strpos($rowid, ':') !== FALSE) {
                list($paintid, $classid) = explode(':', $rowid);
            }

            if ($idx === $rowCount) {
                $rowClass = ' lastRow';
            } else if ($idx !== 1) {
                $rowClass = ' midRow';
            }

            echo '<tr rowid="'.$classid.'" class="';

            if (($idx % 2) === 0) {
                echo 'rowEven';
            } else {
                echo 'rowOdd';
            }

            echo ' row-'.$classid;
            echo $customRowClass;
            echo '">';

            if (Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete) === TRUE) {
                if (isset($row['_remove']) === FALSE) {
                    $row['_remove'] = '&nbsp;<div class="GUI-delete"><span class="GUI-deleteIcon"/></div>';
                }
            }

            if (Util::getArrayIndex($settings, 'rowNumbering', self::$rowNumbering) === TRUE) {
                if (isset($row['_number']) === FALSE) {
                    if ($actualRowids === TRUE) {
                        if (strpos($rowid, ':') === FALSE) {
                            $paintid = $rowid;
                        } else {
                            list($paintid, $rowid) = explode(':', $rowid);
                        }

                        $rowNumber = $paintid;
                    } else {
                        $rowNumber = $idx;
                    }

                    $row['_number'] = '<div class="GUI-rowNumber" style="vertical-align:middle">'.$rowNumber.'.</div>';
                }
            }

            $colIdx      = 1;
            $colspanSkip = 0;
            foreach ($colids as $colid) {
                if ($colspanSkip > 0) {
                    $colspanSkip--;
                    $colIdx++;
                    continue;
                }

                $col = Util::getArrayIndex($row, $colid, '');

                $colspan         = '';
                $customCellClass = '';
                if (is_array($col) === TRUE) {
                    if (array_key_exists('colContent', $col) === TRUE) {
                        $colOptions = $col;
                        $col        = $col['colContent'];

                        if (array_key_exists('colspan', $colOptions) === TRUE) {
                            $colspan     = ' colspan="'.$colOptions['colspan'].'"';
                            $colspanSkip = (((int) $colOptions['colspan']) - 1);
                        }

                        if (array_key_exists('cellClass', $colOptions) === TRUE) {
                            $customCellClass = ' '.$colOptions['cellClass'];
                        }
                    }
                }

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

                echo '<td'.$colspan.' colid="'.$id.'-'.$classid.'-'.$colid;
                echo '" class="'.$class.'-'.$colClass.$rowClass.' col-'.$colid.$customCellClass.' ';

                if ($colid === '_remove') {
                    echo $id.'-_remove ';
                    echo Util::getArrayIndex($settings, 'customDeleteClass', self::$customDeleteClass);
                }

                echo '" ';
                echo 'style="text-align: '.$columns[$colid]['align'].';';
                if (isset($columns[$colid]['width']) === TRUE) {
                    echo ' width: '.$columns[$colid]['width'].';';
                }

                echo '">';
                echo '<div class="'.$class.'-colWrapper">';
                echo $col;
                if (Util::getArrayIndex($settings, 'allowDelete', self::$allowDelete) === TRUE) {
                    echo '<div class="'.$class.'-colRemovedOverlay"></div>';
                }

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
     * @api            read
     * @api-permission public
     */
    public static function generateRows(array $rowsData, array $channel, array $settings)
    {
        // Check if channel is callable.
        Channels::includeSystem('API');
        $api = API::getAPI($channel['system'], $channel['action']);
        if ($api === FALSE) {
            throw new ChannelException(_('Specified channel is invalid'));
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
            $rowContent = API::executeRequest($channel['system'], $channel['action'], array($rowsData[$rowid]), $api['options']);
            if ($rowContent === FALSE) {
                continue;
            }

            $rows[$rowid] = $rowContent;
        }

        GUITable::paintRows($rows, $columnNames, $settings, TRUE);

    }//end generateRows()


    /**
     * Generated new table rows in bulk using the specified data provider.
     *
     * This differs from generateRows() in the following way:
     * - generateRows() expects an array of rowids, and the data provider channel
     *   must return ONE row for EACH rowid passed, which are then painted and sent.
     * - generateBulkRows() is for when the rowids are not known in advance, but you
     *   have the arguments to send to the data provider in question (whether passed,
     *   or gathered through refiners). The data provider then returns ZERO OR MORE
     *   rows, which are then painted and sent. The expected result is the same as
     *   is what is expected from a data provider in the "rows" setting in a
     *   template.
     *
     * The data provider specified in $channel must be API-enabled.
     *
     * The only required parameters for this API call are the channel and widget
     * settings. However, the data provider's required parameters must also be
     * passed, or they need to be able to be gathered from other sources (such as
     * refining API::getActionParameters). This is automatically set up through the
     * JavaScript call that calls this. If the required parameters for the data
     * provider channel are not found, a 400 (Bad Request) error will be sent.
     *
     * @param array $channel  The channel to call which generates the column
     *                        contents using the given params.
     * @param array $settings The widget settings.
     *
     * @return void
     * @throws ChannelException If invalid API channel is specified.
     *
     * @api            read
     * @api-permission public
     */
    public static function generateBulkRows(array $channel, array $settings)
    {
        // Check if channel is callable.
        Channels::includeSystem('API');
        $api = API::getAPI($channel['system'], $channel['action']);
        if ($api === FALSE) {
            throw new ChannelException(_('Specified channel is invalid'));
        }

        // This is an API request therefore we need to set the current template, it
        // should be the same as the table's template.
        Channels::includeSystem('GUI');
        $template = Util::getArrayIndex($settings, 'template', array());
        GUI::setCurrentTemplateKey(implode('::', $template));

        Channels::includeSystem($channel['system']);
        $columnNames = Util::getArrayIndex($settings, 'columns', array());

        // Gather any parameters from the request, including those that may need to
        // be populated from elsewhere.
        $channelParams = API::getActionParameters(
            'post',
            $api['system'],
            $api['action'],
            $api['arguments']
        );

        if ($channelParams === FALSE) {
            // Insufficient parameters were provided.
            API::sendHeader(400);
            $responseContent['exception'] = 'Provided parameters are not sufficient';
            API::sendResponse($responseContent);
            exit;
        }

        $rows = API::executeRequest($channel['system'], $channel['action'], $channelParams, $api['options']);

        GUITable::paintRows($rows, $columnNames, $settings, TRUE);

    }//end generateBulkRows()


}//end class

?>
