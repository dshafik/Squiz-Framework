<?php
/**
 * DAL. Database Abstraction Layer.
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

require_once dirname(dirname(__FILE__)).'/Channels/Channels.inc';
require_once dirname(__FILE__).'/Exceptions/DALException.inc';
require_once 'DAL/Query.inc';

/**
 * DAL.
 *
 * DAL (Database Abstraction Layer) class handles parsing of XML SQL queries.
 * It converts XML based SQL queries to generic PHP arrays so that they can be
 * converted to actual SQL queries by the DALConverter class.
 *
 * DAL class can also validates schemas and quries.
 *
 * DAL class also handles database connection and access.
 */
class DAL
{

    /**
     * PDO Database handle.
     *
     * Prepare statements will use this PDO object.
     *
     * @var object
     */
    private static $_dbh = NULL;

    /**
     * Transaction handler.
     *
     * If a transaction is already started then another transaction will not
     * be started. Commit and rollBack will set this var to FALSE.
     *
     * @var boolean $_inTransaction Transaction handler.
     */
    private static $_inTransaction = FALSE;

    /**
     * Valid database types and their names.
     *
     * A database type must be specified in the DALConfig file.
     *
     * @var array
     */
    private static $_dbNames = array(
                                'db2'   => 'DB2',
                                'dblib' => 'SQLServer',
                                'mysql' => 'MySQL',
                                'oci'   => 'Oracle',
                                'pgsql' => 'PostgreSQL',
                               );

    /**
     * Name of the current database.
     *
     * @var string
     */
    private static $_currentDbName = NULL;

    /**
     * Transaction counter.
     *
     * @var integer
     */
    private static $_transactionCount = 0;

    /**
     * List of querys that were executed and their timings.
     *
     * @var array
     */
    private static $_queryLog = array();

    /**
     * Log queries.
     *
     * @var boolean
     */
    private static $_logQueries = FALSE;

    /**
     * Current query.
     *
     * @var array
     */
    private static $_currentQueryLog = NULL;

    /**
     * ID for different DALConf files.
     *
     * When its
     *
     * @var string
     */
    public static $confId = '';


    /**
     * DAL constructor.
     *
     * Private to avoid instantiating the object.
     * All DAL methods should be called statically.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the PDO query object for the given system and queryid.
     *
     * @param string  $systemid   System name that the query belongs to.
     * @param string  $queryid    ID of the query.
     * @param boolean $bindValues If TRUE, will add the query to a PDOStatement
     *                            object and bind its' variables. Otherwise, it
     *                            will return an array representation of it.
     *
     * @return object
     * @throws DALException If failed to load the query.
     */
    public static function getQuery($systemid, $queryid, $bindValues=TRUE)
    {
        // Work out the class and method name to call.
        $class    = $systemid.'Queries';
        $function = 'prepare'.$queryid.'Query';

        self::requireQueryClass($systemid);

        // Should we include the class here?
        if (method_exists($class, $function) === FALSE) {
            // Query not found. Throw DALException exception?
            $msg = 'Could not load query '.$queryid.' in system '.$systemid;
            throw new DALException($msg);
        } else {
            // Get the event data for the current channel.
            $eventData = Channels::getCurrentEventBasketData();

            $res = call_user_func(array($class, $function), $eventData, $bindValues);
            return $res;
        }

    }//end getQuery()


    /**
     * Returns all rows as an associative array.
     *
     * @param mixed   $dalQuery SQL Query to execute.
     * @param integer $col      Column to retrieve.
     *
     * @return array
     * @throws PDOException Column index is invalid.
     */
    public static function getAssoc($dalQuery, $col=NULL)
    {
        $result = NULL;
        if (is_array($dalQuery) === TRUE) {
            // The query object has already been processed.
            if ($col !== NULL) {
                $ret = array();
                foreach ($dalQuery as $item) {
                    if (is_array($item) === TRUE) {
                        $max = (count($item) - 1);
                        if ($max < $col) {
                            throw new PDOException('Invalid column index');
                        }

                        $i = 0;
                        foreach ($item as $val) {
                            if ($i === $col) {
                                $ret[] = $val;
                            }

                            $i++;
                        }
                    } else {
                        $ret = $dalQuery;
                        break;
                    }
                }

                $result = $ret;
            } else {
                $result = $dalQuery;
            }//end if
        } else if ($dalQuery instanceof Query) {
            self::_startQueryTimer($dalQuery->getQueryid());
            $query = $dalQuery->prepare();
            self::execPdoQuery($query);
            if ($col !== NULL) {
                $result = $query->fetchAll(PDO::FETCH_COLUMN, $col);
            } else {
                $result = $query->fetchAll(PDO::FETCH_ASSOC);
            }

            self::_endQueryTimer();
        }//end if

        return $result;

    }//end getAssoc()


