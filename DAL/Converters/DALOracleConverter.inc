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
 * DALOracleConverter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 *
 * @since 4.0.0
 */
class DALOracleConverter extends DALConverter
{

    /**
     * Name of the database.
     *
     * @var   string $name Database name.
     * @since 4.0.0
     */
    protected $name = 'Oracle';

    /**
     * List of data types for Oracle.
     *
     * @var   $dataTypes List of sql constructs.
     * @since 4.0.0
     */
    protected $dataTypes = array(
                            'char'                => 'CHAR',
                            'varchar'             => 'VARCHAR2',
                            'clob'                => 'CLOB',
                            'blob'                => 'BLOB',
                            'integer'             => 'INT',
                            'smallint'            => 'SMALLINT',
                            'numeric'             => 'NUMERIC',
                            'float'               => 'FLOAT',
                            'real'                => 'REAL',
                            'double_precision'    => 'DOUBLE PRECISION',
                            'boolean'             => 'CHAR(1)',
                            'date'                => 'DATE',
                            'time'                => 'TIMESTAMP',
                            'timestamp'           => 'TIMESTAMP',
                            'interval'            => '',
                            'time with time zone' => 'TIMESTAMP',
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
     * Note: Create table may return more than one SQL statement, use
     * split(':', $sql); to get each SQL statament.
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
     * Converts single create column to its SQL string.
     *
     * Single create column consists of column's name, type (size and scale),
     * null/not null, and its default value.
     *
     * For Postgres SQL will look like: assetid VARCHAR2(15) NOT NULL DEFAUL 1.
     *
     * @param array $col Single column in create clause.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertSingleCreateColumn(array $col)
    {
        if (strtolower($col['type']) === 'boolean') {
            if ($col['default'] !== '') {
                if (strtolower($col['default']) === 'false') {
                    $col['default'] = "'0'";
                } else if (strtolower($col['default']) === 'true') {
                    $col['default'] = "'1'";
                }
            }
        }

        $str = parent::convertSingleCreateColumn($col);
        return $str;

    }//end convertSingleCreateColumn()


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
        $sql = 'SELECT '.$seqName[0].'.nextval FROM dual';
        return $sql;

    }//end handleFunctionSeqNextVal()


    /**
     * Handles SeqCurrVal(seqName) function.
     *
     * SeqCurrVal will return the current sequence value.
     *
     * @param string $seqName Name of the sequence.
     *
     * @since  4.0.0
     * @return string
     */
    protected function handleFunctionSeqCurrVal($seqName)
    {
        $sql = 'SELECT '.$seqName[0].'.currval FROM dual';
        return $sql;

    }//end handleFunctionSeqCurrVal()


    /**
     * Handles table exists.
     *
     * @param (array|string) $tableName Table name argument.
     * @param string         $schema    The schema to check the table for.
     *
     * @since  4.0.0
     * @return string
     */
    public function handleFunctionTableExists($tableName, $schema=NULL)
    {
        if (is_array($tableName) === TRUE) {
            $tableName = $tableName[0];
        }

        $sql = 'SELECT count(table_name) FROM user_tables WHERE table_name = UPPER('.$tableName.')';
        return $sql;

    }//end handleFunctionTableExists()


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

