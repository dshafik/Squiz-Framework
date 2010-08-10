<?php
/**
 * Class for the conversion of MSSQL specific SQL structures.
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
 * @subpackage DAL
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(__FILE__).'/DALConverter.inc';

/**
 * DALMysqlConverter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 *
 */
class DALMysqlConverter extends DALConverter
{

    /**
     * Name of the table that is being converted.
     *
     * @var   string $tableName
     */
    protected $tableName = '';

    /**
     * Name of the database.
     *
     * @var   string $name Database name.
     */
    protected $name = 'MySQL';

    /**
     * List of data types for MySQL.
     *
     * @var   $dataTypes List of sql constructs.
     */
    protected $dataTypes = array(
                            'char'                => 'CHAR',
                            'varchar'             => 'VARCHAR',
                            'clob'                => 'LONGTEXT',
                            'blob'                => 'LONGBLOB',
                            'integer'             => 'INT',
                            'smallint'            => 'SMALLINT',
                            'numeric'             => 'NUMERIC',
                            'float'               => 'FLOAT',
                            'real'                => 'REAL',
                            'double_precision'    => 'DOUBLE PRECISION',
                            'boolean'             => 'VARCHAR(10)',
                            'date'                => 'DATE',
                            'time'                => 'TIME',
                            'timestamp'           => 'DATETIME',
                            'time with time zone' => 'TIME',
                           );


    /**
     * Constructor.
     *
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Converts create statement to SQL string.
     *
     * @param array $table CREATE array.
     *
     * @return string
     */
    public function convertCreateTable(array $table)
    {
        $this->tableName = $table['table'];

        $sql  = $this->convertCreateHeader($table);
        $sql .= "\n".$this->convertCreateColumns($table);
        $sql .= ')TYPE=InnoDB;';

        $indexes = $this->convertCreateIndexes($table);
        if ($indexes !== '') {
            $sql .= "\n".$indexes;
        }

        $constraints = $this->convertAlterQueryAddConstraint(
            $table['CONSTRAINTS'],
            TRUE
        );
        $sql        .= "\n".$constraints;

        if (isset($table['SEQUENCES']) === TRUE) {
            $sql .= "\n".$this->convertCreateSequences($table['SEQUENCES']);
        }

        return $sql;

    }//end convertCreateTable()


    /**
     * Converts ALTER TABLE ADD CONSTRAINT statement to its SQL format.
     *
     * Returns empty string if no constraints found.
     *
     * @param array   $constraints List of constraints.
     * @param boolean $incHeader   If true ALTER query header will be added.
     *
     * @return string
     */
    protected function convertAlterQueryAddConstraint(
        array $constraints,
        $incHeader=FALSE
    ) {
        $sql = '';
        if (empty($constraints) === FALSE) {
            $convertedCons = $this->convertConstraints($constraints, TRUE);
            $cons          = array();
            foreach ($convertedCons as $constraint) {
                $sql = '';
                if ($incHeader === TRUE) {
                    $sql .= $this->convertAlterQueryHeader($this->tableName);
                }

                $sql .= 'ADD '.$constraint;

                $cons[] = $sql;
            }

            $sql = implode(";\n", $cons).";\n";
        }

        return $sql;

    }//end convertAlterQueryAddConstraint()


    /**
     * Converts a single CREATE SEQUENCE statament.
     *
     * @param array $sequence Info array of the sequence.
     *
     * @return string
     */
    protected function convertSingleSequence(array $sequence)
    {
        $sql = 'CREATE TABLE '.$sequence['name'].' (
                    id INT AUTO_INCREMENT NOT NULL PRIMARY KEY)';

        return $sql;

    }//end convertSingleSequence()


    /**
     * Handles the SeqNextVal() function call.
     *
     * This function should be used to get the next value of a sequence.
     *
     * @param array $seqName Name of the sequence.
     *
     * @return string
     */
    protected function handleFunctionSeqNextVal(array $seqName)
    {
        $sql = 'INSERT INTO '.$seqName[0].' VALUES(0)';
        return $sql;

    }//end handleFunctionSeqNextVal()


    /**
     * Handles the seqNextVal() function call.
     *
     * This function should be used to get the current value of a sequence.
     * Note: SeqNextVal must be called before this function.
     *
     * @param array $seqName Name of the sequence.
     *
     * @return string
     */
    protected function handleFunctionSeqCurrVal(array $seqName)
    {
        $sql = 'SELECT LAST_INSERT_ID()';
        return $sql;

    }//end handleFunctionSeqCurrVal()


    /**
     * Converts DROP SEQUENCE statement.
     *
     * MySQL does not support sequences, tables created instead of sequences.
     *
     * @param string $sequenceName Name of the sequence to drop from the db.
     *
     * @return string
     */
    public function convertDropSequence($sequenceName)
    {
        $sql = $this->convertDropTable($sequenceName);
        return $sql;

    }//end convertDropSequence()


    /**
     * Converts DROP TABLE statement.
     *
     * @param string  $tableName Name of the table to drop from the db.
     * @param boolean $cascade   If TRUE, deletions will be cascaded to
     *                           dependent objects.
     *
     * @return string
     */
    public function convertDropTable($tableName, $cascade=FALSE)
    {
        $sql = 'DROP TABLE IF EXISTS '.$tableName;
        if ($cascade === TRUE) {
            $sql .= ' CASCADE';
        }

        return $sql;

    }//end convertDropTable()


    /**
     * Handles sequence exists.
     *
     * @param (array|string) $sequenceName Sequence name.
     *
     * @return string
     */
    public function handleFunctionSequenceExists($sequenceName)
    {
        $str = $this->handleFunctionTableExists($sequenceName);
        return $str;

    }//end handleFunctionSequenceExists()


    /**
     * Handles the CONCAT() function call.
     *
     * @param array $args The arguments to the concat function.
     *
     * @return string
     */
    protected function handleFunctionConcat(array $args)
    {
        $sql = 'CONCAT('.implode(', ', $args).')';
        return $sql;

    }//end handleFunctionConcat()


    /**
     * Converts a single CALL query.
     *
     * @param array $callQuery The structure of the call query to convert.
     *
     * @return string
     */
    protected function convertCallQuery(array $callQuery)
    {
        $sql  = 'SELECT ';
        $sql .= $this->convertSingleFunction($callQuery);
        return $sql;

    }//end convertCallQuery()


}//end class

?>