    /**
     * Returns all rows in an array.
     *
     * @param Query $dalQuery SQL Query to execute.
     *
     * @return array
     */
    public static function getAll(Query $dalQuery)
    {
        self::_startQueryTimer($dalQuery->getQueryid());
        $result = array();
        $query  = $dalQuery->prepare();
        self::execPdoQuery($query);
        $result = $query->fetchAll();
        self::_endQueryTimer();

        return $result;

    }//end getAll()


    /**
     * Returns a single value.
     *
     * @param mixed   $dalQuery    SQL Query to execute.
     * @param integer $columnIndex The column to retrieve.
     *
     * @return mixed
     */
    public static function getOne($dalQuery, $columnIndex=0)
    {
        // The query object has already been processed.
        if (is_array($dalQuery) === TRUE) {
            foreach ($dalQuery as $item) {
                if (is_array($item) === TRUE) {
                    if ($columnIndex === 0) {
                        return current($item);
                    } else {
                        if (isset($item[$columnIndex]) === TRUE) {
                            return $item[$columnIndex];
                        } else {
                            $index = 0;
                            foreach ($item as $field) {
                                if ($index === $columnIndex) {
                                    return $field;
                                }

                                $index++;
                            }
                        }
                    }
                } else {
                    return $item;
                }
            }//end foreach
        } else if (DAL::isDalQuery($dalQuery) === TRUE) {
            self::_startQueryTimer($dalQuery->getQueryid());
            $query = $dalQuery->prepare();
            self::execPdoQuery($query);

            $res = $query->fetchColumn($columnIndex);

            self::_endQueryTimer();

            return $res;
        } else {
            return $dalQuery;
        }//end if

        return '';

    }//end getOne()


    /**
     * Returns an array of data about the SQL query.
     *
     * This is used if the query is not to be executed yet, and rather a
     * representation of the query's SQL, along with its' bindings and their
     * values.
     *
     * @param string $systemid System name that the query belongs to.
     * @param string $queryid  ID of the query.
     * @param string $method   Method used to retrieve the query(getAssoc etc).
     *
     * @return array
     */
    public static function getDALQuery($systemid, $queryid, $method='')
    {
        $queryArray = self::getQuery($systemid, $queryid, FALSE);
        // Add the method data to the query array, as this will allow
        // methods that this is passed to to execute it.
        $queryArray->setExecMethod($method);
        return $queryArray;

    }//end getDALQuery()


    /**
     * Returns all rows grouped by first column.
     *
     * @param mixed $dalQuery SQL Query to execute.
     *
     * @return array
     * @throws DALException If query execution fails.
     */
    public static function getGrouped($dalQuery)
    {
        $result = array();
        if (is_array($dalQuery) === TRUE) {
            // The query object has already been processed.
            $ret = array();
            foreach ($dalQuery as $item) {
                if (is_array($item) === TRUE) {
                    $i     = 0;
                    $first = '';
                    $sub   = array();
                    foreach ($item as $val) {
                        if ($i === 0) {
                            $first = $val;
                            if (isset($ret[$first]) === FALSE) {
                                $ret[$first] = array();
                            }
                        } else {
                            $sub[] = $val;
                        }

                        $i++;
                    }

                    $ret[$first][] = $sub;
                } else {
                    $ret[$item] = array();
                }//end if
            }//end foreach

            $result = $ret;
        } else if ($dalQuery instanceof Query) {
            $query = $dalQuery->prepare();
            try {
                self::execPdoQuery($query);
            } catch (PDOException $e) {
                throw new DALException($e);
            }

            $done = FALSE;
            while ($done !== TRUE) {
                $row = $query->fetch(PDO::FETCH_NUM);
                if (empty($row) === TRUE) {
                    $done = TRUE;
                    break;
                }

                if (isset($result[$row[0]]) === FALSE) {
                    $result[$row[0]] = array();
                }

                $current = array();
                $i       = 1;
                while (isset($row[$i]) === TRUE) {
                    $current[] = $row[$i];
                    $i++;
                }

                $result[$row[0]][] = $current;
            }
        }//end if

        return $result;

    }//end getGrouped()


