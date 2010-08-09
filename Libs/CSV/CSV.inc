<?php
/**
 * CSV Lib.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * A library class used to create and parse CSV files.
 *
 */
class CSV
{

    /**
     * Name of this CSV file.
     *
     * @var   string
     */
    private $_filename = '';

    /**
     * The seperator that separates data in the CSV file.
     *
     * This is normally a comma or a tab character
     *
     * @var   string
     */
    private $_deliminator = ',';

    /**
     * An array of CSV line values.
     *
     * @var   array
     */
    private $_values = array();


    /**
     * Constructor.
     *
     * @param string $filename The name of this CSV file.
     *
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
     * @return void
     */
    public function setValues(array $values)
    {
        $this->_values = $values;

    }//end setValues()


    /**
     * Export the current values to screen.
     *
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