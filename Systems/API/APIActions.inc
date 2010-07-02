<?php
/**
 * Actions for the API System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Asset
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * API Actions.
 *
 * Actions for the Asset system. This system stores all standard assets.
 * It contains actions for deleting, creating, cloning and morphing local
 * assets.
 *
 * @since 4.0.0
 */
class APIActions
{


    /**
     * Special web path to handle API request.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_serviceWebPath = '__api';

    /**
     * Output data format. Either XML or JSON.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_outputFormat = 'json';

    /**
     * HTTP response codes and messages.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_responseCode = array(
                                     200 => 'OK',
                                     304 => 'Not Modified',
                                     400 => 'Bad Request',
                                     401 => 'Unauthorized',
                                     403 => 'Forbidden',
                                     404 => 'Not Found',
                                     406 => 'Not Acceptable',
                                     420 => 'Enhance Your Calm',
                                     500 => 'Internal Server Error',
                                     502 => 'Bad Gateway',
                                     503 => 'Service Unavailable',
                                    );


    /**
     * Central point to handle all API request.
     *
     * @since  4.0.0
     * @return void
     */
    public static function processRequest()
    {
        $responseContent           = array();
        $responseContent['system'] = '';
        $responseContent['action'] = '';

        // Only support GET and POST method.
        $requestMethod = strtolower($_SERVER['REQUEST_METHOD']);
        if ($requestMethod !== 'get' && $requestMethod !== 'post') {
            self::_sendHeader(503);
            $responseContent['exception'] = 'The request method is not supported.';
            self::_sendResponse($responseContent);
            exit;
        }

        // Get the requested service details from GET or POST request.
        $request = self::_getRequestedAction($requestMethod);
        if ($request === FALSE) {
            self::_sendHeader(400);
            $responseContent['exception'] = 'Failed to get system and action from the request';
            self::_sendResponse($responseContent);
            exit;
        }

        // Is the requested service available?
        $api = API::getAPI($request['system'], $request['action']);
        $responseContent['system']  = $request['system'];
        $responseContent['action']  = $request['action'];
        $responseContent['assetid'] = $request['assetid'];
        if ($api === FALSE) {
            // Requested service is not available. Not Found.
            $responseContent['exception'] = 'Requested service & is not found.';
            self::_sendHeader(404);
            self::_sendResponse($responseContent);
            exit;
        }

        // Note that we set system and action name again here with
        // the values from API::getAPI() to set the correct case-sensitive names.
        $responseContent['system'] = $api['system'];
        $responseContent['action'] = $api['action'];

        // What parameters are required for the action?
        $params = self::_getActionParameters(
            $requestMethod,
            $api['system'],
            $api['action'],
            $request['assetid']
        );
        if ($params === FALSE) {
            // Insufficient parameters are provided.
            self::_sendHeader(400);
            $responseContent['exception'] = 'Provided parameters are not sufficient';
            self::_sendResponse($responseContent);
            exit;
        }

        $system       = $api['system'];
        $action       = $api['action'];
        $assetid      = $request['assetid'];
        $outputFormat = $request['outputFormat'];

        // Permission and locking check should come here.
        Channels::includeSystem('Role');
        Channels::includeSystem('Authentication');
        $userid = Authentication::getCurrentUserId();
        $perm   = Role::checkPermission($assetid, $userid, $api['options']['privilege']);
        if ($perm === FALSE) {
            // Permission denied.
            self::_sendHeader(401);
            $responseContent['exception'] = 'You do not have permission to perform the requested action';
            self::_sendResponse($responseContent);
            exit;
        }

        Channels::includeSystem($system);
        try {
            $responseContent['result'] = self::_getActionResult($system, $action, $params);
            $response = 200;
        } catch (Exception $e) {
            $responseContent['exception'] = $e->getMessage();
            $response = 500;
        }

        self::_sendHeader($response);
        self::_sendResponse($responseContent, $outputFormat);

    }//end processRequest()


