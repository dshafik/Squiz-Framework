<?php
/**
 * Lightweight Directory Access Protocol(LDAP) Lib.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Library for handling LDAP methods.
 *
 * @since 4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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