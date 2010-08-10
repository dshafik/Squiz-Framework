<?php
/**
 * DALSchemaParser.
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

require_once 'DAL/Parsers/DALQueryParser.inc';
require_once 'DAL/DALBaker.inc';

/**
 * DALSchemaParser.
 *
 * Parses anything that is related to schemas (create table, constraints, etc).
 *
 */
class DALSchemaParser extends DALQueryParser
{

    /**
     * List of field types that require the size attribute.
     *
     * @var   array $_requiresSizeAttr List of field types that require the size
     *                                 attribute.
     */
    private static $_requiresSizeAttr = array(
                                         'VARCHAR',
                                         'CHAR',
                                         'NUMERIC',
                                         'DECIMAL',
                                        );

    /**
     * List of data types.
     *
     * @var   array $_columnDataTypes List of SQL constructs.
     */
    private static $_columnDataTypes = array(
                                        'CHAR',
                                        'VARCHAR',
                                        'CLOB',
                                        'BLOB',
                                        'NUMERIC',
                                        'DECIMAL',
                                        'INTEGER',
                                        'SMALLINT',
                                        'REAL',
                                        'DOUBLE PRECISION',
                                        'FLOAT',
                                        'BOOLEAN',
                                        'DATE',
                                        'TIME WITH TIME ZONE',
                                        'TIME WITHOUT TIME ZONE',
                                        'TIMESTAMP WITH TIME ZONE',
                                        'TIMESTAMP WITHOUT TIME ZONE',
                                        'INTERVAL',
                                       );

    /**
     * A list of User Defined Types.
     *
     * @var   array
     */
    private static $_udts = array();


    /**
     * Constructor.
     *
     * Private to avoid instantiating the object.
     *
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Converts XML schema to php array.
     *
     * @param DomElement $schema Schema node of a schema.
     *
     * @return array
     */
    public static function parse(DomElement $schema)
    {
        self::validate($schema);

        // Schema is valid, start parsing.
        $query['CREATE'] = array();
        $tables          = $schema->getElementsByTagName('table');
        self::$_udts     = self::getSchemaUdts($schema);

        foreach ($tables as $table) {
            $current                = array();
            $current['table']       = $table->getAttribute('name');
            $current['COLUMNS']     = self::getTableColumns($table);
            $current['INDEXES']     = self::getTableIndexes($table);
            $current['CONSTRAINTS'] = self::getTableConstraints($table);
            $current['SEQUENCES']   = self::getTableSequences($table);

            $query['CREATE'][] = $current;
        }

        return $query;

    }//end parse()


    /**
     * Converts XML schema UDTs to php array.
     *
     * @param DomElement $schema Schema node of a schema.
     *
     * @return array
     */
    public static function getSchemaUdts(DomElement $schema)
    {
        $schemaUdts = array(
                       'LOCAL'    => array(),
                       'IMPORTED' => array()
                      );

        $udtsTag = $schema->getElementsByTagName('udts')->item(0);
        if ($udtsTag !== NULL) {
            // Get the local udts.
            $ludts = $udtsTag->getElementsByTagName('udt');
            foreach ($ludts as $udt) {
                $current          = array();
                $current['type']  = $udt->getAttribute('type');
                $current['size']  = $udt->getAttribute('size');
                $current['scale'] = $udt->getAttribute('scale');

                $schemaUdts['LOCAL'][$udt->nodeValue] = $current;
            }

            // Get the imported udts.
            $ludts = $udtsTag->getElementsByTagName('udt-import');
            foreach ($ludts as $udt) {
                $current           = array();
                $current['system'] = $udt->getAttribute('system');

                $schemaUdts['IMPORTED'][$udt->nodeValue] = $current;
            }
        }//end if

        return $schemaUdts;

    }//end getSchemaUdts()


    /**
     * Converts XML table sequences to php array.
     *
     * @param DomElement $table Table node of a schema.
     *
     * @return array
     */
    public static function getTableSequences(DomElement $table)
    {
        $squencesTag = $table->getElementsByTagName('sequences')->item(0);
        $sequences   = array();

        if ($squencesTag !== NULL) {
            $seqs = $squencesTag->getElementsByTagName('sequence');
            foreach ($seqs as $seq) {
                $sequences[] = array(
                                'name'      => $seq->getAttribute('name'),
                                'increment' => $seq->getAttribute('increment'),
                                'min'       => $seq->getAttribute('min'),
                                'max'       => $seq->getAttribute('max'),
                               );
            }
        }

        return $sequences;

    }//end getTableSequences()