    /**
     * Get the details of requested action.
     *
     * The returned array will include system, action, assetid
     * and outputFormat for the request. It returns FALSE if
     * valid system and action names can not be found.
     *
     * @param string $requestMethod Request Method, GET or POST.
     *
     * @since  4.0.0
     * @return array|boolean
     */
    private static function _getRequestedAction($requestMethod)
    {
        include_once 'Libs/Web/Web.inc';
        $url = Web::getCurrentURL();

        // Confirm the URL includes the service suffix.
        $serviceStartPos = strpos($url, '/'.self::$_serviceWebPath.'/');
        if ($serviceStartPos === FALSE) {
            return FALSE;
        }

        $result  = array();
        $currUrl = substr($url, 0, $serviceStartPos);

        if ($requestMethod === 'get') {
            // Get the URL without service request part so that we can get
            // the assetid from Lookup system.
            $queryStrPos   = strpos($url, '?');
            $serviceEndPos = strlen($url) - 1;
            if ($queryStrPos !== FALSE) {
                $currUrl      .= substr($url, $queryStrPos, strlen($url) - $queryStrPos);
                $serviceEndPos = $queryStrPos - 1;
            }

            Channels::includeSystem('Lookup');
            $assetid = Lookup::getAssetFromUrl($currUrl);

            // Now, find the system and action names for the requested service.
            // By default it checks '_serviceWebPath/SYSTEM_NAME/ACTION_NAME/' format.
            // Optionally, output format can be included prior to SYSTEM_NAME like following.
            // '_serviceWebPath/(xml|json)/SYSTEM_NAME/ACTION_NAME/'.
            $matches        = array();
            $serviceUrlPart = substr($url, $serviceStartPos, ($serviceEndPos - $serviceStartPos) + 1);

            $serviceUrlPattern           = '/\/'.self::$_serviceWebPath.'\/([^\/(xml)(json)]+)\/([^\/]+)/';
            $serviceUrlPatternWithFormat = '/\/'.self::$_serviceWebPath.'\/(xml|json)\/([^\/]+)\/([^\/]+)/';

            $res = preg_match($serviceUrlPattern, $serviceUrlPart, $matches);
            if ($res === 1) {
                $system = $matches[1];
                $action = $matches[2];
                $format = self::$_outputFormat;
            } else  {
                $res = preg_match($serviceUrlPatternWithFormat, $serviceUrlPart, $matches);
                if ($res === 1) {
                    $format = $matches[1];
                    $system = $matches[2];
                    $action = $matches[3];
                } else {
                    return FALSE;
                }
            }
        } else if ($requestMethod === 'post') {
            // POST request is much simpler.
            include_once 'Libs/Security/Security.inc';
            Channels::includeSystem('Lookup');
            $assetid = Lookup::getAssetFromUrl($currUrl);

            $default    = NULL;
            $jsonDecode = FALSE;
            $system     = Security::getReuqestData('_system', $default, $jsonDecode, $requestMethod);
            $action     = Security::getReuqestData('_action', $default, $jsonDecode, $requestMethod);
            $format     = Security::getReuqestData('_format', self::$_outputFormat, $jsonDecode, $requestMethod);
            $aid        = Security::getReuqestData('_assetid', $default, $jsonDecode, $requestMethod);
            if ($aid !== NULL) {
                $assetid = $aid;
            }
        }

        $result['system']       = strtolower($system);
        $result['action']       = strtolower($action);
        $result['assetid']      = $assetid;
        $result['outputFormat'] = $format;
        return $result;

    }//end _getRequestedAction()


