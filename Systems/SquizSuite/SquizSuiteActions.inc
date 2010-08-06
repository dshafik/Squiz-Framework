<?php
/**
 * Actions for the SquizSuite System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * SquizSuite Actions Class.
 *
 * @since 4.0.0
 */
class SquizSuiteActions
{

    /**
     * User Agent String prefix.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_userAgentPrefix = 'SquizSuite-';

    /**
     * Encryption algorithm.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_opensslEncryptionAlgorithm = 'blowfish';

    /**
     * Local cache variable for product information.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_productCache = array();


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Get the list of all suite products.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getAllProducts()
    {
        $query  = DAL::getDALQuery('SquizSuite', 'getAllProducts');
        $result = DAL::getAssoc($query);
        return $result;

    }//end getAllProducts()


    /**
     * Get the product information for the given systemid.
     *
     * If NULL is given for the systemid, then the current product
     * will be returned.
     *
     * @param mixed $systemid Unique ID of the product.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getProduct($systemid=NULL)
    {
        if (isset(self::$_productCache[$systemid]) === TRUE) {
            return self::$_productCache[$systemid];
        }

        $query  = DAL::getDALQuery('SquizSuite', 'getProduct');
        $result = DAL::getAssoc($query);
        if (empty($result) === TRUE) {
            return $result;
        }

        $product               = array_pop($result);
        $product['connection'] = unserialize($product['connection']);

        // Cache the result.
        self::$_productCache[$systemid] = $product;
        return $product;

    }//end getProduct()


    /**
     * Get the product list of the given type.
     *
     * @param string $type Type of product to get.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getProductsByType($type)
    {
        $query  = DAL::getDALQuery('SquizSuite', 'getProductsByType');
        $result = DAL::getAssoc($query);
        return $result;

    }//end getProductsByType()


    /**
     * Update the product information for the given systemid.
     *
     * @param string $systemid  Unique ID of the product.
     * @param string $attrName  Attribute name to update.
     * @param mixed  $attrValue Attribute value to update.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function updateProduct($systemid, $attrName, $attrValue)
    {
        if ($attrName === 'connection' && is_array($attrValue) === TRUE) {
            $attrValue = serialize($attrValue);
            Channels::modifyBasket('attrValue', $attrValue);
        }

        $result = DAL::executeQuery('SquizSuite', 'updateProduct');
        if (isset(self::$_productCache[$systemid]) === TRUE) {
            unset(self::$_productCache[$systemid]);
        }

        return $result;

    }//end updateProduct()


    /**
     * Register a new product.
     *
     * @param string $systemid   The name of the product.
     * @param string $type       The type of the product to invite.
     * @param string $connection The connection details of the product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function registerProduct($systemid, $type, $connection)
    {
        if (is_array($connection) === TRUE) {
            Channels::modifyBasket('connection', serialize($connection));
        }

        DAL::executeQuery('SquizSuite', 'registerProduct');

        return $systemid;

    }//end registerProduct()


    /**
     * Remove the product from the suite.
     *
     * @param string $systemid Unique ID of the product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function removeProduct($systemid)
    {
        $result = DAL::executeQuery('SquizSuite', 'removeProduct');
        return $result;

    }//end removeProduct()


    /**
     * Generates private/public RSA keypairs for the current product.
     *
     * It also creates a self signed certificate and stores it in DB.
     * This certificate will be sent to other product as a part of sign-in
     * process.
     *
     * @since  4.0.0
     * @return void
     */
    public static function generateSystemKeyPair()
    {
        // Get the current product information.
        $currProduct = SquizSuite::getProduct();

        // Generate public/private key pairs.
        $keyInfo = array(
                    'private_key_bits' => 2048,
                    'private_key_type' => OPENSSL_KEYTYPE_RSA,
                   );

        $keyGenerated = openssl_pkey_new($keyInfo);

        // Keys will be stored in SquizSuite data directory.
        $suiteDir = BaseSystem::getDataDir('SquizSuite');
        openssl_pkey_export_to_file($keyGenerated, $suiteDir.'/privatekey.pem');

        $pubKey = openssl_pkey_get_details($keyGenerated);

        $info = array(
                 'countryName'            => 'AU',
                 'stateOrProvinceName'    => 'NSW',
                 'localityName'           => 'Sydney',
                 'organizationName'       => 'Squiz Suite',
                 'organizationalUnitName' => $currProduct['type'],
                 'commonName'             => $currProduct['systemid'],
                 'emailAddress'           => $currProduct['systemid'].'@squizsuite.com',
                );
        $csr  = openssl_csr_new($info, $keyGenerated);
        openssl_csr_export_to_file($csr, $suiteDir.'/system.csr');

        $cert = openssl_csr_sign($csr, NULL, $keyGenerated, (365 * 10));
        openssl_x509_export_to_file($cert, $suiteDir.'/system.crt');
        openssl_x509_export($cert, $certStr);

        file_put_contents($suiteDir.'/publickey', $pubKey['key']);

        // Also update the current product connection information with public key in it.
        $currProduct['connection']['cert'] = $certStr;
        SquizSuite::updateProduct($currProduct['systemid'], 'connection', $currProduct['connection']);

    }//end generateSystemKeyPair()