    /**
     * Returns constraints from parent node.
     *
     * @param DomElement $parent Parent node.
     *
     * @return array
     */
    public static function getConstraintsFromParent(DomElement $parent)
    {
        $keys = self::_getPrimaryKeyConstraints($parent);
        if (empty($keys) === FALSE) {
            $constraints['PRIMARY-KEYS'] = $keys;
        }

        $keys = self::_getForeignKeyConstraints($parent);
        if (empty($keys) === FALSE) {
            $constraints['FOREIGN-KEYS'] = $keys;
        }

        $keys = self::_getUniqueConstraints($parent);
        if (empty($keys) === FALSE) {
            $constraints['UNIQUES'] = $keys;
        }

        return $constraints;

    }//end getConstraintsFromParent()


    /**
     * Converts XML table constraints to php array.
     *
     * @param DomElement $table Table node of a schema.
     *
     * @return array
     */
    public static function getTableConstraints(DomElement $table)
    {
        $constraints    = array();
        $constraintsTag = $table->getElementsByTagName('constraints')->item(0);

        if ($constraintsTag === NULL) {
            return $constraints;
        }

        $constraints = self::getConstraintsFromParent($constraintsTag);

        return $constraints;

    }//end getTableConstraints()


    /**
     * Returns PRIMARY KEY constraints from parent node.
     *
     * @param DomElement $parent Parent node.
     *
     * @return array
     */
    private static function _getPrimaryKeyConstraints(DomElement $parent)
    {
        // Primary key constraints.
        $pks = $parent->getElementsByTagName('primary-key');

        $primaryKeys = array();
        foreach ($pks as $pk) {
            $current            = array();
            $current['name']    = $pk->getAttribute('name');
            $current['COLUMNS'] = array();

            $columns = $pk->getElementsByTagName('column');
            foreach ($columns as $column) {
                $current['COLUMNS'][] = $column->nodeValue;
            }

            $primaryKeys[] = $current;
        }

        return $primaryKeys;

    }//end _getPrimaryKeyConstraints()


    /**
     * Returns FOREIGN KEY constraints from parent node.
     *
     * @param DomElement $parent Parent node.
     *
     * @return array
     */
    private static function _getForeignKeyConstraints(DomElement $parent)
    {
        // Foreign key constraints.
        $fks = $parent->getElementsByTagName('foreign-key');

        $foreignKeys = array();
        foreach ($fks as $fk) {
            $current          = array();
            $current['name']  = $fk->getAttribute('name');
            $current['table'] = $fk->getAttribute('foreign-table');
            $onDelete         = $fk->getAttribute('on-delete');
            if ($onDelete === '') {
                $onDelete = 'NO ACTION';
            }

            $current['on-delete'] = $onDelete;
            $current['COLUMNS']   = array();

            $columns = $fk->getElementsByTagName('column');
            foreach ($columns as $column) {
                $col['name']          = $column->nodeValue;
                $col['references']    = $column->getAttribute('references');
                $current['COLUMNS'][] = $col;
            }

            $foreignKeys[] = $current;
        }//end foreach

        return $foreignKeys;

    }//end _getForeignKeyConstraints()


    /**
     * Returns UNIQUE constraints from parent node.
     *
     * @param DomElement $parent Parent node.
     *
     * @return array
     */
    private static function _getUniqueConstraints(DomElement $parent)
    {
        // Unique constraints.
        $uns = $parent->getElementsByTagName('unique');

        $uniques = array();
        foreach ($uns as $uq) {
            $current         = array();
            $current['name'] = $uq->getAttribute('name');
            $columns         = $uq->getElementsByTagName('column');
            foreach ($columns as $column) {
                $current['COLUMNS'][] = $column->nodeValue;
            }

            $uniques[] = $current;
        }

        return $uniques;

    }//end _getUniqueConstraints()


