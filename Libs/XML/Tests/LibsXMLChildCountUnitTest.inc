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
 * Unit tests for the childCount() method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLChildCountUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the childCount() returns 0 for a node that has no child elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text</test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(0, $ret);

    }//end testNoChildren()


    /**
     * Test the childCount() returns 0 for a node that has no child elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSingleChild()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(1, $ret);

    }//end testSingleChild()


    /**
     * Test the childCount() returns correct result for multiple children.
     *
     * @since  4.0.0
     * @return void
     */
    public function testChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /><tmp /><tmp /></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(3, $ret);

    }//end testChildren()


    /**
     * Sub-children test.
     *
     * Test the childCount() returns correct result for a node that has children
     * with sub children.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSubChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /><tmp /><tmp><child /></tmp></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(3, $ret);

    }//end testSubChildren()


}//end class

?>