    /**
     * Executes the given PDO statement and returns all rows.
     *
     * Rows can be grouped by specified column with subsequent fields keyed by
     * their field name.
     *
     * @param mixed   $query  PDOStatement object to execute.
     * @param string  $col    Column name.
     * @param boolean $unique If TRUE and $col is specfied then only one value
     *                        will be returned per group.
     *                        E.g. array('x' => array('c2' => 1, 'c3' => 2)).
     *
     * @return array
     * @throws DALException If pdo fails to execute the query.
     */
    public static function getGroupedAssoc($query, $col=NULL, $unique=FALSE)
    {
        $result = array();
        if (is_array($query) === TRUE) {
            // The query object has already been processed.
            $ret = array();
            foreach ($query as $item) {
                if (is_array($item) === TRUE) {
                    $i     = 0;
                    $first = '';
                    $sub   = array();
                    foreach ($item as $key => $val) {
                        if ($col === NULL) {
                            // No col specified, use first column.
                            if ($i === 0) {
                                $first = $val;
                                if (isset($ret[$first]) === FALSE) {
                                    $ret[$first] = array();
                                }
                            } else {
                                $sub[$key] = $val;
                            }
                        } else {
                            if ($key === $col) {
                                $first = $val;
                                if (isset($ret[$first]) === FALSE) {
                                    $ret[$first] = array();
                                }
                            } else {
                                $sub[$key] = $val;
                            }
                        }

                        $i++;
                    }//end foreach

                    $ret[$first][] = $sub;
                } else {
                    $ret[$item] = array();
                }//end if
            }//end foreach

            $result = $ret;
        } else if ($query instanceof Query) {
            $query = $query->prepare();
            try {
                self::execPdoQuery($query);
            } catch (PDOException $e) {
                throw new DALException($e->getMessage());
            }

            $done = FALSE;
            while ($done !== TRUE) {
                $row = $query->fetch(PDO::FETCH_ASSOC);
                if (empty($row) === TRUE) {
                    $done = TRUE;
                    break;
                }

                if (isset($firstField) === FALSE) {
                    reset($row);
                    if ($col === NULL) {
                        $firstField = key($row);
                    } else {
                        $firstField = $col;
                    }
                }

                $firstValue = $row[$firstField];

                if (isset($result[$firstValue]) === FALSE) {
                    $result[$firstValue] = array();
                }

                $current = $row;
                unset($current[$firstField]);

                if ($col !== NULL && $unique === TRUE) {
                    $result[$firstValue] = $current;
                } else {
                    $result[$firstValue][] = $current;
                }
            }//end while
        }//end if

        return $result;

    }//end getGroupedAssoc()


    /**
     * Returns all rows grouped by first column.
     *
     * @param string $systemid System's ID.
     * @param string $queryid  ID of the SQL query.
     *
     * @return array
     */
    public static function executeGrouped($systemid, $queryid)
    {
        $query = self::getQuery($systemid, $queryid);
        $res   = self::getGrouped($query);
        return $res;

    }//end executeGrouped()


    /**
     * Returns all rows as an associative array.
     *
     * This function will call getQuery() and getAssoc().
     *
     * @param string  $systemid System's ID.
     * @param string  $queryid  ID of the SQL query.
     * @param integer $col      Column to retrieve.
     *
     * @return array
     */
    public static function executeAssoc($systemid, $queryid, $col=NULL)
    {
        $query = self::getQuery($systemid, $queryid);
        $res   = self::getAssoc($query, $col);
        return $res;

    }//end executeAssoc()


    /**
     * Returns all rows in an array.
     *
     * This function will call getQuery() and getAll().
     *
     * @param string $systemid System's ID.
     * @param string $queryid  ID of the SQL query.
     *
     * @return array
     */
    public static function executeAll($systemid, $queryid)
    {
        $query = self::getQuery($systemid, $queryid);
        $res   = self::getAll($query);
        return $res;

    }//end executeAll()