    /**
     * Converts XML table indexes to php array.
     *
     * @param DomElement $table Table node of a schema.
     *
     * @return array
     */
    public static function getTableIndexes(DomElement $table)
    {
        $indexesTag = $table->getElementsByTagName('indexes')->item(0);
        $idx        = array();

        if ($indexesTag !== NULL) {
            $indexes = $indexesTag->getElementsByTagName('index');
            foreach ($indexes as $index) {
                $current            = array();
                $current['name']    = $index->getAttribute('name');
                $current['COLUMNS'] = array();

                $columns = $index->getElementsByTagName('column');
                foreach ($columns as $column) {
                    $current['COLUMNS'][] = $column->nodeValue;
                }

                $idx[] = $current;
            }
        }

        return $idx;

    }//end getTableIndexes()


    /**
     * Retrieves a UDT's real column type.
     *
     * Converts a UDT column type into a generic DB column type, given a
     * list of UDTs to convert from.
     *
     * @param DomElement $column The column element to convert.
     * @param array      $udts   The list of UDTs.
     *
     * @return array The partial php representation of the column.
     */
    private static function _getColumnFromUdt(DomElement $column, array $udts)
    {
        $type               = $column->getAttribute('type');
        $newColumn['type']  = $udts[$type]['type'];
        $newColumn['size']  = $udts[$type]['size'];
        $newColumn['scale'] = $udts[$type]['scale'];

        return $newColumn;

    }//end _getColumnFromUdt()


    /**
     * Converts XML table columns to php array.
     *
     * @param DomElement $table Table node of a schema.
     *
     * @return array
     * @throws DALParserException If an invalid User-Defined Type is referenced.
     */
    public static function getTableColumns(DomElement $table)
    {
        // Get the columns tag.
        $cols       = array();
        $columnsTag = $table->getElementsByTagName('columns')->item(0);
        if ($columnsTag !== NULL) {
            $columns = $columnsTag->getElementsByTagName('column');
            foreach ($columns as $column) {
                // Check to see if we have any columns that are in our local
                // or imported UDT list.
                if ((isset(self::$_udts['LOCAL']) === TRUE)
                    && (in_array($column->getAttribute('type'), array_keys(self::$_udts['LOCAL'])) === TRUE)
                ) {
                    // Covert the column from its UDT type to its Generic DB type.
                    $current = self::_getColumnFromUdt($column, self::$_udts['LOCAL']);
                } else if ((isset(self::$_udts['IMPORTED']) === TRUE)
                    && (in_array($column->getAttribute('type'), array_keys(self::$_udts['IMPORTED'])) === TRUE)
                ) {
                    // We have a imported UDT so we have to get the local UDTs
                    // from the imported system so that we can convert it into
                    // its generic DB type.
                    $systemName = self::$_udts['IMPORTED'][$column->getAttribute('type')]['system'];
                    $schemaDoc  = DALBaker::getSystemSchemaDocument($systemName);

                    if ($schemaDoc === NULL) {
                        $msg = 'Imported UDT from the system "'.$schemaName.'" without a schema.xml';
                        throw new DALParserException($msg);
                    }

                    $schemaNode = $schemaDoc->getElementsByTagName('schema')->item(0);
                    $udts       = self::getSchemaUdts($schemaNode);
                    $current    = self::_getColumnFromUdt($column, $udts['LOCAL']);
                } else {
                    // It's just a normal type.
                    $current          = array();
                    $current['type']  = $column->getAttribute('type');
                    $current['size']  = $column->getAttribute('size');
                    $current['scale'] = $column->getAttribute('scale');
                }//end if

                $current['name']       = $column->nodeValue;
                $current['allow-null'] = $column->getAttribute('allow-null');
                $current['default']    = $column->getAttribute('default');

                $cols[] = $current;
            }//end foreach
        }//end if

        return $cols;

    }//end getTableColumns()


    /*
        Schema Validation
    */


    /**
     * Validates the given schema.
     *
     * Validation will make sure all required elements, attributes, etc are
     * available and contain no errors. Throws ChannelException if any errors
     * are found.
     *
     * @param DomElement $schema Xml elements for a single schema.
     *
     * @return void
     * @throws DALParserException If the parsed element is not a schema.
     */
    public static function validate(DomElement $schema)
    {
        // Make sure this is a schema.
        if ($schema->tagName !== 'schema') {
            throw new DALParserException('Given DomElements is not a schema.');
        }

        // All tables need to have name attribute and valid columns.
        $tables = $schema->getElementsByTagName('table');

        // Validate UDTs.
        self::validateSchemaUdts($schema);

        foreach ($tables as $table) {
            self::validateTable($table);
        }

    }//end validate()


