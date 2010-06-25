<?php
/**
 * Unit Tests for the DAL::getGrouped().
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * GetGrouped UnitTest.
 *
 * @since 4.0.0
 */
class DALGetGroupedUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetGroupedAsQuery()
    {
        Channels::createEventBasket();
        Channels::addToBasket('assetid', 19);
        $query = DAL::getQuery('Attribute', 'getAttributeDataSourcesByAssetid');
        Channels::removeEventBasket();

        $result   = DAL::getGrouped($query);
        $expected = array(
                     'AttributeDataSource' => array(
                                               array(
                                                'name',
                                                'text',
                                               ),
                                               array(
                                                'page_content',
                                                'long_text',
                                               ),
                                               array(
                                                'created_datetime',
                                                'datetime',
                                               ),
                                               array(
                                                'updated_datetime',
                                                'datetime',
                                               ),
                                               array(
                                                'published_datetime',
                                                'datetime',
                                               ),
                                              ),
                    );
        PHPUnit_Framework_Assert::assertEquals(1, count($result));
        foreach ($expected['AttributeDataSource'] as $elem) {
            PHPUnit_Framework_Assert::assertContains($elem, $result['AttributeDataSource']);
        }

    }//end testGetGroupedAsQuery()


    /**
     * Using array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetGroupedAsArray()
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
                  2 => array(
                        'data_source'    => 'AttributeDataSource',
                        'attributeid'    => 'created_datetime',
                        'attribute_type' => 'datetime',
                       ),
                 );

        $expected = array(
                     'AttributeDataSource' => array(
                                               array(
                                                'name',
                                                'text',
                                               ),
                                               array(
                                                'page_content',
                                                'long_text',
                                               ),
                                               array(
                                                'created_datetime',
                                                'datetime',
                                               ),
                                              ),
                    );

        $result = DAL::getGrouped($array);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testGetGroupedAsArray()


}//end class

?>
