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
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit Tests for the addSystemQueries() method of DALBaker.
 *
 */
class MergeQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Simple Assertion node.
     *
     * @var   string $_assertionNode
     */
    private static $_assertionNode = '<query id="getAttributesTextAttribute" hookid="MergeSys1.getLinks">
                                          <assert-true>
                                              <function-call function="isset">
                                                <arg type="event_data">$attributes[\'text\']</arg>
                                              </function-call>
                                              <select>
                                                ...
                                              </select>
                                          </assert-true>
                                       </query>';


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(__FILE__).'/TestSystems/MergeSys1',
               );

    }//end getRequiredSystems()


    /**
     * Adds some fragments to the base queries.
     *
     * @return array(string)
     */
    public function setupSystems()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        FileSystem::clearDirectory(DAL::getQueryStorePath());
        DALBaker::addSystemQueries('MergeSys1');
        $doc = new DomDocument();
        $doc->loadXML(self::$_assertionNode);
        $fragment = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQueryFragment('MergeSys1', $fragment);

    }//end setupSystems()


    /**
     * Test mergeQuery() produces the correct number of conditions.
     *
     * @return void
     */
    public function testMergeSystemConditionCount()
    {
        $this->setupSystems();
        $ret     = DALBaker::mergeQuery('MergeSys1', 'getLinks');
        $matches = array();
        preg_match_all('/if \(/i', $ret, $matches);
        PHPUnit_Framework_Assert::assertEquals(1, count($matches[0]));

    }//end testMergeSystemConditionCount()


    /**
     * Test mergeQuery() produces the right variable name for the conditions.
     *
     * @return void
     */
    public function testMergeSystemConditionVariable()
    {
        $this->setupSystems();
        $ret     = DALBaker::mergeQuery('MergeSys1', 'getLinks');
        $matches = array();
        preg_match_all('/if \(\$issetAttributesText === TRUE\) {/i', $ret, $matches);
        PHPUnit_Framework_Assert::assertEquals(1, count($matches[0]));

    }//end testMergeSystemConditionVariable()


    /**
     * Test mergeQuery() has the correct number of $sql assignments.
     *
     * @return void
     */
    public function testMergeSystemAssignmentCount()
    {
        $this->setupSystems();
        $ret     = DALBaker::mergeQuery('MergeSys1', 'getLinksTwo');
        $matches = array();
        preg_match_all('/if \(/i', $ret, $matches);
        PHPUnit_Framework_Assert::assertEquals(0, count($matches[0]));

    }//end testMergeSystemAssignmentCount()


    /**
     * Test mergeQuery() still has no conditions for no assertions.
     *
     * @return void
     */
    public function testMergeSystemNoAssertionsConditions()
    {
        $this->setupSystems();
        $ret     = DALBaker::mergeQuery('MergeSys1', 'getLinksTwo');
        $matches = array();
        preg_match_all('/if \(/i', $ret, $matches);
        PHPUnit_Framework_Assert::assertEquals(0, count($matches[0]));

    }//end testMergeSystemNoAssertionsConditions()


    /**
     * Test mergeQuery() still has an $sql declaration for no assertions.
     *
     * @return void
     */
    public function testMergeSystemNoAssertionsAssignment()
    {
        $this->setupSystems();
        $ret     = DALBaker::mergeQuery('MergeSys1', 'getLinksTwo');
        $matches = array();
        preg_match_all('/\$query/i', $ret, $matches);
        PHPUnit_Framework_Assert::assertEquals(1, count($matches[0]));

    }//end testMergeSystemNoAssertionsAssignment()


}//end class

?>
