<?php
/**
 * Class for the conversion of DB/2 specific SQL structures.
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
 * DALDb2Converter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 *
 * @since 4.0.0
 */
class DALDb2Converter extends DALConverter
{

    /**
     * Name of the database.
     *
     * @var   string $name Database name.
     * @since 4.0.0
     */
    protected $name = 'DB2';

    /**
     * List of data types for DB2.
     *
     * @var   $dataTypes List of sql constructs.
     * @since 4.0.0
     */
    protected $dataTypes = array(
                            'char'                => 'CHAR',
                            'varchar'             => 'VARCHAR',
                            'clob'                => 'CLOB',
                            'blob'                => 'BLOB',
                            'integer'             => 'INT',
                            'smallint'            => 'SMALLINT',
                            'numeric'             => 'NUMERIC',
                            'float'               => 'FLOAT',
                            'real'                => 'REAL',
                            'double_precision'    => 'DOUBLE PRECISION',
                            'boolean'             => 'CHAR',
                            'date'                => 'DATE',
                            'time'                => 'TIME',
                            'timestamp'           => 'TIMESTAMP',
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
     * Handles the seqNextVal() function call.
     *
     * This function should be used to get the next value of a sequence.
     *
     * @param string $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqNextVal($seqName)
    {
        $sql = 'SELECT next value for '.$seqName[0].' FROM SYSIBM.SYSDUMMY1';
        return $sql;

    }//end handleFunctionSeqNextVal()


    /**
     * Handles the seqCurrVal() function call.
     *
     * This function should be used to get the next value of a sequence.
     *
     * @param string $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqCurrVal($seqName)
    {
        $sql = 'SELECT prevval for '.$seqName[0].' FROM SYSIBM.SYSDUMMY1';
        return $sql;

    }//end handleFunctionSeqCurrVal()


    /**
     * Handles table exists.
     *
     * @param array|string $tableName Table name argument.
     * @param string       $schema    The schema to check the table for.
     *
     * @since  4.0.0
     * @return string
     */
    public function handleFunctionTableExists($tableName, $schema=NULL)
    {
        // TODO: $schema argument is not used here right now. Check the parent class.
        if (is_array($tableName) === TRUE) {
            $tableName = $tableName[0];
        }

        $sql = 'SELECT count(*) FROM syscat.tables WHERE tabname = UPPER(CAST('.$tableName.' AS VARCHAR(50)))';
        return $sql;

    }//end handleFunctionTableExists()


    /**
     * Converts sequence exists.
     *
     * @param (array|string) $sequenceName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    public function handleFunctionSequenceExists($sequenceName)
    {
        if (is_array($sequenceName) === TRUE) {
            $sequenceName = $sequenceName[0];
        }

        $sql = 'SELECT count(*) FROM syscat.sequences WHERE seqname = '.$sequenceName;
        return $sql;

    }//end handleFunctionSequenceExists()


    /**
     * Returns the type of the union.
     *
     * @param array $union Array of a union/union-all.
     *
     * @since  4.0.0
     * @return string
     */
    protected function getUnionType(array $union)
    {
        $type = '';
        if (isset($union['UNION']) === TRUE) {
            $type = 'UNION';
        } else if (isset($union['UNION-ALL']) === TRUE) {
            $type = 'UNION-ALL';
        }

        return $type;

    }//end getUnionType()


    /**
     * DB2 needs to use UNION ALL (no hyphen) so return that here.
     *
     * @param string $type The type of the union (UNION or UNION-ALL).
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertUnionDisplayName($type)
    {
        if ($type === 'UNION-ALL') {
            return 'UNION ALL';
        }

        return $type;

    }//end convertUnionDisplayName()


    /**
     * Handles the CONCAT() function call.
     *
     * @param array $args Method arguments.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionConcat(array $args)
    {
        $sql = implode(' || ', $args);
        return $sql;

    }//end handleFunctionConcat()


    /**
     * Handles LIKE conditions.
     *
     * @param array $condition The condition information.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertLikeCondition(array $condition)
    {
        $compare = $this->convertSingleField($condition['compare']);
        $to      = $this->convertSingleField($condition['to']);
        $sql     = 'LOCATE('.str_replace('%', '', $to);
        $sql    .= ', '.str_replace('%', '', $compare).') <> 0';

        return $sql;

    }//end convertLikeCondition()


    /**
     * Handles the BINDCAST() function call.
     *
     * BINDCAST is used to wrap bind vars.
     * For DB2 bind vars must be cast to a valid data type if they are used in a
     * SELECT or as function argument.
     *
     * @param array $args Method arguments.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionBindcast(array $args)
    {
        $sql = $this->handleFunctionCast($args);
        return $sql;

    }//end handleFunctionBindcast()


}//end class

?>