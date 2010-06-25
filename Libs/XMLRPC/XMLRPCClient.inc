<?php
/**
 * XMLRPC client.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Web
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Web/HTTPRequest.inc';

/**
 * XMLRPC client.
 *
 * @since 4.0.0
 */
class XMLRPCClient
{

    /**
     * List of glossary term ids.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_uri = '';

    /**
     * List of glossary term ids.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_options = array();

    /**
     * List of glossary term ids.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_reqObj = NULL;


    /**
     * Construct.
     *
     * @param string $uri     URI to connect.
     * @param array  $options Options to pass to HTTPRequest obj.
     *
     * @since 4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
     * @return object
     */
    public function getHttpRequest()
    {
        return $this->_reqObj;

    }//end getHttpRequest()


}//end class

?>
