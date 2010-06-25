<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the transaction methods.
 *
 * @since 4.0.0
 */
class DALTransactionsUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
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
     * @since  4.0.0
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Tests that rollBack method will revert all changes since begining of transaction.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
