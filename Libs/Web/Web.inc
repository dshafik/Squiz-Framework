<?php
/**
 * Web related methods.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Web
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Web related methods.
 *
 * @since 4.0.0
 */
class Web
{

    /**
     * List of HTTP headers being sent out.
     *
     * @var   array
     * @since 4.0.0
     */
    public static $headersSent = array();

    /**
     * The current URL.
     *
     * @var   string
     * @since 4.0.0
     * @see   getCurrentUrl()
     */
    private static $_currentUrl = '';

    /**
     * The current URL with query string.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_currentUrlWithQueryStr = '';


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     *
     * @since 4.0.0
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Returns the current URL that has been navigated to.
     *
     * @param string $includeQueryString If TRUE the query string will be
     *                                   included with the url.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCurrentUrl($includeQueryString=TRUE)
    {
        if (self::$_currentUrl !== '' && $includeQueryString === FALSE) {
            return self::$_currentUrl;
        }

        if (self::$_currentUrlWithQueryStr !== '' && $includeQueryString === TRUE) {
            return self::$_currentUrlWithQueryStr;
        }

        $protocol = 'http';
        $url      = '';
        if (isset($_SERVER['HTTPS']) === TRUE) {
            if (strpos(strtolower($_SERVER['HTTPS']), 'on') !== FALSE) {
                $protocol = 'https';
            }
        }

        if (isset($_SERVER['HTTP_HOST']) === TRUE
            && isset($_SERVER['REQUEST_URI']) === TRUE
        ) {
            $url = $protocol.'://'.$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'];
        }

        if ($includeQueryString === FALSE) {
            if (isset($_SERVER['QUERY_STRING']) === TRUE) {
                $url = str_replace('?'.$_SERVER['QUERY_STRING'], '', $url);
                 // Remove the trailing slash if there is one.
                $url = rtrim($url, '/');
            }

            self::$_currentUrl = $url;
        } else {
            self::$_currentUrlWithQueryStr = $url;
        }

        return $url;

    }//end getCurrentUrl()


    /**
     * Returns the protocol string of the current URL.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCurrentWebProtocol()
    {
        $protocol = 'http';
        if (isset($_SERVER['HTTPS']) === TRUE) {
            if (strpos(strtolower($_SERVER['HTTPS']), 'on') !== FALSE) {
                $protocol = 'https';
            }
        }

        return $protocol;

    }//end getCurrentWebProtocol()


    /**
     * Clears the URL cache, mainly for unit test.
     *
     * @since  4.0.0
     * @return void
     */
    public static function clearCurrentUrl()
    {
        self::$_currentUrl             = '';
        self::$_currentUrlWithQueryStr = '';

    }//end clearCurrentUrl()


    /**
     * Resets the current url cache.
     *
     * @since  4.0.0
     * @return void
     */
    public static function resetCurrentUrl()
    {
        self::$_currentUrl = '';

    }//end resetCurrentUrl()


    /**
     * Strips the admin/edit suffix from the end of the specified url.
     *
     * @param string $url The url to strip the suffix from.
     *
     * @since  4.0.0
     * @return string
     */
    public static function stripUrlSuffix($url)
    {
        if (strpos($url, '/'.Init::EDIT_SUFFIX) !== FALSE) {
            $url = rtrim(substr($url, 0, (strlen($url) - strlen(Init::EDIT_SUFFIX))), '/');
        }

        return $url;

    }//end stripUrlSuffix()


    /**
     * Returns the reserved suffix (if any).
     *
     * @param string $url The url to test the suffix from.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getUrlSuffix($url)
    {
        $suffixes = array(
                     Init::EDIT_SUFFIX,
                     Init::PROFILE_SUFFIX,
                     Init::MODE_SUFFIX,
                    );

        foreach ($suffixes as $suffix) {
            if (strpos($url, '/'.$suffix) !== FALSE) {
                return $suffix;
            }
        }

        return '';

    }//end getUrlSuffix()


    /**
     * Strips the query string from the end of the specified url.
     *
     * @param string $url The url to strip the suffix from.
     *
     * @since  4.0.0
     * @return string
     */
    public static function stripUrlQueryString($url)
    {
        $parsed = parse_url($url);
        if (isset($parsed['query']) === TRUE) {
            $queryStr = '?'.$parsed['query'];
            if (strpos($url, $queryStr) !== FALSE) {
                $url = str_replace($queryStr, '', $url);
            }
        }

        return $url;

    }//end stripUrlQueryString()


