<?php
/**
 * The Query Object used to store database objects.
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

require_once 'DAL/DAL.inc';

/**
 * Query.
 *
 * Object that represents a query in the MySource 4 system.
 *
 */
class Query
{

    /**
     * The ID of this query.
     *
     * @var   string $_queryid Full ID of the query.
     */
    private $_queryid = '';

    /**
     * Total number of bindings in this query.
     *
     * @var   integer Number of bindings.
     */
    private $_lastBindNum = 0;

    /**
     * The representation of this query as an array.
     *
     * @var   array $_arrayRepresentation The multidimensional structure of the
     *                                    array.
     */
    private $_arrayRepresentation = array();

    /**
     * The SQL of the query.
     *
     * This is the initial conversion. If a complex merge is required, then the
     * SQL string will need to be regenerated.
     *
     * @var   string $_sqlString The SQL of the query.
     */
    private $_sqlString = '';

    /**
     * List of bindings.
     *
     * @var   array $_bindings Array of bind variables in this query.
     */
    private $_bindings = array();

    /**
     * List of keywords.
     *
     * Keywords will be replaced just before query prepare.
     *
     * @var   array $_keywords Array of keywords in this query.
     */
    private $_keywords = array();

    /**
     * Query execution method.
     *
     * Query will be executed using the given method (i.e. executeAll).
     *
     * @var   string $_execMethod
     */
    private $_execMethod = NULL;

    /**
     * Set to TRUE when the query array is converted to SQL string.
     *
     * @var   boolean
     */
    private $_converted = FALSE;

    /**
     * The list of hooks in the query that may need to be replaced during prepare.
     *
     * @var   boolean
     */
    private $_hookStrs = array();


    /**
     * Creates a new query object with the data provided.
     *
     * @param string $queryid             The ID of the query to bake out.
     * @param array  $arrayRepresentation The structured array representation
     *                                    of the query.
     * @param string $sqlString           The SQL of the query.
     *
     * @throws DALException If query represantation is invalid.
     */
    public function __construct(
        $queryid='',
        array $arrayRepresentation=array(),
        $sqlString=''
    ) {
        $this->_queryid = $queryid;

        if (isset($arrayRepresentation['query']) === FALSE) {
            throw new DALException('Invalid array representation');
        }

        $this->_arrayRepresentation = $arrayRepresentation;
        $this->_sqlString           = $sqlString;

    }//end __construct()


    /**
     * Returns the type of execution this method has specified.
     *
     * @return string
     */
    public function getQueryType()
    {
        return $this->_arrayRepresentation['query_type'];

    }//end getQueryType()


    /**
     * Returns the array representation.
     *
     * @return string
     */
    public function getArrayRepresentation()
    {
        return $this->_arrayRepresentation;

    }//end getArrayRepresentation()


    /**
     * Merge a query into this one.
     *
     * @param string $bindName The bind variable the query will replace.
     * @param Query  $query    The query to merge as part of this one.
     *
     * @return boolean
     */
    public function merge($bindName, Query $query)
    {
        $subQuery = clone $query;

        $this->_addBindingsFromQuery($subQuery);
        $this->_findAndReplacePlaceHolder($bindName, $subQuery->getSql());

        // Also get hook strings from sub query.
        $subHooks        = $subQuery->getHookStrings();
        $this->_hookStrs = array_merge($this->_hookStrs, $subHooks);

        return TRUE;

    }//end merge()


    /**
     * Add bindings from specified query to this query.
     *
     * @param Query $subQuery Query object.
     *
     * @return void
     */
    private function _addBindingsFromQuery(Query $subQuery)
    {
        $bindings = $subQuery->getBindings();
        foreach ($bindings as $binding) {
            $newName = $this->_getNextValidBindName($subQuery->getLastBindNum());
            $subQuery->_findAndReplacePlaceHolder($binding['name'], $newName);
            $this->_addBindVar(
                $newName,
                $binding['value'],
                $binding['type'],
                $binding['name']
            );
        }

    }//end _addBindingsFromQuery()


    /**
     * Returns a valid name for a new bind variable for this query.
     *
     * @param integer $offset Offset is used to prevent overlapping bind names.
     *
     * @return string
     */
    private function _getNextValidBindName($offset=0)
    {
        $next = ($this->_lastBindNum + $offset + 1);
        $this->_lastBindNum = $next;

        $bindName = ':bind_'.$next;
        return $bindName;

    }//end _getNextValidBindName()