    /**
     * Send suite specific message to other system.
     *
     * @param string $systemid ID of the destined system.
     * @param string $msgType  Type of message to send.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException Error occurred.
     */
    public static function sendMessage($systemid, $msgType)
    {
        $productInfo = SquizSuite::getProduct($systemid);
        if (empty($productInfo) === TRUE) {
            $errMsg = $systemid.' is not known. Cannot send '.$msgType.' to it.';
            throw new ChannelException($errMsg);
        }

        $dom = self::_createSuiteMessageDOMTemplate();
        self::_createSuiteMessageDOM($dom, $msgType);
        $xml = $dom->saveXML();

        $msg            = array();
        $msg['_format'] = 'xml';
        $msg['_system'] = 'SquizSuite';
        $msg['_action'] = $msgType;

        // Do not encrypt suiteConnect message. In fact, it can not encrypt as
        // the current system does not know the public key of the destined system.
        if ($msgType === 'suiteConnect') {
            $msg['xml']         = $xml;
            $msg['con_request'] = 1;
        } else {
            $msg['_enc']            = $xml;
            $msg['_pubKeySystemid'] = $systemid;
        }

        $currProductInfo = SquizSuite::getProduct();
        $userAgentStr    = self::$_userAgentPrefix.$currProductInfo['type'].'/4.0';
        Channels::includeSystem('API');
        self::_logSentMessage($systemid, $msgType);
        $result = API::sendCURLAPIRequest(
            $productInfo['connection']['url'],
            $msg,
            $userAgentStr
        );

        if ($result['curlInfo']['http_code'] !== 200) {
            $errMsg  = 'HTTP Response '.$result['curlInfo']['http_code']."\n";
            $errMsg .= var_export($result, 1)."\n";
            self::_logErrorMessage($errMsg);
        }

    }//end sendMessage()


    /**
     * Return the template message XML in DOM document format.
     *
     * Every suite messages are based on this template. It includes
     * systemid, system type and the URL of the system.
     *
     * @since  4.0.0
     * @return object
     */
    private static function _createSuiteMessageDOMTemplate()
    {
        Channels::includeSystem('API');
        $dom     = new DomDocument('1.0', 'utf-8');
        $msgElem = $dom->createElement('message');
        $dom->appendChild($msgElem);

        // Get the product information about itself.
        $productInfo = SquizSuite::getProduct();

        // System and Action requested.
        $sysidEl   = $dom->createElement('system_id');
        $systypeEl = $dom->createElement('system_type');
        $sysurlEl  = $dom->createElement('system_url');

        API::buildResponseXMLDom($sysidEl, $productInfo['systemid']);
        API::buildResponseXMLDom($systypeEl, $productInfo['type']);
        API::buildResponseXMLDom($sysurlEl, $productInfo['connection']['url']);
        $msgElem->appendChild($sysidEl);
        $msgElem->appendChild($systypeEl);
        $msgElem->appendChild($sysurlEl);

        return $dom;

    }//end _createSuiteMessageDOMTemplate()


