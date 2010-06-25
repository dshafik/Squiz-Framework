<?php
/**
 * CSV Lib.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * A library class used to create and parse CSV files.
 *
 * @since 4.0.0
 */
class CSV
{

    /**
     * Name of this CSV file.
     *
     * @var   string
     * @since 4.0.0
     */
    private $_filename = '';

    /**
     * The seperator that separates data in the CSV file.
     *
     * This is normally a comma or a tab character
     *
     * @var   string
     * @since 4.0.0
     */
    private $_deliminator = ',';

    /**
     * An array of CSV line values.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_values = array();


    /**
     * Constructor.
     *
     * @param string $filename The name of this CSV file.
     *
     * @since 4.0.0
     */
    public function __construct($filename='file.csv')
    {
        $this->_filename = $filename;

    }//end __construct()


    /**
     * Sends the PHP mime header.
     *
     * @param string $header Any custom headers that need to be set.
     *
     * @since  4.0.0
     * @return void
     */
    public function sendHeader($header='')
    {
        if ($header !== '') {
            header($header);
        } else {
            header('Content-Type: application/csv');
            header('Content-Disposition: attachment; filename='.$this->_filename.';');
        }

    }//end sendHeader()


    /**
     * Sets the deliminator for the CSV file.
     *
     * @param string $deliminator The deliminator character.
     *
     * @since  4.0.0
     * @return void
     */
    public function setDeliminator($deliminator)
    {
        $this->_deliminator = $deliminator;

    }//end setDeliminator()


    /**
     * Set the values for the CSV file.
     *
     * @param array $values A multi-dimensional array of line values.
     *
     * @since  4.0.0
     * @return void
     */
    public function setValues(array $values)
    {
        $this->_values = $values;

    }//end setValues()


    /**
     * Export the current values to screen.
     *
     * @since  4.0.0
     * @return void
     */
    public function export()
    {
        $this->sendHeader();

        foreach ($this->_values as $value) {
            $line = $this->_formatLine($value, $this->_deliminator);
            echo $line."\n";
        }

    }//end export()


    /**
     * Format a CSV line nicely.
     *
     * @param array  $array       An array of data values.
     * @param string $deliminator The deliminator to use between data values.
     *
     * @since  4.0.0
     * @return string
     */
    private function _formatLine(array $array, $deliminator)
    {
        $ret = array();
        foreach ($array as $val) {
            if (is_array($val) === TRUE) {
                $ret[] = '"'.$this->_formatLine($val, $deliminator).'"';
            } else {
                if (ereg('['.$deliminator.'\"\n\r]', $val) !== FALSE) {
                    $val = '"'.str_replace('"', '""', $val).'"';
                } else {
                    $val = '"'.$val.'"';
                }

                $ret[] = $val;
            }
        }

        $line = implode($deliminator, $ret);
        $line = str_replace('<br />', "\n", $line);
        return $line;

    }//end _formatLine()


}//end class

?>