<?php
/**
 * Main class for the conversion of DAL arrays into SQL.
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

require_once dirname(dirname(__FILE__)).'/Exceptions/DALConverterException.inc';

/**
 * DALConverter.
 *
 * This class will have base functions that converts parsed XQDs to their SQL
 * representation. XQDs are parsed by DALBaker and converted to PHP arrays.
 * If new databases have different SQL constructs then they should extend this
 * class and override its functions & members.
 */
class DALConverter
{

    /**
     * List of data types (for Postgres).
     *
     * Sub classes will need to override this array and change the array values.
     *
     * @var $dataTypes List of SQL constructs.
     */
    protected $dataTypes = array(
                            'char'             => 'CHAR',
                            'varchar'          => 'VARCHAR',
                            'clob'             => 'TEXT',
                            'blob'             => 'BYTEA',
                            'integer'          => 'INT',
                            'smallint'         => 'SMALLINT',
                            'numeric'          => 'NUMERIC',
                            'float'            => 'FLOAT',
                            'real'             => 'REAL',
                            'double_precision' => 'DOUBLE PRECISION',
                            'boolean'          => 'BOOLEAN',
                            'date'             => 'DATE',
                            'time'             => 'TIME',
                            'timestamp'        => 'TIMESTAMP',
                            'interval'         => 'INTERVAL',
                           );

    /**
     * Name of the constructs for this database.
     *
     * Sub classes will need to override this array and change the array values.
     *
     * @var $sqlConstructs List of SQL constructs.
     */
    protected $sqlConstructs = array(
                                'select'     => 'SELECT',
                                'from'       => 'FROM',
                                'where'      => 'WHERE',
                                'in'         => 'IN',
                                'not-in'     => 'NOT IN',
                                'limit'      => 'LIMIT',
                                'group-by'   => 'GROUP BY',
                                'having'     => 'HAVING',
                                'order-by'   => 'ORDER BY',
                                'exists'     => 'EXISTS',
                                'not-exists' => 'NOT EXISTS',
                                'start-with' => 'START WITH',
                                'connect-by' => 'CONNECT BY PRIOR',
                                'case'       => 'CASE',
                                'when'       => 'WHEN',
                                'then'       => 'THEN',
                                'else'       => 'ELSE',
                               );

    /**
     * List of math operators.
     *
     * Values of this array will be used to separate fields/values from each
     * other. For example: ((table.col1 - table.col2) + value)
     *
     * @var array $mathOps List of available math operators.
     */
    protected $mathOps = array(
                          'SUBTRACT' => '-',
                          'ADD'      => '+',
                          'MULTIPLY' => '*',
                          'DIVIDE'   => '/',
                         );

    /**
     * SQL query array that is being converted.
     *
     * @var array $_query SQL query array.
     */
    protected $query = array();

    /**
     * Name of the database.
     *
     * @var string $name Database name.
     */
    protected $name = '';

    /**
     * List of where conditions and their signs.
     *
     * @var array $_whereConditions List of conditions that can be used in
     *                                where clause.
     */
    private $_whereConditions = array(
                                 'equal'            => '=',
                                 'not-equal'        => '!=',
                                 'equal-or-greater' => '>=',
                                 'equal-or-less'    => '<=',
                                 'greater'          => '>',
                                 'less'             => '<',
                                 'like'             => 'LIKE',
                                 'not-like'         => 'NOT LIKE',
                                 'is-null'          => 'IS NULL',
                                 'not-null'         => 'NOT NULL',
                                 'similar-to'       => 'SIMILAR TO',
                                 'not-similar-to'   => 'NOT SIMILAR TO',
                                );

    /**
     * List of hooks that were found during conversion.
     *
     * This array is used by the Query object when Query->prepare() is called.
     * Structure should be the [hookSqlCode] = value if not replaced.
     *
     * @var array
     */
    private $_hookStrings = array();


    /**
     * Returns the database specific data type.
     *
     * Sub classes should override the $dataTypes member.
     *
     * @param string $type Base data type.
     *
     * @return string
     */
    public function getDataType($type)
    {
        $dataType = strtolower($type);
        if (isset($this->dataTypes[$dataType]) === TRUE) {
            $dataType = $this->dataTypes[$dataType];
        }

        return $dataType;

    }//end getDataType()


    /**
     * Returns the readable database name.
     *
     * @return string
     */
    public function getName()
    {
        return $this->name;

    }//end getName()


    /**
     * Returns the construct string for the current database type.
     *
     * Classes that extend this class should override the sqlConstructs member.
     *
     * @param string $name Base name of the construct to get.
     *
     * @return string
     */
    public function getConstructName($name)
    {
        $construct = '';
        if (isset($this->sqlConstructs[$name]) === TRUE) {
            $construct = $this->sqlConstructs[$name];
        }

        return $construct;

    }//end getConstructName()


    /**
     * Converts given query to its SQL.
     *
     * @param array $sqlArray Single query.
     *
     * @return string
     */
    public function convertToSql(array $sqlArray)
    {
        $this->query = $sqlArray;

        $sql = '';
        if (isset($sqlArray['SELECT']) === TRUE) {
            $sql = $this->convertSelectQuery($sqlArray);
        } else if (isset($sqlArray['INSERT']) === TRUE) {
            $sql = $this->convertInsertQuery($sqlArray);
        } else if (isset($sqlArray['UPDATE']) === TRUE) {
            $sql = $this->convertUpdateQuery($sqlArray);
        } else if (isset($sqlArray['DELETE']) === TRUE) {
            $sql = $this->convertDeleteQuery($sqlArray);
        } else if (isset($sqlArray['ALTER']) === TRUE) {
            $sql = $this->convertAlterQuery($sqlArray);
        } else if (isset($sqlArray['FUNCTION']) === TRUE) {
            $sql = $this->convertSingleFunction($sqlArray);
        } else if (isset($sqlArray['DROP']) === TRUE) {
            if (isset($sqlArray['DROP']['TABLE']) === TRUE) {
                $sql = $this->convertDropTable($sqlArray['DROP']['TABLE']);
            } else if (isset($sqlArray['DROP']['SEQUENCE']) === TRUE) {
                $sql = $this->convertDropSequence($sqlArray['DROP']['SEQUENCE']);
            }
        } else if (isset($sqlArray['WITH']) === TRUE) {
            $sql = $this->convertWithQuery($sqlArray['WITH']);
        } else if (isset($sqlArray['CALL']) === TRUE) {
            $sql = $this->convertCallQuery($sqlArray['CALL']);
        } else if (isset($sqlArray['AND']) === TRUE) {
            $sqlArray = array('WHERE' => $sqlArray);
            $sql      = $this->convertWhereClause($sqlArray['WHERE']);
        } else if (isset($sqlArray['FROM']) === TRUE) {
            $sql = $this->convertFromList($sqlArray['FROM']);
        } else if (isset($sqlArray['EXCEPT']) === TRUE) {
            $sql = $this->convertExcepts($sqlArray['EXCEPT'], 'EXCEPT');
        } else if (isset($sqlArray['EXCEPT-ALL']) === TRUE) {
            $sql = $this->convertExcepts($sqlArray['EXCEPT-ALL'], 'EXCEPT ALL');
        } else if (isset($sqlArray['INTERSECT']) === TRUE) {
            $sql = $this->convertIntersect($sqlArray['INTERSECT']);
        }//end if

        return $sql;

    }//end convertToSql()


