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
        $content  = '<table class="%class%" id="%id%" cellspacing="0" cellpadding="0">';
        $content .= '<thead>';
        $content .= '<tr>';

        $lastColid = (count($settings['columns']) - 1);
        foreach ($settings['columns'] as $colid => $column) {
            $colClass = 'middle';
            if ($colid === 0) {
                $colClass = 'first';
            } else if ($colid === $lastColid) {
                $colClass = 'last';
            }

            $content .= '<th align="'.$column['align'].'" class="%class%-'.$colClass.'col">&lt;?php echo _(\''.$column['name'].'\'); ?&gt;</th>';
        }

        $content .= '</tr>';
        $content .= '</thead>';
        $content .= '<tbody>';

        // Load the table rows.
        list($providerSystem, $providerMethod) = explode('::', $settings['dataProvider']);
        $content .= '<?php ';
        $content .= 'Channels::includeSystem(\'GUITable\');';
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
        $content .= '$columns = '.var_export($settings['columns'], TRUE).';';
        $content .= '$data = '.$settings['dataProvider'].'();';
        $content .= 'GUITable::paintRows($data, $columns, \'%id%\', \'%class%\');';
        $content .= ' ?>';

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
     * @param array  $rows    An array of arrays that contain row data.
     * @param array  $columns An array of column names and information.
     * @param string $id      The ID used by the table.
     * @param string $class   The class used by the table.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintRows(array $rows, array $columns, $id, $class)
    {
        $lastColid = (count($columns) - 1);

        foreach ($rows as $row) {
            echo '<tr>';
            foreach ($row as $colid => $col) {
                $colClass = 'middle';
                if ($colid === 0) {
                    $colClass = 'first';
                } else if ($colid === $lastColid) {
                    $colClass = 'last';
                }

                echo '<td align="'.$columns[$colid]['align'].'" class="'.$class.'-'.$colClass.'col">'.$col.'</td>';
            }

            echo '</tr>';
        }

    }//end paintRows()


}//end class

?>