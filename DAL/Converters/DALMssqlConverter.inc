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
 * DALMssqlConverter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 *
 * @since 4.0.0
 */
class DALMssqlConverter extends DALConverter
{

    /**
     * Name of the table that is being converted.
     *
     * @var   string $name
     * @since 4.0.0
     */
    protected $tableName = '';

    /**
     * Name of the database.
     *
     * @var   string $name Database name.
     * @since 4.0.0
     */
    protected $name = 'MSSQL';

    /**
     * List of data types for SQL Server.
     *
     * @var   $dataTypes List of sql constructs.
     * @since 4.0.0
     */
    protected $dataTypes = array(
                            'char'                => 'NCHAR',
                            'varchar'             => 'NVARCHAR',
                            'clob'                => 'NVARCHAR(4000)',
                            'blob'                => 'VARBINARY',
                            'integer'             => 'INT',
                            'smallint'            => 'SMALLINT',
                            'numeric'             => 'NUMERIC',
                            'float'               => 'FLOAT',
                            'real'                => 'REAL',
                            'double_precision'    => 'FLOAT',
                            'boolean'             => 'CHAR(1)',
                            'date'                => 'DATETIME',
                            'time'                => 'DATETIME',
                            'timestamp'           => 'DATETIME',
                            'time with time zone' => 'DATETIME',
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
     * Indexes are added after creating the table.
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
        $sql .= $this->convertCreateFooter($table);

        if (isset($table['CONSTRAINTS']) === TRUE
            && empty($table['CONSTRAINTS']) === FALSE
        ) {
            $constraints = $this->convertAlterQueryAddConstraint(
                $table['CONSTRAINTS'],
                TRUE
            );
            $sql        .= "\n".$constraints;
        }

        $indexes = $this->convertCreateIndexes($table);
        if ($indexes !== '') {
            $sql .= "\n".$indexes;
        }

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
     * @param boolean $incHeader   If true ALTER query heading will be added.
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

            $sql = implode(";\n", $cons).';';
        }

