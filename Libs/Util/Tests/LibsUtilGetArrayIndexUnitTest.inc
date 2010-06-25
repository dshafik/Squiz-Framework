<?php
/**
 * Unit Tests for the getArrayIndex() action.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Util/Util.inc';

/**
 * Unit Tests for the getArrayIndex() action.
 *
 * @since 4.0.0
 */
class LibsUtilGetArrayIndexUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test correct result is returned with an array of items.
     *
     * @since  4.0.0
     * @return void
     */
    public function testArrayWithItems()
    {
        $array = array(
                  'first item',
                  'second item',
                 );

        $result = Util::getArrayIndex($array, -1, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 0, NULL);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 1, NULL);
        PHPUnit_Framework_Assert::assertEquals('second item', $result);

        $result = Util::getArrayIndex($array, 2, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 'one', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testArrayWithItems()


    /**
     * Test correct result is returned with an array of named items.
     *
     * @since  4.0.0
     * @return void
     */
    public function testArrayWithNamedItems()
    {
        $array = array(
                  'one' => 'first item',
                  'two' => 'second item',
                 );

        $result = Util::getArrayIndex($array, '', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 'one', NULL);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 'two', NULL);
        PHPUnit_Framework_Assert::assertEquals('second item', $result);

        $result = Util::getArrayIndex($array, 'three', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 1, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testArrayWithNamedItems()


    /**
     * Test passing through a default value with an array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPassingThroughDefaultValues()
    {
        // Array of items.
        $default = 'Default';
        $array   = array('first item');

        $result = Util::getArrayIndex($array, 0, $default);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 2, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // Array of named items.
        $array = array('one' => 'first item');

        $result = Util::getArrayIndex($array, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex($array, 'one', $default);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 'three', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // No array.
        $result = Util::getArrayIndex(NULL, 0, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex(NULL, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // Different default value.
        $default = 6;

        $result = Util::getArrayIndex(NULL, 0, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex(NULL, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

    }//end testPassingThroughDefaultValues()


    /**
     * Test passing through empty values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPassingThroughNullValues()
    {
        $result = Util::getArrayIndex(NULL, NULL, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testPassingThroughNullValues()


}//end class

?>
