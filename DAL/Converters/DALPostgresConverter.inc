<?php
/**
 * Class for the conversion of Oracle specific SQL structures.
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
 * DALPostgresConverter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 *
 * @since 4.0.0
 */
class DALPostgresConverter extends DALConverter
{

    /**
     * The name of the database type that this converter uses.
     *
     * @var   $name Specifies that this converter is for 'Postgres'.
     * @since 4.0.0
     */
    protected $name = 'Postgres';


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
        $sql  = $this->convertCreateHeader($table);
        $sql .= "\n".$this->convertCreateColumns($table).',';
        $sql .= "\n".$this->convertConstraints($table['CONSTRAINTS']);
        $sql .= $this->convertCreateFooter($table);

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

        $sql .= implode(";\n", $indexes).';';

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
        $sql  = 'CREATE SEQUENCE ';
        $sql .= $sequence['name'].' ';

        if (isset($sequence['increment']) === TRUE) {
            $inc = $sequence['increment'];
            if (empty($inc) === FALSE) {
                $sql .= 'INCREMENT '.$inc.' ';
            }
        }

        if (isset($sequence['min']) === TRUE) {
            $min = $sequence['min'];
            if (empty($min) === FALSE) {
                $sql .= 'MINVALUE '.$min.' ';
            }
        }

        if (isset($sequence['max']) === TRUE) {
            $max = $sequence['max'];
            if (empty($max) === FALSE) {
                $sql .= 'MAXVALUE '.$max.' ';
            }
        }

        return $sql;

    }//end convertSingleSequence()


    /**
     * Handles the seqNextVal() function call.
     *
     * This function should be used to get the next value of a sequence.
     * Note: seqName is a keyword and will not be quoted by PDO.
     *
     * @param array $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqNextVal(array $seqName)
    {
        $sql = 'SELECT nextVal(\''.$seqName[0].'\')';
        return $sql;

    }//end handleFunctionSeqNextVal()


    /**
     * Handles the seqCurrVal() function call.
     *
     * This function should be used to get the current value of a sequence.
     * Note: seqName is a keyword and will not be quoted by PDO.
     *
     * @param array $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqCurrVal(array $seqName)
    {
        $sql = 'SELECT currVal(\''.$seqName[0].'\')';
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
        if (is_array($sequenceName) === TRUE) {
            $sequenceName = $sequenceName[0];
        }

        $sql = 'SELECT count(c.relname) FROM pg_catalog.pg_class c, pg_catalog.pg_roles r, pg_catalog.pg_namespace n
                WHERE    r.oid = c.relowner AND n.oid = c.relnamespace AND c.relkind = \'S\'
                AND n.nspname <> \'pg_catalog\'
                AND pg_catalog.pg_table_is_visible(c.oid)
                AND c.relname = '.$sequenceName;

        return $sql;

    }//end handleFunctionSequenceExists()


    /**
     * Handles the CONCAT() function call.
     *
     * @param array $args The arguments of the concat function.
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
     * Converts a single CALL query.
     *
     * @param array $callQuery The structure defining the CALL query.
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


    /**
     * Handles table exists.
     *
     * @param (array|string) $tableName Table name argument.
     * @param string         $schema    The schema to check the table exists in.
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

        $sql = 'SELECT count(*) FROM INFORMATION_SCHEMA.tables WHERE TABLE_NAME = '.$tableName.' AND TABLE_SCHEMA = \'public\' AND TABLE_CATALOG = '.$schema;
        return $sql;

    }//end handleFunctionTableExists()


}//end class

?>