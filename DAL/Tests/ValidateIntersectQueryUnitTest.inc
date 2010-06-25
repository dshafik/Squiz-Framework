<?php
/**
 * Tests for validating query using INTERSECT clause.
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
require_once dirname(dirname(__FILE__)).'/Parsers/DALIntersectParser.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALDeleteParser.inc';

/**
 * Unit tests for the validateQuery method.
 *
 * @since 4.0.0
 */
class ValidateIntersectQueryUnitTest extends AbstractMySourceUnitTest
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
     * Test validating primary intersect query of correct syntax.
     *
     * It should throw an excepion for missing tags and attributes or when node
     * or attr values are invalid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateValidPrimaryIntersectQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateIntersectQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The primary INTERSECT query should parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'validPrimaryQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALIntersectParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertFalse($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateValidPrimaryIntersectQuery()


    /**
     * Test validating primary intersect query with missing select statement.
     *
     * It should throw an excepion for missing sub select statment
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateInvalidPrimaryIntersectQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateIntersectQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The primary INTERSECT query should not parse correctly';
        $tc         = 1;
        $caught     = FALSE;

        foreach ($queries as $query) {
            if ($query->getAttribute('id') === 'invalidPrimaryQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    DALIntersectParser::validate($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertTrue($caught, $msg.$tc.'.');
        $tc++;

    }//end testValidateInvalidPrimaryIntersectQuery()


    /**
     * Test validating intersect sub query with correct syntax.
     *
     * It should not throw an excepion
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateValidSubIntersectQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateIntersectQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The sub INTERSECT query should parse correctly';
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

    }//end testValidateValidSubIntersectQuery()


    /**
     * Test validating intersect sub query with no select statement.
     *
     * It should throw an excepion for missing sub select statements
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidateInvalidSubIntersectQuery()
    {
        $fileName = dirname(__FILE__).'/Files/ValidateIntersectQuery.xml';
        $queryXml = new DOMDocument();
        $queryXml->load($fileName);
        $queriesTag = $queryXml->getElementsByTagName('queries')->item(0);
        $queries    = $queriesTag->getElementsByTagName('query');
        $msg        = 'The sub INTERSECT query should parse correctly';
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

    }//end testValidateInvalidSubIntersectQuery()


}//end class

?>