    /**
     * Add message specific element to the passed DOM object.
     *
     * @param object &$dom    DOM object to work with.
     * @param string $msgType Type of the message to send.
     *
     * @since  4.0.0
     * @return object
     */
    private static function _createSuiteMessageDOM(&$dom, $msgType)
    {
        Channels::includeSystem('API');
        $msgEl     = $dom->getElementsByTagName('message')->item(0);
        $msgTypeEl = $dom->createElement('message_type');

        // Append message_type element.
        API::buildResponseXMLDom($msgTypeEl, $msgType);
        $msgEl->appendChild($msgTypeEl);

        switch ($msgType) {
            case 'suiteConnect':
            case 'suiteConnectAck':
            case 'suiteConnectAckAck':
                // Both of CON_REQUEST and CON_REQUEST_ACK will send
                // the system's public key together.
                $currProduct = SquizSuite::getProduct();
                $pubKey      = $currProduct['connection']['cert'];
                $pubkeyEl    = $dom->createElement('cert');
                API::buildResponseXMLDom($pubkeyEl, $pubKey);
                $msgEl->appendChild($pubkeyEl);
            break;

            default:
                // No default behaviour.
            break;
        }//end switch

        return $dom;

    }//end _createSuiteMessageDOM()


    /**
     * Suite system connection request handler.
     *
     * Other suite system will make a request to connect by sharing
     * the security certificate that includes public key information.
     * The submitter's key certificate will be registered together and
     * it will send suiteConnectAck message back as a response.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException Error occurred.
     * @api    read
     */
    public static function suiteConnect()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            $systemType = self::_getSystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
            if ($systemType === FALSE) {
                $errMsg = 'User-agent string can not be parsed.';
                throw new ChannelException($errMsg);
            }

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            include_once 'Libs/Security/Security.inc';
            $xml     = Security::getRequestData('xml', NULL, FALSE, 'post');
            $xml     = html_entity_decode($xml);
            $message = self::_parseSuiteXML($xml);

            // All the required fields exist?
            $required = array(
                         'system_id',
                         'system_url',
                         'cert',
                        );
            foreach ($required as $field) {
                if ($message[$field] === NULL) {
                    $errMsg = $field.' has not been specified.';
                    throw new ChannelException($errMsg);
                }
            }

            // TODO: Maximum daily request exceeded?
            $systemid    = $message['system_id'];
            $systemUrl   = $message['system_url'];
            $certificate = $message['cert'];

            self::_logReceivedMessage($systemid, 'suiteConnect');

            DAL::beginTransaction();

            // The systemid already exists?
            $product = SquizSuite::getProduct($systemid);
            if (empty($product) === FALSE) {
                // Is the status Live already?
                if ($product['status'] === 'live') {
                    $errMsg = 'The requested system is already registered.';
                    throw new ChannelException($errMsg);
                }

                // The systemid exists and it's not live yet. But make sure
                // it doesn't have URL or certificate yet before we accept the
                // connection request. Or not, anyone can re-write the existing
                // system's URL or certificate with their own!
                if (array_key_exists('url', $product['connection']) === FALSE
                    && array_key_exists('cert', $product['connection']) === FALSE
                ) {
                    $product['connection']['url']  = $systemUrl;
                    $product['connection']['cert'] = $certificate;
                    SquizSuite::updateProduct($systemid, 'connection', $product['connection']);
                } else {
                    $errMsg = 'The requested system can not be accepted.';
                    throw new ChannelException($errMsg);
                }
            } else {
                $connection         = array();
                $connection['url']  = $systemUrl;
                $connection['cert'] = $certificate;
                SquizSuite::registerProduct($systemid, $systemType, $connection);
            }