        return $sql;

    }//end convertAlterQueryAddConstraint()


    /**
     * Converts list of Index to SQL string.
     *
     * @param array $table The table array.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertCreateIndexes(array $table)
    {
        $sql     = '';
        $indexes = array();
        foreach ($table['INDEXES'] as $index) {
            $indexes[] = $this->convertSingleIndex($index, $table['table']);
        }

        if (empty($indexes) === FALSE) {
            $sql .= implode(";\n", $indexes).';';
        }

        return $sql;

    }//end convertCreateIndexes()


    /**
     * Converts single Index to its SQL string.
     *
     * Postgres: INDEX index_name (assetid, linkid).
     *
     * @param array  $idx       Single index array.
     * @param string $tableName Name of the table.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertSingleIndex(array $idx, $tableName)
    {
        $sql  = 'CREATE INDEX '.$idx['name'].' ON '.$tableName.' (';
        $sql .= $this->separateFields($idx['COLUMNS']).')';
        return $sql;

    }//end convertSingleIndex()


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
        // Create a new sequence table and a row.
        $sql = 'CREATE TABLE '.$sequence['name'].' ( id INT NOT NULL);
        INSERT INTO '.$sequence['name'].' VALUES(0);';

        return $sql;

    }//end convertSingleSequence()


    /**
     * Handles SeqNextVal(seqName) function.
     *
     * SewNextVal will increment sequence value and return the new sequence
     * value.
     *
     * @param string $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqNextVal($seqName)
    {
        $sql = 'UPDATE '.$seqName[0].' SET id=id+1';
        return $sql;

    }//end handleFunctionSeqNextVal()


    /**
     * Handles SeqCurrVal(seqName) function.
     *
     * SeqCurrVal will return the current sequence value.
     * Note: SeqNextVal must be called before this function.
     *
     * @param string $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqCurrVal($seqName)
    {
        $sql = 'SELECT TOP 1 id FROM '.$seqName[0].'';
        return $sql;

    }//end handleFunctionSeqCurrVal()


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
     * Handles the CONCAT() function call.
     *
     * @param array $args The arguments to the concat function.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionConcat(array $args)
    {
        $sql = implode(' + ', $args);
        return $sql;

    }//end handleFunctionConcat()


    /**
     * SQL Server needs to use UNION ALL (no hyphen) so return that here.
     *
     * @param string $type The type of the union (UNION or UNION-ALL).
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertUnionDisplayName($type)
    {
        if (strtoupper($type) === 'UNION-ALL') {
            return 'UNION ALL';
        }

        return $type;

    }//end convertUnionDisplayName()


    /**
     * Converts Insert query's values to their SQL format.
     *
     * Converting values require the fields array. Because, fields list and
     * values list must be in the same order.
     *
     * SQLServer throws error if there are brackets () around SELECT statement.
     *
     * @param array $insert Array of the insert query.
     *
     * @since  4.0.0
     * @return string
     * @throws DALConverterException If the INSERT query cannot be converted.
     */
    protected function convertInsertQueryValues(array $insert)
    {
        $sql = '';
        if (isset($insert['VALUES']['SELECT']) === FALSE) {
            $valuesList = array();
            foreach ($insert['FIELDS'] as $field) {
                if (isset($insert['VALUES'][$field]) === TRUE) {
                    $value = $insert['VALUES'][$field];
                } else {
                    $msg  = 'Could not convert INSERT query.';
                    $msg .= ' Field "'.$field.'" not found in values list.';
                    throw new DALConverterException($msg);
                }

                $valuesList[] = $this->convertSingleField($value);
            }

            $sql .= 'VALUES ('.$this->separateFields($valuesList).') ';
        } else {
            // Select statement.
            $sql .= $this->convertSelectQuery($insert['VALUES']);
        }

        return $sql;

    }//end convertInsertQueryValues()


    /**
     * Converts a single CALL query.
     *
     * @param array $callQuery The array structure of the call query.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertCallQuery(array $callQuery)
    {
        $sql  = 'EXEC ';
        $sql .= $this->convertSingleFunction($callQuery, TRUE);
        return $sql;

    }//end convertCallQuery()


    /**
     * Converts a single function to its SQL string.
     *
     * SQL Server does not like the brackets when calling a procedure using EXEC.
     *
     * @param array   $function    Function array.
     * @param boolean $isProcedure Set this to true if this is a procedure.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertSingleFunction(array $function, $isProcedure=FALSE)
    {
        $sql       = '';
        $fnName    = $function['FUNCTION']['function'];
        $fnHandler = 'handleFunction'.$fnName;
        $args      = array();
        foreach ($function['FUNCTION']['ARGS'] as $arg) {
            $args[] = $this->convertSingleField($arg);
        }

        if (method_exists($this, $fnHandler) === FALSE) {
            $sql .= $function['FUNCTION']['function'];
            if ($isProcedure === TRUE) {
                $sql .= ' ';
            } else {
                $sql .= '(';
            }

            $sql .= $this->separateFields($args);

            if ($isProcedure === TRUE) {
                $sql .= ' ';
            } else {
                $sql .= ')';
            }
        } else {
            $sql .= $this->$fnHandler($args);
        }

        if (isset($function['FUNCTION']['alias']) === TRUE) {
            $sql .= ' as '.$function['FUNCTION']['alias'];
        }

        return $sql;

    }//end convertSingleFunction()


    /**
     * Handles table exists.
     *
     * @param (array|string) $tableName Table name argument.
     * @param string         $schema    The schema that the exists function
     *                                  should be called on.
     *
     * @since  4.0.0
     * @return string
     */
    public function handleFunctionTableExists($tableName, $schema=NULL)
    {
        if (is_array($tableName) === TRUE) {
            $tableName = $tableName[0];
        }

        if ($schema === NULL) {
            $schema = '\''.DAL::getDbName().'\'';
        }

        $sql  = 'SELECT count(*) FROM INFORMATION_SCHEMA.tables WHERE TABLE_NAME = ';
        $sql .= $tableName.' AND TABLE_SCHEMA = \'dbo\' AND TABLE_CATALOG = '.$schema;
        return $sql;

    }//end handleFunctionTableExists()


    /**
     * Converts Select's field list to its SQL format.
     *
     * @param array   $fields Array of fields.
     * @param integer $level  Number of levels to indent.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertSelectFieldList(array $fields, $level=1)
    {
        $sql      = '';
        $sqlFlist = '';
        if (isset($this->query['LIMIT']) === TRUE) {
            $offset = $this->query['LIMIT']['offset'];
            if ($offset === 0) {
                $offset = 1;
            }

            if (isset($this->query['ORDER-BY']) === TRUE) {
                $orderBy = $this->query['ORDER-BY'];
                foreach ($orderBy as $key => $field) {
                    if ($key !== 'direction') {
                        if (is_array($field) === TRUE) {
                            $fields[] = $this->convertSingleField($field);
                        } else {
                            $fields[] = $field;
                        }
                    }
                }
            }

            $sql .= '* FROM (SELECT TOP '.$offset;
            $sql .= ' * FROM (';

            $sqlFlist = 'SELECT TOP '.$this->query['LIMIT']['rows'].' ';
        }//end if

        $list = array();
        foreach ($fields as $field) {
            $list[] = $this->convertSingleField($field);
        }

        $list = array_unique($list);

        $sqlFlist .= $this->addSpaces($level).$this->separateFields($list);
        $sql      .= $sqlFlist;

        return $sql;

    }//end convertSelectFieldList()


    /**
     * Converts LIMIT.
     *
     * @param array $limit Single LIMIT clause.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertLimit(array $limit)
    {
        // Reverse order by direction.
        $sql = '';
        if (isset($this->query['ORDER-BY']) === TRUE) {
            $orderBy = $this->query['ORDER-BY'];

            foreach ($orderBy as $key => $field) {
                if (isset($field['table']) === TRUE) {
                    $field['table'] = '';
                }

                $orderBy[$key] = $field;
            }

            for ($i = 0; $i < 2; $i++) {
                if (isset($orderBy['direction']) === TRUE) {
                    if (strtolower($orderBy['direction']) === 'desc') {
                        $orderBy['direction'] = '';
                    } else {
                        $orderBy['direction'] = 'desc';
                    }
                }

                $sql .= ') as limCl '.$this->convertOrderBy($orderBy);
            }
        }//end if

        return $sql;

    }//end convertLimit()


}//end class

?>