    /**
     * Converts Select query to its SQL string.
     *
     * @param array   $sqlArray The select query array.
     * @param integer $level    Number of levels to indent.
     * @param array   $ignore   Tag names that will be ignored while converting
     *                          the select statement. Example: where,join..
     *
     * @return string
     */
    protected function convertSelectQuery(
        array $sqlArray,
        $level=0,
        array $ignore=array()
    ) {
        $sql  = $this->addSpaces($level);
        $sql .= $this->getConstructName('select');

        if (isset($sqlArray['CASE']) === TRUE) {
            $sql .= ' '.$this->getConstructName('case');
            foreach ($sqlArray['CASE']['CONDITIONS'] as $condition) {
                $sql .= "\n".$this->getConstructName('when');
                $sql .= "\n".$this->convertWhereClause($condition['WHEN'], $level);
                $sql .= $this->getConstructName('then');
                $sql .= "\n(".$this->convertToSql($condition['THEN']).")\n";
            }

            $sql .= "\n".$this->getConstructName('else');
            $sql .= "\n(".$this->convertToSql($sqlArray['ELSE']).') END';

            if (empty($sqlArray['ALIAS']) === FALSE) {
                $sql .= ' AS '.$sqlArray['ALIAS'];
            }
        } else {
            // Add Distinct clause?
            if (isset($sqlArray['SELECT']['distinct']) === TRUE) {
                if ($sqlArray['SELECT']['distinct'] === TRUE) {
                    $sql .= ' DISTINCT ';
                }
            } else if (isset($sqlArray['SELECT']['DISTINCT-ON']) === TRUE
                && empty($sqlArray['SELECT']['DISTINCT-ON']) === FALSE
            ) {
                $sql .= ' DISTINCT ON (';
                $sql .= $this->convertSelectFieldList(
                    $sqlArray['SELECT']['DISTINCT-ON'],
                    ($level + 1)
                );
                $sql .= ') ';
            }

            $sql .= "\n".$this->convertSelectFieldList(
                $sqlArray['SELECT']['FIELDS'],
                ($level + 1)
            );

            $sql .= "\n".$this->addSpaces($level);
            if (isset($sqlArray['FROM']) === TRUE) {
                $sql .= $this->getConstructName('from');
                $sql .= $this->convertFromList($sqlArray['FROM'], ($level + 1));
            }

            $sql .= "\n".$this->addSpaces($level);

            if (in_array('join', $ignore) === FALSE) {
                $joins = '';
                if (isset($sqlArray['JOIN']) === TRUE) {
                    $joins = $this->convertJoins($sqlArray['JOIN'], $sqlArray['FROM']);
                }

                if ($joins !== '') {
                    $sql .= "\n".$joins."\n";
                }
            }

            if (in_array('where', $ignore) === FALSE) {
                if (isset($sqlArray['WHERE']) === TRUE) {
                    $sql .= $this->getConstructName('where');
                    $sql .= "\n".$this->convertWhereClause(
                        $sqlArray['WHERE'],
                        $level
                    );
                }
            }

            $sql .= $this->addSqlFilters($sqlArray);
        }//end if

        return $sql;

    }//end convertSelectQuery()


