<?php
/**
 * Unit tests for the permission cascade().
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
 * Unit tests for permission cascade.
 *
 */
class DALTransactionRollbackIntegrityUnitTest extends AbstractDataCleanUnitTest
{

    /**
     * Array of assets.
     *
     * Each element is the id of the asset that is created.
     *
     * @var   array
     */
    private static $_assets = array(
                               10000,
                               10001,
                              );


    /**
     * This function override the parent setup function.
     *
     * @return void
     * @access public
     */
    protected function setup()
    {
        // Override the parent method so it doesn't start the transaction.
        // We will cleanup by ourselves.
        return;

    }//end setup()


    /**
     * Test a normal rollback transaction.
     *
     * @return void
     * @access public
     */
    public function testRollbackCommitValid()
    {
        DAL::beginTransaction();
            $sql = 'insert into asset values ('.self::$_assets[0].', \'page\')';
            DAL::query($sql);
            $sql             = 'select * from asset where assetid = '.self::$_assets[0];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');
        DAL::commit();
        DAL::beginTransaction();
            $sql = 'insert into asset values ('.self::$_assets[1].', \'page\')';
            DAL::query($sql);
            $sql             = 'select * from asset where assetid ='.self::$_assets[1];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');
        DAL::rollback();

            $sql             = 'select * from asset where assetid ='.self::$_assets[1];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertTrue(empty($result), 'There is should not be a page entry in the asset table.');

    }//end testRollbackCommitValid()


    /**
     * This function test a rollback which is continued with a begin validity.
     *
     * @return void
     * @throws Exception Throw DB exception.
     * @access public
     */
    public function testRollbackBeginCommitInvalid()
    {
        DAL::beginTransaction();
        DAL::beginTransaction();
            $sql = 'insert into asset values ('.self::$_assets[0].', \'page\')';
            DAL::query($sql);
            $sql             = 'select * from asset where assetid = '.self::$_assets[0];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');
        DAL::commit();

        DAL::beginTransaction();
        $rollback = FALSE;
        try {
            $sql = 'insert into asset values ('.self::$_assets[0].', \'page\')';
            DAL::query($sql);
            $sql             = 'select * from asset where assetid = '.self::$_assets[0];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');
            DAL::commit();
        } catch (Exception $e) {
            DAL::rollback();
            $rollback      = TRUE;
            $currentTCount = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertTrue($currentTCount === 1, 'The transaction count should be 1 here, and commit should have been done.');
        }//end try

        PHPUnit_Framework_Assert::assertTrue($rollback, 'The transaction should have been rolled back');

        $sql             = 'select * from asset where assetid = '.self::$_assets[0];
        $resultStatement = DAL::query($sql);
        $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
        PHPUnit_Framework_Assert::assertTrue(empty($result), 'There is should be a page entry in the asset table.');

        $exception = FALSE;
        try {
            DAL::beginTransaction();
        } catch (Exception $e) {
            $exception = TRUE;
        }
        PHPUnit_Framework_Assert::assertTrue($exception, 'There should have been an exception throw when trying to start a transaction.');

        $currentTCount = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertTrue($currentTCount === 1, 'The transaction count should be 1 here.');

        // This would reset counter for now.
        DAL::commit();

        $exception = FALSE;
        try {
            DAL::beginTransaction();
        } catch (Exception $e) {
            $exception = TRUE;
        }//end try
        PHPUnit_Framework_Assert::assertFalse($exception, 'There should have not been an exception throw when trying to start a transaction.');

        try {
            $sql = 'insert into asset values ('.self::$_assets[1].', \'page\')';
            DAL::query($sql);
            $sql             = 'select * from asset where assetid = '.self::$_assets[1];
            $resultStatement = DAL::query($sql);
            $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
            PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');
            DAL::commit();
        } catch (Exception $e) {
            throw new Exception($e->getMessage());
        }//end try

        // Called commit, but it just decreasing the count, not a real commit.
        $currentTCount = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertTrue($currentTCount === 0, 'The transaction count should be 1 here.');

        // Back to normal, no more transaction block.
        $currentTCount = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertTrue($currentTCount === 0, 'The transaction count should be 0 here, and commit should have been done.');
        $sql             = 'select * from asset where assetid in ('.self::$_assets[1].','.self::$_assets[1].')';
        $resultStatement = DAL::query($sql);
        $result          = $resultStatement->fetchAll(PDO::FETCH_ASSOC);
        PHPUnit_Framework_Assert::assertFalse(empty($result), 'There is should be a page entry in the asset table.');

    }//end testRollbackBeginCommitInvalid()


    /**
     * This function override the parent tearDown function.
     *
     * @see    Testing/classes/AbstractDataCleanUnitTest#tearDown()
     * @return void
     * @throws Exception Throw an exception for db error.
     * @access protected
     */
    protected function tearDown()
    {
        // Remove the inserted entries.
        foreach (self::$_assets as $assetid) {
            $sql = 'delete from asset where assetid = \''.$assetid.'\'';
            try {
                $res = DAL::executeSql($sql);
            } catch (Exception $e) {
                throw new Exception($e->getMessage());
            }//end try
        }//end foreach

    }//end tearDown()


}//end class

?>