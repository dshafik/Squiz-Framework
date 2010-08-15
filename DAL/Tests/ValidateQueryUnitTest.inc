<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the validateQuery method.
 *
 * validateQuery() method throws exception if there is an error in the query.
 */
class ValidateQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Queries xml string.
     *
     * @var string $_queries Queries xml.
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
