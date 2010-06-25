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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the queryIdIsUnique() method.
 *
 * queryIdIsUnique() returns FALSE if given query id already used by another
 * query.
 *
 * @since 4.0.0
 */
class QueryIdIsUniqueUnitTest extends AbstractMySourceUnitTest
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
     * Test that queryIdIsUnique returns false if query id is not unique.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonUniqueId()
    {
        $xml = '<queries><query id="test"></query><query id="test"></query></queries>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::queryIdIsUnique($doc, $query);

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testNonUniqueId()


    /**
     * Test that queryIdIsUnique returns true if query id is unique.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUniqueId()
    {
        $xml = '<queries><query id="test"></query><query id="test2"></query></queries>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $query  = $doc->getElementsByTagName('query')->item(0);
        $result = DALBaker::queryIdIsUnique($doc, $query);

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testUniqueId()


}//end class

?>
