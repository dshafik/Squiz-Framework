<?php
/**
 * Unit tests for the permission updates before and after links are created.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage System
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for special characters in page_content attribute.
 *
 * @since 4.0.0
 */
class AttributeValueUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Test all characters available on the US keyboard.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSimple()
    {
        $tid = 'multiParent4';
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $value = 'Test Value';

        Attribute::setAttributeValue($this->asts[$tid]['pid1'], 'name', $value);
        $attrVal = Attribute::getAttributeValue($this->asts[$tid]['pid1'], 'name');
        self::assertEquals($value, $attrVal);

        // Test 0 value.
        $value = '0';
        Attribute::setAttributeValue($this->asts[$tid]['pid1'], 'name', $value);
        $attrVal = Attribute::getAttributeValue($this->asts[$tid]['pid1'], 'name');
        self::assertEquals($value, $attrVal);

    }//end testSimple()


    /**
     * Test batchGetAttributeValue() with normal assetids.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBatchGetAttributeValue()
    {
        $tid = 'multiParent4';
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $name  = 'Test Value';
        $ctime = '2009-11-17 11:48:15+11';
        $utime = '2009-11-17 11:49:22+11';

        $assetid = $this->asts[$tid]['pid1'];
        Attribute::setAttributeValue($assetid, 'name', $name);
        Attribute::setAttributeValue($assetid, 'created_datetime', $ctime);
        Attribute::setAttributeValue($assetid, 'updated_datetime', $utime);
        $assetid2 = $this->asts[$tid]['pid2'];
        $name2    = $name.' 2';
        Attribute::setAttributeValue($assetid2, 'name', $name2);
        Attribute::setAttributeValue($assetid2, 'created_datetime', $ctime);

        $expected = array(
                     $assetid2 => array(
                                   'created_datetime' => $ctime,
                                   'name'             => $name2,
                                  ),
                     $assetid  => array(
                                   'created_datetime' => $ctime,
                                   'updated_datetime' => $utime,
                                   'name'             => $name,
                                  ),
                    );

        $attrVal = Attribute::batchGetAttributeValue(array($assetid, $assetid2), array('name', 'created_datetime', 'updated_datetime'));
        PHPUnit_Framework_Assert::assertEquals($expected, $attrVal);

    }//end testBatchGetAttributeValue()


    /**
     * Test that context attributes work.
     *
     * @since  4.0.0
     * @return void
     */
    public function testContexts()
    {
        $tid = 'multiParent4';
        Channels::includeSystem('Context');
        Context::setCurrentContext('');
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $context   = '00000001';
        $masterVal = 'Test Value';
        $assetid   = $this->asts[$tid]['pid1'];
        Attribute::setAttributeValue($assetid, 'name', $masterVal);

        // Get the attr val of assetid+context. It should be the same as master.
        Context::setCurrentContext($context);
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($masterVal, $attrVal);

        // Create the context asset.
        $contextAssetid = Context::createAsset($assetid);
        PHPUnit_Framework_Assert::assertEquals($assetid.'.'.$context, $contextAssetid);

        // Get the attr val for the context. Should be same as master still, cause it was not modified.
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($masterVal, $attrVal);

        // Update context value.
        $contextVal = 'ABCD';
        Attribute::setAttributeValue($assetid, 'name', $contextVal);

        // Set the current context to empty and make sure master value is not changed.
        Context::setCurrentContext('');
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($masterVal, $attrVal);

        // Set the context again and get its name attr. Should be the new context value.
        Context::setCurrentContext($context);
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($contextVal, $attrVal);

        // Create another context (safe-edit).
        $newContext = '01000001';

        // Switch to new context and create the asset.
        Context::setCurrentContext($newContext);
        $safeEditAssetid = Context::createAsset($assetid);
        PHPUnit_Framework_Assert::assertEquals($assetid.'.'.$newContext, $safeEditAssetid);

        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($contextVal, $attrVal);

        $safeEditVal = 'SAFE EDIT';
        Attribute::setAttributeValue($assetid, 'name', $safeEditVal);
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($safeEditVal, $attrVal);

        // Check non-safe edit context and the master.
        Context::setCurrentContext('');
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($masterVal, $attrVal);

        Context::setCurrentContext($context);
        $attrVal = Attribute::getAttributeValue($assetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($contextVal, $attrVal);

        // Get the safe-edit context with contextid already in assetid.
        $attrVal = Attribute::getAttributeValue($safeEditAssetid, 'name');
        PHPUnit_Framework_Assert::assertEquals($safeEditVal, $attrVal);

    }//end testContexts()


    /**
     * Test batchSet/GetAttributeValue functions with contexts.
     *
     * @since  4.0.0
     * @return void
     */
    public function testContextsWithBatchGetSetAttrVal()
    {
        $tid = 'multiParent1';
        Channels::includeSystem('Context');
        Context::setCurrentContext('');
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $context    = '00000001';
        $masterVals = array(
                       'Test Value',
                       'Test Val 2',
                      );
        $assetids   = array(
                       $this->asts[$tid]['pid1'],
                       $this->asts[$tid]['pid2'],
                      );

        Attribute::setAttributeValue($assetids[0], 'name', $masterVals[0]);
        Attribute::setAttributeValue($assetids[1], 'name', $masterVals[1]);

        $retVal   = Attribute::batchGetAttributeValue($assetids, array('name'));
        $expected = array(
                     $assetids[1] => array('name' => $masterVals[1]),
                     $assetids[0] => array('name' => $masterVals[0]),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

        // Now change context and test again..
        Context::setCurrentContext($context);
        foreach ($assetids as $assetid) {
            Context::createAsset($assetid);
        }

        $retVal   = Attribute::batchGetAttributeValue($assetids, array('name'));
        $expected = array(
                     $assetids[1] => array('name' => $masterVals[1]),
                     $assetids[0] => array('name' => $masterVals[0]),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

        // Set the attribute value of both.
        $ctime       = '2009-11-17 11:48:15+11';
        $utime       = '2009-11-17 12:49:22+11';
        $contextVals = array(
                        'Context Val 1',
                        'Context Val 2',
                       );

        Attribute::setAttributeValue($assetids[0], 'name', $contextVals[0]);
        Attribute::setAttributeValue($assetids[1], 'name', $contextVals[1]);
        Attribute::setAttributeValue($assetids[0], 'created_datetime', $ctime);
        Attribute::setAttributeValue($assetids[0], 'updated_datetime', $utime);
        Attribute::setAttributeValue($assetids[1], 'updated_datetime', $utime);

        $expected = array(
                     $assetids[1] => array(
                                      'updated_datetime' => $utime,
                                      'name'             => $contextVals[1],
                                     ),
                     $assetids[0] => array(
                                      'created_datetime' => $ctime,
                                      'updated_datetime' => $utime,
                                      'name'             => $contextVals[0],
                                     ),
                    );

        $retVal = Attribute::batchGetAttributeValue($assetids, array('name', 'created_datetime', 'updated_datetime'));
        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

        // Change context to master and make sure values of master asset is not changed.
        Context::setCurrentContext('');
        $expected = array(
                     $assetids[1] => array('name' => $masterVals[1]),
                     $assetids[0] => array('name' => $masterVals[0]),
                    );
        $retVal   = Attribute::batchGetAttributeValue($assetids, array('name', 'created_datetime', 'updated_datetime'));
        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

        // Batch set.
        Context::setCurrentContext($context);

        $ctime = '2009-11-17 12:12:12+11';
        $name  = 'TEST 1 2 3';
        Attribute::batchSetAttributeValue(
            $assetids, array(
                        'name'             => $name,
                        'created_datetime' => $ctime,
                       )
        );

        $expected = array(
                     $assetids[1] => array(
                                      'created_datetime' => $ctime,
                                      'name'             => $name,
                                     ),
                     $assetids[0] => array(
                                      'created_datetime' => $ctime,
                                      'name'             => $name,
                                     ),
                    );

        $retVal = Attribute::batchGetAttributeValue($assetids, array('name', 'created_datetime'));
        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testContextsWithBatchGetSetAttrVal()


}//end class

?>
