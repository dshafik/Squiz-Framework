<?php
/**
 * XMLRPC client.
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

require_once 'Libs/Web/HTTPRequest.inc';

/**
 * XMLRPC client.
 *
 */
class XMLRPCClient
{

    /**
     * List of glossary term ids.
     *
     * @var   array
     */
    private $_uri = '';

    /**
     * List of glossary term ids.
     *
     * @var   array
     */
    private $_options = array();

    /**
     * List of glossary term ids.
     *
     * @var   array
     */
    private $_reqObj = NULL;


    /**
     * Construct.
     *
     * @param string $uri     URI to connect.
     * @param array  $options Options to pass to HTTPRequest obj.
     *
     */
    public function __construct($uri, array $options=array())
    {
        $this->_uri     = $uri;
        $this->_options = $options;

    }//end __construct()


    /**
     * Make XMLRPC call.
     *
     * @param string $methodName Name of the method to call.
     * @param array  $params     Parameters.
     *
     * @return string
     */
    public function makeCall($methodName, array $params=array())
    {
        $requestData = xmlrpc_encode_request($methodName, $params);
        if ($this->_reqObj === NULL) {
            $this->_reqObj = new HTTPRequest($this->_uri, $this->_options);
        }

        $this->_reqObj->setData($requestData);
        $this->_reqObj->sendRequest();
        $body = $this->_reqObj->getData();
        return xmlrpc_decode($body);

    }//end makeCall()


    /**
     * Returns HTTPRequest object.
     *
     * @return object
     */
    public function getHttpRequest()
    {
        return $this->_reqObj;

    }//end getHttpRequest()


}//end class

?>
