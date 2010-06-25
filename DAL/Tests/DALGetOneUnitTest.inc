<?php
/**
 * Unit Tests for the DAL::GetOne().
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * GetOne UnitTest.
 *
 * @since 4.0.0
 */
class DALGetOneUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOneAsQuery()
    {
        Channels::createEventBasket();
        Channels::addToBasket('assetid', 20);
        $query = DAL::getQuery('Attribute', 'getAttributeDataSourcesByAssetid');
        Channels::removeEventBasket();

        // No specific column.
        $result = DAL::getOne($query);
        PHPUnit_Framework_Assert::assertEquals('AttributeDataSource', $result);

        // Column 1.
        $result   = DAL::getOne($query, 1);
        $possible = array(
                     'name',
                     'page_content',
                     'created_datetime',
                     'updated_datetime',
                     'published_datetime',
                    );
        PHPUnit_Framework_Assert::assertType('string', $result);
        PHPUnit_Framework_Assert::assertContains($result, $possible);

        // Column 2.
        $result   = DAL::getOne($query, 2);
        $possible = array(
                     'text',
                     'long_text',
                     'datetime',
                    );
        PHPUnit_Framework_Assert::assertType('string', $result);
        PHPUnit_Framework_Assert::assertContains($result, $possible);

        // Column 3, not found.
        $result = DAL::getOne($query, 3);
        PHPUnit_Framework_Assert::assertEquals('', $result);

    }//end testGetOneAsQuery()


    /**
     * Using string.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOneAsString()
    {
        $str    = 'test';
        $result = DAL::getOne($str);
        PHPUnit_Framework_Assert::assertEquals($str, $result);

    }//end testGetOneAsString()


    /**
     * Using array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOneAsArray()
    {
        $array = array(
                  0 => array(
                        'data_source'    => 'AttributeDataSource',
                        'attributeid'    => 'name',
                        'attribute_type' => 'text',
                       ),
                  1 => array(
                        'data_source'    => 'AttributeDataSource',
                        'attributeid'    => 'page_content',
                        'attribute_type' => 'long_text',
                       ),
                 );

        // No specific column.
        $result = DAL::getOne($array);
        PHPUnit_Framework_Assert::assertEquals('AttributeDataSource', $result);

        // Column 1.
        $result = DAL::getOne($array, 1);
        PHPUnit_Framework_Assert::assertEquals('name', $result);

        // Column 2.
        $result = DAL::getOne($array, 2);
        PHPUnit_Framework_Assert::assertEquals('text', $result);

        // Column 3, not found.
        $result = DAL::getOne($array, 3);
        PHPUnit_Framework_Assert::assertEquals('', $result);

    }//end testGetOneAsArray()


}//end class

?>