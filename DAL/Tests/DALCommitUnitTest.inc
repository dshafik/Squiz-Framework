<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the commit method.
 */
class DALCommitUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that no exceptions thrown when trying to commit when no transactions started.
     *
     * @return void
     */
    public function testCommitOnNoTransaction()
    {
        try {
            DAL::commit();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(-1, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        // Make sure this test does not mess up the transaction count.
        DAL::beginTransaction();
        $count = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertEquals(0, $count);

    }//end testCommitOnNoTransaction()


    /**
     * Tests that no exceptions thrown when commiting an open transactions.
     *
     * @return void
     */
    public function testCommitTransaction()
    {
        try {
            DAL::beginTransaction();
            DAL::commit();

            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testCommitTransaction()


}//end class

?>