    /**
     * Returns a single value.
     *
     * This function will call getQuery() and getOne().
     *
     * @param string $systemid System's ID.
     * @param string $queryid  ID of the SQL query.
     *
     * @return array
     */
    public static function executeOne($systemid, $queryid)
    {
        $query  = self::getQuery($systemid, $queryid);
        $result = self::getOne($query);

        // Oracle returns resourceid for LOB fields.
        // If result is a resource then retreive its contents.
        if (is_resource($result) === TRUE) {
            $result = stream_get_contents($result);
        }

        return $result;

    }//end executeOne()


    /**
     * Executes a query and returns the number of rows affected.
     *
     * Note: $systemid can be a PDOStatement object.
     * If $systemid is PDOStatement then $queryid (2nd argument) can be NULL.
     *
     * @param string|object $systemid System's ID.
     * @param string        $queryid  ID of the SQL query.
     *
     * @return integer
     * @throws DALException If no query provided or query execution failed.
     */
    public static function executeQuery($systemid, $queryid=NULL)
    {
        $query = NULL;
        if (($systemid instanceof Query) === TRUE) {
            self::_startQueryTimer($systemid->getQueryid());
            $query = $systemid;
            $query = $query->prepare();
        } else if ($queryid === NULL) {
            throw new DALException('No queryid provided for executeQuery.');
        } else {
            self::_startQueryTimer($queryid);
            $query = self::getQuery($systemid, $queryid);
            $query = $query->prepare();
        }

        try {
            self::execPdoQuery($query);
        } catch (PDOException $e) {
            throw new DALException($e);
        }

        $rows = (int) $query->rowCount();

        // DB2 returns -1 rows if no rows were affected.
        if ($rows === -1) {
            $rows = 0;
        }

        self::_endQueryTimer();

        return $rows;

    }//end executeQuery()


    /**
     * Alias for executeQuery.
     *
     * @param string|object $systemid System's ID.
     * @param string        $queryid  ID of the SQL query.
     *
     * @return integer
     */
    public static function getExecuteQuery($systemid, $queryid=NULL)
    {
        $res = self::executeQuery($systemid, $queryid);
        return $res;

    }//end getExecuteQuery()


    /**
     * Alias for executeAll.
     *
     * @param string $systemid System's ID.
     * @param string $queryid  ID of the SQL query.
     *
     * @return integer
     */
    public static function getExecuteAll($systemid, $queryid)
    {
        $res = self::executeAll($systemid, $queryid);
        return $res;

    }//end getExecuteAll()


    /**
     * Executes a DALQuery.
     *
     * @param Query  $query      The Query to execute.
     * @param string $execMethod Optional execute method i.e. Assoc, Grouped.
     *
     * @return array
     */
    public static function executeDALQuery(Query $query, $execMethod=NULL)
    {
        if ($execMethod === NULL) {
            $execMethod = $query->getExecMethod();
        }

        $methodArray = array(
                        'self',
                        'get'.ucfirst($execMethod),
                       );

        $result = call_user_func($methodArray, $query);
        return $result;

    }//end executeDALQuery()


    /**
     * Executes the given PDO statment.
     *
     * @param PDOStatement $query PDOStatement object.
     *
     * @return void
     */
    public static function execPdoQuery(PDOStatement $query)
    {
        $query->execute();

    }//end execPdoQuery()


    /**
     * Executes the given SQL query.
     *
     * Returns the number of rows affected by the statement.
     *
     * @param string $sql Sql query to execute.
     *
     * @return integer
     */
    public static function executeSql($sql)
    {
        $dbh  = self::getDb();
        $rows = $dbh->exec($sql);
        return $rows;

    }//end executeSql()


    /**
     * Executes multiple queries in one SQL string.
     *
     * Each sql statement must be separated by a semi-colon. Create table
     * statements should use this function since they may have multiple queries
     * to create indexes, constraints, etc. Also, it should not be used for
     * output statements (i.e. SELECT).
     *
     * @param string $sql SQL statements.
     *
     * @return void
     */
    public static function executeQueries($sql)
    {
        // Split SQL statements.
        $queries = split(';', $sql);
        if (empty($queries) === FALSE) {
            foreach ($queries as $query) {
                if (trim($query) !== '') {
                    self::executeSql($query);
                }
            }
        }

    }//end executeQueries()