    /**
     * Returns string with the SQL filters included.
     *
     * @param array $sqlArray The Query array.
     *
     * @return string
     */
    protected function addSqlFilters(array $sqlArray)
    {
        $sql = '';

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


    /**
     * Converts Select's field list to its SQL format.
     *
     * @param array   $fields Array of fields.
     * @param integer $level  Number of levels to indent.
     *
     * @return string
     */
    protected function convertSelectFieldList(array $fields, $level=1)
    {
        $list = array();
        foreach ($fields as $field) {
            $list[] = $this->convertSingleField($field);
        }

        $sql = $this->addSpaces($level).$this->separateFields($list);

        return $sql;

    }//end convertSelectFieldList()


    /**
     * Converts single field to its SQL format.
     *
     * Field maybe a column, value, hook, function, math operation.
     *
     * @param (string|array) $field The field array or string.
     *
     * @return string
     */
    protected function convertSingleField($field)
    {
        $sql = '';
        if (is_array($field) === TRUE) {
            if (isset($field['FUNCTION']) === TRUE) {
                // Function call (i.e. CONCAT()).
                $sql = $this->convertSingleFunction($field);
            } else if (isset($field['table']) === TRUE) {
                // Table column.
                $sql = $this->convertField($field);
            } else if (isset($field['MATH-OP']) === TRUE) {
                $sql = $this->convertMathOperation($field);
            } else if (isset($field['CASE']) === TRUE) {
                $sql = $this->convertCaseStatement($field);
            } else if (isset($field['SELECT']) === TRUE) {
                $sql = '('.$this->convertSelectQuery($field).')';
            } else if (isset($field['EXCEPT']) === TRUE) {
                $sql = '('.$this->convertExcepts($field['EXCEPT'], 'EXCEPT').')';
            } else if (isset($field['EXCEPT-ALL']) === TRUE) {
                $sql = '('.$this->convertExcepts(
                    $field['EXCEPT-ALL'],
                    'EXCEPT ALL'
                ).')';
            } else if (isset($field['INTERSECT']) === TRUE) {
                $sql = '('.$this->convertIntersect($field['INTERSECT']).')';
            } else {
                $sql = $this->convertSingleValue($field);
            }//end if
        } else {
            // Must be a single value.
            $sql = $this->convertSingleValue($field);
        }//end if

        return $sql;

    }//end convertSingleField()


    /**
     * Converts a case statement to its SQL format.
     *
     * @param array $caseStatement The CASE array.
     *
     * @return string
     */
    protected function convertCaseStatement(array $caseStatement)
    {
        $sql = ' CASE';
        foreach ($caseStatement['CASE']['WHEN'] as $conditionResult) {
            // If it is the condition then what are we going to do?
            $sql .= ' WHEN ';
            $sql .= $this->convertWhereClause($conditionResult['CONDITION']);
            $sql .= ' THEN ';
            $sql .= $this->convertSingleField($conditionResult['THEN']);
        }//end foreach

        if (isset($caseStatement['CASE']['ELSE']) !== FALSE
            && empty($caseStatement['CASE']['ELSE']) === FALSE
        ) {
            $sql .= ' ELSE '.$this->convertSingleField(
                $caseStatement['CASE']['ELSE']
            );
        }//end if

        $sql .= ' END ';
        if (isset($caseStatement['CASE']['alias']) === TRUE) {
            $sql .= ' as '.$caseStatement['CASE']['alias'];
        }//end if

        return $sql;

    }//end convertCaseStatement()


    /**
     * Given a field it returns its name.
     *
     * @param array $field Single field from select field list.
     *
     * @return array
     */
    protected function getSelectFieldName(array $field)
    {
        $name = '';
        if (isset($field['FUNCTION']) === TRUE) {
            // Function call (i.e. CONCAT()).
            if (isset($function['FUNCTION']['alias']) === TRUE) {
                $name = $function['FUNCTION']['alias'];
            }
        } else if (isset($field['table']) === TRUE) {
            // Table column.
            if (isset($field['alias']) === TRUE) {
                $name = $field['alias'];
            } else {
                $name = $field['column'];
            }
        } else if (isset($field['MATH-OP']) === TRUE) {
            if (isset($mathOp['MATH-OP']['alias']) === TRUE) {
                $name = $mathOp['MATH-OP']['alias'];
            }
        } else if (isset($field['SELECT']) === TRUE) {
            $name = '';
        } else {
            if (isset($field['alias']) === TRUE) {
                $name = $field['alias'];
            } else {
                $name = $field['value'];
            }
        }//end if

        return $name;

    }//end getSelectFieldName()


    /**
     * Given a field it will update its alias.
     *
     * @param array  $field A field from Select field list.
     * @param string $alias New alias for the field.
     *
     * @return array
     */
    protected function setSelectFieldAlias(array $field, $alias)
    {
        if (isset($field['FUNCTION']) === TRUE) {
            // Function call (i.e. CONCAT()).
            $function['FUNCTION']['alias'] = $alias;
        } else if (isset($field['table']) === TRUE) {
            // Table column.
            $field['alias'] = $alias;
        } else if (isset($field['MATH-OP']) === TRUE) {
            $mathOp['MATH-OP']['alias'] = $alias;
        } else if (isset($field['SELECT']) === FALSE) {
            // TODO: Aliases for SELECTs.
            $field['alias'] = $alias;
        }

        return $field;

    }//end setSelectFieldAlias()


    /**
     * Given list of fields it returns their name.
     *
     * @param array $fields Fields from Select field list.
     *
     * @return array
     */
    protected function getSelectFieldNames(array $fields)
    {
        $names = array();
        foreach ($fields as $field) {
            $names[] = $this->getSelectFieldName($field);
        }

        return $names;

    }//end getSelectFieldNames()


    /**
     * Converts single field to its SQL format.
     *
     * @param array $field Field's name.
     *
     * @return string
     */
    protected function convertField(array $field)
    {
        $sql = '';
        if ($field['table'] !== '') {
            $sql .= $field['table'];
            if ($field['column'] !== '') {
                $sql .= '.';
            }
        }

        if ($field['column'] !== '') {
            $sql .= $field['column'];
            if (isset($field['alias']) === TRUE) {
                $sql .= ' as '.$field['alias'];
            }
        }

        return $sql;

    }//end convertField()


    /**
     * Converts a single value to its SQL format.
     *
     * @param array|string $field The field value.
     *
     * @return string
     */
    protected function convertSingleValue($field)
    {
        $sql = '';
        if (is_array($field) === TRUE) {
            if ((isset($field['value']) === TRUE)
                && (isset($field['alias']) === TRUE)
            ) {
                $sql .= $field['value'].' as '.$field['alias'];
            }
        } else {
            if (empty($field) === FALSE) {
                $sql = $field;
            } else {
                if (is_numeric($field) === FALSE) {
                    $sql = "'".$field."'";
                } else {
                    $sql = $field;
                }
            }
        }

        return $sql;

    }//end convertSingleValue()


    /**
     * Converts a single function to its SQL string.
     *
     * @param array $function Function array.
     *
     * @return string
     */
    protected function convertSingleFunction(array $function)
    {
        $sql        = '';
        $fnName     = $function['FUNCTION']['function'];
        $fnHandler  = 'handleFunction'.$fnName;
        $args       = array();
        $hasHandler = method_exists($this, $fnHandler);

        foreach ($function['FUNCTION']['ARGS'] as $arg) {
            if ($hasHandler === FALSE) {
                $args[] = '('.$this->convertSingleField($arg).')';
            } else {
                $args[] = $this->convertSingleField($arg);
            }
        }

        if ($hasHandler === FALSE) {
            $sql .= $function['FUNCTION']['function'].'(';
            $sql .= $this->separateFields($args);
            $sql .= ')';
        } else {
            $sql .= $this->$fnHandler($args);
        }

        if (isset($function['FUNCTION']['alias']) === TRUE) {
            $sql .= ' as '.$function['FUNCTION']['alias'];
        }

        return $sql;

    }//end convertSingleFunction()


    /**
     * Converts From clause to its SQL format.
     *
     * @param array   $from  The from clause array.
     * @param integer $level Number of levels to indent.
     *
     * @return string
     */
    protected function convertFromList(array $from, $level=1)
    {
        $sql = "\n".$this->addSpaces($level);
        if ((isset($from['UNION']) !== TRUE)
            && (isset($from['UNION-ALL']) !== TRUE)
        ) {
            $tables    = array();
            $subTables = '';
            foreach ($from as $table) {
                $newTable = '';
                if (is_array($table) === TRUE && isset($table['FROM']) === TRUE) {
                    $subTables = $this->convertFromList($table['FROM']);
                    if ($subTables !== '') {
                        $subTables = ','.$subTables;
                    }
                } else {
                    if (is_array($table) === TRUE) {
                        if (isset($table['name']) === TRUE) {
                            // This is could be a derived table.
                            if (is_array($table['name']) === TRUE
                                && isset($table['name']['SELECT']) === TRUE
                            ) {
                                $newTable = '('.$this->convertSelectQuery($table['name']).')';
                            } else {
                                $newTable = $table['name'];
                            }

                            // Append the alias.
                            if (isset($table['alias']) === TRUE) {
                                $newTable .= $this->convertTableAlias($table['alias']);
                            }
                        } else if (isset($table['FUNCTION']) === TRUE) {
                            $newTable = $this->convertSingleFunction($table);
                        }
                    } else {
                        $newTable = $table;
                    }//end if

                    $tables[] = $newTable;
                    if (strpos($newTable, 'HOOKID') === 0) {
                        $this->_hookStrings[', '.$newTable] = '';
                    }
                }//end if
            }//end foreach

            $sql .= $this->separateTables($tables).$subTables;
        } else {
            // We have unions in from clause.
            $sql .= $this->convertUnionsInFromClause($from);
        }//end if

        return $sql;

    }//end convertFromList()


    /**
     * Converts given alias string to table alias.
     *
     * @param string $alias Alias of the table.
     *
     * @return array
     */
    protected function convertTableAlias($alias)
    {
        return ' as '.$alias;

    }//end convertTableAlias()


    /**
     * Converts a math operation block to its SQL format.
     *
     * This function will call it self when there are multiple operations.
     * For example: ((x + y) - z). Math ops can contain fields, values, function
     * calls and hooks.
     *
     * @param array $mathOp The math op block.
     *
     * @return string
     */
    protected function convertMathOperation(array $mathOp)
    {
        // If this is the begining tag then jump to the 1st op.
        $alias = '';
        if (isset($mathOp['MATH-OP']) === TRUE) {
            if (isset($mathOp['MATH-OP']['alias']) === TRUE) {
                $alias = ' as '.$mathOp['MATH-OP']['alias'];
            }

            $mathOp = $mathOp['MATH-OP'][0];
        }

        $operations = array();
        foreach ($mathOp as $currentOp => $args) {
            if (is_array($args) === TRUE) {
                foreach ($args as $value) {
                    $field = TRUE;
                    if (is_array($value) === TRUE) {
                        $keys = array_keys($value);
                        $op   = array_shift($keys);
                        if (isset($this->mathOps[$op]) === TRUE) {
                            $field        = FALSE;
                            $operations[] = $this->convertMathOperation($value);
                        }
                    }

                    if ($field === TRUE) {
                        if (is_array($value) === TRUE
                            && isset($value['SELECT']) === TRUE
                        ) {
                            $operations[] = '('.$this->convertSelectQuery($value).')';
                        } else {
                            $operations[] = $this->convertSingleField($value);
                        }
                    }
                }//end foreach
            }//end if
        }//end foreach

        $sql = '('.implode(' '.$this->mathOps[$currentOp].' ', $operations).')';
        // If we have a alias then we should have something like (3+4) as ALIAS.
        $sql .= $alias;

        return $sql;

    }//end convertMathOperation()


    /**
     * Separates arary of fields/values and returns its SQL string.
     *
     * @param array $fields Array of fields to separate.
     *
     * @return string
     */
    protected function separateFields(array $fields)
    {
        $sFields = implode(', ', $fields);
        return $sFields;

    }//end separateFields()


    /**
     * Separates arary of tables and returns its SQL string.
     *
     * @param array $tables Array of fields to separate.
     *
     * @return string
     */
    protected function separateTables(array $tables)
    {
        $sTables = implode(', ', $tables);
        return $sTables;

    }//end separateTables()


    /**
     * Converts Where clause to its SQL format.
     *
     * @param array   $where       The where clasue array.
     * @param integer $level       Number of levels to indent.
     * @param boolean $addBrackets Add brackets around first where condition.
     *
     * @return string
     * @throws DALConverterException Fails to convert where clause.
     */
    protected function convertWhereClause(array $where, $level=0, $addBrackets=TRUE)
    {
        $sql = $this->addSpaces($level);
        if ($addBrackets === TRUE) {
            $sql .= "(\n";
        }

        $wc = $this->getComparisonOperators();
        foreach ($where as $condType => $conditions) {
            $beg = TRUE;
            foreach ($conditions as $condition) {
                $sql .= str_repeat(' ', (($level + 1) * 4));
                if ($beg === FALSE) {
                    $sql .= $condType.' ';
                } else {
                    $beg = FALSE;
                }

                if (is_array($condition) === FALSE) {
                    // This is a hook.
                    $sql .= '('.$condition.')';
                    $this->_hookStrings[$condition] = '1=1';
                } else if (isset($condition['IN']) === TRUE) {
                    // Get IN clause.
                    $sql .= $this->convertInCondition($condition, ($level + 1));
                } else if (isset($condition['NOT-IN']) === TRUE) {
                    // Get NOT IN clause.
                    $sql .= $this->convertInCondition(
                        $condition,
                        ($level + 1),
                        'NOT-IN'
                    );
                } else if (isset($condition['EXISTS']) === TRUE) {
                    $sql .= $this->convertExistsCondition($condition, ($level + 1));
                } else if (isset($condition['NOT-EXISTS']) === TRUE) {
                    $sql .= $this->convertExistsCondition(
                        $condition,
                        ($level + 1),
                        'NOT-EXISTS'
                    );
                } else if (isset($condition['OR']) === TRUE
                    || isset($condition['AND']) === TRUE
                ) {
                    // Get OR/AND conditions.
                    $sql .= "\n".$this->convertWhereClause($condition, ($level + 1));
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'is-null')
                ) {
                    $sql .= $this->convertIsNull($condition);
                    $sql .= "\n";
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'not-null')
                ) {
                    $sql .= $this->convertNotNull($condition);
                    $sql .= "\n";
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'like')
                ) {
                    $sql .= $this->convertLikeCondition($condition);
                    $sql .= "\n";
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'not-like')
                ) {
                    $sql .= $this->convertNotLikeCondition($condition);
                    $sql .= "\n";
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'similar-to')
                ) {
                    $sql .= $this->convertSimilarToCondition($condition);
                    $sql .= "\n";
                } else if ((isset($condition['type']) === TRUE)
                    && ($condition['type'] === 'not-similar-to')
                ) {
                    $sql .= $this->convertNotSimilarToCondition($condition);
                    $sql .= "\n";
                } else if (isset($condition['compare']) === TRUE) {
                    // Basic condition block.
                    $sql .= $this->convertSingleField($condition['compare']);
                    $sql .= ' '.$wc[$condition['type']];
                    $sql .= ' ('.$this->convertSingleField($condition['to']).')';
                    $sql .= "\n";
                } else if (isset($condition['NOT']) === TRUE) {
                    $sql .= $this->convertNot($condition);
                } else if (isset($condition['KEYWORD']) === TRUE) {
                    $sql .= '['.$condition['KEYWORD'].']';
                } else {
                    $msg = 'Unable to convert WHERE/HAVING condition.';
                    throw new DALConverterException($msg);
                }//end if
            }//end foreach
        }//end foreach

        if ($addBrackets === TRUE) {
            $sql .= str_repeat(' ', ($level * 4)).")\n";
        }

        return $sql;

    }//end convertWhereClause()


    /**
     * Converts IN condition to its SQL format.
     *
     * @param array   $in    The in condition array.
     * @param integer $level Number of levels to indent.
     * @param string  $type  Type of in condition (in/not-in).
     *
     * @return string
     */
    protected function convertInCondition(array $in, $level=0, $type='IN')
    {
        $sql = '';

        // Check if this is a complex in query with left and right cond. specified.
        if (isset($in[$type]['LEFT']) === TRUE
            && isset($in[$type]['RIGHT']) === TRUE
        ) {
            $conditions = array();
            foreach ($in[$type]['LEFT'] as $condition) {
                $conditions[] = $this->convertSingleField($condition);
            }

            $sql .= $this->separateFields($conditions);
            $sql .= ' '.$this->getConstructName(strtolower($type)).'(';

            $conditions = array();
            foreach ($in[$type]['RIGHT'] as $condition) {
                $conditions[] = $this->convertSingleField($condition);
            }

            $sql .= $this->separateFields($conditions);

            $sql .= ')';

            return $sql;
        }//end if

        $tables  = explode(',', $in[$type]['table']);
        $columns = explode(',', $in[$type]['column']);

        $singleTable = '';
        if (count($tables) === 1) {
            $singleTable = $in[$type]['table'].'.';
        }//end if

        if (count($columns) > 1) {
            $sql .= '(';
            foreach ($columns as $index => $column) {
                if (empty($singleTable) === TRUE) {
                    if (isset($tables[$index]) === TRUE) {
                        $table = $tables[$index].'.';
                    } else {
                        $table = '';
                    }
                } else {
                    $table = $singleTable;
                }

                $sql .= $table.trim($column);
                if ($index !== (count($columns) - 1)) {
                    $sql .= ', ';
                }
            }//end foreach

            $sql .= ')';
        } else if (empty($in[$type]['table']) === TRUE
            && empty($in[$type]['column']) === TRUE
            && empty($in[$type]['CONDITIONS']) === FALSE
        ) {
            // Use the first condition as left value since no column is provided.
            // E.g. condition-0 IN (con-1, con-2, ..)
            // or as xml <in><value>:values</value><field... /></in>.
            $sql .= $this->convertSingleField($in[$type]['CONDITIONS'][0]);
            array_shift($in[$type]['CONDITIONS']);
        } else {
            $sql .= $in[$type]['table'].'.'.$in[$type]['column'];
        }//end if

        $sql .= ' '.$this->getConstructName(strtolower($type)).'(';

        // Get the elements of IN().
        if (isset($in[$type]['CONDITIONS']['SELECT']) === TRUE) {
            $sql .= "\n".$this->convertSelectQuery(
                $in[$type]['CONDITIONS'],
                ($level + 1)
            );
            $sql .= str_repeat(' ', ($level * 4));
        } else if (isset($in[$type]['CONDITIONS']['EXCEPT']) === TRUE
            && is_array($in[$type]['CONDITIONS']['EXCEPT']) === TRUE
        ) {
            $sql .= "\n".$this->convertExcepts(
                $in[$type]['CONDITIONS']['EXCEPT'],
                'EXCEPT',
                ($level + 1)
            );

            $sql .= str_repeat(' ', ($level * 4));
        } else if (isset($in[$type]['CONDITIONS']['INTERSECT']) === TRUE
            && is_array($in[$type]['CONDITIONS']['INTERSECT']) === TRUE
        ) {
            $sql .= "\n".$this->convertIntersect(
                $in[$type]['CONDITIONS']['INTERSECT'],
                ($level + 1)
            );

            $sql .= str_repeat(' ', ($level * 4));
        } else {
            $conditions = array();
            foreach ($in[$type]['CONDITIONS'] as $condition) {
                $conditions[] = $this->convertSingleField($condition);
            }

            $sql .= $this->separateFields($conditions);
        }//end if

        $sql .= ")\n";

        return $sql;

    }//end convertInCondition()


    /**
     * Converts EXISTS condition to its SQL format.
     *
     * @param array   $exists The exists condition array.
     * @param integer $level  Number of levels to indent.
     * @param string  $type   The type of exists condition (exists/not-exists).
     *
     * @return string
     */
    protected function convertExistsCondition(
        array $exists,
        $level=0,
        $type='EXISTS'
    ) {
        $sql  = '';
        $sql .= $this->getConstructName(strtolower($type))."(\n";
        $sql .= $this->convertSelectQuery(
            $exists[$type]['CONDITIONS'],
            ($level + 1)
        );
        $sql .= str_repeat(' ', ($level * 4)).")\n";

        return $sql;

    }//end convertExistsCondition()


    /**
     * Converts IS NULL statament.
     *
     * @param array $isNull IS NULL array.
     *
     * @return string
     * @throws DALConverterException Is-null value is empty.
     */
    protected function convertIsNull(array $isNull)
    {
        $sql = '';
        if (isset($isNull['compare']) === TRUE) {
            if (empty($isNull['compare']) === FALSE) {
                $sql .= $this->convertSingleField($isNull['compare']);
                $sql .= ' IS NULL';
            } else {
                $msg = 'is-null value cannot be empty.';
                throw new DALConverterException($msg);
            }
        } else {
            $msg = 'Found is-null tag with no value.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertIsNull()


    /**
     * Converts NOT NULL statament.
     *
     * @param array $notNull IS NULL array.
     *
     * @return string
     * @throws DALConverterException Not null value is empty.
     */
    protected function convertNotNull(array $notNull)
    {
        $sql = '';
        if (isset($notNull['compare']) === TRUE) {
            if (empty($notNull['compare']) === FALSE) {
                $sql .= $this->convertSingleField($notNull['compare']);
                $sql .= ' IS NOT NULL';
            } else {
                $msg = 'not-null value cannot be empty.';
                throw new DALConverterException($msg);
            }
        } else {
            $msg = 'Found not-null tag with no value.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertNotNull()


    /**
     * Converts NOT statament.
     *
     * E.g. ... AND NOT (col1 = 0 AND col2 = 0).
     * XML: <not><and><equal...><not-equal...></and></not>.
     *
     * @param array $not Condition.
     *
     * @return string
     */
    protected function convertNot(array $not)
    {
        $sql  = 'NOT ';
        $sql .= $this->convertWhereClause($not['NOT'][0]);

        return $sql;

    }//end convertNot()


    /**
     * Convert list of union that are in side a FROM clause.
     *
     * @param array   $union Array of selects in union/union-all.
     * @param integer $level Number of levels to indent.
     *
     * @return string
     */
    protected function convertUnionsInFromClause(array $union, $level=0)
    {
        $type = $this->getUnionType($union);
        $sql  = "(\n".$this->addSpaces($level);
        $sql .= $this->convertUnions($union, ($level + 2));
        $sql .= ') AS '.$union[$type]['alias'];
        return $sql;

    }//end convertUnionsInFromClause()


    /**
     * Returns the type of the union.
     *
     * @param array $union Array of a union/union-all.
     *
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
     * Convert list of union and union-all.
     *
     * @param array   $union Array of selects in union/union-all.
     * @param integer $level Number of levels to indent.
     *
     * @return string
     */
    protected function convertUnions(array $union, $level=1)
    {
        $sql         = '';
        $type        = $this->getUnionType($union);
        $displayType = $this->convertUnionDisplayName($type);
        if (isset($union[$type]['SELECTS']) === TRUE) {
            $selectSqls = array();
            foreach ($union[$type]['SELECTS'] as $select) {
                if (is_array($select) === TRUE) {
                    $selectSqls[] = $this->convertSelectQuery($select, $level);
                } else {
                    $selectSqls[] = $this->addSpaces($level).$select."\n";
                }
            }

            $spaces = "\n".$this->addSpaces($level);
            $sql   .= implode($spaces.$displayType."\n\n", $selectSqls);
        }

        return $sql;

    }//end convertUnions()


    /**
     * Convert the display name of union and union-all.
     *
     * If the DB needs to display, in the SQL, something other than UNION or
     * UNION-ALL (eg. DB2 needs UNION ALL), then return it here.
     *
     * @param string $type The type of the union (UNION or UNION-ALL).
     *
     * @return string
     */
    protected function convertUnionDisplayName($type)
    {
        return $type;

    }//end convertUnionDisplayName()


    /**
     * Convert list of except and except-all.
     *
     * @param array   $except Array of selects in except/except-all.
     * @param string  $type   EXCEPT or EXCEPT-ALL.
     * @param integer $level  Number of levels to indent.
     *
     * @return string
     */
    protected function convertExcepts(array $except, $type, $level=1)
    {
        $sql = '';
        if (isset($except['SELECTS']) === TRUE) {
            $selectSqls = array();
            foreach ($except['SELECTS'] as $select) {
                if (is_array($select) === TRUE) {
                    $selectSqls[] = $this->convertSelectQuery($select, $level);
                } else {
                    $selectSqls[] = $this->addSpaces($level).$select."\n";
                }
            }

            $spaces = "\n".$this->addSpaces($level);
            $sql   .= implode($spaces.$type."\n\n", $selectSqls);
        }

        return $sql;

    }//end convertExcepts()


    /**
     * Convert list of intersect.
     *
     * @param array   $intersect Array of selects in intersect.
     * @param integer $level     Number of levels to indent.
     *
     * @return string
     */
    protected function convertIntersect(array $intersect, $level=1)
    {
        $sql  = '';
        $type = 'INTERSECT';
        if (isset($intersect['SELECTS']) === TRUE) {
            $selectSqls = array();
            foreach ($intersect['SELECTS'] as $select) {
                if (is_array($select) === TRUE) {
                    $selectSqls[] = $this->convertSelectQuery($select, $level);
                } else {
                    $selectSqls[] = $this->addSpaces($level).$select."\n";
                }
            }

            $spaces = "\n".$this->addSpaces($level);
            $sql   .= implode($spaces.$type."\n\n", $selectSqls);
        }

        return $sql;

    }//end convertIntersect()


    /**
     * Converts GROUP BY clause.
     *
     * @param array $groupBy Single GROUP BY clause.
     *
     * @return string
     */
    protected function convertGroupBy(array $groupBy)
    {
        $sql  = 'GROUP BY ';
        $list = array();
        foreach ($groupBy as $field) {
            if (is_array($field) === TRUE) {
                $list[] = $this->convertSingleField($field);
            } else {
                $list[] = $field;
            }
        }

        $sql .= $this->separateFields($list);

        return $sql;

    }//end convertGroupBy()


    /**
     * Converts ORDER BY clauses.
     *
     * @param array $orderBys Multiple ORDER BY clauses.
     *
     * @return string
     */
    protected function convertOrderBy(array $orderBys)
    {
        $sql = 'ORDER BY ';

        foreach ($orderBys as $orderBy) {
            $list      = array();
            $direction = '';
            if (isset($orderBy['direction']) === TRUE) {
                $direction = $orderBy['direction'];
                unset($orderBy['direction']);
            }

            foreach ($orderBy as $key => $field) {
                $newItem = '';
                if (is_array($field) === TRUE) {
                    $newItem = $this->convertSingleField($field);
                } else {
                    $newItem = $field;
                }

                if (strpos($direction, 'abs-') === 0) {
                    $direction = str_replace('abs-', '', $direction);
                    $newItem   = 'abs('.$newItem.')';
                }

                $list[] = $newItem;
            }

            $sql .= $this->separateFields($list).' '.$direction.', ';
        }//end foreach

        $sql = rtrim($sql, ' ,');
        return $sql;

    }//end convertOrderBy()


    /**
     * Converts LIMIT.
     *
     * @param array $limit Single LIMIT clause.
     *
     * @return string
     */
    protected function convertLimit(array $limit)
    {
        $sql  = 'LIMIT ';
        $sql .= $limit['rows'].' OFFSET '.$limit['offset'];

        return $sql;

    }//end convertLimit()


    /**
     * Converts array of joins to their SQL form.
     *
     * @param array $joins Array of joins.
     * @param array $from  The FROM clause in the original query.
     *
     * @return string
     */
    protected function convertJoins(array $joins, array $from)
    {
        $sql   = '';
        $list  = array();
        $count = 1;
        foreach ($joins as $join) {
            $list[] = $this->convertSingleJoin($join, $count, $from);
            $count++;
        }

        $sql = implode("\n", $list);

        return $sql;

    }//end convertJoins()


    /**
     * Converts a single join.
     *
     * @param array   $join  Single join array.
     * @param integer $count Count of the join in the table.
     * @param array   $from  The FROM clause in the original query.
     *
     * @return string
     */
    protected function convertSingleJoin(array $join, $count=1, array $from=array())
    {
        $sql   = '';
        $cross = FALSE;
        $exist = FALSE;
        $names = $this->getFromTableNames($from);

        // Special Case, where multiple joins require a cross join to the original query.
        // eg. 'SELECT * FROM table1 INNER JOIN table2 ON table1.id=table2.id, table3 INNER JOIN table4 ON table3.id=table4'.
        if (isset($join['cross']) === TRUE && strtolower($join['cross']) === 'true') {
            $cross = TRUE;
        }

        // Does the first table exist?
        if (isset($join['ARGS'][0]['table']) === TRUE && in_array($join['ARGS'][0]['table'], $names) === TRUE) {
            $exist = TRUE;
        }

        if (isset($join['ARGS']) === TRUE) {
            // Not the first time this has been called, we *possibly* need a table prefix.
            if ($count > 1) {
                if ($cross === TRUE && $exist === TRUE) {
                    $sql .= $join['ARGS'][0]['table'].' AS '.$join['ARGS'][0]['table'].$count.' ';
                } else if ($cross === TRUE && $exist === FALSE) {
                    $sql .= $join['ARGS'][0]['table'].' ';
                }
            }

            $joinType = strtoupper($join['type']);
            if ($joinType === '') {
                $joinType = 'INNER JOIN';
            }

            $sql .= $joinType.' ';
            $sql .= $join['ARGS'][1]['table'].' ';

            // Do not write ON clause if this is a Natural or Cross join.
            if ($joinType !== 'NATURAL JOIN' || $joinType === 'CROSS JOIN') {
                $sql .= ' ON ';
                $sql .= $join['ARGS'][0]['table'];
                if ($cross === TRUE && $exist === TRUE && $count > 1) {
                    // This is a special case, that requires an alias.
                    $sql .= $count;
                }

                $sql .= '.'.$join['ARGS'][0]['column'];
                $sql .= ' = ';
                $sql .= $join['ARGS'][1]['table'].'.'.$join['ARGS'][1]['column'];
            }

            // Special Case, where multiple joins require a cross join to the original query.
            if ($cross === TRUE) {
                $sql = 'CROSS JOIN ('.$sql.')';
            }
        }//end if

        return $sql;

    }//end convertSingleJoin()


    /**
     * Get a list of names from the FROM clause.
     *
     * @param array $from The array of the FROM clause.
     *
     * @return array
     */
    protected function getFromTableNames(array $from)
    {
        $names = array();
        if (empty($from) === FALSE) {
            foreach ($from as $table) {
                if (is_array($table) === TRUE && isset($table['FROM']) === TRUE) {
                    $subTables = $this->getFromTableNames($table['FROM']);
                    if (empty($subTables) === FALSE) {
                        $names = array_merge($names, $subTables);
                    }
                } else {
                    if (is_array($table) === TRUE) {
                        if (isset($table['name']) === TRUE) {
                            $names[] = $table['name'];
                            if (isset($table['alias']) === TRUE) {
                                $names[] = $table['alias'];
                            }
                        }
                    } else {
                        $names[] = $table;
                    }//end if
                }//end if
            }//end foreach
        }//end if

        return $names;

    }//end getFromTableNames()


    /*
        Convert Insert Query
    */


    /**
     * Converts Insert query to its SQL format.
     *
     * @param array $sqlArray The insert query.
     *
     * @return string
     */
    protected function convertInsertQuery(array $sqlArray)
    {
        $sql  = 'INSERT INTO '.$sqlArray['INSERT']['into'];
        $sql .= $this->convertInsertQueryFields($sqlArray['INSERT']['FIELDS']);
        $sql .= "\n";
        $sql .= $this->convertInsertQueryValues($sqlArray['INSERT']);
        return $sql;

    }//end convertInsertQuery()


    /**
     * Returns string of spaces.
     *
     * @param integer $level  Number of levels to indent.
     * @param integer $spaces Number of spaces for each indent level.
     *
     * @return string
     */
    protected function addSpaces($level=0, $spaces=4)
    {
        $str = str_repeat(' ', ($level * 4));
        return $str;

    }//end addSpaces()


    /**
     * Converts Insert query's fields to their SQL format.
     *
     * @param array $fields Array of fields.
     *
     * @return string
     */
    protected function convertInsertQueryFields(array $fields)
    {
        $sql = '';
        if (empty($fields) === FALSE) {
            $sql = '('.$this->separateFields($fields).') ';
        }

        return $sql;

    }//end convertInsertQueryFields()


    /**
     * Converts Insert query's values to their SQL format.
     *
     * Converting values require the fields array. Because, fields list and
     * values list must be in the same order.
     *
     * @param array $insert Array of the insert query.
     *
     * @return string
     * @throws DALConverterException If field is missing.
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

                $valuesList[] = '('.$this->convertSingleField($value).')';
            }

            $sql .= 'VALUES ('.$this->separateFields($valuesList).') ';
        } else {
            // Select statement.
            $sql .= '('.$this->convertSelectQuery($insert['VALUES']).')';
        }

        return $sql;

    }//end convertInsertQueryValues()


    /*
        Convert Update Query
    */


    /**
     * Converts update query to its SQL string.
     *
     * @param array $sqlArray Delete query array.
     *
     * @return string
     */
    protected function convertUpdateQuery(array $sqlArray)
    {
        $sql  = 'UPDATE '.$sqlArray['UPDATE']['table'];
        $sql .= "\n".'SET';
        $sql .= "\n".$this->convertUpdateQuerySetValues(
            $sqlArray['UPDATE']['VALUES']
        );

        if (isset($sqlArray['UPDATE']['FROM']) === TRUE) {
            $sql .= "\n".'FROM ';
            $sql .= $this->convertFromList($sqlArray['UPDATE']['FROM']);
        }

        if (isset($sqlArray['UPDATE']['WHERE']) === TRUE) {
            $sql .= "\n".'WHERE ';
            $sql .= $this->convertWhereClause($sqlArray['UPDATE']['WHERE']);
        }

        return $sql;

    }//end convertUpdateQuery()


    /**
     * Converts update query's set values to its SQL format.
     *
     * @param array $values Array of values to set.
     *
     * @return string
     */
    protected function convertUpdateQuerySetValues(array $values)
    {
        $vals = array();
        foreach ($values as $col => $val) {
            $val    = $this->convertSingleField($val);
            $vals[] = $col.' = '.$val;
        }

        $spaces = $this->addSpaces(1);
        $sql    = $spaces.implode(",\n".$spaces, $vals);

        return $sql;

    }//end convertUpdateQuerySetValues()


    /*
        Convert Delete Query
    */


    /**
     * Converts delete query to its SQL string.
     *
     * @param array $sqlArray Delete query array.
     *
     * @return string
     */
    protected function convertDeleteQuery(array $sqlArray)
    {
        $sql  = 'DELETE FROM '.$sqlArray['DELETE']['from'];
        $sql .= "\n".$this->getConstructName('where');
        $sql .= $this->convertWhereClause($sqlArray['DELETE']['WHERE']);

        return $sql;

    }//end convertDeleteQuery()


    /*
        Convert ALTER TABLE Query
    */


    /**
     * Starts the ALTER query statement.
     *
     * @param string $tableName Name of the table to alter.
     *
     * @return string
     */
    protected function convertAlterQueryHeader($tableName)
    {
        $sql = 'ALTER TABLE '.$tableName."\n";
        return $sql;

    }//end convertAlterQueryHeader()


    /**
     * Converts ALTER TABLE statement to its SQL form.
     *
     * @param array $sqlArray Single ALTER TABLE query.
     *
     * @return string
     */
    protected function convertAlterQuery(array $sqlArray)
    {
        $sql = $this->convertAlterQueryHeader($sqlArray['ALTER']['table']);

        foreach ($sqlArray['ALTER'] as $key => $value) {
            if ($key !== 'table') {
                $key       = ucwords(str_replace('-', ' ', strtolower($key)));
                $alterType = 'convertAlterQuery'.str_replace(' ', '', $key);
                $sql      .= $this->$alterType($value);
            }
        }

        return $sql;

    }//end convertAlterQuery()


    /**
     * Converts ALTER TABLE ADD COLUMN statement to its SQL form.
     *
     * Throws DALConverterException if no columns found.
     *
     * @param array $addColumn ADD COLUMN array.
     *
     * @return string
     * @throws DALConverterException If columns not specified.
     */
    protected function convertAlterQueryAddColumn(array $addColumn)
    {
        $sql = 'ADD (';

        if (isset($addColumn['COLUMNS']) === TRUE) {
            $cols = array();
            foreach ($addColumn['COLUMNS'] as $column) {
                $cols[] = $this->convertSingleCreateColumn($column);
            }

            $sql .= implode(', ', $cols);
        } else {
            $msg = 'Cannot convert ALTER TABLE ADD COLUMN without columns.';
            throw new DALConverterException($msg);
        }

        $sql .= ')';

        return $sql;

    }//end convertAlterQueryAddColumn()


    /**
     * Converts ALTER TABLE DROP COLUMN statement to its SQL format.
     *
     * Throws DALConverterException if no column name given.
     *
     * @param string $column Column name.
     *
     * @return string
     * @throws DALConverterException If column name is not specified.
     */
    protected function convertAlterQueryDropColumn($column)
    {
        $sql = 'DROP COLUMN ';
        if (($column !== '') && (is_string($column) === TRUE)) {
            $sql .= $column;
        } else {
            $msg = 'Cannot convert ALTER TABLE DROP COLUMN without column name.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertAlterQueryDropColumn()


    /**
     * Converts ALTER TABLE RENAME COLUMN statement to its SQL format.
     *
     * Throws DALConverterException if old column name or new column name is not
     * provided.
     *
     * @param array $renameColumn Old and New column names.
     *
     * @return string
     * @throws DALConverterException If old or new column name is not specified.
     */
    protected function convertAlterQueryRenameColumn(array $renameColumn)
    {
        $sql = 'RENAME COLUMN ';
        if ((isset($renameColumn['OLD']) === TRUE)
            && (isset($renameColumn['NEW'])) === TRUE
        ) {
            $sql .= $renameColumn['OLD'].' TO '.$renameColumn['NEW'];
        } else {
            $msg = 'Cannot convert ALTER TABLE RENAME COLUMN.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertAlterQueryRenameColumn()


    /**
     * Converts ALTER TABLE MODIFY COLUMN statement to its SQL format.
     *
     * Throws DALConverterException if no columns provided.
     *
     * @param array $modifyColumns Columns to be modified.
     *
     * @return string
     * @throws DALConverterException If columns not specified.
     */
    protected function convertAlterQueryModifyColumn(array $modifyColumns)
    {
        $sql = 'MODIFY (';

        if (isset($modifyColumns['COLUMNS']) === TRUE) {
            $cols = array();
            foreach ($modifyColumns['COLUMNS'] as $column) {
                $cols[] = $this->convertSingleCreateColumn($column);
            }

            $sql .= implode(', ', $cols);
        } else {
            $msg = 'Cannot convert ALTER TABLE MODIFY COLUMN without columns.';
            throw new DALConverterException($msg);
        }

        $sql .= ')';

        return $sql;

    }//end convertAlterQueryModifyColumn()


    /**
     * Converts ALTER TABLE ADD CONSTRAINT statement to its SQL format.
     *
     * Returns empty string if no constraints found.
     *
     * @param array  $constraints List of constraints.
     * @param string $tableName   Name of the table to alter.
     *
     * @return string
     */
    protected function convertAlterQueryAddConstraint(
        array $constraints,
        $tableName=NULL
    ) {
        $sql = '';
        if (empty($constraints) === FALSE) {
            if ($tableName !== NULL) {
                $sql .= $this->convertAlterQueryHeader($tableName)."\n";
            }

            $sql .= 'ADD ';
            $sql .= $this->convertConstraints($constraints);
        }

        return $sql;

    }//end convertAlterQueryAddConstraint()


    /**
     * Converts ALTER TABLE DROP CONSTRAINT to its SQL format.
     *
     * Throws DALConverterException if no column name given.
     *
     * @param string $dropConstraint Name of the constraint to drop.
     *
     * @return string
     * @throws DALConverterException If constraint name is empty.
     */
    protected function convertAlterQueryDropConstraint($dropConstraint)
    {
        $sql = '';
        if ($dropConstraint !== '') {
            $sql .= 'DROP '.$dropConstraint;
        } else {
            $msg = 'Cannot convert ALTER TABLE DROP CONSTRAINT without constraint name.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertAlterQueryDropConstraint()


    /**
     * Converts ALTER TABLE RENAME TABLE statement to its SQL format.
     *
     * Throws DALConverterException if old column name or new column name is not
     * provided.
     *
     * @param array $renameTable Old and New table name.
     *
     * @return string
     * @throws DALConverterException If old or new name is not specified.
     */
    protected function convertAlterQueryRenameTable(array $renameTable)
    {
        $sql = 'RENAME ';
        if ((isset($renameTable['OLD']) === TRUE)
            && (isset($renameTable['NEW'])) === TRUE
        ) {
            $sql .= $renameTable['OLD'].' TO '.$renameTable['NEW'];
        } else {
            $msg = 'Cannot convert ALTER TABLE RENAME.';
            throw new DALConverterException($msg);
        }

        return $sql;

    }//end convertAlterQueryRenameTable()


    /*
        Convert Schema
    */


    /**
     * Converts create statement to SQL string.
     *
     * @param array $table CREATE array.
     *
     * @return string
     */
    public function convertCreateTable(array $table)
    {
        $sql  = $this->convertCreateHeader($table);
        $sql .= "\n".$this->convertCreateColumns($table);

        $constraints = $this->convertConstraints($table['CONSTRAINTS']);
        if ($constraints !== '') {
            $sql .= ",\n".$constraints;
            $sql .= $this->convertCreateFooter($table);
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
     * Returns the start of the create table statement.
     *
     * Postgres: CREATE TABLE assets (.
     *
     * @param array $table CREATE array.
     *
     * @return string
     */
    protected function convertCreateHeader(array $table)
    {
        $sql  = 'CREATE TABLE '.$table['table'];
        $sql .= ' (';
        return $sql;

    }//end convertCreateHeader()


    /**
     * Returns the start of the create table statement.
     *
     * Create stetement close bracket and semi-colon. Postgres: );.
     *
     * @param array $table CREATE array.
     *
     * @return string
     */
    protected function convertCreateFooter(array $table)
    {
        $sql = ');';
        return $sql;

    }//end convertCreateFooter()


    /**
     * Converts array of create clause columns to their SQL format.
     *
     * @param array $table The table array.
     *
     * @return string
     */
    protected function convertCreateColumns(array $table)
    {
        $sql = array();
        foreach ($table['COLUMNS'] as $col) {
            $sql[] = $this->convertSingleCreateColumn($col);
        }

        $sql = implode(",\n", $sql);

        return $sql;

    }//end convertCreateColumns()


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
     * @return string
     */
    protected function convertSingleCreateColumn(array $col)
    {
        // Add column's name.
        $sql = $col['name'].' '.$this->getDataType($col['type']);

        // Add the size to the end of the type.
        if ($col['size'] !== '') {
            $sql .= '('.$col['size'];
            if ($col['scale'] !== '') {
                $sql .= ','.$col['scale'];
            }

            $sql .= ')';
        }

        // Add column's default value if it has one.
        if ($col['default'] !== '') {
            $sql .= ' DEFAULT '.$col['default'];
        }

        // Add NOT NULL if allow-null is set to false.
        if ($col['allow-null'] === 'false') {
            $sql .= ' NOT NULL';
        } else {
            $sql .= ' NULL';
        }

        return $sql;

    }//end convertSingleCreateColumn()


    /**
     * Converts constraints list to their SQL format.
     *
     * Each constraint type (PK, FK, UQ) has its own method to convert
     * a single constraint. This function is used by CREATE TABLE and
     * ALTER TABLE CONSTRAINTS.
     *
     * @param array   $constraintsList The constraints array.
     * @param boolean $asArray         If true result will be array of strings.
     *
     * @return string
     */
    protected function convertConstraints(array $constraintsList, $asArray=FALSE)
    {
        $sql         = '';
        $constraints = array();

        // Primary keys.
        foreach ($constraintsList['PRIMARY-KEYS'] as $pk) {
            $constraints[] = $this->convertSinglePrimaryKey($pk);
        }

        // Foreign keys.
        if (isset($constraintsList['FOREIGN-KEYS']) === TRUE) {
            foreach ($constraintsList['FOREIGN-KEYS'] as $fk) {
                $constraints[] = $this->convertSingleForeignKey($fk);
            }
        }

        if (isset($constraintsList['UNIQUES']) === TRUE) {
            foreach ($constraintsList['UNIQUES'] as $un) {
                $constraints[] = $this->convertSingleUnique($un);
            }
        }

        if ($asArray === FALSE) {
            $sql .= implode(",\n", $constraints);
            return $sql;
        } else {
            return $constraints;
        }

    }//end convertConstraints()


    /**
     * Converts single primary key constraint to its SQL string.
     *
     * Postgres: CONSTRAINT assets_table_pk PRIMARY KEY (assetid).
     *
     * @param array $pk Single primary key constraint.
     *
     * @return string
     */
    protected function convertSinglePrimaryKey(array $pk)
    {
        $sql  = 'CONSTRAINT '.$pk['name'].' PRIMARY KEY (';
        $sql .= $this->separateFields($pk['COLUMNS']).')';
        return $sql;

    }//end convertSinglePrimaryKey()


    /**
     * Converts single foreign key constraint to its SQL string.
     *
     * Postgres: CONSTRAINT links_table_fk FOREIGN KEY (assetid) REFERENCES
     * assets_table (assetid) ON DELETE CASCADE.
     *
     * @param array $fk Single foreign key constraint.
     *
     * @return string
     */
    protected function convertSingleForeignKey(array $fk)
    {
        $sql  = 'CONSTRAINT '.$fk['name'].' FOREIGN KEY (';
        $cols = array();
        $refs = array();
        foreach ($fk['COLUMNS'] as $col) {
            $cols[] = $col['name'];
            $refs[] = $col['references'];
        }

        $sql .= $this->separateFields($cols).') ';
        $sql .= 'REFERENCES '.$fk['table'].'(';
        $sql .= $this->separateFields($refs).')';
        if ($fk['on-delete'] !== 'NO ACTION') {
            $sql .= ' ON DELETE '.$fk['on-delete'];
        }

        return $sql;

    }//end convertSingleForeignKey()


    /**
     * Converts single unique constraint to its SQL string.
     *
     * Postgres: CONSTRAINT unique_uk UNIQUE (someid).
     *
     * @param array $un Single unique constraint.
     *
     * @return string
     */
    protected function convertSingleUnique(array $un)
    {
        $sql  = 'CONSTRAINT '.$un['name'].' UNIQUE (';
        $sql .= $this->separateFields($un['COLUMNS']).')';
        return $sql;

    }//end convertSingleUnique()


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
     * @return string
     */
    protected function convertSingleIndex(array $idx, $tableName)
    {
        $sql  = 'CREATE INDEX '.$idx['name'].' ON '.$tableName.' (';
        $sql .= $this->separateFields($idx['COLUMNS']).')';
        return $sql;

    }//end convertSingleIndex()


    /**
     * Converts list of SEQUENCE stataments.
     *
     * @param array $sequences Create sequence stataments.
     *
     * @return string
     */
    protected function convertCreateSequences(array $sequences)
    {
        $sql  = '';
        $seqs = array();
        if (empty($sequences) === FALSE) {
            foreach ($sequences as $sequence) {
                $seqs[] = $this->convertSingleSequence($sequence);
            }

            if (empty($seqs) === FALSE) {
                $sql = implode(";\n", $seqs);
            }
        }

        return $sql;

    }//end convertCreateSequences()


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
        $sql .= $sequence['name'];

        return $sql;

    }//end convertSingleSequence()


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
        $sql = 'DROP TABLE '.$tableName;
        if ($cascade === TRUE) {
            $sql .= ' CASCADE';
        }

        return $sql;

    }//end convertDropTable()


    /**
     * Converts TRUNCATE TABLE statement.
     *
     * @param string $tableName Name of the table to truncate.
     *
     * @return string
     */
    public function convertTruncateTable($tableName)
    {
        $sql = 'delete from '.$tableName;
        return $sql;

    }//end convertTruncateTable()


    /**
     * Converts DROP TABLE statement.
     *
     * @param string $sequenceName Name of the sequence to drop from the db.
     *
     * @return string
     */
    public function convertDropSequence($sequenceName)
    {
        $sql = 'DROP SEQUENCE '.$sequenceName;
        return $sql;

    }//end convertDropSequence()


    /**
     * Converts DROP TABLE statement.
     *
     * @param string $sequenceName Name of the sequence to drop from the db.
     *
     * @return string
     */
    public function convertResetSequence($sequenceName)
    {
        // Need to sort out the min_val from the sequence.
        $sql = "SELECT setval('".$sequenceName."', 1); ";
        return $sql;

    }//end convertResetSequence()


    /**
     * Converts a WITH query.
     *
     * @param array $withQuery Array containing the with query information.
     *
     * @return string
     */
    protected function convertWithQuery(array $withQuery)
    {
        $sql  = 'WITH ';
        $sql .= $this->convertSingleFunction($withQuery);
        $sql .= "\nAS\n(\n";
        $sql .= $this->convertUnions($withQuery);
        $sql .= "\n)\n";
        $sql .= $this->convertSelectQuery($withQuery['AFTER']);

        return $sql;

    }//end convertWithQuery()


    /**
     * Converts a 'CALL' query into SQL.
     *
     * @param array $callQuery The array containing the function structure.
     *
     * @return string
     */
    protected function convertCallQuery(array $callQuery)
    {
        $sql  = 'CALL ';
        $sql .= $this->convertSingleFunction($callQuery);
        return $sql;

    }//end convertCallQuery()


    /**
     * Handles table exists.
     *
     * @param (array|string) $tableName Table name argument.
     * @param string         $schema    The schema to check the table for.
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

        $sql  = 'SELECT count(*) FROM INFORMATION_SCHEMA.tables WHERE TABLE_NAME = ';
        $sql .= $tableName.' AND TABLE_SCHEMA = '.$schema;
        return $sql;

    }//end handleFunctionTableExists()


    /**
     * Handles the CONCAT() function call.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionConcat(array $args)
    {
        $sql = 'CONCAT('.implode(', ', $args).')';
        return $sql;

    }//end handleFunctionConcat()


    /**
     * Handles ARRAY cast function.
     *
     * @param array $args String to cast to array.
     *
     * @return string
     */
    protected function handleFunctionArray(array $args)
    {
        $sql = 'ARRAY('.$args[0].')';
        return $sql;

    }//end handleFunctionArray()


    /**
     * Handles LIKE conditions.
     *
     * @param array $condition The condition information.
     *
     * @return string
     */
    protected function convertLikeCondition(array $condition)
    {
        $compare = $this->convertSingleField($condition['compare']);
        $to      = $this->convertSingleField($condition['to']);
        $sql     = $compare.' LIKE '.$to;

        return $sql;

    }//end convertLikeCondition()


    /**
     * Handles NOT LIKE conditions.
     *
     * @param array $condition The condition information.
     *
     * @return string
     */
    protected function convertNotLikeCondition(array $condition)
    {
        $compare = $this->convertSingleField($condition['compare']);
        $to      = $this->convertSingleField($condition['to']);
        $sql     = $compare.' NOT LIKE '.$to;

        return $sql;

    }//end convertNotLikeCondition()


    /**
     * Handles SIMILAR TO conditions.
     *
     * @param array $condition The condition information.
     *
     * @return string
     */
    protected function convertSimilarToCondition(array $condition)
    {
        $compare = $this->convertSingleField($condition['compare']);
        $to      = $this->convertSingleField($condition['to']);
        $sql     = $compare.' SIMILAR TO '.$to;

        return $sql;

    }//end convertSimilarToCondition()


    /**
     * Handles NOT SIMILAR TO conditions.
     *
     * @param array $condition The condition information.
     *
     * @return string
     */
    protected function convertNotSimilarToCondition(array $condition)
    {
        $compare = $this->convertSingleField($condition['compare']);
        $to      = $this->convertSingleField($condition['to']);
        $sql     = $compare.' NOT SIMILAR TO '.$to;

        return $sql;

    }//end convertNotSimilarToCondition()


    /**
     * Handles the CAST() function call.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionCast(array $args)
    {
        $sql = 'CAST('.$args[0].' AS '.$this->getDataType($args[1]).')';
        return $sql;

    }//end handleFunctionCast()


    /**
     * Handles the BINDCAST() function call.
     *
     * BINDCAST is used to wrap bind vars. Default will only print the bind var
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionBindcast(array $args)
    {
        $sql = $args[0];
        return $sql;

    }//end handleFunctionBindcast()


    /**
     * Handles the LENGTH() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionLength(array $args)
    {
        $sql = 'LENGTH('.$this->convertSingleField($args[0]).')';
        return $sql;

    }//end handleFunctionLength()


    /**
     * Handles the COALESCE() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionCoalesce(array $args)
    {
        $sql    = 'COALESCE(';
        $fnArgs = array();
        foreach ($args as $arg) {
            $fnArgs[] = $this->convertSingleField($arg);
        }

        $sql .= implode(',', $fnArgs).')';

        return $sql;

    }//end handleFunctionCoalesce()


    /**
     * Handles the MAX() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionMax(array $args)
    {
        if (count($args) !== 1) {
            return '';
        }

        $sql   = 'MAX(';
        $fnArg = $this->convertSingleField($args[0]);
        $sql  .= $fnArg.')';

        return $sql;

    }//end handleFunctionMax()


    /**
     * Handles the MIN() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionMin(array $args)
    {
        if (count($args) !== 1) {
            return '';
        }

        $sql   = 'MIN(';
        $fnArg = $this->convertSingleField($args[0]);
        $sql  .= $fnArg.')';

        return $sql;

    }//end handleFunctionMin()


    /**
     * Handles the COUNT() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionCount(array $args)
    {
        if (count($args) !== 1) {
            return '';
        }

        $sql   = 'COUNT(';
        $fnArg = $this->convertSingleField($args[0]);
        $sql  .= $fnArg.')';

        return $sql;

    }//end handleFunctionCount()


    /**
     * Handles the SUM() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionSum(array $args)
    {
        if (count($args) !== 1) {
            return '';
        }

        $sql   = 'SUM(';
        $fnArg = $this->convertSingleField($args[0]);
        $sql  .= $fnArg.')';

        return $sql;

    }//end handleFunctionSum()


    /**
     * Handles the REPLACE() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionReplace(array $args)
    {
        $sql    = 'REPLACE(';
        $fnArgs = array();
        foreach ($args as $arg) {
            $fnArgs[] = $this->convertSingleField($arg);
        }

        $sql .= implode(',', $fnArgs).')';

        return $sql;

    }//end handleFunctionReplace()


    /**
     * Handles the lower() function.
     *
     * @param array $args Method arguments.
     *
     * @return string
     */
    protected function handleFunctionLower(array $args)
    {
        $sql    = 'lower(';
        $fnArgs = array();
        foreach ($args as $arg) {
            $fnArgs[] = $this->convertSingleField($arg);
        }

        $sql .= implode(',', $fnArgs).')';

        return $sql;

    }//end handleFunctionLower()


    /**
     * Returns list of allowed where clause conditions.
     *
     * @param string $type Type of the condition. It will return its sign.
     *
     * @return array
     */
    protected function getComparisonOperators($type=NULL)
    {
        if ($type !== NULL) {
            if (isset($this->_whereConditions[$type]) === TRUE) {
                return $this->_whereConditions[$type];
            } else {
                return '';
            }
        }

        return $this->_whereConditions;

    }//end getComparisonOperators()


    /**
     * Returns list of hooks that needs to be replaced during sql prepare by Query.
     *
     * @return array
     */
    public function getHookStrings()
    {
        return $this->_hookStrings;

    }//end getHookStrings()


}//end class

?>
