<?php
/**
 * Class for the conversion of MSSQL specific SQL structures.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
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
 * @since 4.0.0
 */
class DALMysqlConverter extends DALConverter
{

    /**
     * Name of the table that is being converted.
     *
     * @var   string $tableName
     * @since 4.0.0
     */
    protected $tableName = '';

    /**
     * Name of the database.
     *
     * @var   string $name Database name.
     * @since 4.0.0
     */
    protected $name = 'MySQL';

    /**
     * List of data types for MySQL.
     *
     * @var   $dataTypes List of sql constructs.
     * @since 4.0.0
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
     * @since 4.0.0
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Converts create statement to SQL string.
     *
     * @param array $table CREATE array.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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