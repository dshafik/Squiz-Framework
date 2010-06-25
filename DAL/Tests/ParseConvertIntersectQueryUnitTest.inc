<?php
/**
 * Tests for parsing query with INTERSECT.
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
require_once dirname(dirname(__FILE__)).'/Converters/DALConverter.inc';

/**
 * Unit tests for the validateQuery method.
 *
 * @since 4.0.0
 */
class ParseConvertIntersectQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * The converted Intersect sub query.
     *
     * @var   $_subQuery The converted Intersect sub query
     * @since 4.0.0
     */
    private $_subQuery = 'DELETE FROM role_permission
WHERE(
    (role_permission.roleid, role_permission.userid, role_permission.assetid) IN(
        SELECT
            role_permission.roleid, role_permission.userid, role_permission.assetid
        FROM
            role_permission
        WHERE
        (
            role_permission.assetid = (:assetid)
            AND role_permission.cascade = (\'t\')
        )

        INTERSECT

        SELECT
            role_permission.roleid, role_permission.userid, [assetid]
        FROM
            role_permission
        WHERE
        (
            role_permission.assetid IN(:parentids)
        )
    )
)';

    /**
     * The converted Intersect primary query.
     *
     * @var   $_subQuery The converted Intersect primary query
     * @since 4.0.0
     */
    private $_primaryQuery = 'SELECT
        asset_link.childid as assetid
    FROM
        asset_link
    WHERE
    (
        asset_link.active = (false)
        AND asset_link.childid IN(:assetids)
    )

    INTERSECT

    SELECT
        asset_link.childid as assetid
    FROM
        asset_link
    WHERE
    (
        asset_link.active = (true)
    )';


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
     * Test validating INTERSECT sub query with correct syntax.
     *
     * It should not throw an excepion
     *
     * @since  4.0.0
     * @return void
     */
    public function testParseConvertPrimaryIntersectQuery()
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
            if ($query->getAttribute('id') === 'validPrimaryQuery') {
                $primary = $query->getElementsByTagName('primary')->item(0);
                try {
                    $sqlArray = DALQueryParser::parse($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
                $converter = new DALConverter();
                $sql       = $converter->convertToSql($sqlArray['query']);

                PHPUnit_FrameWork_Assert::assertTrue(trim($sql) === trim($this->_primaryQuery), 'Converted query is not as expected');
                $missingIntersectMsg = 'There is no INTERSECT clause in the returned array';
                PHPUnit_FrameWork_Assert::assertTrue(isset($sqlArray['query']['INTERSECT']), $missingIntersectMsg.$tc.'.');
                $missingSelectMsg = 'There should be two SELECT clause in the returned array';
                PHPUnit_FrameWork_Assert::assertTrue(count($sqlArray['query']['INTERSECT']['SELECTS']) === 2, $missingSelectMsg.$tc.'.');
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertFalse($caught, $msg.$tc.'.');
        $tc++;

    }//end testParseConvertPrimaryIntersectQuery()


    /**
     * Test parsing Intersect sub query.
     *
     * It should not throw an excepion
     *
     * @since  4.0.0
     * @return void
     */
    public function testParseConvertSubIntersectQuery()
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
                    $sqlArray = DALQueryParser::parse($primary);
                } catch (DALParserException $e) {
                    $caught = TRUE;
                }
                $converter = new DALConverter();
                $sql       = $converter->convertToSql($sqlArray['query']);

                PHPUnit_FrameWork_Assert::assertTrue(trim($sql) === $this->_subQuery, 'Converted query is not as expected');
                $missingIntersectMsg = 'There is no INTERSECT clause in the returned array';
                PHPUnit_FrameWork_Assert::assertTrue(isset($sqlArray['query']['DELETE']['WHERE']['AND'][0]['IN']['CONDITIONS']['INTERSECT']), $missingIntersectMsg.$tc.'.');
                $missingSelectMsg = 'There should be two SELECT clause in the returned array';
                PHPUnit_FrameWork_Assert::assertTrue(count($sqlArray['query']['DELETE']['WHERE']['AND'][0]['IN']['CONDITIONS']['INTERSECT']['SELECTS']) === 2, $missingSelectMsg.$tc.'.');
            }//end if
        }//end foreach

        PHPUnit_Framework_Assert::assertFalse($caught, $msg.$tc.'.');
        $tc++;

    }//end testParseConvertSubIntersectQuery()


}//end class

?>