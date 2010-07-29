<?php
/**
 * Actions for the GUI Table System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Button
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Table Actions Class.
 *
 * @since 4.0.0
 */
class GUITableActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        $tableClass = '%class%';

        include_once 'Libs/Util/Util.inc';
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

        $content = '<table class="'.$tableClass.'" id="%id%" cellspacing="0" cellpadding="0">';

        $columns = $settings['columns'];
        if (isset($settings['hideHeader']) === FALSE
            || $settings['hideHeader'] !== 'true'
        ) {
            $content .= '<thead>';
            $content .= '<tr>';

            $lastColid = (count($columns) - 1);
            foreach ($columns as $colid => $column) {
                $colClass = 'middle';
                if ($colid === 0) {
                    $colClass = 'first';
                } else if ($colid === $lastColid) {
                    $colClass = 'last';
                }

                $content .= '<th align="'.$column['align'].'" class="%class%-'.$colClass.'Col">&lt;?php echo _('.var_export($column['name'], TRUE).'); ?&gt;</th>';
            }

            $content .= '</tr>';
            $content .= '</thead>';
        }//end if

        $content .= '<tbody>';

        // Load the table rows.
        list($providerSystem, $providerMethod) = explode('::', $settings['dataProvider']);
        $content .= '<?'.'php ';
        $content .= 'Channels::includeSystem(\'GUITable\');';
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
        $content .= '$columns = '.var_export($columns, TRUE).';';
        $content .= '$data = '.$settings['dataProvider'].'();';
        $content .= '$settings = '.var_export($settings, TRUE).';';
        $content .= 'GUITable::paintRows($data, $columns, $settings);';
        $content .= ' ?'.'>';

        $content .= '</tbody>';
        $content .= '</table>';

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Paint the rows of the table.
     *
     * @param array $rows     An array of arrays that contain row data.
     * @param array $columns  An array of column names and information.
     * @param array $settings Settings for the widget.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintRows(array $rows, array $columns, array $settings)
    {
        $id         = $settings['id'];
        $class      = 'GUITable';
        $colids     = array_keys($columns);
        $lastColid  = $colids[(count($colids) - 1)];
        $firstColid = $colids[0];
        $rowCount   = count($rows);
        $rowClass   = ' firstRow';

        foreach ($rows as $idx => $row) {
            if ($idx === ($rowCount - 1)) {
                $rowClass = ' lastRow';
            } else if ($idx !== 0) {
                $rowClass = ' midRow';
            }

            echo '<tr class="';

            if ((($idx + 1) % 2) === 0) {
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

                if ((($colIdx + 1) % 2) === 0) {
                    $colClass .= ' colEven';
                } else {
                    $colClass .= ' colOdd';
                }

                echo '<td align="'.$columns[$colid]['align'].'" class="'.$class.'-'.$colClass.$rowClass.'">'.$col.'</td>';

                $colIdx++;
            }//end foreach

            echo '</tr>';
        }//end foreach

    }//end paintRows()


}//end class

?>