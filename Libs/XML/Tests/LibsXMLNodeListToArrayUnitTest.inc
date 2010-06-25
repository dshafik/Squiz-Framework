<?php
/**
 * XML Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/XML.inc';

/**
 * Unit tests for the NodeListToArray Method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLNodeListToArrayUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test that clearDirectory() with deleteRoot === FALSE leaves an empty dir.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNodeListSize()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $root    = $dom->documentElement;
        $systems = $root->getElementsByTagName('system');
        $ret     = XML::nodeListToArray($systems);
        PHPUnit_Framework_Assert::assertTrue(count($ret) === 5);

    }//end testNodeListSize()


    /**
     * Test the content/order of values returned by NodeListToArray().
     *
     * @since  4.0.0
     * @return void
     */
    public function testNodeListOrder()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $root    = $dom->documentElement;
        $systems = $root->getElementsByTagName('system');
        $ret     = XML::nodeListToArray($systems);

        $firstElement = $ret[0];
        $firstName    = $firstElement->getAttribute('name');
        PHPUnit_Framework_Assert::assertTrue($firstName === 'Url');

    }//end testNodeListOrder()


}//end class

?>
