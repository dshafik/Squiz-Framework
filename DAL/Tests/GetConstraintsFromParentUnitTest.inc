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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the getConstraintsFromParent() method.
 *
 * getConstraintsFromParent() returns array of constraints from a parent node.
 *
 * @since 4.0.0
 */
class GetConstraintsFromParentUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test that getConstraintsFromParent returns valid array for a single PK.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPrimaryKeyOnly()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
                </test></query>';

        $doc = new DomDocument();
        $doc->loadXML($xml);
        $parent = $doc->getElementsByTagName('test')->item(0);
        $result = DALSchemaParser::getConstraintsFromParent($parent);

        $expected = array(
                     'PRIMARY-KEYS' => array(
                                        0 => array(
                                              'name'    => 'asset_type_pk',
                                              'COLUMNS' => array(
                                                            0 => 'type_code',
                                                           ),
                                             ),
                                       ),
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testPrimaryKeyOnly()


    /**
     * Test that getConstraintsFromParent returns valid array for a PK and FK.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPrimaryKeyAndForeignKey()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
                    <foreign-key ref-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                        <column references="type_code">parent_type</column>
                    </foreign-key>
                </test></query>';

        $doc = new DomDocument();
        $doc->loadXML($xml);
        $parent = $doc->getElementsByTagName('test')->item(0);
        $result = DALSchemaParser::getConstraintsFromParent($parent);

        $expected = array(
                     'PRIMARY-KEYS' => array(
                                        0 => array(
                                              'name'    => 'asset_type_pk',
                                              'COLUMNS' => array(
                                                            0 => 'type_code',
                                                           ),
                                             ),
                                       ),
                     'FOREIGN-KEYS' => array(
                                        0 => array(
                                              'name'      => 'asset_type_fk1',
                                              'table'     => '',
                                              'on-delete' => 'CASCADE',
                                              'COLUMNS'   => array(
                                                              0 => array(
                                                                    'name'       => 'parent_type',
                                                                    'references' => 'type_code',
                                                                   ),
                                                             ),
                                             ),
                                       ),
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testPrimaryKeyAndForeignKey()


}//end class

?>