    /**
     * The the list of parameters needed to make Channel call.
     *
     * It first check the actual function parameters using Reflection.
     * Then, it tries to get the requied values to from submitted data.
     * If all the required parameters can not be found, then it will
     * return FALSE.
     *
     * Note that the submited data will be JSON decoded.
     *
     * @param string $requestMethod Request Method being used. POST or GET.
     * @param string $system        System name where action resides.
     * @param string $action        Action name to call.
     * @param float  $assetid       Optional assetid to use. If the action requires
     *                              a parameter called 'assetid' and it's not passed in
     *                              POST or GET, this value will be used if specified.
     *
     * @since  4.0.0
     * @return array|boolean
     */
    private static function _getActionParameters($requestMethod, $system, $action, $assetid=NULL)
    {
        $requiredParams = array();

        Channels::includeSystem('Attribute');
        $method     = new ReflectionMethod('Attribute', $action);
        $parameters = $method->getParameters();
        foreach ($parameters as $parameter) {
            $requiredParams[] = array(
                                 'name'     => $parameter->getName(),
                                 'optional' => $parameter->isOptional(),
                                );
        }

        $params          = array();
        $parametersExist = TRUE;
        include_once 'Libs/Security/Security.inc';
        foreach ($requiredParams as $param) {
            $default    = NULL;
            $jsonDecode = TRUE;
            $value      = Security::getRequestData(
                $param['name'],
                $default,
                $jsonDecode,
                $requestMethod
            );

            // Special case for assetid parameter binding. If the assetid is not
            // specified, then use the one from URL.
            if ($value === NULL && $assetid !== NULL && $param['name'] === 'assetid') {
                $value = $assetid;
            }

            if ($value === NULL && $param['optional'] === FALSE) {
                return FALSE;
            } else {
                $params[] = json_decode($value);
            }
        }

        return $params;

    }//end _getActionParameters()


    /**
     * It executes the requested action and returns the result.
     *
     * If DAL Query has been returned, then it converts it to
     * SQL string. If an exception is thrown, it's handled on the caller.
     *
     * @param string $system System name where action resides.
     * @param string $action Action name to call.
     * @param array  $params Parameters to pass to the action.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _getActionResult($system, $action, $params)
    {
        $callback = array(
                     $system,
                     $action,
                    );
        $result   = call_user_func_array($callback, $params);

        // DAL Query has been returned! In general this should not happen as
        // we don't want the product's API return DAL Query. If this happens,
        // we will return SQL string of the query instead.
        if ($result instanceof Query) {
            ob_start();
                echo $result;
                $result = ob_get_contents();
            ob_end_clean();
        }

        return $result;

    }//end _getActionResult()


    /**
     * Private wrapper function to send different types of response.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _sendResponse($responseContent, $format='json')
    {
        switch ($format) {
            case 'json':
                self::_sendJSONResponse($responseContent);
            break;

            case 'xml':
                self::_sendXMLResponse($responseContent);
            break;
        }

    }//end _sendResponse()


    /**
     * Sends XML response data for the request.
     *
     * @param array $responseContent Array contains the requied data to 
     *                               send reponse. It includes 'system', 'action',
     *                               'assetid', 'result' and optional 'exception'
     *                               in case for error case.
     *
     * @since  4.0.0
     * @return void
     */
    public static function _sendXMLResponse($responseContent)
    {
        header('Content-Type: text/xml; charset=utf-8');
        $dom     = new DomDocument('1.0', 'utf-8');
        $rspElem = $dom->createElement('rsp');
        $dom->appendChild($rspElem);

        // System and Action requested.
        $systemElem  = $dom->createElement('system');
        $actionElem  = $dom->createElement('action');
        $assetidElem = $dom->createElement('assetid');
        self::_addTextToNode($systemElem, $responseContent['system']);
        self::_addTextToNode($actionElem, $responseContent['action']);
        self::_addTextToNode($assetidElem, $responseContent['assetid'].'');
        $rspElem->appendChild($systemElem);
        $rspElem->appendChild($actionElem);
        $rspElem->appendChild($assetidElem);

        // Any exception?
        if (isset($responseContent['exception']) === TRUE) {
            $excepElem = $dom->createElement('exception');
            $rspElem->appendChild($excepElem);
            self::_addTextToNode($excepElem, $responseContent['exception']);
        }

        if (isset($responseContent['result']) === TRUE) {
            $resultElem = $dom->createElement('result');
            API::buildResponseXMLDom($resultElem, $responseContent['result']);
            $rspElem->appendChild($resultElem);
        }

        $xml = $dom->saveXML();
        echo $xml;

    }//end _sendXMLResponse()


