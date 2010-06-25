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
 * Unit tests for the validateQuery method.
 *
 * validateQuery() method throws exception if there is an error in the query.
 *
 * @since 4.0.0
 */
class ValidateQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Queries xml string.
     *
     * @var   string $_queries Queries xml.
     * @since 4.0.0
     */
    private $_queries = '<queries system="test">
                 <query id="test">
                     <update>
                        <fields table="link_table">
                            <field>parentid</field>
                            <field>childid</field>
                        </fields>
                        <values>
                            <value column="parentid">123</value>
                            <value column="childid">234</value>
                        </values>
                        <where>
                            <equal table="link_table" column="linkid">1</equal>
                        </where>
                    </update>
                 </query>
                 <query id="test">
                     <update>
                        <fields table="link_table">
                            <field>parentid</field>
                            <field>childid</field>
                        </fields>
                        <values>
                            <value column="parentid">123</value>
                            <value column="childid">234</value>
                        </values>
                        <where>
                            <equal table="link_table" column="linkid">1</equal>
                        </where>
                    </update>
                 </query>
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
     * Test validateSelectQuery when a required tag or attr is missing.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateSelectQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateSelectQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'validateSelectQuery() should have thrown an exception for query ';
        $tc         = 1;

        foreach ($queries as $query) {
            $caught = FALSE;
            try {
                DALSelectParser::validate($query);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateSelectQuery()


    /**
     * Test validateInsertQuery when a required tag or attr is missing.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateInsertQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateInsertQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'validateInsertQuery() should have thrown an exception for query ';
        $tc         = 1;

        foreach ($queries as $query) {
            $caught = FALSE;
            try {
                DALInsertParser::validate($query);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateInsertQuery()


    /**
     * Test validateDeleteQuery when a required tag or attr is missing.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateDeleteQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateDeleteQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'validateDeleteQuery() should have thrown an exception for query ';
        $tc         = 1;

        foreach ($queries as $query) {
            $caught = FALSE;
            try {
                DALDeleteParser::validate($query);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateDeleteQuery()


    /**
     * Test validateUpdateQuery when a required tag or attr is missing.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateUpdateQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateUpdateQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'validateUpdateQuery() should have thrown an exception for query ';
        $tc         = 1;

        foreach ($queries as $query) {
            $caught = FALSE;
            try {
                DALUpdateParser::validate($query);
            } catch (DALParserException $e) {
                $caught = TRUE;
            }

            PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
            $tc++;
        }

    }//end testValidateUpdateQuery()


    /**
     * Test validateQuery when query uses a non unique query id.
     *
     * ValidateQuery() should throw an exception in this case.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateQueryDuplicateId()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $caught = FALSE;
        $msg    = '';
        try {
            DALBaker::validateQuery($query);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        $expectedMsg = 'Query must have a unique id. Id "test" already used by another query.';

        PHPUnit_Framework_Assert::assertTrue($caught);
        PHPUnit_Framework_Assert::assertEquals($expectedMsg, $msg);

    }//end testValidateQueryDuplicateId()


    /**
     * Test that validateQuery throws exception when query has no query id attr.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateQueryNoQueryId()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_queries);
        $query = $doc->getElementsByTagName('query')->item(0);
        $query->setAttribute('id', '');

        $caught = FALSE;
        $msg    = '';
        try {
            DALBaker::validateQuery($query);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        $expectedMsg = 'Query must have id attribute.';

        PHPUnit_Framework_Assert::assertTrue($caught);
        PHPUnit_Framework_Assert::assertEquals($expectedMsg, $msg);

    }//end testValidateQueryNoQueryId()


    /**
     * Test that validateQuery throws exception when query has no query id attr.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidQueryType()
    {
        $xml = '<queries system="test"><query id="test">
                <notvalidtype></notvalidtype></query></queries>';

        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $caught = FALSE;
        $msg    = '';
        try {
            DALBaker::validateQuery($query);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        $expectedMsg = 'Invalid validate query. Type:notvalidtype';

        PHPUnit_Framework_Assert::assertTrue($caught);
        PHPUnit_Framework_Assert::assertEquals($expectedMsg, $msg);

    }//end testInvalidQueryType()


}//end class

?>