    /**
     * Executes an SQL statement, returning a result set as a PDOStatement.
     *
     * @param string $sql The sql statement.
     *
     * @return PDOStatement
     */
    public static function query($sql)
    {
        $dbh    = self::getDb();
        $result = $dbh->query($sql);
        return $result;

    }//end query()


    /**
     * Returns the number of open transactions.
     *
     * @return integer
     */
    public static function getTransactionCount()
    {
        return self::$_transactionCount;

    }//end getTransactionCount()


    /**
     * Initiates a transaction.
     *
     * @return void
     * @throws DALException If transaction counter is not zero.
     */
    public static function beginTransaction()
    {
        if (self::$_transactionCount > 0 && self::$_inTransaction === FALSE) {
            // We were in a transaction (counter is non-zero) but someone set the
            // transaction flag back to FALSE, so something bad has happened.
            throw new DALException('Could not start a transaction when counter is not 0.');
        }

        if (self::$_inTransaction === FALSE) {
            $dbh = self::getDb();
            $dbh->beginTransaction();
            self::$_inTransaction = TRUE;
        }

        // This must happen after the block above
        // in case we are creating a connection, which would
        // set this counter back to zero.
        self::$_transactionCount++;

    }//end beginTransaction()


    /**
     * Commits a transaction.
     *
     * @return void
     */
    public static function commit()
    {
        self::$_transactionCount--;
        if (self::$_inTransaction === TRUE && self::$_transactionCount === 0) {
            $dbh = self::getDb();
            $dbh->commit();
            self::$_inTransaction = FALSE;
        }

    }//end commit()


    /**
     * Rolls back a transaction.
     *
     * All changes made since last commit will be lost.
     *
     * @return void
     */
    public static function rollBack()
    {
        self::$_transactionCount--;
        if (self::$_inTransaction === TRUE) {
            $dbh = self::getDb();
            $dbh->rollBack();
            self::$_inTransaction = FALSE;
        }

    }//end rollBack()


    /**
     * Returns the ID of the last inserted row or sequence value.
     *
     * @param string $sequenceName Name of the sequence from which the ID
     *                             should be returned.
     *
     * @return integer
     */
    public static function lastInsertId($sequenceName)
    {
        $dbh = self::getDb();
        $res = (int) $dbh->lastInsertId($sequenceName);
        return $res;

    }//end lastInsertId()


    /*
        Database Connection
    */


    /**
     * Connects to the selected database.
     *
     * Throws DALException if cannot connect to db else returns PDO object.
     *
     * @param array $dsn DSN array.
     *
     * @return object
     * @throws DALException If db connection fails.
     */
    public static function dbConnect(array $dsn=NULL)
    {
        try {
            if ($dsn === NULL) {
                $dsn = self::getDsn();
            }

            $user = NULL;
            $pass = NULL;
            if (isset($dsn['user']) === TRUE) {
                $user = $dsn['user'];
            }

            if (isset($dsn['password']) === TRUE) {
                $pass = $dsn['password'];
            }

            self::$_dbh = new PDO(
                $dsn['DSN'],
                $user, $pass,
                array(PDO::ATTR_PERSISTENT => FALSE)
            );
            self::$_dbh->exec('SET NAMES \'UTF8\'');
            self::$_dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            self::$_dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);

            self::$_inTransaction    = FALSE;
            self::$_transactionCount = 0;

        } catch (PDOException $e) {
            throw new DALException('DBError!:'.$e->getMessage());
        }//end try

