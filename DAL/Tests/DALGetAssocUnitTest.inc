<?php
/**
 * Unit Tests for the DAL::getAssoc().
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * GetAssoc UnitTest.
 *
 * @since 4.0.0
 */
class DALGetAssocUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetAssocAsQuery()
    {
        Channels::createEventBasket();
        Channels::addToBasket('assetid', 19);
        $query = DAL::getQuery('Attribute', 'getAttributeDataSourcesByAssetid');
        Channels::removeEventBasket();

        // No specific column.
        $result   = DAL::getAssoc($query);
        $expected = array(
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
                     3 => array(
                           'data_source'    => 'AttributeDataSource',
                           'attributeid'    => 'updated_datetime',
                           'attribute_type' => 'datetime',
                          ),
                     4 => array(
                           'data_source'    => 'AttributeDataSource',
                           'attributeid'    => 'published_datetime',
                           'attribute_type' => 'datetime',
                          ),
                    );
        PHPUnit_Framework_Assert::assertEquals(5, count($result));
        PHPUnit_Framework_Assert::assertArrayHasKey('data_source', $result[0]);
        PHPUnit_Framework_Assert::assertArrayHasKey('attributeid', $result[0]);
        PHPUnit_Framework_Assert::assertArrayHasKey('attribute_type', $result[0]);

        // Column 1.
        $result   = DAL::getAssoc($query, 1);
        $expected = array(
                     'name',
                     'page_content',
                     'created_datetime',
                     'updated_datetime',
                     'published_datetime',
                    );
        sort($result);
        sort($expected);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Column 2.
        $result   = DAL::getAssoc($query, 2);
        $expected = array(
                     'text',
                     'long_text',
                     'datetime',
                     'datetime',
                     'datetime',
                    );
        sort($result);
        sort($expected);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Column 3, not found.
        try {
            $result = DAL::getAssoc($query, 3);
        } catch (PDOException $e) {
            $error = 'SQLSTATE[HY000]: General error: Invalid column index';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
        }

    }//end testGetAssocAsQuery()


    /**
     * Using array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetAssocAsArray()
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

        // No specific column.
        $result = DAL::getAssoc($array);
        PHPUnit_Framework_Assert::assertEquals($array, $result);

        // Column 1.
        $result   = DAL::getAssoc($array, 1);
        $expected = array(
                     'name',
                     'page_content',
                     'created_datetime',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Column 2.
        $result   = DAL::getAssoc($array, 2);
        $expected = array(
                     'text',
                     'long_text',
                     'datetime',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        // Column 3, not found.
        try {
            $result = DAL::getAssoc($array, 3);
        } catch (PDOException $e) {
            $error = 'Invalid column index';
            PHPUnit_Framework_Assert::assertEquals($error, $e->getMessage());
        }

    }//end testGetAssocAsArray()


}//end class

?>
