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
 * Unit tests for the getElementsByTagNames() Method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLGetElementsByTagNamesUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetElementsOrder()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $elem      = $dom->getElementsByTagName('systems')->item(0);
        $systems   = XML::getElementsByTagNames(array('system'), $elem);
        $firstName = $systems[0]->getAttribute('name');
        PHPUnit_Framework_Assert::assertEquals('Url', $firstName);

    }//end testGetElementsOrder()


    /**
     * Test the wildcard element selector (*).
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetAllElements()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $elem        = $dom->getElementsByTagName('systems')->item(0);
        $allElements = XML::getElementsByTagNames(array('*'), $elem);
        PHPUnit_Framework_Assert::assertEquals(5, count($allElements));

    }//end testGetAllElements()


}//end class

?>