    /**
     * Validates the given table.
     *
     * Validation will make sure all required elements, attributes, etc are
     * available and contain no errors. Throws ChannelException if any errors
     * are found.
     *
     * @param DomElement $table Xml elements for a single table.
     *
     * @return void
     * @throws DALParserException If the table doesn't contain a name.
     */
    public static function validateTable(DomElement $table)
    {
        if ($table->getAttribute('name') === '') {
            $msg = 'One of the tables does not contain "name" attribute.';
            throw new DALParserException($msg);
        }

        // Validate this tables columns.
        self::validateTableColumns($table);

        // Validate constraints.
        self::validateTableConstraints($table);

        // Validate Indexes.
        self::validateTableIndexes($table);

        // Validate Sequences.
        self::validateTableSequences($table);

    }//end validateTable()


    /**
     * Validates given table's columns.
     *
     * Throws ChannelException on error.
     *
     * @param DomElement $table Xml node of the table.
     *
     * @return void
     * @throws DALParserException If the Table columns are incorrectly defined.
     */
    public static function validateTableColumns(DomElement $table)
    {
        // Get table's columns.
        $columnsTag = $table->getElementsByTagName('columns')->item(0);
        $tableName  = $table->getAttribute('name');
        if ($tableName === '') {
            $tableName = $table->getAttribute('table');
        }

        if ($columnsTag === NULL) {
            $msg = $tableName.'\'s table has no columns.';
            throw new DALParserException($msg);
        }

        $cols = $columnsTag->getElementsByTagName('column');

        if ($cols->length === 0) {
            $msg = $tableName.'\'s table has no columns.';
            throw new DALParserException($msg);
        }

        $msg = $tableName.'\'s table column at #';

        // Check column attributes.
        $attr = 1;
        foreach ($cols as $col) {
            $type = $col->getAttribute('type');
            if ($type === '') {
                $msg .= $attr.' does not have "type" attribute.';
                throw new DALParserException($msg);
            } else {
                self::validSchemaColumnType($table, $type);
            }

            if (in_array($type, self::$_requiresSizeAttr) === TRUE) {
                $size = $col->getAttribute('size');
                if ($size === '') {
                    $msg .= $attr.' does not have "size" attribute.';
                    throw new DALParserException($msg);
                }
            }

            // If type is NUMERIC then we need scale attribute.
            if ($type === 'NUMERIC') {
                if ($col->getAttribute('scale') === '') {
                    $msg .= $attr.' is type NUMERIC and it does not have';
                    $msg .= ' "scale" attribute.';
                    throw new DALParserException($msg);
                }
            }

            if ($col->nodeValue === '') {
                $msg .= $attr.' does not have a column name.';
                throw new DALParserException($msg);
            }

            $attr++;
        }//end foreach

    }//end validateTableColumns()


    /**
     * Determines if the given type is a valid column type.
     *
     * Checks UDTs, CDTs, and FUDTs. Throws DALParserException.
     *
     * @param DomElement $table Table or schema that this column belongs to.
     * @param string     $type  Type of the column.
     *
     * @return boolean
     */
    public static function validSchemaColumnType(DomElement $table, $type)
    {
        // If type is not uppercase then its not valid.
        if (strtoupper($type) !== $type) {
            return FALSE;
        }

        // Check if this is in available column types.
        if (in_array($type, self::$_columnDataTypes) === TRUE) {
            return TRUE;
        }

        // Get the owner document.
        $doc = $table->ownerDocument;

        // Check udts.
        $xpath  = new DomXpath($doc);
        $query  = '//schema/udts[udt="'.$type.'" or udt-import="'.$type.'"]';
        $result = $xpath->query($query);

        if ($result->length > 0) {
            return TRUE;
        }

        return FALSE;

    }//end validSchemaColumnType()


