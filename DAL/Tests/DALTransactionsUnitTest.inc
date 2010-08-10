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

/**
 * Unit tests for the transaction methods.
 *
 */
class DALTransactionsUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                'AssetType',
                'Asset',
                'LocalAsset',
                'Attribute',
                'AttributeDataSource',
                'Lookup',
                'Linking',
                'AdjacencyList',
                'MaterialisedPathLinking',
                'RecursiveLinking',
                'Design',
                'Keyword',
                'Frontend',
                'DataSource',
                'FileDataSource',
                'Project',
                'RelationshipDataSource',
                dirname(__FILE__).'/TestSystems/TestDALSys1/',
               );

    }//end getRequiredSystems()


    protected function setup()
    {

    }//end setup()


    protected function tearDown()
    {

    }//end tearDown()

    /**
     * This test messes around with the ovens, so we need to bake separately.
     *
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Tests that rollBack method will revert all changes since begining of transaction.
     *
     * @return void
     */
    public function testRollBack()
    {
        try {
            $assets = array();
            $projectid = 1;
            Channels::includeSystem('Asset');
            Channels::includeSystem('Linking');
            DAL::beginTransaction();
                $assets[] = Asset::create('asset');
                Linking::createLink($projectid, $assets[0], 0);
                $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
                PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
            DAL::rollback();
            $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
            PHPUnit_Framework_Assert::assertFalse(in_array($assets[0], $children));
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testRollBack()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testCommit()
    {
        try {
            $assets = array();
            $projectid = 1;
            Channels::includeSystem('Asset');
            Channels::includeSystem('Linking');
            DAL::beginTransaction();
                $assets[] = Asset::create('asset');
                Linking::createLink($projectid, $assets[0], 0);
                $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
                PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
            DAL::commit();
            $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
            PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
        } catch (ChannelException $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testCommit()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testMultiTransactionsSingleCommit()
    {
        try {
            $assets = array();
            $projectid = 1;
            Channels::includeSystem('Asset');
            Channels::includeSystem('Linking');
            DAL::beginTransaction();
            DAL::beginTransaction();
            DAL::beginTransaction();
                $assets[] = Asset::create('asset');
                Linking::createLink($projectid, $assets[0], 0);
                $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
                PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
            DAL::commit();
            DAL::commit();

            DAL::rollback();

            $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
            PHPUnit_Framework_Assert::assertFalse(in_array($assets[0], $children));
        } catch (ChannelException $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testMultiTransactionsSingleCommit()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testMultiTransactionsCommit()
    {
        try {
            $assets = array();
            $projectid = 1;
            Channels::includeSystem('Asset');
            Channels::includeSystem('Linking');
            DAL::beginTransaction();
            DAL::beginTransaction();
            DAL::beginTransaction();
                $assets[] = Asset::create('asset');
                Linking::createLink($projectid, $assets[0], 0);
                $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
                PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
            DAL::commit();
            DAL::commit();
            DAL::commit();

            $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
            PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
        } catch (Exception $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testMultiTransactionsCommit()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testMultiTransactionsRollback()
    {
        try {
            $assets = array();
            $projectid = 1;
            Channels::includeSystem('Asset');
            Channels::includeSystem('Linking');
            DAL::beginTransaction();
            DAL::beginTransaction();
            DAL::beginTransaction();
                $assets[] = Asset::create('asset');
                Linking::createLink($projectid, $assets[0], 0);
                $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
                PHPUnit_Framework_Assert::assertTrue(in_array($assets[0], $children));
            DAL::rollback();
            // Just to restore the count to get ready for the next test.
            DAL::commit();
            DAL::commit();

            $children = DAL::getAssoc(Linking::getChildren($projectid, 0, 0), 0);
            PHPUnit_Framework_Assert::assertFalse(in_array($assets[0], $children));
        } catch (Exception $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testMultiTransactionsCommit()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testTransactionsDbClose()
    {
        $exception = FALSE;
        try {
            Channels::includeSystem('Asset');
            DAL::beginTransaction();
            Asset::create('asset');
            DAL::dbClose();
        } catch (DALException $e) {
            DAL::rollback();
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testMultiTransactionsCloseDB()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testMultiTransactionsDbClose()
    {
        $exception = FALSE;
        try {
            Channels::includeSystem('Asset');
            DAL::beginTransaction();
            DAL::beginTransaction();
            DAL::beginTransaction();
            Asset::create('asset');
            DAL::dbClose();
        } catch (DALException $e) {
            DAL::rollback();
            // Just to restore the count to get ready for the next test.
            DAL::commit();
            DAL::commit();
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testMultiTransactionsCloseDB()


    /**
     * Tests that commit method will save all changes since begining of transaction.
     *
     * @return void
     */
    public function testTransactionsRollbackDbClose()
    {
        try {
            Channels::includeSystem('Asset');
            DAL::beginTransaction();
            Asset::create('asset');
            DAL::rollback();
            DAL::dbClose();
        } catch (Exception $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testMultiTransactionsCloseDB()


    /**
     * Tests that closing DB after a commit does not throw exception.
     *
     * @return void
     */
    public function testTransactionsCommitDbClose()
    {
        try {
            Channels::includeSystem('Asset');
            DAL::beginTransaction();
            Asset::create('asset');
            DAL::commit();
            DAL::dbClose();
        } catch (Exception $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testMultiTransactionsCloseDB()


}//end class

?>