    /**
     * Returns the URL query string.
     *
     * @param string $url The URL to get query string for.
     *                    Use current URL if this is not provided.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getQueryString($url='')
    {
        if ($url === '') {
            $url = self::getCurrentUrl();
        }

        $queryStr = parse_url($url, PHP_URL_QUERY);

        return $queryStr;

    }//end getQueryString()


    /**
     * Returns the URL query string params.
     *
     * @param string $url The URL to parse query string param for.
     * @param string $var The specific param to look for.
     *                    If not provided, will return the full param array.
     *
     * @since  4.0.0
     * @return mixed string|array
     */
    public static function getQueryStringParams($url='', $var='')
    {
        if ($url === '') {
            $url = self::getCurrentUrl();
        }

        $params   = array();
        $queryStr = parse_url($url, PHP_URL_QUERY);
        parse_str($queryStr, $params);

        if ($var !== '') {
            if (isset($params[$var]) === TRUE) {
                return $params[$var];
            } else {
                return '';
            }
        }

        return $params;

    }//end getQueryStringParams()


    /**
     * Adds given (var => value) list to the URL query string.
     *
     * @param array $vars Array index by var name.
     *
     * @since  4.0.0
     * @return string
     */
    public static function addToQueryString(array $vars)
    {
        $query = array();
        parse_str(self::getQueryString(), $query);
        $query = array_merge($query, $vars);
        $query = http_build_query($query);
        return $query;

    }//end addToQueryString()


    /**
     * Append query string to a URL.
     *
     * @param array  $vars Array index by var name.
     * @param string $url  The URL to add query string to.
     *                     Use current URL if this is not provided.
     *
     * @since  4.0.0
     * @return string
     */
    public static function appendQueryString(array $vars, $url)
    {
        if ($url === '') {
            $url = self::getCurrentUrl();
        }

        $query    = array();
        $queryStr = parse_url($url, PHP_URL_QUERY);

        parse_str($queryStr, $query);
        $query = array_merge($query, $vars);
        $query = http_build_query($query);

        $plain = self::stripUrlQueryString($url);
        $url   = $plain.'?'.$query;

        return $url;

    }//end appendQueryString()


    /**
     * Removes given list of vars from the URL query string.
     *
     * @param array  $vars Array index by var name.
     * @param string $url  The URL to remove query string from.
     *                     Use current URL if this is not provided.
     *
     * @since  4.0.0
     * @return string
     */
    public static function removeFromQueryString(array $vars, $url='')
    {
        $query = array();
        parse_str(self::getQueryString($url), $query);
        foreach ($vars as $var) {
            if (isset($query[$var]) === TRUE) {
                unset($query[$var]);
            }
        }

        $query = http_build_query($query);
        return $query;

    }//end removeFromQueryString()


    /**
     * Send HTTP headers.
     *
     * It also performs unit test clean up.
     *
     * @param array $headers List of key and value pairs.
     *
     * @since  4.0.0
     * @return void
     */
    public static function headers(array $headers)
    {
        if (headers_sent() === FALSE) {
            foreach ($headers as $header => $value) {
                if ($header === 'http') {
                    header($value);
                } else {
                    header($header.': '.$value);
                }
            }
        }

        if (isset(BaseSystem::$dataId) === TRUE
            && BaseSystem::$dataId === 'testing'
        ) {
            self::$headersSent = array_merge(self::$headersSent, $headers);
        }

    }//end headers()


    /**
     * Send HTTP header with the given key and value.
     *
     * @param string $key   If 'http' is specified for the key, then it does not
     *                      form 'key: value' format, instead it just sends the value
     *                      as a header.
     * @param string $value Value to send.
     *
     * @since  4.0.0
     * @return void
     */
    public static function header($key, $value)
    {
        if (headers_sent() === FALSE) {
            if ($key === 'http') {
                header($value);
            } else {
                header($key.': '.$value);
            }
        }

        if (isset(BaseSystem::$dataId) === TRUE
            && BaseSystem::$dataId === 'testing'
        ) {
            self::$headersSent[$key] = $value;
        }

    }//end header()


}//end class

?>
