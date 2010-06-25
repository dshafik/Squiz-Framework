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
 * Unit tests for the getElementsByAttributeValue() Method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLGetElementsByAttributeValueUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test for no attribute name.
     *
     * Test that getElementsByAttributeValue() return empty array when no
     * attribute name is given.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoAttributeName()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue($doc, $el1, '', 'cool');

        // Should be empty.
        PHPUnit_Framework_Assert::assertTrue(empty($retVal));

    }//end testNoAttributeName()


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @since  4.0.0
     * @return void
     */
    public function testSingleLevelSearch()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue(
            $doc,
            $el1,
            'name',
            'cool',
            FALSE
        );

        // Should be empty.
        PHPUnit_Framework_Assert::assertEquals(2, $retVal->length);

    }//end testSingleLevelSearch()


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @since  4.0.0
     * @return void
     */
    public function testMultiLevelSearch()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue($doc, $el1, 'name', 'cool');

        // Should be empty.
        PHPUnit_Framework_Assert::assertEquals(3, $retVal->length);

    }//end testMultiLevelSearch()


}//end class

?>
