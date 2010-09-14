<?php
/**
 * Actions for the API System.
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
 * @subpackage API
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * API Actions.
 *
 * This system provides an API to Squiz Suite products.
 */
class APIActions
{

    /**
     * Special web path to handle API request.
     *
     * @var string
     */
    private static $_serviceWebPath = '__api';

    /**
     * Output data format. Either XML or JSON.
     *
     * @var string
     */
    private static $_outputFormat = 'json';

    /**
     * HTTP response codes and messages.
     *
     * @var array
     */
    private static $_responseCode = array(
                                     200 => 'OK',
                                     304 => 'Not Modified',
                                     400 => 'Bad Request',
                                     401 => 'Unauthorized',
                                     403 => 'Forbidden',
                                     404 => 'Not Found',
                                     405 => 'Method Not Allowed',
                                     406 => 'Not Acceptable',
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
            if (array_key_exists('_enc', $msg) === TRUE) {
                if (isset($msg[$field]) === FALSE
                    && isset($msg['_enc'][$field]) === FALSE
                ) {
                    $errMsg = 'The required field '.$field.' is missing.';
                    throw new ChannelException($errMsg);
                }
            } else {
                if (isset($msg[$field]) === FALSE) {
                    $errMsg = 'The required field '.$field.' is missing.';
                    throw new ChannelException($errMsg);
                }
            }
        }

