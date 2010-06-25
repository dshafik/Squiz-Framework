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
 * Unit tests for the validateSchemaUdts method.
 *
 * validateSchemaUdts() method throws exception if there is a problem with the
 * definition of a system's User-Defined types (UDT's).
 *
 * @since 4.0.0
 */
class ParseSchemaUnitTest extends AbstractMySourceUnitTest
{

    /**
     * The schema source used by unit tests.
     *
     * @var   string $_schemaSrc The XML source of the schema.
     * @since 4.0.0
     */
    private static $_schemaSrc = '<schema system="asset_type">
             <udts>
              <udt type="VARCHAR" size="100">TYPECODE</udt>
              <udt type="CHAR" size="100">UDTONE</udt>
              <udt type="DECIMAL" size="100">UDTTWO</udt>
              <udt type="FLOAT" size="100">UDTTHREE</udt>
              <udt type="VARCHAR" size="100">UDTFOUR</udt>
              <udt type="VARCHAR" size="100">UDTFIVE</udt>
             </udts>
             <tables>
              <table name="asset_type">
               <columns>
                <column type="TYPECODE" allow-null="true">type_code</column>
                <column type="VARCHAR" size="20" allow-null="true">version</column>
                <column type="VARCHAR" size="100" allow-null="true">name</column>
                <column type="VARCHAR" size="255" default="" allow-null="true">description</column>
               </columns>
               <indexes>
                <index name="asset_type_type_code">
                 <column>type_code</column>
                </index>
                <index name="asset_type_lvl">
                 <column>version</column>
                </index>
               </indexes>
               <constraints>
                <primary-key name="asset_type_pk">
                 <column>type_code</column>
                </primary-key>
                <foreign-key foreign-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                 <column references="type_code">type_code</column>
                </foreign-key>
               </constraints>
              </table>
             </tables>
            </schema>';


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
     * Tests that parseSchema returns a valid array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testParseSchema()
    {

        $doc = new DomDocument();
        $doc->loadXML(self::$_schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);
        $result     = DALBaker::parseSchema($schemaNode);

        $expected = array(
                     'CREATE' => array(
                                  array(
                                   'table'       => 'asset_type',
                                   'COLUMNS'     => array(
                                                     array(
                                                      'type'       => 'VARCHAR',
                                                      'size'       => '100',
                                                      'scale'      => '',
                                                      'name'       => 'type_code',
                                                      'allow-null' => 'true',
                                                      'default'    => '',
                                                     ),
                                                     array(
                                                      'type'       => 'VARCHAR',
                                                      'size'       => '20',
                                                      'scale'      => '',
                                                      'name'       => 'version',
                                                      'allow-null' => 'true',
                                                      'default'    => '',
                                                     ),
                                                     array(
                                                      'type'       => 'VARCHAR',
                                                      'size'       => '100',
                                                      'scale'      => '',
                                                      'name'       => 'name',
                                                      'allow-null' => 'true',
                                                      'default'    => '',
                                                     ),
                                                     array(
                                                      'type'       => 'VARCHAR',
                                                      'size'       => '255',
                                                      'scale'      => '',
                                                      'name'       => 'description',
                                                      'allow-null' => 'true',
                                                      'default'    => '',
                                                     ),
                                                    ),
                                   'INDEXES'     => array(
                                                     array(
                                                      'name'    => 'asset_type_type_code',
                                                      'COLUMNS' => array(
                                                                    'type_code',
                                                                   ),
                                                     ),
                                                     array(
                                                      'name'    => 'asset_type_lvl',
                                                      'COLUMNS' => array(
                                                                    'version',
                                                                   ),
                                                     ),
                                                    ),
                                   'CONSTRAINTS' => array(
                                                     'PRIMARY-KEYS' => array(
                                                                        array(
                                                                         'name'    => 'asset_type_pk',
                                                                         'COLUMNS' => array(
                                                                                       'type_code',
                                                                                      ),
                                                                        ),
                                                                       ),
                                                     'FOREIGN-KEYS' => array(
                                                                        array(
                                                                         'name'      => 'asset_type_fk1',
                                                                         'table'     => 'asset_type',
                                                                         'on-delete' => 'CASCADE',
                                                                         'COLUMNS'   => array(
                                                                                         array(
                                                                                          'name'       => 'type_code',
                                                                                          'references' => 'type_code',
                                                                                         ),
                                                                                        ),
                                                                        ),
                                                                       ),
                                                    ),
                                   'SEQUENCES'   => array(),
                                  ),
                                 ),
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testParseSchema()


    /**
     * Skips the test if the current db type is not the specified type.
     *
     * @param string $type The DB type to check.
     *
     * @since  4.0.0
     * @return void
     */
    protected function skipOnDbType($type)
    {
        $converter = DALBaker::getConverter();
        $dbType    = $converter->getName();

        if (strtolower($dbType) !== strtolower($type)) {
            $this->markTestSkipped();
            return TRUE;
        }

        return FALSE;

    }//end skipOnDbType()


    /**
     * Tests that convertCreateTable returns valid SQL string for each DB type.
     *
     * @since  4.0.0
     * @return void
     */
    protected function getSqlForTestSchema()
    {
        $doc = new DomDocument();
        $doc->loadXML(self::$_schemaSrc);
        $schemaNode = $doc->getElementsByTagName('schema')->item(0);
        $parsed     = DALBaker::parseSchema($schemaNode);
        $converter  = DALBaker::getConverter();
        $sql        = $converter->convertCreateTable($parsed['CREATE'][0]);

        return $sql;

    }//end getSqlForTestSchema()


}//end class

?>