        return self::$_dbh;

    }//end dbConnect()


    /**
     * Close the database connection.
     *
     * @return void
     * @throws DALException If there is a transaction.
     */
    public static function dbClose()
    {
        if (self::$_inTransaction === TRUE) {
            $error = 'Cannot close DB when there are open transactions';
            throw new DALException($error);
        }

        self::$_inTransaction    = FALSE;
        self::$_transactionCount = 0;
        self::$_dbh              = NULL;

    }//end dbClose()


    /**
     * Returns true if there is an active database connection.
     *
     * @return boolean
     */
    public static function dbConnected()
    {
        if (self::$_dbh === NULL) {
            return FALSE;
        }

        return TRUE;

    }//end dbConnected()


    /**
     * Returns DAL Configuration file path.
     *
     * @return boolean
     */
    public static function getDALConfPath()
    {
        $path = dirname(__FILE__).'/';
        if (self::$confId === '') {
            $path .= 'DALConf.inc';
        } else {
            $path .= self::$confId.'DALConf.inc';
        }

        return $path;

    }//end getDALConfPath()


    /**
     * Returns the DSN array for the current database configuration.
     *
     * @return array
     * @throws DALException If DSN not configured.
     */
    public static function getDsn()
    {
        $dsn = include self::getDALConfPath();
        if (isset($dsn['DSN']) === FALSE) {
            $error = 'DSN configuration not defined.';
            throw new DALException($error);
        }

        return $dsn;

    }//end getDsn()


    /**
     * Returns the current database name using its DSN.
     *
     * @return string
     */
    public static function getDbName()
    {
        if (self::$_currentDbName === NULL) {
            // Try to get the database name from DSN string.
            $dsn = self::getDsn();
            if (isset($dsn['DSN']) === TRUE) {
                $matches = array();
                $reg     = '/.*dbname=([a-zA-Z0-9_]+)[;]?/i';
                preg_match($reg, $dsn['DSN'], $matches);
                if (isset($matches[1]) === TRUE) {
                    self::$_currentDbName = $matches[1];
                }
            }
        }

        return self::$_currentDbName;

    }//end getDbName()


    /**
     * Returns the PDO object for the current database.
     *
     * @return PDO
     */
    public static function getDb()
    {
        // Connect to database if not connected already.
        if (self::$_dbh === NULL) {
            self::dbConnect();
        }

        return self::$_dbh;

    }//end getDb()


    /**
     * Returns the current database connection's type.
     *
     * @param boolean $readable If true readable name will be returned.
     *                          For example, myql => Mysql, pgsql => Postgres.
     *
     * @return string
     * @throws DALException If db type is not found or it is invalid.
     */
    public static function getDbType($readable=FALSE)
    {
        $conf = include self::getDALConfPath();
        if (isset($conf['type']) === FALSE) {
            throw new DALException('Could not find "type" in config.');
        } else if (isset(self::$_dbNames[$conf['type']]) === FALSE) {
            $msg = $conf['type'].' is not a valid database type.';
            throw new DALException($msg);
        }

        if ($readable === TRUE) {
            return self::$_dbNames[$conf['type']];
        }

        return $conf['type'];

    }//end getDbType()


    /**
     * Returns the DAL converter class for a given db type.
     *
     * If no dbType is given then the db type specified in DALConf will be used.
     *
     * @param string $dbType Database Type.
     *
     * @return object
     */
    public static function getConverter($dbType=NULL)
    {
        if ($dbType === NULL) {
            $dbType = ucfirst(self::getConverterType(self::getDBType()));
        }

        // If there is no database type returned, the processing will still be
        // able to proceed, as the base type: "DALConverter" will be used.
        $converterClass = 'DAL'.$dbType.'Converter';
        include_once 'Converters/'.$converterClass.'.inc';
        // Here we will get the current DB type and use its converter class.
        $converter = eval('return new DAL'.$dbType.'Converter();');
        return $converter;

    }//end getConverter()


    /**
     * Parses the connection string and returns the database type.
     *
     * @param string $dbType The Database Type.
     *
     * @return string
     */
    public static function getConverterType($dbType)
    {
        switch ($dbType) {
            case 'pgsql':
                $fullType = 'postgres';
            break;

            case 'oci':
                $fullType = 'oracle';
            break;

            case 'odbc':
                $fullType = 'db2';
            break;

            case 'dblib':
                $fullType = 'mssql';
            break;

            case 'mysql':
                $fullType = $dbType;
            break;

            default:
                $fullType = '';
            break;
        }//end switch

        return $fullType;

    }//end getConverterType()


    /**
     * Requires the queries class file for a system.
     *
     * @param string $systemid ID of the system.
     *
     * @return void
     */
    public static function requireQueryClass($systemid)
    {
        $classFile = $systemid.'Queries.inc';
        $path      = self::getOvenPath($systemid).'/'.$classFile;

        include_once $path;

    }//end requireQueryClass()


    /**
     * Returns the oven dir path.
     *
     * If system name is provided then, that system's oven dir will be returned.
     *
     * @param string $system System name.
     *
     * @return string
     */
    public static function getOvenPath($system=NULL)
    {
        $path = dirname(__FILE__).'/Oven';

        if ($system !== '') {
            $path .= '/'.$system;
        }

        return $path;

    }//end getOvenPath()


    /**
     * Returns the path to store query objects.
     *
     * @return string
     */
    public static function getQueryStorePath()
    {
        $path = dirname(__FILE__).'/QueryStore';
        return $path;

    }//end getQueryStorePath()


    /**
     * Returns the path to store query objects.
     *
     * @param string $id The ID of the query to retrieve.
     *
     * @return Query
     * @throws DALException When object is not found.
     */
    public static function getQueryObject($id)
    {
        $queryStore = self::getQueryStorePath();
        $path       = $queryStore.'/'.$id.'.qob';
        if (file_exists($path) === FALSE) {
            $msg = 'Query object doesn\'t exist';
            throw new DALException($msg);
        }

        $content  = file_get_contents($path);
        $queryObj = unserialize($content);
        return $queryObj;

    }//end getQueryObject()


    /**
     * Split SQL quries that are in a file and seperated by "--$$".
     *
     * @param string $path File path of the SQL file.
     *
     * @return array
     */
    public static function getSqlQueriesFromFile($path)
    {
        $queries = array();
        if (file_exists($path) === TRUE) {
            $fileContents = file_get_contents($path);
            $queries      = explode('--$$', $fileContents);
        }

        return $queries;

    }//end getSqlQueriesFromFile()


    /**
     * Returns TRUE if passed variable is a DAL query.
     *
     * @param mixed $query Variable to test.
     *
     * @return boolean
     */
    public static function isDALQuery($query)
    {
        if (($query instanceof Query) === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end isDALQuery()


    /**
     * Starts query timer for given queryid.
     *
     * @param string $queryid Id of the query.
     *
     * @return void
     */
    private static function _startQueryTimer($queryid)
    {
        if (self::$_logQueries === TRUE) {
            self::$_currentQueryLog = array();
            self::$_currentQueryLog['id']    = $queryid;
            self::$_currentQueryLog['start'] = microtime(TRUE);
        }

    }//end _startQueryTimer()


    /**
     * Stops query timer.
     *
     * @return void
     */
    private static function _endQueryTimer()
    {
        if (self::$_currentQueryLog !== NULL) {
            if (isset(self::$_queryLog[self::$_currentQueryLog['id']]) === FALSE) {
                self::$_queryLog[self::$_currentQueryLog['id']] = array();
            }

            $time = (microtime(TRUE) - self::$_currentQueryLog['start']);
            self::$_queryLog[self::$_currentQueryLog['id']][] = $time;
        }

        self::$_currentQueryLog = NULL;

    }//end _endQueryTimer()


    /**
     * Returns list of logged queries.
     *
     * @return array
     */
    public static function getLoggedQueries()
    {
        return self::$_queryLog;

    }//end getLoggedQueries()


    /**
     * Sets the list of logged queries.
     *
     * @param array $queries An array of database queries in the form returned
     *                       by getLoggedQueries().
     *
     * @return void
     */
    public static function setLoggedQueries(array $queries)
    {
        self::$_queryLog = $queries;

    }//end setLoggedQueries()


    /**
     * Clears the list of logged queries.
     *
     * @return void
     */
    public static function clearLoggedQueries()
    {
        self::$_queryLog = array();

    }//end clearLoggedQueries()


    /**
     * Returns the next sequence value.
     *
     * @param string $seqName Name of the sequence.
     *
     * @return integer
     */
    public static function seqNextVal($seqName)
    {
        $dbh  = self::getDb();
        $sql  = 'SELECT nextVal(:seqName)';
        $stmt = $dbh->prepare($sql);
        $stmt->bindValue(':seqName', $seqName);
        $stmt->execute();
        $result = $stmt->fetchColumn();
        return $result;

    }//end seqNextVal()


    /**
     * Returns the current sequence value.
     *
     * @param string $seqName Name of the sequence.
     *
     * @return integer
     */
    public static function seqCurrVal($seqName)
    {
        $dbh  = self::getDb();
        $sql  = 'SELECT currVal(:seqName)';
        $stmt = $dbh->prepare($sql);
        $stmt->bindValue(':seqName', $seqName);
        $stmt->execute();
        $result = $stmt->fetchColumn();
        return $result;

    }//end seqCurrVal()


}//end class

?>