            // Now we are all good. Register the system and set the status
            // as pending. Then send acknowlegement message back.
            SquizSuite::updateProduct($systemid, 'status', 'pending');
            DAL::commit();

            $success = SquizSuite::sendMessage($systemid, 'suiteConnectAck');
        } catch (Exception $e) {
            $errMsg  = 'suiteConnect failed'."\n";
            $errMsg .= $e->getMessage()."\n";
            self::_logErrorMessage($errMsg);
            DAL::rollback();

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw $e;
        }//end try

    }//end suiteConnect()


    /**
     * Suite system connection acknowlegement request handler.
     *
     * Once the initial connect message has been accepted by the
     * other system and the returning acknowlegement will be handled
     * here. The incoming message is encrypted with the current system's
     * public key.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException Error occurred.
     * @api    read
     */
    public static function suiteConnectAck()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            $systemType = self::_getSystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
            if ($systemType === FALSE) {
                $errMsg = 'User-agent string can not be parsed.';
                throw new ChannelException($errMsg);
            }

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            include_once 'Libs/Security/Security.inc';
            $xml     = Security::getRequestData('_enc', NULL, FALSE, 'post');
            $xml     = html_entity_decode($xml);
            $message = self::_parseSuiteXML($xml);

            // All the required fields exist?
            $required = array(
                         'system_id',
                         'system_url',
                         'cert',
                        );
            foreach ($required as $field) {
                if ($message[$field] === NULL) {
                    $errMsg = $field.' has not been specified.';
                    throw new ChannelException($errMsg);
                }
            }

            $systemid    = $message['system_id'];
            $systemUrl   = $message['system_url'];
            $certificate = $message['cert'];

            $productInfo = SquizSuite::getProduct($systemid);
            if (empty($productInfo) === TRUE) {
                $errMsg = 'Unknown system has been requested.';
                throw new ChannelException($errMsg);
            }

            self::_logReceivedMessage($systemid, 'suiteConnectAck');

            DAL::beginTransaction();
            // Connection Request accepted by the other system.
            // Set the system live and send the final acknowledgement.
            $productInfo['connection']['cert'] = $certificate;
            SquizSuite::updateProduct($systemid, 'connection', $productInfo['connection']);
            SquizSuite::updateProduct($systemid, 'status', 'live');
            DAL::commit();

            $success = SquizSuite::sendMessage($systemid, 'suiteConnectAckAck');
        } catch (Exception $e) {
            $errMsg  = 'suiteConnectAck failed.'."\n";
            $errMsg .= $e->getMessage()."\n";
            self::_logErrorMessage($errMsg);
            DAL::rollback();

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw $e;
        }//end try

    }//end suiteConnectAck()


    /**
     * Suite system connection acknowlegement acknowlegement request handler.
     *
     * The final acknowlegement for suite connection request.
     *
     * @since  4.0.0
     * @return void
     * @throws ChannelException Error occurred.
     * @api    read
     */
    public static function suiteConnectAckAck()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            $systemType = self::_getSystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
            if ($systemType === FALSE) {
                $errMsg = 'User-agent string can not be parsed.';
                throw new ChannelException($errMsg);
            }

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            include_once 'Libs/Security/Security.inc';
            $xml     = Security::getRequestData('_enc', NULL, FALSE, 'post');
            $xml     = html_entity_decode($xml);
            $message = self::_parseSuiteXML($xml);

            // All the required fields exist?
            $required = array(
                         'system_id',
                         'system_url',
                        );
            foreach ($required as $field) {
                if ($message[$field] === NULL) {
                    $errMsg = $field.' has not been specified.';
                    throw new ChannelException($errMsg);
                }
            }

            $systemid  = $message['system_id'];
            $systemUrl = $message['system_url'];

            // The final acknowledgment has been arrived.
            // Now two systems are connected.
            $productInfo = SquizSuite::getProduct($systemid);
            if (empty($productInfo) === TRUE) {
                $errMsg = 'Unknown system has been requested.';
                throw new ChannelException($errMsg);
            }

            self::_logReceivedMessage($systemid, 'suiteConnectAckAck');

            SquizSuite::updateProduct($systemid, 'status', 'live');
        } catch (Exception $e) {
            $errMsg  = 'suiteConnectAckAck failed.'."\n";
            $errMsg .= $e->getMessage()."\n";
            self::_logErrorMessage($errMsg);
        }//end try

    }//end suiteConnectAckAck()


    /**
     * Parse the passed XML file and return.
     *
     * @param string $xml SquizSuite specific XML message string.
     *
     * @since  4.0.0
     * @return array
     * @throws ChannelException Invalid XML caused DomDocument() error.
     */
    private static function _parseSuiteXML($xml)
    {
        $doc    = new DomDocument();
        $result = $doc->loadXML($xml);
        if ($result === FALSE) {
            $errMsg = 'Failed to load the message XML.';
            throw new ChannelException($errMsg);
        }

        $result = array();
        $fields = array(
                   'system_id',
                   'system_url',
                   'cert',
                  );
        foreach ($fields as $field) {
            $fieldEl = $doc->getElementsByTagName($field)->item(0);
            if ($fieldEl === NULL) {
                $result[$field] = NULL;
            } else {
                $result[$field] = $fieldEl->nodeValue;
            }
        }

        return $result;

    }//end _parseSuiteXML()


    /**
     * Parse the passed user agent string and return the system type.
     *
     * @param string $userAgentStr User agent string to parse.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getSystemTypeFromUserAgent($userAgentStr)
    {
        $matches = array();
        $pattern = '/'.self::$_userAgentPrefix.'([^\/]+)\/4\.0/';
        preg_match_all($pattern, $userAgentStr, $matches);
        if (empty($matches) === TRUE) {
            return FALSE;
        }

        return $matches[1][0];

    }//end _getSystemTypeFromUserAgent()


    /**
     * Log the received message to file.
     *
     * @param string $systemid Source systemid where the message is from.
     * @param string $msgType  Type of message received.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _logReceivedMessage($systemid, $msgType)
    {
        /*
            E.g)
            [RECEIVED 02:23:18] CON_REQUEST from search (http://squiz-search.net) (172.293.283.281)
            [SENT 02:23:18] CON_REQUEST to search (http://squiz-search.net) (172.293.283.281)
        */

        include_once 'Libs/String/String.inc';
        $log  = '[RECEIVED '.String::easyDatetime(time()).'] '.$msgType;
        $log .= ' from '.$systemid;

        $productInfo = SquizSuite::getProduct($systemid);
        if (empty($productInfo) === FALSE) {
            $log .= ' ('.$productInfo['connection']['url'].')';
            if (isset($_SERVER['REMOTE_ADDR']) === TRUE) {
                $log .= ' ('.$_SERVER['REMOTE_ADDR'].')';
            }
        }

        $log .= "\n";

        file_put_contents(self::_getLogFilePath(), $log, FILE_APPEND);

    }//end _logReceivedMessage()


    /**
     * Log the sent message to file.
     *
     * @param string $systemid Destined systemid.
     * @param string $msgType  Type of message sent.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _logSentMessage($systemid, $msgType)
    {
        /*
            E.g)
            [RECEIVED 02:23:18] CON_REQUEST from search (http://squiz-search.net) (172.293.283.281)
            [SENT 02:23:18] CON_REQUEST to search (http://squiz-search.net) (172.293.283.281)
        */

        include_once 'Libs/String/String.inc';
        $log         = '[SENT '.String::easyDatetime(time()).'] '.$msgType;
        $log        .= ' to '.$systemid;
        $productInfo = SquizSuite::getProduct($systemid);
        if (empty($productInfo) === FALSE) {
            $log .= ' ('.$productInfo['connection']['url'].')';
        }

        $log .= "\n";

        file_put_contents(self::_getLogFilePath(), $log, FILE_APPEND);

    }//end _logSentMessage()


    /**
     * Log the passed error message to file.
     *
     * @param string $errorMessage Error message to log.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _logErrorMessage($errorMessage='')
    {
        /*
            E.g)
            [RECEIVED 02:23:18] CON_REQUEST from search (http://squiz-search.net) (172.293.283.281)
            [SENT 02:23:18] CON_REQUEST to search (http://squiz-search.net) (172.293.283.281)
        */

        include_once 'Libs/String/String.inc';
        $log  = '[ERROR '.String::easyDatetime(time()).'] '.$errorMessage;
        $log .= "\n";

        file_put_contents(self::_getLogFilePath(), $log, FILE_APPEND);

    }//end _logErrorMessage()


    /**
     * Return the file path to the SquizSuite system log file.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getLogFilePath()
    {
        $logFilePath  = BaseSystem::getDataDir('SquizSuite');
        $logFilePath .= '/message_log.txt';
        return $logFilePath;

    }//end _getLogFilePath()


    /**
     * Encrypt the outgoing API message with the destined system's public key.
     *
     * This runs as a modifier on API::sendCURLAPIRequest() action and checks
     * $msg array. If _enc and _pubKeySystemid fields are included, then it
     * encrypts the data with the destined system's public key.
     *
     * @param string $url       URL of the system to send the request.
     * @param array  $msg       Array of messages to send.
     * @param string $userAgent Optional User Agent string to send.
     *
     * @since  4.0.0
     * @return array
     */
    public static function encryptAPIData($url, array $msg, $userAgent)
    {
        if (isset($msg['_enc']) === TRUE && isset($msg['_pubKeySystemid']) === TRUE) {
            $destProduct = SquizSuite::getProduct($msg['_pubKeySystemid']);
            if (empty($destProduct) === TRUE) {
                return $msg;
            }

            // JSON encode the data to handle various types.
            $msg['_enc'] = json_encode($msg['_enc']);
            $enc         = SquizSuite::encryptData($msg['_pubKeySystemid'], $msg['_enc']);

            $msg['_enc']      = $enc['encrypted'];
            $msg['_password'] = $enc['password'];
        }

        return $msg;

    }//end encryptAPIData()


    /**
     * Decrypt the posted data with the current system's private key.
     *
     * This runs as an after runner on API::getActionResult() action and
     * checks _enc and _password fields exist in POST var. If they are
     * provided, decrypt the value of _enc with the current system's
     * private key and put it back to POST var.
     *
     * @param string $system System name where action resides.
     * @param string $action Action name to call.
     * @param array  $args   Parameters to pass to the action.
     *
     * @since  4.0.0
     * @return void
     */
    public static function decryptAPIData($system, $action)
    {
        include_once 'Libs/Security/Security.inc';
        $encrypted = Security::getRequestData('_enc', NULL, FALSE, 'post');
        $password  = Security::getRequestData('_password', NULL, FALSE, 'post');
        if ($encrypted === NULL || $password === NULL) {
            return;
        }

        $encrypted     = SquizSuite::decryptData($encrypted, $password);
        $_POST['_enc'] = json_decode($encrypted, TRUE);

    }//end decryptAPIData()


    /**
     * Encrypt the passed data with the public key of the given system.
     *
     * It encrypts the data with the password based encryption algorithm.
     * Then the password will be encrypted with the public key of the
     * given system. The returned array includes both of the password and
     * encrypted data.
     *
     * @param string $systemid Unique ID of the product.
     * @param mixed  $data     Data to encrypt.
     *
     * @since  4.0.0
     * @return array
     */
    public static function encryptData($systemid, $data)
    {
        $encrypted = NULL;
        $product   = SquizSuite::getProduct($systemid);
        if (isset($product['connection']['cert']) === TRUE) {
            $certStr      = $product['connection']['cert'];
            $pubKey       = openssl_pkey_get_public($certStr);
            $password     = sha1(microtime(TRUE));
            $encryptedMsg = self::_opensslEncrypt($data, self::$_opensslEncryptionAlgorithm, $password);
            if ($encryptedMsg === FALSE) {
                return FALSE;
            }

            $success       = openssl_public_encrypt($password, $encryptedPass, $pubKey);
            $encryptedPass = base64_encode($encryptedPass);
            if ($success === FALSE) {
                return FALSE;
            }

            $result = array(
                       'encrypted' => $encryptedMsg,
                       'password'  => $encryptedPass,
                      );
            return $result;
        }//end if

        return FALSE;

    }//end encryptData()


    /**
     * Decrypt the passed data with the given password.
     *
     * It decrypts the given password with the current product's private
     * key. Then it uses the decrypted password to decrypt the data.
     * It returns the decrypted string or FALSE on error.
     *
     * @param string $data     Data to decrypt.
     * @param string $password Key encrypted password.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function decryptData($data, $password)
    {
        // Get the current product information.
        $currProduct = SquizSuite::getProduct();
        $priKeyPath  = BaseSystem::getDataDir('SquizSuite').'/privatekey.pem';
        $priKey      = openssl_pkey_get_private('file://'.$priKeyPath);
        $password    = base64_decode($password);

        $decrypted = '';
        $result    = openssl_private_decrypt(
            $password,
            $passdec,
            $priKey
        );

        if ($result === FALSE) {
            return FALSE;
        }

        $decrypted = self::_opensslDecrypt($data, self::$_opensslEncryptionAlgorithm, $passdec);
        if ($decrypted === FALSE) {
            return FALSE;
        }

        return $decrypted;

    }//end decryptData()


    /**
     * Encrypt the data with the given algorithm.
     *
     * It uses the command line openssl to perform the encryption and works
     * the same as openssl_encrypt() function, which is only available in
     * PHP > 5.3.0.
     *
     * Note: Replace it with openssl_encrypt() once PHP installation is upgraded.
     *
     * @param string $data      Data to decrypt.
     * @param string $algorithm Algorithm to use for encryption.
     * @param string $password  Password to use for encryption.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _opensslEncrypt($data, $algorithm, $password)
    {
        $filename = uniqid().'.txt';
        $filePath = BaseSystem::getDataDir('SquizSuite').'/'.$filename;
        file_put_contents($filePath, $data);

        $command  = '/usr/bin/openssl enc -'.$algorithm.' -a -salt -in '.$filePath;
        $command .= ' -out '.$filePath.'.enc -pass pass:'.$password;
        exec($command, $output, $returnVal);
        if ($returnVal === 0 && file_exists($filePath.'.enc') === TRUE) {
            $encrypted = file_get_contents($filePath.'.enc');
            unlink($filePath);
            unlink($filePath.'.enc');
            return $encrypted;
        }

        return FALSE;

    }//end _opensslEncrypt()


    /**
     * Decrypt the data with the given algorithm.
     *
     * It uses the command line openssl to perform the decryption and works
     * the same as openssl_decrypt() function, which is only available in
     * PHP > 5.3.0.
     *
     * Note: Replace it with openssl_decrypt() once PHP installation is upgraded.
     *
     * @param string $data      Data to decrypt.
     * @param string $algorithm Algorithm to use for decryption.
     * @param string $password  Password to use for decryption.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _opensslDecrypt($data, $algorithm, $password)
    {
        $filename = uniqid().'.txt';
        $filePath = BaseSystem::getDataDir('SquizSuite').'/'.$filename;
        file_put_contents($filePath, $data);

        $command  = '/usr/bin/openssl enc -d -'.$algorithm.' -a -salt -in '.$filePath;
        $command .= ' -out '.$filePath.'.dec  -pass pass:'.$password;
        exec($command, $output, $returnVal);
        if ($returnVal === 0 && file_exists($filePath.'.dec') === TRUE) {
            $encrypted = file_get_contents($filePath.'.dec');
            unlink($filePath);
            unlink($filePath.'.dec');
            return $encrypted;
        }

        return FALSE;

    }//end _opensslDecrypt()


}//end class

?>
