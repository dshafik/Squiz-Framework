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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';

/**
 * Unit tests for the getQueryType() method.
 *
 * getQueryType() returns the given queries type.
 *
 * @since 4.0.0
 */
class GetQueryTypeUnitTest extends AbstractMySourceUnitTest
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
     * Test that getQueryType returns valid string for non empty query tag.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonEmptyQueryTag()
    {
        $xml = '<query id="test"><select></select></query>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::getQueryType($query);

        PHPUnit_Framework_Assert::assertEquals('select', $result);

    }//end testNonEmptyQueryTag()


    /**
     * Test that getQueryType returns NULL when query tag is empty.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyQueryTag()
    {
        $xml = '<query id="test"></query>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::getQueryType($query);

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testEmptyQueryTag()


}//end class

?>
