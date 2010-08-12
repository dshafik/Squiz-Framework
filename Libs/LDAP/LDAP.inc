<?php
/**
 * Lightweight Directory Access Protocol(LDAP) Lib.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Library for handling LDAP methods.
 */
class LDAP
{


    /**
     * Returns the LDAP object.
     *
     * @param string  $hostname The host name to connect to.
     * @param integer $port     The port to connect to.
     * @param string  $bdn      The DN used to bind to the LDAP server.
     *                          Use NULL for anonymous bind.
     * @param string  $password The password used to connect to the LDAP server.
     *
     * @return object
     */
    public static function connectToLDAP(
        $hostname,
        $port=389,
        $bdn=NULL,
        $password=NULL
    ) {
        $conn = ldap_connect($hostname, $port);
        ldap_set_option($conn, LDAP_OPT_DEREF, LDAP_DEREF_NEVER);
        ldap_set_option($conn, LDAP_OPT_PROTOCOL_VERSION, 3);
        ldap_set_option($conn, LDAP_OPT_TIMELIMIT, 30);
        @ldap_bind($conn, $bdn, $password);

        return $conn;

    }//end connectToLDAP()


    /**
     * Used by authentication system to login a LDAP user.
     *
     * @param object $conn     The LDAP connection.
     * @param string $bdn      The DN used to bind to the LDAP server.
     * @param string $password The password used to connect to the LDAP server.
     *
     * @return boolean
     */
    public static function connectAsUser(
        $conn,
        $bdn,
        $password
    ) {
        $r = @ldap_bind($conn, $bdn, $password);
        return $r;

    }//end connectAsUser()


    /**
     * Disconnect from the LDAP server.
     *
     * @param object $conn The LDAP connection.
     *
     * @return boolean
     */
    public static function disconnect($conn)
    {
        $r = ldap_close($conn);
        return $r;

    }//end disconnect()


    /**
     * Search the directory starting at dn using filters.
     *
     * @param object  $conn        The LDAP connection.
     * @param string  $startdn     The DN to start searching at.
     * @param string  $filter      Search filter.
     * @param boolean $multiLevel  Search for multiple levels.
     * @param boolean $multiResult Search for multiple entries.
     * @param mixed   $attributes  An array of attributes to fetch.
     * @param integer $limit       Limit the count of entries fetched.
     *                             Zero to return all entries.
     * @param string  $sortBy      Sort filter.
     *
     * @return mixed
     */
    public static function search(
        $conn,
        $startdn,
        $filter,
        $multiLevel=TRUE,
        $multiResult=TRUE,
        $attributes=array('*'),
        $limit=0,
        $sortBy=NULL
    ) {
        if ($conn === NULL) {
            return FALSE;
        }

        if ($multiLevel === TRUE && $multiResult === TRUE) {
            // Normal search.
            $sr = @ldap_search($conn, $startdn, $filter, $attributes, FALSE, $limit);
        } else if ($multiResult === TRUE) {
            // Single level search.
            $sr = @ldap_list($conn, $startdn, $filter, $attributes, FALSE, $limit);
        } else {
            // Reading single entry using dn.
            $sr = @ldap_read($conn, $startdn, $filter, $attributes);
        }

        if ($sortBy !== NULL) {
            ldap_sort($conn, $sr, $sortBy);
        }

        return $sr;

    }//end search()


    /**
     * Return the entries in a given search result.
     *
     * @param object $conn The LDAP connection.
     * @param object $sr   The search result identifier.
     *
     * @return array
     */
    public static function getEntries($conn, $sr)
    {
        if ($sr === FALSE) {
            return array();
        }

        $entries = ldap_get_entries($conn, $sr);
        if ($entries === FALSE) {
            $entries = array();
        }

        return $entries;

    }//end getEntries()


    /**
     * Return the number of entries in a given search result.
     *
     * @param object $conn The LDAP connection.
     * @param object $sr   The search result identifier.
     *
     * @return integer
     */
    public static function getNumEntries($conn, $sr)
    {
        if ($sr === FALSE) {
            return 0;
        }

        $count = ldap_count_entries($conn, $sr);
        if ($count === FALSE) {
            $count = 0;
        }

        return $count;

    }//end getNumEntries()


    /**
     * Format search entries into a readable array.
     *
     * @param array $entries Search result entries.
     *
     * @return array
     */
    public static function getData(array $entries)
    {
        $data = array();
        foreach ($entries as $key => $val) {
            $res = array();
            if ($key === 'count') {
                continue;
            }

            if (isset($val['dn']) === FALSE) {
                if (isset($val['distinguishedname']) === TRUE) {
                    $res['dn'] = $val['distinguishedname'];
                }
            } else {
                $res['dn'] = $val['dn'];
            }

            foreach ($val as $k => $v) {
                if (is_array($v) === TRUE) {
                    if (isset($v['count']) === TRUE) {
                        unset($v['count']);
                    }

                    if (count($v) > 1) {
                        $res[$k] = $v;
                    } else {
                        $res[$k] = array_shift($v);
                    }
                }
            }

            $data[] = $res;
        }//end foreach

        return $data;

    }//end getData()


}//end class

?>