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
 * Unit Tests for the reading and writing Query XML files.
 */
class QueryXmlUnitTest extends AbstractMySourceUnitTest
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
     * Creates the base XML files for testing.
     *
     * @return void
     */
    public function generateBaseSystemXmlFile()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        DALbaker::generateQueryXml('TestSystem', 'TestQuery');

    }//end generateBaseSystemXmlFile()


    /**
     * Creates the base XML files for testing.
     *
     * @return DomDocument
     */
    public function getSystemXmlFile()
    {
        return DALbaker::getQueryXml('TestSystem', 'TestQuery');

    }//end getSystemXmlFile()


    /**
     * Tests the queryXmlExists method finds the newly created XML file.
     *
     * @return void
     */
    public function testXmlExists()
    {
        $this->generateBaseSystemXmlFile();
        $ret = DALbaker::queryXmlExists('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testXmlExists()


    /**
     * Tests the queryXmlExists method returns FALSE for non-existent file.
     *
     * @return void
     */
    public function testXmlExistsNoFile()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $ret = DALbaker::queryXmlExists('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testXmlExistsNoFile()


    /**
     * Tests the the XML file exists after saving.
     *
     * @return void
     */
    public function testSaveXmlExists()
    {
        $this->generateBaseSystemXmlFile();
        $doc = $this->getSystemXmlFile();
        DALBaker::saveQueryXml('TestSystem', 'TestQuery', $doc);
        $ret = DALbaker::queryXmlExists('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testSaveXmlExists()


    /**
     * Tests the the saved file contains the correct contents.
     *
     * @return void
     */
    public function testSaveXmlCorrectContents()
    {
        $this->generateBaseSystemXmlFile();
        $doc      = $this->getSystemXmlFile();
        $baseNode = $doc->getElementsByTagName('basequery')->item(0);
        $testNode = $doc->createElement('testnode');
        $baseNode->appendChild($testNode);
        DALBaker::saveQueryXml('TestSystem', 'TestQuery', $doc);
        $doc2         = $this->getSystemXmlFile();
        $testNodeList = $doc->getElementsByTagName('testnode');
        PHPUnit_Framework_Assert::assertEquals(1, $testNodeList->length);

    }//end testSaveXmlCorrectContents()


}//end class

?>