    /**
     * Sends JSON response data for the request.
     *
     * Optionally if '_callback' function is specified in the request,
     * the result JSON string will be a function call to callback function.
     *
     * @param array $responseContent Array contains the requied data to 
     *                               send reponse. It includes 'system', 'action',
     *                               'assetid', 'result' and optional 'exception'
     *                               in case for error case.
     *
     * @since  4.0.0
     * @return void
     */
    public static function _sendJSONResponse($responseContent)
    {
        header('Content-type: application/json');

        $attrs   = array();
        $attrs[] = 'system: '.json_encode($responseContent['system']);
        $attrs[] = 'action: '.json_encode($responseContent['action']);
        $attrs[] = 'assetid: '.json_encode($responseContent['assetid']);

        // Any exception?
        if (isset($responseContent['exception']) === TRUE) {
            $attrs[] = 'exception: '.json_encode($responseContent['exception']);
        }

        if (isset($responseContent['result']) === TRUE) {
            $attrs[] = 'result: '.API::buildResponseJSONString($responseContent['result']);
        }

        $jsonString = '{'.implode(','."\n", $attrs).'}';

        include_once 'Libs/Security/Security.inc';
        $callback = Security::getRequestData('_callback', NULL);
        if ($callback !== NULL) {
            $jsonString = $callback.'('.$jsonString.');';
        }

        echo $jsonString;

    }//end _sendJSONResponse()


    /**
     * Build XML DOM structure for the reponse type.
     *
     * The passed content will be converted to DOM tree structure
     * and appended to the parent node.
     *
     * @param DomNode $parentNode Parent DOM node to have the content tree.
     * @param mixed   $content    Content to create.
     *
     * @since  4.0.0
     * @return void
     */
    public static function buildResponseXMLDom(&$parentNode, $content)
    {
        $type = '';
        if (is_scalar($content) === TRUE) {
            if (is_integer($content) === TRUE) {
                $type    = 'integer';
                $textVal = $content.'';
            } else if (is_bool($content) === TRUE) {
                $type    = 'boolean';
                $textVal = json_encode($content);
            } else if (is_string($content) === TRUE) {
                $type    = 'string';
                $textVal = $content;
            } else if (is_float($content) === TRUE) {
                $type    = 'float';
                $textVal = $content.'';
            }

            $parentNode->setAttribute('type', $type);
            self::_addTextToNode($parentNode, $textVal);
        } else if (is_array($content) === TRUE) {
            $type = 'array';

            include_once 'Libs/Util/Util.inc';
            if (Util::isAssoc($content) === TRUE) {
                $type = 'struct';
            }

            $parentNode->setAttribute('type', $type);
            switch ($type) {
                case 'array':
                    foreach ($content as $val) {
                        $valueElem = $parentNode->ownerDocument->createElement('value');
                        $parentNode->appendChild($valueElem);
                        API::buildResponseXMLDom($valueElem, $val);
                    }
                break;

                case 'struct':
                    foreach ($content as $key => $val) {
                        $keyElemTag = $key;
                        if (is_numeric($key) === TRUE) {
                            $keyElemTag = '_'.$key;
                        }

                        $keyElem = $parentNode->ownerDocument->createElement($keyElemTag);
                        $parentNode->appendChild($keyElem);
                        API::buildResponseXMLDom($keyElem, $val);
                    }
                break;
            }//end switch
        }//end if

    }//end buildResponseXMLDom()


    /**
     * Sets the string content to the DOM node.
     *
     * If the text contains invalid XML characters, CDATA section
     * is appended instead.
     *
     * @param DomNode $node Parent DOM node to have the content tree.
     * @param string  $text Content to create.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _addTextToNode(&$node, $text)
    {
        $invalidXMLChars = preg_match('/[&<>"\']/', $text);
        if ($invalidXMLChars === 1) {
            $node->appendChild($node->ownerDocument->createCDATASection($text));
        } else {
            $node->appendChild($node->ownerDocument->createTextNode($text));
        }

    }//end _addTextToNode()


    /**
     * Build JSON string for the reponse type.
     *
     * The passed content will be converted to JSON string.
     *
     * @param mixed $content Content to create.
     *
     * @since  4.0.0
     * @return void
     */
    public static function buildResponseJSONString($content)
    {
        $type   = '';
        $result = '';
        if (is_scalar($content) === TRUE) {
            $result = json_encode($content);
        } else if (is_array($content) === TRUE) {
            $type = 'array';

            include_once 'Libs/Util/Util.inc';
            if (Util::isAssoc($content) === TRUE) {
                $type = 'struct';
            }

            $arr = array();
            switch ($type) {
                case 'array':
                    foreach ($content as $val) {
                        $arr[] = API::buildResponseJSONString($val);
                    }

                    $result = '['.implode(',', $arr).']';
                break;

                case 'struct':
                    foreach ($content as $key => $val) {
                        $arr[] = $key.':'.API::buildResponseJSONString($val);
                    }

                    $result = '{'.implode(',', $arr).'}';
                break;
            }//end switch
        }//end if

        return $result;

    }//end buildResponseJSONString()


