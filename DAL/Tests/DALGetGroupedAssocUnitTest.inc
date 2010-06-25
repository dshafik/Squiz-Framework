<?php
/**
 * Unit Tests for the DAL::getGroupedAssoc().
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * GetGroupedAssoc UnitTest.
 *
 * @since 4.0.0
 */
class DALGetGroupedAssocUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetGroupedAssocAsQuery()
    {
        Channels::createEventBasket();
        Channels::addToBasket('assetid', 19);
        $query = DAL::getQuery('Attribute', 'getAttributeDataSourcesByAssetid');
        Channels::removeEventBasket();

        $result   = DAL::getGroupedAssoc($query);
        $expected = array(
                     'AttributeDataSource' => array(
                                               array(
                                                'attributeid'    => 'name',
                                                'attribute_type' => 'text',
                                               ),
                                               array(
                                                'attributeid'    => 'page_content',
                                                'attribute_type' => 'long_text',
                                               ),
                                               array(
                                                'attributeid'    => 'created_datetime',
                                                'attribute_type' => 'datetime',
                                               ),
                                               array(
                                                'attributeid'    => 'updated_datetime',
                                                'attribute_type' => 'datetime',
                                               ),
                                               array(
                                                'attributeid'    => 'published_datetime',
                                                'attribute_type' => 'datetime',
                                               ),
                                              ),
                    );
        PHPUnit_Framework_Assert::assertEquals(1, count($result));
        PHPUnit_Framework_Assert::assertArrayHasKey('attributeid', $result['AttributeDataSource'][0]);
        PHPUnit_Framework_Assert::assertArrayHasKey('attribute_type', $result['AttributeDataSource'][0]);

        $result   = DAL::getGroupedAssoc($query, 'attribute_type');
        $expected = array(
                     'text'      => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'name',
                                          ),
                                    ),
                     'long_text' => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'page_content',
                                          ),
                                    ),
                     'datetime'  => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'created_datetime',
                                          ),
                                     1 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'updated_datetime',
                                          ),
                                     2 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'published_datetime',
                                          ),
                                    ),
                    );

        $rKey = array_keys($result);
        $eKey = array_keys($expected);
        sort($rKey);
        sort($eKey);
        PHPUnit_Framework_Assert::assertEquals($eKey, $rKey);

    }//end testGetGroupedAssocAsQuery()


    /**
     * Using array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetGroupedAssocAsArray()
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
                                                'attributeid'    => 'name',
                                                'attribute_type' => 'text',
                                               ),
                                               array(
                                                'attributeid'    => 'page_content',
                                                'attribute_type' => 'long_text',
                                               ),
                                               array(
                                                'attributeid'    => 'created_datetime',
                                                'attribute_type' => 'datetime',
                                               ),
                                              ),
                    );

        $result = DAL::getGroupedAssoc($array);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $result   = DAL::getGroupedAssoc($array, 'attribute_type');
        $expected = array(
                     'text'      => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'name',
                                          ),
                                    ),
                     'long_text' => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'page_content',
                                          ),
                                    ),
                     'datetime'  => array(
                                     0 => array(
                                           'data_source' => 'AttributeDataSource',
                                           'attributeid' => 'created_datetime',
                                          ),
                                    ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testGetGroupedAssocAsArray()


    /**
     * Using a simple array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetGroupedAssocAsSimpleArray()
    {
        $input = array(
                  array(
                   'a' => 1,
                   'b' => 'w',
                  ),
                  array(
                   'a' => 1,
                   'b' => 'x',
                  ),
                  array(
                   'a' => 2,
                   'b' => 'x',
                  ),
                  array(
                   'a' => 3,
                   'b' => 'y',
                  ),
                 );

        $result   = DAL::getGroupedAssoc($input);
        $expected = array(
                     1 => array(
                           array('b' => 'w'),
                           array('b' => 'x'),
                          ),
                     2 => array(
                           array('b' => 'x'),
                          ),
                     3 => array(
                           array('b' => 'y'),
                          ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $result   = DAL::getGroupedAssoc($input, 'b');
        $expected = array(
                     'w' => array(
                             array('a' => 1),
                            ),
                     'x' => array(
                             array('a' => 1),
                             array('a' => 2),
                            ),
                     'y' => array(
                             array('a' => 3),
                            ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testGetGroupedAssocAsSimpleArray()


}//end class

?>