    /**
     * Checks if the given table has valid constraints.
     *
     * I.e. table must have a primary-key. Throws DALParserException.
     *
     * @param DomElement $table Xml elements of the table.
     *
     * @return void
     * @throws DALParserException If there is no constraints tag or if the
     *                            constraints are invalid.
     */
    public static function validateTableConstraints(DomElement $table)
    {
        $constTag  = $table->getElementsByTagName('constraints')->item(0);
        $tableName = $table->getAttribute('name');
        $msg       = $tableName.'\'s table';

        if ($constTag === NULL) {
            $msg .= ' does not have "constraints".';
            throw new DALParserException($msg);
        }

        // Check general constraint rules.
        self::validateConstraints($constTag, $msg);

        // Additional Primary Key checks.
        $pks = $constTag->getElementsByTagName('primary-key');
        if ($pks->length !== 1) {
            $msg .= ' must have a primary-key.';
            throw new DALParserException($msg);
        } else {
            foreach ($pks as $pk) {
                $cols = $pk->getElementsByTagName('column');
                // Each of the columns here must be defined in table columns.
                foreach ($cols as $col) {
                    if (self::tableHasColumn($table, $col->nodeValue) === FALSE) {
                        $msg .= ' does not have column "'.$col->nodeValue;
                        $msg .= '", but it was used in its primary-key.';
                        throw new DALParserException($msg);
                    }
                }
            }
        }//end if

        // Additional Foreign Key checks.
        // Check if table has already defined this column name.
        $fks         = $constTag->getElementsByTagName('foreign-key');
        $keyPosition = 1;
        foreach ($fks as $fk) {
            $cols = $fk->getElementsByTagName('column');
            if ($cols !== NULL) {
                $colLoc = 1;
                foreach ($cols as $col) {
                    $colVal = $col->nodeValue;
                    if (self::tableHasColumn($table, $colVal) === FALSE) {
                        $msg .= ' foreign-key #'.$keyPosition;
                        $msg .= ' has column (#'.$colLoc.')';
                        $msg .= ' with column name that was NOT defined in';
                        $msg .= ' table columns. Name: '.$colVal;
                        throw new DALParserException($msg);
                    }

                    $colLoc++;
                }
            }

            $keyPosition++;
        }

        // Additional unique Key checks.
        $uniques = $constTag->getElementsByTagName('unique');
        foreach ($uniques as $unique) {
            $cols = $unique->getElementsByTagName('column');
            if ($cols !== NULL) {
                foreach ($cols as $col) {
                    $colVal = $col->nodeValue;
                    if (self::tableHasColumn($table, $colVal) === FALSE) {
                        $msg .= ' does not have column "'.$colVal;
                        $msg .= '", but it was used in its unique constraint.';
                        throw new DALParserException($msg);
                    }
                }
            }
        }

    }//end validateTableConstraints()


    /**
     * Validates general constraint rules.
     *
     * @param DomElement $parent Parent node of the constraints.
     * @param string     $prefix Exception message prefix.
     *
     * @return void
     */
    public static function validateConstraints(DomElement $parent, $prefix='')
    {
        // Primary Keys.
        $pks = $parent->getElementsByTagName('primary-key');
        foreach ($pks as $pk) {
            self::_validatePrimaryKeyConstraint($pk, $prefix);
        }

        // Foreign Keys.
        $fks         = $parent->getElementsByTagName('foreign-key');
        $keyPosition = 1;
        foreach ($fks as $fk) {
            self::_validateForeignKeyConstraint($fk, $keyPosition, $prefix);
            $keyPosition++;
        }

        // Unique constraints.
        $uniques     = $parent->getElementsByTagName('unique');
        $keyPosition = 1;
        foreach ($uniques as $unique) {
            self::_validateUniqueConstraint($unique, $keyPosition, $prefix);
            $keyPosition++;
        }

    }//end validateConstraints()


    /**
     * Validates single primary key constraint.
     *
     * @param DomElement $pk     Primary key constraint.
     * @param string     $prefix Exception message prefix.
     *
     * @return void
     * @throws DALParserException If the primary key has no name specified, or
     *                            has no columns specified.
     */
    private static function _validatePrimaryKeyConstraint(DomElement $pk, $prefix='')
    {
        $msg = $prefix;
        // Check that it has name attribute.
        if ($pk->getAttribute('name') === '') {
            $msg .= ' primary-key does not have "name" attribute.';
            throw new DALParserException($msg);
        }

        // Check that it has columns.
        $cols = $pk->getElementsByTagName('column');
        if ($cols->length === 0) {
            $msg .= ' has no columns defined for its primary-key.';
            throw new DALParserException($msg);
        }

    }//end _validatePrimaryKeyConstraint()


