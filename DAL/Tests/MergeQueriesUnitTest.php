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
 * Unit Tests for the addSystemQueries() method of DALBaker.
 *
 * @since 4.0.0
 */
class MergeQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Simple Assertion node.
     *
     * @var   string $_assertionNode
     * @since 4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
