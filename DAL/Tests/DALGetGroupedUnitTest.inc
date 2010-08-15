<?php
/**
 * Unit Tests for the DAL::getGrouped().
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
 * GetGrouped UnitTest.
 */
class DALGetGroupedUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Using query object.
     *
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
