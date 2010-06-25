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
class FragmentExistsUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Assetion node.
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
        DALBaker::addSystemQueries('MergeSys1');
        $doc = new DomDocument();
        $doc->loadXML(self::$_assertionNode);
        $fragment = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQueryFragment('MergeSys2', $fragment);

    }//end setupSystems()


    /**
     * Tests that fragmentExists returns TRUE for the newly added fragment.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentExists()
    {
        self::setupSystems();
        $ret = DALBaker::fragmentExists('MergeSys2', 'getAttributesTextAttribute', 'MergeSys1.getLinks');
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testFragmentExists()


    /**
     * Tests that fragmentExists returns FALSE for a non-existent fragment.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentDoesntExist()
    {
        self::setupSystems();
        $ret = DALBaker::fragmentExists('MergeSys2', 'dummyFragment', 'MergeSys1.getLinks');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testFragmentDoesntExist()


    /**
     * Tests that fragmentExists returns FALSE for a non-installed system.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentSystemExists()
    {
        self::setupSystems();
        $ret = DALBaker::fragmentExists('dummySystem', 'dummyFragment', 'MergeSys1.getLinks');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testFragmentSystemExists()


}//end class