    /**
     * Validates single foreign key constraint.
     *
     * @param DomElement $fk          Foreign key constraint.
     * @param integer    $keyPosition Keys position in the constraints list.
     * @param string     $prefix      Exception message prefix.
     *
     * @return void
     * @throws DALParserException If the Foreign Key is invalid.
     */
    private static function _validateForeignKeyConstraint(
        DomElement $fk,
        $keyPosition,
        $prefix=''
    ) {
        $msg = $prefix;

        if ($fk->getAttribute('name') === '') {
            $msg .= ' foreign-key #'.$keyPosition;
            $msg .= ' does not have "name" attribute.';
            throw new DALParserException($msg);
        }

        if ($fk->getAttribute('foreign-table') === '') {
            $msg .= ' foreign-key #'.$keyPosition;
            $msg .= ' does not have "foreign-table" attribute.';
            throw new DALParserException($msg);
        }

        // Check on-delete attribute value.
        $onDelete = $fk->getAttribute('on-delete');
        if ($onDelete !== '') {
            if (($onDelete !== 'CASCADE') && ($onDelete !== 'NO ACTION')) {
                $msg .= ' foreign-key #'.$keyPosition;
                $msg .= ' has invalid on-delete attribute value.';
                throw new DALParserException($msg);
            }
        }

        // Check each column for this fk.
        $cols = $fk->getElementsByTagName('column');
        if ($cols->length === 0) {
            $msg .= ' foreign-key #'.$keyPosition.' does not have columns.';
            throw new DALParserException($msg);
        } else {
            $colLoc = 1;
            foreach ($cols as $col) {
                if ($col->getAttribute('references') === '') {
                    $msg .= ' foreign-key #'.$keyPosition;
                    $msg .= ' has column (#'.$colLoc.')';
                    $msg .= ' with missing references attribute.';
                    throw new DALParserException($msg);
                }

                $colVal = $col->nodeValue;
                if ($colVal === '') {
                    $msg .= ' foreign-key #'.$keyPosition;
                    $msg .= ' has column (#'.$colLoc.')';
                    $msg .= ' with missing content.';
                    throw new DALParserException($msg);
                }

                $colLoc++;
            }//end foreach foreign-key columns
        }//end if

    }//end _validateForeignKeyConstraint()


    /**
     * Validates single unique constraint.
     *
     * @param DomElement $unique      Unique constraint.
     * @param integer    $keyPosition Keys position in the constraints list.
     * @param string     $prefix      Exception message prefix.
     *
     * @return void
     * @throws DALParserException If the unique constraint doesn't have a name
     *                            or any columns.
     */
    private static function _validateUniqueConstraint(
        DomElement $unique,
        $keyPosition,
        $prefix=''
    ) {
        $msg = $prefix;

        if ($unique->getAttribute('name') === '') {
            $msg .= ' unique constraint #'.$keyPosition;
            $msg .= ' does not have "name" attribute.';
            throw new DALParserException($msg);
        }

        $cols = $unique->getElementsByTagName('column');
        if ($cols->length === 0) {
            $msg .= 'unique constraint #'.$keyPosition.' does now have columns.';
            throw new DALParserException($msg);
        }

    }//end _validateUniqueConstraint()


    /**
     * Checks if the given table has valid indexes.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $table Xml elements of the table.
     *
     * @return void
     * @throws DALParserException If an index doesn't have a name, doesn't have
     *                            any columns, or references a column that
     *                            doesn't exist.
     */
    public static function validateTableIndexes(DomElement $table)
    {
        $tableName = $table->getAttribute('name');
        $msg       = $tableName.'\'s table';

        // Get the indexes.
        $indexes = $table->getElementsByTagName('indexes');
        if ($indexes->length !== 0) {
            $indexes = $indexes->item(0)->getElementsByTagName('index');
            foreach ($indexes as $index) {
                if ($index->getAttribute('name') === '') {
                    $msg .= ' has index with missing "name" attribute.';
                    throw new DALParserException($msg);
                }

                $cols = $index->getElementsByTagName('column');
                if ($cols->length === 0) {
                    $msg .= ' has index with no columns.';
                    throw new DALParserException($msg);
                } else {
                    foreach ($cols as $col) {
                        $colVal = $col->nodeValue;
                        if (self::tableHasColumn($table, $colVal) === FALSE) {
                            $msg .= ' does not have column "'.$colVal;
                            $msg .= '" but it was used in its index.';
                            throw new DALParserException($msg);
                        }
                    }
                }
            }//end foreach indexes
        }//end if indexes !== NULL

    }//end validateTableIndexes()