        $sql = 'SELECT count(sequence_name) FROM user_sequences WHERE sequence_name = UPPER('.$sequenceName.')';
        return $sql;

    }//end handleFunctionSequenceExists()


    /**
     * Converts a WITH query to CONNECT BY PRIOR/START WITH query.
     *
     * @param array $withQuery Array containing the with query information.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertWithQuery(array $withQuery)
    {
        $firstSelectQuery = $withQuery['AFTER'];

        $sql  = '';
        $sql .= $this->convertSelectQuery($firstSelectQuery, 0, array('where'));

        $secondSelectQuery = $withQuery['UNION-ALL']['SELECTS'][1];

        $sql .= 'CONNECT BY PRIOR ';
        $sql .= $this->convertWhereClause($withQuery['CONNECT-BY-PRIOR'], 0, FALSE);

        $sql .= 'START WITH ';
        if (isset($firstSelectQuery['WHERE']) === TRUE) {
            $sql .= $this->convertWhereClause($firstSelectQuery['WHERE'], 0, FALSE);
        }

        return $sql;

    }//end convertWithQuery()


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
        $sql = 'DROP TABLE '.$tableName;
        if ($cascade === TRUE) {
            $sql .= ' CASCADE CONSTRAINTS';
        }

        return $sql;

    }//end convertDropTable()


    /**
     * Converts Select query to its SQL string.
     *
     * @param array   $sqlArray The select query array.
     * @param integer $level    Number of levels to indent.
     * @param array   $ignore   Tag names that will be ignored while converting
     *                          the select statement. Example: where,join..
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertSelectQuery(
        array $sqlArray,
        $level=0,
        array $ignore=array()
    ) {
        $sql = parent::convertSelectQuery($sqlArray, $level, $ignore);

        // Start with.
        if (isset($sqlArray['START-WITH']) === TRUE) {
            $sql      .= "\n".$this->getConstructName('start-with');
            $startWith = $this->convertWhereClause($sqlArray['START-WITH'], $level);
            $startWith = trim($startWith);
            $startWith = trim($startWith, '()');
            $sql      .= "\n".$startWith;
        }

        // Start with.
        if (isset($sqlArray['CONNECT-BY']) === TRUE) {
            $sql      .= "\n".$this->getConstructName('connect-by');
            $connectBy = $this->convertWhereClause($sqlArray['CONNECT-BY'], $level);
            $connectBy = trim($connectBy);
            $connectBy = trim($connectBy, '()');
            $sql      .= "\n".$connectBy;
        }

        return $sql;

    }//end convertSelectQuery()


    /**
     * Converts given alias string to table alias.
     *
     * @param string $alias Table alias.
     *
     * @since  4.0.0
     * @return array
     */
    protected function convertTableAlias($alias)
    {
        return ' '.$alias;

    }//end convertTableAlias()


    /**
     * Convert list of union that are in side a FROM clause.
     *
     * @param array   $union Array of selects in union/union-all.
     * @param integer $level Number of levels to indent.
     *
     * @since  4.0.0
     * @return string
     */
    protected function convertUnionsInFromClause(array $union, $level=0)
    {
        $type = $this->getUnionType($union);
        $sql  = "(\n".$this->addSpaces($level);
        $sql .= $this->convertUnions($union, ($level + 2));
        $sql .= ') '.$union[$type]['alias'];
        return $sql;

    }//end convertUnionsInFromClause()


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
        $sql  = ')';
        $sql .= ' WHERE row_num >= '.($limit['offset'] + 1);

        return $sql;

    }//end convertLimit()


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
        $sql = '';
        // Get Select field names.
        if (isset($this->query['LIMIT']) === TRUE) {
            $names    = $this->getSelectFieldNames($fields);
            $namesStr = array();
            $counter  = 0;
            foreach ($names as $name) {
                $namesStr[] = 'col_'.($counter++).' as '.$name;
            }

            $nameStr = implode(', ', $namesStr);
            $sql     = $nameStr.' FROM (SELECT ';

            $counter = 0;
            foreach ($fields as $field) {
                $field  = $this->setSelectFieldAlias($field, ('col_'.($counter++)));
                $list[] = $this->convertSingleField($field);
            }

            $sql .= $this->addSpaces($level).$this->separateFields($list);
            $sql .= ', rownum as row_num';
        } else {
            $list = array();
            foreach ($fields as $field) {
                $list[] = $this->convertSingleField($field);
            }

            $sql = $this->addSpaces($level).$this->separateFields($list);
        }//end if

        return $sql;

    }//end convertSelectFieldList()


    /**
     * Returns string with the SQL filters included.
     *
     * @param array $sqlArray The Query array.
     *
     * @since  4.0.0
     * @return string
     */
    protected function addSqlFilters(array $sqlArray)
    {
        $sql = '';

        if (isset($this->query['LIMIT']) === TRUE) {
            $limit = $this->query['LIMIT'];
            if (isset($this->query['WHERE']) === TRUE) {
                $sql .= ' AND ';
            } else {
                $sql .= ' WHERE ';
            }

            $sql .= ' rownum  <= '.($limit['rows'] + $limit['offset']);
        }

        // Group by.
        if (isset($sqlArray['GROUP-BY']) === TRUE) {
            $sql .= $this->convertGroupBy($sqlArray['GROUP-BY']);
        }

        // Having.
        if (isset($sqlArray['HAVING']) === TRUE) {
            $sql .= "\n".$this->getConstructName('having');
            $sql .= "\n".$this->convertWhereClause($sqlArray['HAVING']);
        }

        // Order By.
        if (isset($sqlArray['ORDER-BY']) === TRUE) {
            $sql .= "\n".$this->convertOrderBy($sqlArray['ORDER-BY']);
        }

        // Limit.
        if (isset($sqlArray['LIMIT']) === TRUE) {
            $sql .= "\n".$this->convertLimit($sqlArray['LIMIT']);
        }

        return $sql;

    }//end addSqlFilters()


}//end class

?>