    /**
     * Add a bind variable to this query.
     *
     * @param string  $name     Name of the new bind variable.
     * @param mixed   $value    Value of the bind variable.
     * @param integer $type     Data type of the bind variable.
     * @param string  $realName Real name of the bind var.
     *
     * @return void
     */
    private function _addBindVar($name, $value, $type, $realName)
    {
        if ($type === PDO::PARAM_STR) {
            $value = (string) $value;
        }

        $this->_bindings[] = array(
                              'name'  => $name,
                              'value' => $value,
                              'type'  => $type,
                             );

    }//end _addBindVar()


    /**
     * Remove bind variables from end of binds array.
     *
     * @param string $count Number of bind variables to remove.
     *
     * @return void
     */
    private function _removeLastBindVar($count=1)
    {
        for ($i = 0; $i < $count; $i++) {
            array_pop($this->_bindings);
        }

    }//end _removeLastBindVar()


    /**
     * Returns the number of bindings in the query.
     *
     * @return integer
     */
    public function getLastBindNum()
    {
        return $this->_lastBindNum;

    }//end getLastBindNum()


    /**
     * Replaces the given bind varibale name with a new name in SQL string.
     *
     * @param string $oldName Old name of the bind variable.
     * @param string $newName New name of the bind variable.
     *
     * @return void
     */
    public function replaceBindVarName($oldName, $newName)
    {
         $this->_sqlString = str_replace($oldName, $newName, $this->_sqlString);

    }//end replaceBindVarName()


    /**
     * Returns this query's id.
     *
     * @return string
     */
    public function getQueryid()
    {
        return $this->_queryid;

    }//end getQueryid()


    /**
     * Returns list of bind variables.
     *
     * @return array
     */
    public function getBindings()
    {
        return $this->_bindings;

    }//end getBindings()


    /**
     * Returns the array representaion of this Query.
     *
     * @return array
     */
    public function getQueryArray()
    {
        return $this->_arrayRepresentation['query'];

    }//end getQueryArray()


    /**
     * Returns the SQL string representaion of this Query.
     *
     * @param boolean $convert If TRUE will perform conversion of the query into
     *                         SQL before returning, otherwise the SQL is
     *                         returned in its' current form.
     *
     * @return array
     */
    public function getSql($convert=FALSE)
    {
        if ($convert === TRUE || $this->_sqlString === '' || $this->_converted === FALSE) {
             $this->_converted = TRUE;
             $converter        = DAL::getConverter();
             $this->_sqlString = $converter->convertToSql(
                 $this->_arrayRepresentation['query']
             );

             $this->_hookStrs = $converter->getHookStrings();
        }

        return $this->_sqlString;

    }//end getSql()


    /**
     * Prepare the query for execution and returns the PDOStatement.
     *
     * @return PDOStatement
     */
    public function prepare()
    {
        $sql = $this->_sqlString;

        // Replace keywords.
        foreach ($this->_keywords as $keyword => $value) {
            $sql = str_replace('['.$keyword.']', $value, $sql);
        }

        // Replace all hooks that were not replaced already.
        foreach ($this->_hookStrs as $hookCode => $replacement) {
            $sql = str_replace($hookCode, $replacement, $sql);
        }

        $sql = str_replace(' IN()', ' IN(null)', $sql);

        // Update sqlString..
        $this->_sqlString = $sql;

        // Prepare PDO statement.
        $dbh   = DAL::getDb();
        $query = $dbh->prepare($sql);
        // Add bindings to PDO.
        foreach ($this->_bindings as $binding) {
            $query->bindValue($binding['name'], $binding['value'], $binding['type']);
        }

        return $query;

    }//end prepare()


    /**
     * Set the execution method.
     *
     * For example: executeAssoc.
     *
     * @param string $method The method to set for the query.
     *
     * @return void
     */
    public function setExecMethod($method)
    {
        $this->_execMethod = $method;

    }//end setExecMethod()


    /**
     * Returns the execution method.
     *
     * For example: executeAssoc
     *
     * @return string
     */
    public function getExecMethod()
    {
        return $this->_execMethod;

    }//end getExecMethod()


