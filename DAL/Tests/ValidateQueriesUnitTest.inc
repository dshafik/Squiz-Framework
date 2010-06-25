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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the validateQueries() method.
 *
 * validateQueries() Validates all the queries under the given queries node.
 *
 * @since 4.0.0
 */
class ValidateQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var   string $_schema Schema xml.
     * @since 4.0.0
     */
    private $_queries = '<queries system="system1">
                            <query id="test1"></query>
                            <query id="test2"></query>
                        </queries>';


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
     * Test that validateQueries throws exception when system attr is not found.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoSystemAttribute()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);
        $queries->removeAttribute('system');
        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoSystemAttribute()


    /**
     * Tests validateQueries when there are duplicate query ids.
     *
     * An exception should be thrown by validateQueries in this situation.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUniqueQueryIds()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);
        // Set the second query id to same one as first query id.
        $queries->getElementsByTagName('query')->item(1)->setAttribute('id', 'test1');

        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testUniqueQueryIds()


    /**
     * Test validateQueries doesn't throw an exception when queries are valid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidQueries()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);

        // Remove the system attribute.
        $queries = $doc->getElementsByTagName('queries')->item(0);

        $caught = FALSE;
        try {
            $result = DALBaker::validateQueries($queries);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidQueries()


}//end class

?>
