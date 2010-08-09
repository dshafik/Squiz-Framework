<?php
/**
 * HTTP Request class.
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
 * HTTP Request class using cUrl.
 *
 */
class HTTPRequest
{

    /**
     * URI to send the request.
     *
     * @var   array
     */
    private $_uri = '';

    /**
     * Curl object.
     *
     * @var   array
     */
    private $_ch = NULL;

    /**
     * Request data string.
     *
     * @var   array
     */
    private $_requestData;

    /**
     * Response data string.
     *
     * @var   array
     */
    private $_responseData;

    /**
     * Proxy URI.
     *
     * @var   array
     */
    private $_proxy = NULL;

    /**
     * Proxy Auth information.
     *
     * @var   array
     */
    private $_proxyAuth = NULL;

    /**
     * SSL infomation.
     *
     * @var   array
     */
    private $_sslverify = TRUE;

    /**
     * Encoding type.
     *
     * @var   array
     */
    private $_encoding = 'iso-8859-1';

    /**
     * Cookie file path.
     *
     * @var   array
     */
    private $_cookiefile = NULL;

    /**
     * When it's TRUE, it sends HTTP_GET request instead.
     *
     * @var   array
     */
    private $_httpGet = FALSE;


    /**
     * Construct.
     *
     * @param string $uri    URI to connect.
     * @param array  $params Options to pass to HTTPRequest obj.
     *
     * @throws Exception If init failed.
     */
    public function __construct($uri='', array $params=array())
    {
        // Validate URI.
        $this->_uri = $uri;
        if (isset($params['encoding']) === TRUE) {
            $this->_encoding = $params['encoding'];
        }

        if (isset($params['proxy']) === TRUE) {
            $proxy    = $params['proxy'];
            $elements = parse_url($proxy);
            if (is_array($elements) === TRUE) {
                if (isset($elements['scheme']) === TRUE && isset($elements['host']) === TRUE) {
                    $this->_proxy = $elements['scheme'].'://'.$elements['host'];
                }

                if (isset($elements['port']) === TRUE) {
                    $this->_proxy = $this->_proxy.':'.$elements['port'];
                }

                if (isset($elements['user']) === TRUE && isset($elements['pass']) === TRUE) {
                    $this->_proxyAuth = $elements['user'].':'.$elements['pass'];
                }
            }
        }

        if (isset($params['sslverify']) === TRUE) {
            $this->_sslverify = $params['sslverify'];
        }

        if (isset($params['cookiefile']) === TRUE) {
             $this->_cookiefile = $params['cookiefile'];
        }

        $this->_ch = curl_init();

        $httpHeader = array(
                       'Content-type: text/xml; charset='.$this->_encoding,
                       'User-Agent: MySource_Mini/4.0.0',
                      );

        $ready = TRUE;
        if ($this->_proxy !== NULL) {
            $status = curl_setopt($this->_ch, CURLOPT_PROXY, $this->_proxy);
            if ($status === FALSE) {
                $ready = FALSE;
            }
        }

        if ($this->_proxyAuth !== NULL) {
            $status = curl_setopt($this->_ch, CURLOPT_PROXYUSERPWD, $this->_proxyAuth);
            if ($status === FALSE) {
                $ready = FALSE;
            }
        }

        if ($this->_cookiefile !== NULL) {
            $status = curl_setopt($this->_ch, CURLOPT_COOKIEFILE, $this->_cookiefile);
            if ($status === FALSE) {
                $ready = FALSE;
            }
        }

        $s1 = curl_setopt($this->_ch, CURLOPT_URL, $this->_uri);
        $s2 = curl_setopt($this->_ch, CURLOPT_RETURNTRANSFER, TRUE);
        $s3 = curl_setopt($this->_ch, CURLOPT_SSL_VERIFYPEER, $this->_sslverify);
        $s4 = curl_setopt($this->_ch, CURLOPT_HTTPHEADER, $httpHeader);

        // Init failed.
        if ($ready === FALSE
            || $s1 === FALSE
            || $s2 === FALSE
            || $s3 === FALSE
            || $s4 === FALSE
        ) {
            throw new Exception('HTTP Request init failed');
        }

    }//end __construct()


    /**
     * Response getter.
     *
     * @return string
     */
    public function getData()
    {
        return $this->_responseData;

    }//end getData()


    /**
     * Returns the response in array.
     *
     * This only can be used when raw response is a XML format string.
     *
     * @return array
     */
    public function getXMLToArray()
    {
        if (empty($this->_responseData) === TRUE) {
            return array();
        }

        include_once 'Libs/XML/XML.inc';
        $root   = simplexml_load_string($this->_responseData);
        $result = XML::simplexml2array($root);
        return $result;

    }//end getXMLToArray()


    /**
     * POST data setter.
     *
     * @param string $data Data to send.
     *
     * @return void
     */
    public function setData($data)
    {
        $this->_requestData = $data;

    }//end setData()


    /**
     * Set the request option to GET.
     *
     * @return void
     */
    public function sendGetRequest()
    {
        $this->_httpGet = TRUE;

    }//end sendGetRequest()


    /**
     * URI setter.
     *
     * @param string $uri URI to send the request.
     *
     * @return void
     */
    public function setUri($uri)
    {
        $this->_uri = $uri;
        curl_setopt($this->_ch, CURLOPT_URL, $this->_uri);

    }//end setUri()


    /**
     * Encoding setter.
     *
     * @param string $encoding Encoding type.
     *
     * @return void
     */
    public function setEncoding($encoding)
    {
        $this->_encoding = $encoding;

    }//end setEncoding()


    /**
     * Send request.
     *
     * @return boolean
     * @throws Exception If curl request fails.
     */
    public function sendRequest()
    {
        if ($this->_httpGet === FALSE) {
            curl_setopt($this->_ch, CURLOPT_POST, 1);
            curl_setopt($this->_ch, CURLOPT_POSTFIELDS, $this->_requestData);
        } else {
            curl_setopt($this->_ch, CURLOPT_POST, 0);
        }

        $result = curl_exec($this->_ch);
        $errno  = curl_errno($this->_ch);
        if ($errno !== 0) {
            throw new Exception('Sending request failed, Error code : '.$errno);
        }

        $info = curl_getinfo($this->_ch);
        if ($info['http_code'] !== 200) {
            $msg  = 'Curl returned non 200 HTTP code: ';
            $msg .= $info['http_code'];
            $msg .= ' Response body:';
            $msg .= $result;
            throw new Exception($msg);
        }

        $this->_responseData = $result;
        return TRUE;

    }//end sendRequest()


}//end class

?>