    /**
     * Checks if the given table has valid sequences.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $table Xml elements of the table.
     *
     * @return void
     * @throws DALParserException If a sequence has no sequence tags, or is
     *                            missing a name attribute.
     */
    public static function validateTableSequences(DomElement $table)
    {
        $tableName = $table->getAttribute('name');
        $msg       = $tableName.'\'s table';

        // Get the indexes.
        $seqsTag = $table->getElementsByTagName('sequences');
        if ($seqsTag->length !== 0) {
            $seqs = $seqsTag->item(0)->getElementsByTagName('sequence');
            if ($seqs->length === 0) {
                $msg .= ' has sequences tag with no sequence tags.';
                throw new DALParserException($msg);
            }

            foreach ($seqs as $seq) {
                if ($seq->getAttribute('name') === '') {
                    $msg .= ' has sequence with missing "name" attribute.';
                    throw new DALParserException($msg);
                }
            }//end foreach
        }//end if

    }//end validateTableSequences()


    /**
     * Checks if the given schema has valid UDTs.
     *
     * Throws DALParserException if any errors found.
     *
     * @param DomElement $schema Xml elements of the schema.
     *
     * @return void
     * @throws DALParserException If the UDTs are invalid.
     */
    public static function validateSchemaUdts(DomElement $schema)
    {
        $msg = 'Schema "'.$schema->getAttribute('system').'" ';

        // Get schema User Defined Types (UDTs).
        $udtsTag = $schema->getElementsByTagName('udts');

        if ($udtsTag->length !== 0) {
            // Get local UDTs.
            $ludts = $udtsTag->item(0)->getElementsByTagName('udt');

            if ($ludts->length !== 0) {
                foreach ($ludts as $ludt) {
                    // UDTs must be in uppercase.
                    $var = $ludt->nodeValue;
                    if ($var !== strtoupper($var)) {
                        $msg .= ' has UDT with an invalid name "'.$var.'".';
                        $msg .= ' Expected "'.strtoupper($var).'".';
                        throw new DALParserException($msg);
                    }

                    $type = $ludt->getAttribute('type');
                    if ($type === '') {
                        $msg .= ' has UDT with no type attribute.';
                        throw new DALParserException($msg);
                    }

                    if (in_array($type, self::$_requiresSizeAttr) === TRUE) {
                        if ($ludt->getAttribute('size') === '') {
                            $msg .= ' has '.$type.' UDT with no size attr.';
                            throw new DALParserException($msg);
                        }
                    }

                    if ($type === 'NUMERIC') {
                        if ($ludt->getAttribute('scale') === '') {
                            $msg .= ' has NUMERIC UDT with no scale attr.';
                            throw new DALParserException($msg);
                        }
                    }
                }//end foreach ludts
            }//end if

            // Get foreign UDTs.
            $fudts = $udtsTag->item(0)->getElementsByTagName('udt-import');
            if ($fudts->length !== 0) {
                foreach ($fudts as $fudt) {
                    if ($fudt->getAttribute('system') === '') {
                        $msg .= ' has foreign UDT with no "system" attribute';
                        throw new DALParserException($msg);
                    }

                    $var = $fudt->nodeValue;
                    if ($var !== strtoupper($var)) {
                        $msg .= ' has foreign UDT with an invalid var name.';
                        throw new DALParserException($msg);
                    }
                }
            }
        }//end if

    }//end validateSchemaUdts()


    /**
     * Returns true if table has the specified column listed in its columns tag.
     *
     * @param DomElement $table      Xml elements of the table.
     * @param string     $columnName Search for this column name.
     *
     * @return boolean
     * @throws DALParserException If the DomElement passed is not a valid table
     *                            element.
     */
    public static function tableHasColumn(DomElement $table, $columnName)
    {
        // Is this a table node?
        if ($table->tagName !== 'table') {
            $msg = 'tableHasColumn\'s first argument must be a table node.';
            throw new DALParserException($msg);
        }

        /*
            Check UDTs ? If yes, then change the fn name to schemaHasColumn().
        */

        // Check Table Columns.
        $cols = $table->getElementsByTagName('columns')->item(0);
        if ($cols !== NULL) {
            $cols = $cols->getElementsByTagName('column');
            foreach ($cols as $col) {
                if ($col->nodeValue === $columnName) {
                    return TRUE;
                }
            }
        }

        return FALSE;

    }//end tableHasColumn()


}//end class

?>