    /**
     * Add binding to the query.
     *
     * If value is an array then use arraySplit.
     * If value is another Query then use merge.
     *
     * @param string  $name     The name of the bind variable.
     * @param mixed   $value    The value to set the variable to.
     * @param integer $dataType The type to bind the value with.
     *
     * @return void
     */
    public function bind($name, $value, $dataType)
    {
        // Depending on the data type replace, merge, split etc.
        if (($value instanceof Query) === TRUE) {
            $this->merge($name, $value);
        } else if (is_array($value) === TRUE) {
            $bindNames = array();
            foreach ($value as $val) {
                $newBindName = $this->_getNextValidBindName();
                if ($val === NULL) {
                    $dataType = PDO::PARAM_NULL;
                }

                $this->_addBindVar($newBindName, $val, $dataType, $name);
                $bindNames[] = $newBindName;
            }

            if ($this->_findAndReplacePlaceHolder($name, implode(',', $bindNames)) === FALSE) {
                $this->_removeLastBindVar(count($bindNames));
            }
        } else {
            $newBindName = $this->_getNextValidBindName();
            if ($value === NULL) {
                $dataType = PDO::PARAM_NULL;
            } else if (is_bool($value) === TRUE || $dataType === PDO::PARAM_BOOL) {
                $dataType = PDO::PARAM_BOOL;

                if ($value === 'false') {
                    $value = FALSE;
                } else if ($value === 'true') {
                    $value = TRUE;
                }
            }

            $this->_addBindVar($newBindName, $value, $dataType, $name);

            // Find the bind place holder in the query array and replace it.
            if ($this->_findAndReplacePlaceHolder($name, $newBindName) === FALSE) {
                $this->_removeLastBindVar();
            }
        }//end if

    }//end bind()


    /**
     * Recurses the query array and replaces the place holder with given value.
     *
     * @param string $placeHolder Name of the place holder.
     * @param mixed  $value       Value that will replace the placeHolder.
     *
     * @return boolean
     */
    private function _findAndReplacePlaceHolder($placeHolder, $value)
    {
        $count = 0;
        $sql   = $this->getSql();
        $sql   = preg_replace('/'.$placeHolder.'([^a-zA-Z0-9])/', $value.'$1', $sql, 1, $count);

        $this->_sqlString = $sql;

        if ($count === 0) {
            return FALSE;
        }

        return TRUE;

    }//end _findAndReplacePlaceHolder()


    /**
     * Adds sub queries (hooks) to the current query.
     *
     * @param string $subQueryid The sub hookid to merge.
     * @param array  $queries    The queries to add.
     *
     * @return void
     */
    public function subQueries($subQueryid, array $queries)
    {
        foreach ($queries as $query) {
            if (($query instanceof Query) === TRUE) {
                $this->merge('HOOKID:'.$subQueryid, $query);
            } else {
                $this->bind('HOOKID:'.$subQueryid, $query, PDO::PARAM_STR);
            }
        }

    }//end subQueries()


    /**
     * Add keywords to the query.
     *
     * Array('keyword' => value)
     *
     * @param array $keywords Array of keywords to add.
     *
     * @return void
     */
    public function keywords(array $keywords)
    {
        $this->_keywords = array_merge($this->_keywords, $keywords);

    }//end keywords()


    /**
     * Returns the hook strings inside th query.
     *
     * @return array
     */
    public function getHookStrings()
    {
        return $this->_hookStrs;

    }//end getHookStrings()


    /**
     * Returns the actual query string with bindings replaced with their values.
     *
     * @return string
     */
    public function __toString()
    {
        $sql = $this->_sqlString;

        // Replace keywords.
        foreach ($this->_keywords as $keyword => $value) {
            $sql = str_replace('['.$keyword.']', $value, $sql);
        }

        // Replace all hooks that were not replaced already.
        foreach ($this->_hookStrs as $hookCode => $replacement) {
            $sql = str_replace($hookCode, $replacement, $sql);
        }

        // Replace bind vars.
        foreach ($this->_bindings as $binding) {
            $rep = $binding['value'];

            if ($binding['type'] === PDO::PARAM_STR) {
                $rep = "'".$rep."'";
            } else if ($binding['type'] === PDO::PARAM_BOOL) {
                if ($rep === FALSE) {
                    $rep = 'FALSE';
                } else if ($rep === TRUE) {
                    $rep = 'TRUE';
                }
            }

            $sql = preg_replace('/'.$binding['name'].'([^a-zA-Z0-9])/', $rep.'$1', $sql, 1);
        }

        $sql .= ";\n";

        return $sql;

    }//end __toString()


}//end class

?>
