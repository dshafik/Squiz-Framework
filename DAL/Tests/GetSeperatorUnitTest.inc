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
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit Tests for the getSeperator() methods.
 */
class GetSeperatorUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Tests the getSeperator() method for 'union-all'.
     *
     * @return void
     */
    public function testGetSeperatorUnionAll()
    {
        $expected = '\nUNION-ALL\n';
        $ret      = DALBaker::getSeperator('union-all');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorUnionAll()


    /**
     * Tests the getSeperator() method for 'and'.
     *
     * @return void
     */
    public function testGetSeperatorAnd()
    {
        $expected = '\nAND\n';
        $ret      = DALBaker::getSeperator('and');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorAnd()


    /**
     * Tests the getSeperator() method for 'or'.
     *
     * @return void
     */
    public function testGetSeperatorOr()
    {
        $expected = '\nOR\n';
        $ret      = DALBaker::getSeperator('or');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorOr()


}//end class
