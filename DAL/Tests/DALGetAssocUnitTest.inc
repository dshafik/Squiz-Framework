<?php
/**
 * Unit Tests for the DAL::getAssoc().
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage DAL
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * GetAssoc UnitTest.
 */
class DALGetAssocUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
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