    /**
     * Help function to send HTTP header.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _sendHeader($response)
    {
        $headerString = 'HTTP/1.0 '.$response.' '.self::$_responseCode[$response];
        $replace      = TRUE;
        header($headerString, $replace, $response);

    }//end _sendHeader()


    /**
     * Returns TRUE if it is API request.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isAPIRequest()
    {
        include_once 'Libs/Web/Web.inc';
        $url = Web::getCurrentURL(FALSE);
        if (strpos($url, '/'.self::$_serviceWebPath.'/') !== FALSE) {
            return TRUE;
        }

        return FALSE;

    }//end isAPIRequest()


    /**
     * Extract API options from function comment.
     *
     * @param string $docComment Function comment.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAPIOptionsFromComment($docComment)
    {
        $apiStartPos = strpos($docComment, '@api');
        if ($apiStartPos === FALSE) {
            return FALSE;
        }

        $pattern = '/(@api)(-[^ ]+)?\s+([^\n]+)/';
        $part    = substr($docComment, $apiStartPos, (strlen($docComment) - $apiStartPos));
        $matches = array();
        preg_match_all($pattern, $part, $matches);

        if (empty($matches) === TRUE) {
            return FALSE;
        }

        $options = array();
        $count   = count($matches[0]);
        for ($i = 0; $i < $count; $i++) {
            $tag    = $matches[1][$i];
            $option = $matches[2][$i];
            $value  = $matches[3][$i];

            if ($option === '' && ($value === 'read' || $value === 'write')) {
                $options['type'] = $value;
            } else if ($option[0] === '-' && empty($value) === FALSE) {
                $options[substr($option, 1, strlen($option) - 1)] = $value;
            }
        }

        return $options;

    }//end getAPIOptionsFromComment()


    /**
     * Returns the registered API information.
     *
     * If the requested API does not exist in DB,
     * it returns FALSE.
     *
     * @param string $system Name of the system to request.
     * @param string $action Name of the action to request.
     *
     * @since  4.0.0
     * @return void
     */
    public static function getAPI($system, $action)
    {
        $query  = DAL::getDALQuery('API', 'getAPI', 'getAssoc');
        $result = DAL::getAssoc($query);

        if (empty($result) === TRUE) {
            return FALSE;
        }

        $api = $result[0];
        $api['options'] = unserialize($api['options']);
        return $api;

    }//end getAPI()


    /**
     * Register API to the system.
     *
     * @param string $system  Name of the system for action.
     * @param string $action  Name of the action.
     * @param string $type    Type of API call. Either 'read' or 'write'.
     * @param array  $options Options for API call.
     *
     * @since  4.0.0
     * @return void
     */
    public static function addAPI($system, $action, $type, $options)
    {
        if (is_array($options) === TRUE) {
            Channels::modifyBasket('options', serialize($options));
        }

        $query = DAL::getQuery('API', 'addAPI');
        DAL::executeQuery($query);

    }//end addAPI()


    /**
     * Remove API from the system.
     *
     * @param string $system Name of the system for action.
     * @param string $action Name of the action.
     *
     * @since  4.0.0
     * @return void
     */
    public static function removeAPI($system, $action)
    {
        $query = DAL::getQuery('API', 'removeAPI');
        DAL::executeQuery($query);

    }//end removeAPI()


    /**
     * Update API options.
     *
     * @param string $system  Name of the system for action.
     * @param string $action  Name of the action.
     * @param array  $options Options to upadte.
     *
     * @since  4.0.0
     * @return void
     */
    public static function updateAPI($system, $action, $options)
    {
        if (is_array($options) === TRUE) {
            Channels::modifyBasket('options', serialize($options));
        }

        $query = DAL::getQuery('API', 'updateAPI');
        DAL::executeQuery($query);

    }//end updateAPI()


}//end class