        $url = rtrim($url, '/');
        $ch  = curl_init();

        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, TRUE);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:'));

        if (empty($userAgent) === FALSE) {
            curl_setopt($ch, CURLOPT_USERAGENT, $userAgent);
        }

        $success = curl_exec($ch);
        if ($success === FALSE) {
            $errMsg = 'cURL failed: '.curl_error($ch);
            throw new ChannelException($errMsg);
        }

        // As CURLOPT_RETURNTRANSFER is set on, $success will have
        // the result of the request.
        $curlinfo = curl_getinfo($ch);
        $result   = array(
                     'result'   => $success,
                     'curlInfo' => $curlinfo,
                    );
        curl_close($ch);
        return $result;

    }//end sendCURLAPIRequest()


    /**
     * Central point to handle all API request.
     *
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
            // We must send the Allow header to be compliant with the requirements
            // of the 405 definition.
            API::sendHeader(405);
            header('Allow: GET, POST');
            $responseContent['exception'] = 'The request method is not supported';
            API::sendResponse($responseContent);
            exit;
        }

        // Get the requested service details from GET or POST request.
        $request = API::getRequestedAction($requestMethod);
        if ($request === FALSE) {
            API::sendHeader(400);
            $responseContent['exception'] = 'Failed to get system and action from the request';
            API::sendResponse($responseContent);
            exit;
        }

        self::$_outputFormat       = $request['outputFormat'];
        $responseContent['system'] = $request['system'];
        $responseContent['action'] = $request['action'];

        // Token validation for every request.
        Channels::includeSystem('User');
        $userid = User::getCurrentUserid();

        if ($userid !== NULL && $userid !== 0) {
            include_once 'Libs/Security/Security.inc';
            $apiToken = Security::getRequestData('_api_token', NULL, TRUE);
            if ($apiToken === NULL) {
                // Logged in user didn't submit the API token.
                API::sendHeader(403);
                $responseContent['exception'] = 'Invalid API token';
                API::sendResponse($responseContent);
                exit;
            }

            $userTokens = API::getUserToken();
            if ($userTokens === NULL) {
                // What is this token?
                API::sendHeader(403);
                $responseContent['exception'] = 'Invalid API token';
                API::sendResponse($responseContent);
                exit;
            } else if ($userTokens !== $apiToken) {
                API::sendHeader(403);
                $responseContent['exception'] = 'Invalid API token';
                API::sendResponse($responseContent);
                exit;
            }
        }//end if

        // Is the requested service available?
        $request['assetid'] = API::getAssetid($requestMethod);
        $api                = API::getAPI($request['system'], $request['action']);
        if ($request['assetid'] !== NULL) {
            // Add in the assetid as it was found.
            $responseContent['assetid'] = $request['assetid'];
        }

        if ($api === FALSE) {
            // Requested service is not available. Not Found.
            $responseContent['exception'] = 'The requested service was not found';
            API::sendHeader(404);
            API::sendResponse($responseContent);
            exit;
        }

        // Note that we set system and action name again here with
        // the values from API::getAPI() to set the correct case-sensitive names.
        $responseContent['system'] = $api['system'];
        $responseContent['action'] = $api['action'];
        if (isset($api['options']['contentType']) === TRUE) {
            $responseContent['contentType'] = $api['options']['contentType'];
        }

        // What parameters are required for the action?
        $params = API::getActionParameters(
            $requestMethod,
            $api['system'],
            $api['action'],
            $request['assetid']
        );

        if ($params === FALSE) {
            // Insufficient parameters are provided.
            API::sendHeader(400);
            $responseContent['exception'] = 'Provided parameters are not sufficient';
            API::sendResponse($responseContent);
            exit;
        }

        Channels::includeSystem($api['system']);
        try {
            ob_start();
                $returnValue = API::executeRequest($api['system'], $api['action'], $params, $api['options']);
                $output      = ob_get_contents();
            ob_end_clean();

            if ($returnValue === NULL) {
                $responseContent['result'] = $output;
            } else {
                $responseContent['result'] = $returnValue;
            }

            $response = 200;
        } catch (Exception $e) {
            $responseContent['exception'] = $e->getMessage();
            $apiToken = Security::getRequestData('_api_token', NULL, TRUE);
            if ($apiToken === NULL) {
                $response = 500;
            } else {
                // For JS send 200 otherwise callback function won't be executed.
                $response = 200;
            }
        }//end try

        API::sendHeader($response);
        API::sendResponse($responseContent);

    }//end processRequest()


    /**
     * It executes the requested action and returns the result.
     *
     * If DAL Query has been returned, then it converts it to
     * SQL string. If an exception is thrown, it's handled by the caller.
     *
     * @param string $system   System name where action resides.
     * @param string $action   Action name to call.
     * @param array  $args     Parameters to pass to the action.
     * @param array  $settings API settings defined on the method being called.
     *
     * @return mixed
     */
    public static function executeRequest($system, $action, array $args, array $settings)
    {
        $callback = array(
                     $system,
                     $action,
                    );
        $result   = call_user_func_array($callback, $args);

        // DAL Query has been returned! In general this should not happen as
        // we don't want the product's API to return DAL uueries. If this happens,
        // we will return the SQL string of the query instead.
        if ($result instanceof Query) {
            ob_start();
                echo $result;
                $result = ob_get_contents();
            ob_end_clean();
        }

        return $result;

    }//end executeRequest()



    /**
     * Get assetid for the requested action if applicable.
     *
     * @param string $requestMethod GET or POST request.
     *
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
     * @return array|boolean
     */
    public static function getRequestedAction($requestMethod)
    {
        include_once 'Libs/Web/Web.inc';
        $url = Web::getCurrentURL();

        // Confirm the URL includes the service suffix.
        $pattern = '(\/'.self::$_serviceWebPath.'$|\/'.self::$_serviceWebPath.'\/)';
        $result  = preg_match($pattern, $url, $matches, PREG_OFFSET_CAPTURE);
        if ($result === 0) {
            return FALSE;
        } else {
            $serviceStartPos = $matches[0][1];
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
            // By default it checks '_serviceWebPath/SYSTEM_NAME/ACTION_NAME/'
            // format. Optionally, output format can be included prior to
            // SYSTEM_NAME like following:
            // '_serviceWebPath/(xml|json)/SYSTEM_NAME/ACTION_NAME/'.
            $matches        = array();
            $serviceUrlPart = substr(
                $url,
                $serviceStartPos,
                (($serviceEndPos - $serviceStartPos) + 1)
            );

            $formatTypes = array(
                            'xml',
                            'json',
                            'raw',
                           );
            $serviceUrlPattern           = '/\/'.self::$_serviceWebPath.'\/([^\/]+)\/([^\/]+)/';
            $serviceUrlPatternWithFormat = '/\/'.self::$_serviceWebPath.'\/('.implode('|', $formatTypes).')\/([^\/]+)\/([^\/]+)/';
            $res = preg_match($serviceUrlPattern, $serviceUrlPart, $matches);

            if ($res === 1 && in_array($matches[1], $formatTypes) === FALSE) {
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
            $encrypted  = Security::getRequestData('_enc', $default, FALSE, 'post');
            $system     = Security::getRequestData('_system', $default, $jsonDecode, $requestMethod);
            $action     = Security::getRequestData('_action', $default, $jsonDecode, $requestMethod);
            $format     = Security::getRequestData('_format', self::$_outputFormat, $jsonDecode, $requestMethod);
            if ($system === $default && $encrypted !== NULL) {
                // When the message is encrypted, look for the variables in _enc field.
                include_once 'Libs/Util/Util.inc';
                $system = Util::getArrayIndex($encrypted, '_system', $default);
                $action = Util::getArrayIndex($encrypted, '_action', $default);
                $format = Util::getArrayIndex($encrypted, '_format', self::$_outputFormat);
            }
        }//end if

        $result['system']       = strtolower($system);
        $result['action']       = strtolower($action);
        $result['outputFormat'] = $format;
        return $result;

    }//end getRequestedAction()


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
     *                              a parameter called 'assetid' and it's not passed
     *                              in POST or GET, this value will be used
     *                              if specified.
     *
     * @return array|boolean
     */
    public static function getActionParameters($requestMethod, $system, $action, $assetid=NULL)
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
        $default    = NULL;
        $jsonDecode = TRUE;

        // Action parameters can come from '_enc' field if
        // system key ecryption is being used.
        $encrypted = Security::getRequestData('_enc', $default, FALSE, 'post');
        foreach ($requiredParams as $param) {
            $value = Security::getRequestData(
                $param['name'],
                $default,
                $jsonDecode,
                $requestMethod
            );

            // Special case for assetid parameter binding. If the assetid is not
            // specified, then use the one from URL.
            if ($value === NULL
                && $assetid !== NULL
                && $param['name'] === 'assetid'
            ) {
                $value = $assetid;
            }

            if ($value === NULL
                && $encrypted !== NULL
                && array_key_exists($param['name'], $encrypted) === TRUE
            ) {
                $value = json_decode($encrypted[$param['name']], TRUE);
            }

            if ($value === NULL && $param['optional'] === FALSE) {
                return FALSE;
            } else {
                $params[$param['name']] = $value;
            }
        }//end foreach

        return $params;

    }//end getActionParameters()


    /**
     * Private wrapper function to send different types of response.
     *
     * @param array  $responseContent Array contains the requied data to
     *                                send reponse. It includes 'system', 'action',
     *                                'assetid', 'result' and optional 'exception'
     *                                in case for error case.
     * @param string $format          Response format type.
     *
     * @return void
     */
    public static function sendResponse(array $responseContent, $format=NULL)
    {
        if ($format === NULL) {
            $format = self::$_outputFormat;
        }

        switch ($format) {
            case 'json':
                self::_sendJSONResponse($responseContent);
            break;

            case 'xml':
                self::_sendXMLResponse($responseContent);
            break;

            case 'raw':
                if (isset($responseContent['contentType']) === TRUE) {
                    header('Content-Type: '.$responseContent['contentType']);
                }

                echo $responseContent['result'];
            break;

            default:
                // No default behaviour.
            break;
        }//end switch

    }//end sendResponse()


    /**
     * Sends XML response data for the request.
     *
     * @param array $responseContent Array contains the requied data to
     *                               send reponse. It includes 'system', 'action',
     *                               'assetid', 'result' and optional 'exception'
     *                               in case for error case.
     *
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
            if (array_key_exists($field, $responseContent) === TRUE) {
                $fieldElem = $dom->createElement($field);
                self::_addTextToNode($fieldElem, $responseContent[$field]);
                $rspElem->appendChild($fieldElem);
            }
        }

        if (array_key_exists('result', $responseContent) === TRUE) {
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
     * @return void
     */
    private static function _sendJSONResponse(array $responseContent)
    {
        require_once 'Libs/Util/Util.inc';
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
            if (array_key_exists($field, $responseContent) === TRUE) {
                $attrs[] = '"'.$field.'": '.json_encode($responseContent[$field]);
            }
        }

        if (array_key_exists('result', $responseContent) === TRUE) {
            $attrs[] = '"result": '.Util::encodeJson($responseContent['result']);
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

            if (isset($content['_base64']) === TRUE) {
                $type = 'base64';
            }

            $parentNode->setAttribute('type', $type);
            $class = 'APIActions';
            $func  = '_build'.ucwords($type).'Response';
            if (method_exists($class, $func) === TRUE) {
                $callback = array(
                             $class,
                             $func,
                            );
                $params   = array(
                             $parentNode,
                             $content,
                            );
                $response = call_user_func_array($callback, $params);
            }
        }//end if

    }//end buildResponseXMLDom()


    /**
     * Build the array response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     * @param array  $content    Content to create.
     *
     * @return void
     */
    private static function _buildArrayResponse($parentNode, array $content)
    {
        foreach ($content as $val) {
            $valueElem = $parentNode->ownerDocument->createElement('value');
            $parentNode->appendChild($valueElem);
            API::buildResponseXMLDom($valueElem, $val);
        }

    }//end _buildArrayResponse()


    /**
     * Build the struct response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     * @param array  $content    Content to create.
     *
     * @return void
     */
    private static function _buildStructResponse($parentNode, array $content)
    {
        foreach ($content as $key => $val) {
            $keyElemTag = $key;
            if (is_numeric($key) === TRUE) {
                $keyElemTag = '_'.$key;
            }

            $keyElem = $parentNode->ownerDocument->createElement($keyElemTag);
            $parentNode->appendChild($keyElem);
            API::buildResponseXMLDom($keyElem, $val);
        }

    }//end _buildStructResponse()


    /**
     * Build the base64 response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     * @param array  $content    Content to create.
     *
     * @return void
     */
    private static function _buildBase64Response($parentNode, array $content)
    {
        foreach ($content as $key => $val) {
            if ($key === '_base64') {
                continue;
            }

            $parentNode->setAttribute($key, $val);
        }

        self::_addTextToNode($parentNode, $content['_base64'], TRUE);

    }//end _buildBase64Response()


    /**
     * Get the response type from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     *
     * @return mixed
     */
    public static function getResponseFromXMLDom($parentNode)
    {
        $response = '';
        if ($parentNode->hasAttribute('type') === TRUE) {
            $type  = $parentNode->getAttribute('type');
            $class = 'APIActions';
            $func  = '_get'.ucwords($type).'Response';
            if (method_exists($class, $func) === TRUE) {
                $callback = array(
                             $class,
                             $func,
                            );
                $params   = array($parentNode);
                $response = call_user_func_array($callback, $params);
            } else {
                $response = $parentNode->nodeValue;
                settype($response, $type);
            }
        }//end if

        return $response;

    }//end getResponseFromXMLDom()


    /**
     * Get the array response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     *
     * @return array
     */
    private static function _getArrayResponse($parentNode)
    {
        $response = array();
        if ($parentNode->hasChildNodes() === TRUE) {
            foreach ($parentNode->childNodes as $child) {
                if ($child->nodeType === XML_ELEMENT_NODE) {
                    $response[] = API::getResponseFromXMLDom($child);
                }
            }
        }

        return $response;

    }//end _getArrayResponse()


    /**
     * Get the struct response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     *
     * @return array
     */
    private static function _getStructResponse($parentNode)
    {
        $response = array();
        if ($parentNode->hasChildNodes() === TRUE) {
            foreach ($parentNode->childNodes as $child) {
                $key = $child->nodeName;
                if ($child->nodeType === XML_ELEMENT_NODE) {
                    $response[$key] = API::getResponseFromXMLDOM($child);
                }
            }
        }

        ksort($response);

        return $response;

    }//end _getStructResponse()


    /**
     * Get the base64 response from the XML DOM structure.
     *
     * @param object $parentNode Parent DOM node to have the content tree.
     *
     * @return array
     */
    private static function _getBase64Response($parentNode)
    {
        $response = array();
        if ($parentNode->hasAttributes() === TRUE) {
            foreach ($parentNode->attributes as $attribute) {
                $name  = $attribute->name;
                $value = $attribute->value;
                if ($name !== 'type') {
                    $response[$name] = $value;
                }
            }
        }

        // Finally, add in the base64 content.
        $response['_base64'] = $parentNode->nodeValue;
        ksort($response);

        return $response;

    }//end _getBase64Response()


    /**
     * Sets the string content to the DOM node.
     *
     * If the text contains invalid XML characters, CDATA section
     * is appended instead.
     *
     * @param object  &$node Parent DOM node to have the content tree.
     * @param string  $text  Content to create.
     * @param boolean $force Force to append a CDATA section.
     *
     * @return void
     */
    private static function _addTextToNode(&$node, $text, $force=FALSE)
    {
        if ($force === TRUE) {
            $node->appendChild($node->ownerDocument->createCDATASection($text));
        } else {
            $invalidXMLChars = preg_match('/[&<>"\']/', $text);
            if ($invalidXMLChars === 1) {
                $node->appendChild($node->ownerDocument->createCDATASection($text));
            } else {
                $node->appendChild($node->ownerDocument->createTextNode($text));
            }
        }

    }//end _addTextToNode()


    /**
     * Help function to send HTTP header.
     *
     * @param integer $response HTTP reponse code.
     *
     * @return void
     */
    public static function sendHeader($response)
    {
        $headerString = 'HTTP/1.0 '.$response.' '.self::$_responseCode[$response];
        $replace      = TRUE;
        header($headerString, $replace, $response);

    }//end sendHeader()


    /**
     * Returns TRUE if it is API request.
     *
     * @return boolean
     */
    public static function isAPIRequest()
    {
        include_once 'Libs/Web/Web.inc';
        $url     = Web::getCurrentURL();
        $pattern = '(\/'.self::$_serviceWebPath.'$|\/'.self::$_serviceWebPath.'\/)';
        $result  = preg_match($pattern, $url, $matches);
        if ($result === 1) {
            return TRUE;
        }

        return FALSE;

    }//end isAPIRequest()


    /**
     * Extract API options from function comment.
     *
     * @param string $docComment Function comment.
     *
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
                $optionType = substr($option, 1, (strlen($option) - 1));
                if (isset($options[$optionType]) === FALSE) {
                    $options[$optionType] = array();
                }

                $options[$optionType][] = $value;
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
     * @return string
     */
    public static function getUniqueToken()
    {
        $prefix      = '';
        $moreEntropy = TRUE;
        $token       = sha1(uniqid($prefix, $moreEntropy));
        return $token;

    }//end getUniqueToken()


    /**
     * Get user token.
     *
     * @return string
     */
    public static function getUserToken()
    {
        Channels::includeSystem('Session');
        Channels::includeSystem('User');

        $userid    = User::getCurrentUserid();
        $userToken = Session::getSessionData('API', 'userToken:'.$userid);
        return $userToken;

    }//end getUserToken()


    /**
     * Set user token to a new token.
     *
     * @param string $token The new token.
     *
     * @return void
     */
    public static function setUserToken($token)
    {
        Channels::includeSystem('Session');
        Channels::includeSystem('User');

        $userid    = User::getCurrentUserid();
        $userToken = Session::setSessionData('API', 'userToken:'.$userid, $token);

    }//end setUserToken()


}//end class

?>
