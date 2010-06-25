<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for parsing and converting queries.
 *
 * Test will make sure that parsed query array is same as the original.
 * It will make sure that converted sql string is same as the original.
 * This test requires <queryType>Queries.xml and <queryType>ParsedResults.inc
 * files (i.e. selectQueries.xml and selectParsedResults.inc).
 *
 * @since 4.0.0
 */
class ParseAndConvertUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test that parsed and converted query returns the correct result.
     *
     * Each query will be parsed and compared to the original serialized string.
     * Then, it will be converted for each DB type and the result will be
     * compared to the original base64 encoded string.
     *
     * @since  4.0.0
     * @return void
     */
    public function testParsedAndConvertedQuery()
    {
        $queryTypes = array(
                       'select',
                       'insert',
                       'update',
                       'delete',
                      );

        foreach ($queryTypes as $type) {
            $this->_parsedAndConvertedQueryTest($type);
        }

    }//end testParsedAndConvertedQuery()


    /**
     * Test that parsed and converted query returns the correct result.
     *
     * Each query will be parsed and compared to the original serialized string.
     * Then, it will be converted for each DB type and the result will be
     * compared to the original base64 encoded string.
     *
     * @param string $type Type of the query.
     *
     * @since  4.0.0
     * @return void
     */
    private function _parsedAndConvertedQueryTest($type)
    {
        $doc     = $this->_loadQueries($type);
        $results = $this->_loadParsedResults($type);
        $queries = $doc->getElementsByTagName('query');
        $tc      = 0;
        $dbTypes = array(
                    'Mysql',
                    'Postgres',
                    'Oracle',
                    'Db2',
                    'Mssql',
                   );

        try {
            $msg = 'Parsed '.$type.' query has different result! Query: ';
            foreach ($queries as $query) {
                $sqlArray = DALBaker::constructSql($query);
                $str      = serialize($sqlArray['query']);
                PHPUnit_Framework_Assert::assertTrue($results['parsed'][$tc] === $str, $msg.$tc.".\n");

                $convertedMsg = 'Converted '.$type.' query '.$tc.' has different result for database type "';
                // Parsed query was OK, now test converted.
                foreach ($dbTypes as $dbType) {
                    $converter = $this->_getConverterClass($dbType);
                    $sql       = $converter->convertToSql($sqlArray['query']);
                    $sql       = base64_encode($sql);
                    PHPUnit_Framework_Assert::assertEquals($results['converted'][$dbType][$tc], $sql, $convertedMsg.$dbType."\".\n");
                }

                $tc++;
            }
        } catch (DALParserException $e) {
            $msg = 'Query '.$tc.' failed validation! '.$e->getMessage();
            PHPUnit_Framework_Assert::fail($msg);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end _parsedAndConvertedQueryTest()


    /**
     * Returns the converter class for given database type.
     *
     * @param string $dbType Type of the database.
     *
     * @since  4.0.0
     * @return object
     */
    private function _getConverterClass($dbType)
    {
        $converterClass = 'DAL'.$dbType.'Converter';

        $path = dirname(dirname(__FILE__)).'/Converters/'.$converterClass.'.inc';
        require_once $path;
        // Here we will get the current DB type and use its converter class.
        $converter = eval("return new DAL$dbType".'Converter();');
        return $converter;

    }//end _getConverterClass()


    /**
     * Returns the DomDocument object for an xml file.
     *
     * @param string $type Type of the query.
     *
     * @since  4.0.0
     * @return DomDocument
     */
    private function _loadQueries($type)
    {
        $path = dirname(__FILE__).'/Files/'.$type.'Queries.xml';
        $doc  = new DomDocument();
        $doc->load($path);
        return $doc;

    }//end _loadQueries()


    /**
     * Reads the parsed/converted results from file.
     *
     * Each results file must have results array with parsed and converted
     * indexes.
     *
     * @param string $type Type of the query.
     *
     * @since  4.0.0
     * @return array
     */
    private function _loadParsedResults($type)
    {
        $results = array();
        $path    = dirname(__FILE__).'/Files/'.$type.'ParsedResults.inc';
        require_once($path);
        return $results;

    }//end _loadParsedResults()


}//end class

?>
