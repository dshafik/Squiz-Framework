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
 * Unit Tests for the reading and writing Query XML files.
 *
 * @since 4.0.0
 */
class QueryXmlUnitTest extends AbstractMySourceUnitTest
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
     * Creates the base XML files for testing.
     *
     * @since  4.0.0
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
     * @since  4.0.0
     * @return DomDocument
     */
    public function getSystemXmlFile()
    {
        return DALbaker::getQueryXml('TestSystem', 'TestQuery');

    }//end getSystemXmlFile()


    /**
     * Tests the queryXmlExists method finds the newly created XML file.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
