<?php
/**
 * Tests for parsing query with INTERSECT.
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
require_once dirname(dirname(__FILE__)).'/Parsers/DALIntersectParser.inc';
require_once dirname(dirname(__FILE__)).'/Converters/DALConverter.inc';

/**
 * Unit tests for the validateQuery method.
 */
class ParseConvertIntersectQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * The converted Intersect sub query.
     *
     * @var $_subQuery The converted Intersect sub query
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
     * @var $_subQuery The converted Intersect primary query
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