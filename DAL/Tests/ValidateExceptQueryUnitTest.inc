<?php
/**
 * Tests for validating query using EXCEPT clause.
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
require_once dirname(dirname(__FILE__)).'/Parsers/DALExceptParser.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALDeleteParser.inc';

/**
 * Unit tests for the validateQuery method.
 *
 */
class ValidateExceptQueryUnitTest extends AbstractMySourceUnitTest
{


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
     * Test validating primary except query of correct syntax.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @return void
     */
    public function testValidateValidPrimaryExceptQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateExceptQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The primary EXCEPT query should parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'validPrimaryQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALExceptParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertFalse($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateValidPrimaryExceptQuery()


    /**
     * Test validating primary except query with missing select statement.
     *
     * It should throw an excepion for missing sub select statment
     *
     * @return void
     */
    public function testValidateInvalidPrimaryExceptQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateExceptQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The primary EXCEPT query should not parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'invalidPrimaryQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALExceptParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateInvalidPrimaryExceptQuery()


    /**
     * Test validating except sub query with correct syntax.
     *
     * It should not throw an excepion
     *
     * @return void
     */
    public function testValidateValidSubExceptQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateExceptQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The sub EXCEPT query should parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'validSubQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALDeleteParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertFalse($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateValidSubExceptQuery()


    /**
     * Test validating except sub query with no select statement.
     *
     * It should throw an excepion for missing sub select statements
     *
     * @return void
     */
    public function testValidateInvalidSubExceptQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateExceptQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The sub EXCEPT query should parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'invalidSubQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALDeleteParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateInvalidSubExceptQuery()


}//end class

?>