<?php
/**
 * Unit Tests for the DAL::GetOne().
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
 * GetOne UnitTest.
 *
 */
class DALGetOneUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
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