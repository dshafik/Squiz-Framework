<?php
/**
 * Unit tests for Attribute::setAttribute().
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for special characters in page_content attribute.
 *
 * @since 4.0.0
 */
class AttributeSetAttributeValueUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Test all characters available on the US keyboard.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAllCharacters()
    {
        $tid = 'oneLevel1';
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $string = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`~!@#$%^&*()-_=+{}[]\|;\':",<.>/?';
        Attribute::setAttributeValue($this->asts[$tid]['cid1'], 'page_content', $string);
        $value = Attribute::getAttributeValue($this->asts[$tid]['cid1'], 'page_content');
        PHPUnit_Framework_Assert::assertEquals($string, $value);

    }//end testAllCharacters()


}//end class

?>