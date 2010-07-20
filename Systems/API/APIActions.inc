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
     * Send POST API request to the destinated system.
     *
     * It sends API request via POST to the destined system.
     *
     * @param string $url       URL of the system to send the request.
     * @param array  $msg       Array of messages to send.
     * @param string $userAgent Optional User Agent string to send.
     *
     * @since  4.0.0
     * @return array
     * @throws ChannelException Error occurred.
     */
    public static function sendCURLAPIRequest($url, array $msg, $userAgent='')
    {
        $requiredFields = array(
                           '_system',
                           '_action',
                           '_format',
                          );
        foreach ($requiredFields as $field) {
            if (isset($msg[$field]) === FALSE) {
                $errMsg = '';
                throw new ChannelException($errMsg);
            }
        }

        $url = rtrim($url, '/');
        $ch  = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url.'/'.self::$_serviceWebPath.'/');
        curl_setopt($ch, CURLOPT_POST, TRUE);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:'));

        if (empty($userAgent) === FALSE) {
            curl_setopt($ch, CURLOPT_USERAGENT, $userAgent);
        }

        $result   = curl_exec($ch);
        $curlinfo = curl_getinfo($ch);
        curl_close($ch);

        return $curlinfo;

    }//end sendCURLAPIRequest()


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

        // Token validation for every request.
        Channels::includeSystem('User');
        $userid = User::getCurrentUserid();
        if ($userid !== NULL) {
            Channels::includeSystem('Session');
            include_once 'Libs/Security/Security.inc';
            $apiToken    = Security::getRequestData('_api_token', NULL, TRUE);
            $returnToken = FALSE;
            if ($apiToken === NULL) {
                // Token is not submitted with the API call.
                // Just return the new token.
                $returnToken = TRUE;
            } else {
                $currToken = Session::getSessionData('API', 'user:'.$userid);
                if ($apiToken !== $currToken) {
                    // Token mis-matched. Let's return a new token.
                    $returnToken = TRUE;
                } else {
                    // The current token is good. Now generate the new one
                    // for the next request.
                    $newToken = API::getUniqueToken();
                    Session::setSessionData('API', 'user:'.$userid, $newToken);
                    $responseContent['next_token'] = $newToken;
                }
            }

            if ($returnToken === TRUE) {
                $newToken = API::getUniqueToken();
                Session::setSessionData('API', 'user:'.$userid, $newToken);

                self::_sendHeader(200);
                self::_sendResponse(array('new_token' => $newToken));
                exit;
            }
        }//end if

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

        $assetid            = API::getAssetid($requestMethod);
        $request['assetid'] = $assetid;

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
        $perm = API::checkPermission($assetid, $api['options']);
        if ($perm === FALSE) {
            // Permission denied.
            self::_sendHeader(401);
            $responseContent['exception'] = 'You do not have permission to perform the requested action';
            self::_sendResponse($responseContent);
            exit;
        }

        Channels::includeSystem($system);
        try {
            $responseContent['result'] = API::getActionResult($system, $action, $params);
            $response = 200;
        } catch (Exception $e) {
            $responseContent['exception'] = $e->getMessage();
            $response = 500;
        }

        self::_sendHeader($response);
        self::_sendResponse($responseContent, $outputFormat);

    }//end processRequest()


    /**
     * Check whether the caller can perform the requested action.
     *
     * @param float $assetid Assetid to check the permission.
     * @param array $options Request Method, GET or POST.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function checkPermission($assetid, array $options)
    {
        return TRUE;

    }//end checkPermission()


    /**
     * Get assetid for the requested action if applicable.
     *
     * @param string $requestMethod GET or POST request.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function getAssetid($requestMethod)
    {
        return NULL;

    }//end getAssetid()


    /**
     * Return the current URL without service web path.
     *
     * @param string $url URL to process.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getRealURL($url)
    {
        $serviceStartPos = strpos($url, '/'.self::$_serviceWebPath.'/');
        if ($serviceStartPos === FALSE) {
            return $url;
        }

        $currUrl = substr($url, 0, $serviceStartPos);
        return $currUrl;

    }//end getRealURL()


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
            $serviceEndPos = (strlen($url) - 1);
            if ($queryStrPos !== FALSE) {
                $currUrl      .= substr($url, $queryStrPos, (strlen($url) - $queryStrPos));
                $serviceEndPos = ($queryStrPos - 1);
            }

            // Now, find the system and action names for the requested service.
            // By default it checks '_serviceWebPath/SYSTEM_NAME/ACTION_NAME/' format.
            // Optionally, output format can be included prior to SYSTEM_NAME like following.
            // '_serviceWebPath/(xml|json)/SYSTEM_NAME/ACTION_NAME/'.
            $matches        = array();
            $serviceUrlPart = substr(
                $url,
                $serviceStartPos,
                (($serviceEndPos - $serviceStartPos) + 1)
            );

            $serviceUrlPattern           = '/\/'.self::$_serviceWebPath.'\/([^\/]+)\/([^\/]+)/';
            $serviceUrlPatternWithFormat = '/\/'.self::$_serviceWebPath.'\/(xml|json)\/([^\/]+)\/([^\/]+)/';
            $res = preg_match($serviceUrlPattern, $serviceUrlPart, $matches);

            if ($res === 1 && $matches[1] !== 'xml' && $matches[1] !== 'json') {
                $system = $matches[1];
                $action = $matches[2];
                $format = self::$_outputFormat;
            } else {
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

            $default    = NULL;
            $jsonDecode = FALSE;
            $system     = Security::getRequestData('_system', $default, $jsonDecode, $requestMethod);
            $action     = Security::getRequestData('_action', $default, $jsonDecode, $requestMethod);
            $format     = Security::getRequestData('_format', self::$_outputFormat, $jsonDecode, $requestMethod);
        }//end if

        $result['system']       = strtolower($system);
        $result['action']       = strtolower($action);
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

        Channels::includeSystem($system);
        $method     = new ReflectionMethod($system, $action);
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
        }//end foreach

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
     * @param array  $args   Parameters to pass to the action.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function getActionResult($system, $action, array $args)
    {
        $callback = array(
                     $system,
                     $action,
                    );
        $result   = call_user_func_array($callback, $args);

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

    }//end getActionResult()


    /**
     * Private wrapper function to send different types of response.
     *
     * @param array  $responseContent Array contains the requied data to
     *                                send reponse. It includes 'system', 'action',
     *                                'assetid', 'result' and optional 'exception'
     *                                in case for error case.
     * @param string $format          Response format type.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _sendResponse(array $responseContent, $format='json')
    {
        switch ($format) {
            case 'json':
                self::_sendJSONResponse($responseContent);
            break;

            case 'xml':
                self::_sendXMLResponse($responseContent);
            break;

            default:
                // No default behaviour.
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
    private static function _sendXMLResponse(array $responseContent)
    {
        header('Content-Type: text/xml; charset=utf-8');
        $dom     = new DomDocument('1.0', 'utf-8');
        $rspElem = $dom->createElement('rsp');
        $dom->appendChild($rspElem);

        // System and Action requested.
        $fields = array(
                   'system',
                   'action',
                   'assetid',
                   'new_token',
                   'next_token',
                   'exception',
                  );
        foreach ($fields as $field) {
            $fieldElem = $dom->createElement($field);
            self::_addTextToNode($fieldElem, $responseContent[$field]);
            $rspElem->appendChild($fieldElem);
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
    private static function _sendJSONResponse(array $responseContent)
    {
        header('Content-type: application/json');

        $attrs  = array();
        $fields = array(
                   'system',
                   'action',
                   'assetid',
                   'new_token',
                   'next_token',
                   'exception',
                  );
        foreach ($fields as $field) {
            if (isset($responseContent[$field]) === TRUE) {
                $attrs[] = $field.': '.json_encode($responseContent[$field]);
            }
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
     * @param object &$parentNode Parent DOM node to have the content tree.
     * @param mixed  $content     Content to create.
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

                default:
                    // No default behaviour.
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
     * @param object &$node Parent DOM node to have the content tree.
     * @param string $text  Content to create.
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
     * @return string
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
                        $arr[] = json_encode((string) $key).':'.API::buildResponseJSONString($val);
                    }

                    $result = '{'.implode(',', $arr).'}';
                break;

                default:
                    // No default behaviour.
                break;
            }//end switch
        }//end if

        return $result;

    }//end buildResponseJSONString()


    /**
     * Help function to send HTTP header.
     *
     * @param integer $response HTTP reponse code.
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
        $url = Web::getCurrentURL();
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
                $options[substr($option, 1, (strlen($option) - 1))] = $value;
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
     * @return array
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
    public static function addAPI($system, $action, $type, array $options)
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
    public static function updateAPI($system, $action, array $options)
    {
        if (is_array($options) === TRUE) {
            Channels::modifyBasket('options', serialize($options));
        }

        $query = DAL::getQuery('API', 'updateAPI');
        DAL::executeQuery($query);

    }//end updateAPI()


    /**
     * Get a new unique token for API call.
     *
     * @since  4.0.0
     * @return vaoid
     */
    public static function getUniqueToken()
    {
        $token = sha1(microtime(TRUE));
        return $token;

    }//end getUniqueToken()


}//end class
