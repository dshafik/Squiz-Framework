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
 */
class FragmentExistsUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Assetion node.
     *
     * @var string $_assertionNode
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
        DALBaker::addSystemQueries('MergeSys1');
        $doc = new DomDocument();
        $doc->loadXML(self::$_assertionNode);
        $fragment = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQueryFragment('MergeSys2', $fragment);

    }//end setupSystems()


    /**
     * Tests that fragmentExists returns TRUE for the newly added fragment.
     *
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
     * @return void
     */
    public function testFragmentSystemExists()
    {
        self::setupSystems();
        $ret = DALBaker::fragmentExists('dummySystem', 'dummyFragment', 'MergeSys1.getLinks');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testFragmentSystemExists()


}//end class
