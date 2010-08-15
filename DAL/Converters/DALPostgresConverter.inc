<?php
/**
 * Class for the conversion of Oracle specific SQL structures.
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
 * DALPostgresConverter.
 *
 * Contains methods to perform any database specific conversions into SQL.
 * The methods to convert XML structures into SQL are defined in the parent
 * class "DALConverter", and if any conversions need to be altered for this
 * database the corresponding methods of DALConverter should be redefined here.
 */
class DALPostgresConverter extends DALConverter
{

    /**
     * The name of the database type that this converter uses.
     *
     * @var $name Specifies that this converter is for 'Postgres'.
     */
    protected $name = 'Postgres';


    /**
     * Constructor.
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