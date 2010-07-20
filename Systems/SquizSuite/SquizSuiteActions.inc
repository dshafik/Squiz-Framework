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

    // User Agent string prefix.
    private static $_userAgentPrefix = 'SquizSuite-';

    private static $_opensslEncryptionAlgorithm = 'blowfish';

    // Product information cache.
    private static $_productCache    = array();


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
        if ($attrName === 'connection' && is_array($attrValue)) {
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
    public static function encryptMessage($systemid, $data)
    {
        $encrypted = NULL;
        $product   = SquizSuite::getProduct($systemid);
        if (isset($product['connection']['cert']) === TRUE) {
            $certStr = $product['connection']['cert'];
            $pubKey  = openssl_pkey_get_public($certStr);

            $password     = sha1(microtime(true));
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
        }

        return FALSE;

    }//end encryptMessage()


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
    public static function decryptMessage($data, $password)
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

    }//end decryptMessage()


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
        $csr = openssl_csr_new($info, $keyGenerated);
        openssl_csr_export_to_file($csr, $suiteDir.'/system.csr');

        $cert = openssl_csr_sign($csr, NULL, $keyGenerated, (365 * 10));
        openssl_x509_export_to_file($cert, $suiteDir.'/system.crt');
        openssl_x509_export($cert, $certStr);

        file_put_contents($suiteDir.'/publickey', $pubKey['key']);

        // Also update the current product connection information with public key in it.
        $currProduct['connection']['cert'] = $certStr;
        SquizSuite::updateProduct($currProduct['systemid'], 'connection', $currProduct['connection']);

    }//end generateKeyPair()


    // Send SquizSuite message to the target system.
    // TODO: $data?
    public static function sendMessage($systemid, $msgType, $data=NULL)
    {
        try {
            DAL::beginTransaction();
            $productInfo = SquizSuite::getProduct($systemid);
            if (empty($productInfo) === TRUE) {
                $errMsg = $systemid.' is not known. Cannot send '.$msgType.' to it.';
                throw new ChannelException($errMsg);
            }

            $dom = self::_createSuiteMessageDOMTemplate();
            self::_createSuiteMessageDOM($dom, $systemid, $productInfo, $msgType);
            $xml = $dom->saveXML();
            $msg = array();

            $msg['_format'] = 'xml';
            $msg['_system'] = 'SquizSuite';
            switch ($msgType) {
                case 'CON_REQUEST':
                    $msg['_action'] = 'suiteConnect';
                break;

                case 'CON_REQUEST_ACK':
                    $msg['_action'] = 'suiteConnectAck';
                break;
 
                case 'CON_REQUEST_ACK_ACK':
                    $msg['_action'] = 'suiteConnectAckAck';
                break;
            }

            if ($msgType === 'CON_REQUEST') {
                $msg['con_request'] = 1;
                $msg['xml']         = $xml;
            } else {
                $enc = SquizSuite::encryptMessage($systemid, $xml);
                if ($enc === FALSE) {
                    // TODO: Encrytion Failed!
                    $errMsg = 'Encryting data to '.$systemid.' has failed.';
                    throw new ChannelException($errMsg);
                }

                $msg['xml']      = $enc['encrypted'];
                $msg['password'] = $enc['password'];

            }

            $ch = curl_init();
            curl_setopt($ch, CURLOPT_URL, $productInfo['connection']['url'].'/__api/');
            curl_setopt($ch, CURLOPT_POST, TRUE);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER  ,1);
            curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:'));
            curl_setopt($ch, CURLOPT_USERAGENT, self::_getSystemUserAgent());

            self::_logSentMessage($systemid, $msgType);
            $result   = curl_exec($ch);
            $curlinfo = curl_getinfo($ch);
            curl_close($ch);

            if ($curlinfo['http_code'] === 200) {
                //self::_logSentMessage($systemid, $msgType);
            } else {
                $errMsg = 'Sending '.$msgType.' to '.$systemid.' '.$curlinfo['http_code'].' returned.';
                throw new ChannelException($errMsg);
            }

            DAL::commit();
            return TRUE;
        } catch (Exception $e) {
            DAL::rollback();
            self::_logErrorMessage($e->getMessage());
            return FALSE;
        }

    }//end sendMessage()


    // Create DOM template for the message to send with the current
    // system's information.
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

    }//end _getSuiteMessageDOMTemplate()


    // Create SquizSuite message DOM.
    private static function _createSuiteMessageDOM(&$dom, $systemid, $productInfo, $msgType)
    {
        Channels::includeSystem('API');
        $msgEl     = $dom->getElementsByTagName('message')->item(0);
        $msgTypeEl = $dom->createElement('message_type');

        // Append message_type element.
        API::buildResponseXMLDom($msgTypeEl, $msgType);
        $msgEl->appendChild($msgTypeEl);

        switch ($msgType) {
            case 'CON_REQUEST':
            case 'CON_REQUEST_ACK':
                // Both of CON_REQUEST and CON_REQUEST_ACK will send
                // the system's public key together.
                $currProduct = SquizSuite::getProduct();
                $pubKey      = $currProduct['connection']['cert'];
                $pubkeyEl    = $dom->createElement('cert');
                API::buildResponseXMLDom($pubkeyEl, $pubKey);
                $msgEl->appendChild($pubkeyEl);
            break;

            default:
            break;
        }//end switch

        return $dom;

    }//end _getSuiteMessageDOMTemplate()


    public static function preProcessSuiteRequest($system, $action, $args)
    {
    }//end preProcessSuiteRequest()


    /**
     *
     * @since  4.0.0
     * @return void
     * @api    read
     */
    public static function suiteConnect()
    {
        try {
            DAL::beginTransaction();
            $systemType = self::_getsystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
            if ($systemType === FALSE) {
                $errMsg = 'User-agent string can not be parsed.';
                throw new ChannelException($errMsg);
            }

            include_once 'Libs/Security/Security.inc';
            $xml        = Security::getRequestData('xml', NULL, FALSE, 'post');
            $password   = Security::getRequestData('password', NULL, FALSE, 'post');
            $conRequest = Security::getRequestData('con_request', NULL, FALSE, 'post');
            $xml        = html_entity_decode($xml);

            $doc = new DomDocument();
            $doc->loadXML($xml);

            $sysidEl   = $doc->getElementsByTagName('systemid')->item(0);
            $sysurlEl  = $doc->getElementsByTagName('system_url')->item(0);
            $systemid  = $sysidEl->nodeValue;
            $systemUrl = $sysurlEl->nodeValue;

            $msgTypeEl = $doc->getElementsByTagName('message_type')->item(0);
            $msgType   = $msgTypeEl->nodeValue;
            self::_logReceivedMessage($systemid, $msgType);

            // TODO: Maximum daily request exceeded?
            // TODO: System already exists?

            // Add the system and set the status as 'pending'.

            // TODO: What if pub key is not given?
            // Anyway to validate the integrity of Public Key????
            $pubKeyEl = $doc->getElementsByTagName('cert')->item(0);
            $pubKey   = $pubKeyEl->nodeValue;

            $connection         = array();
            $connection['url']  = $systemUrl;
            $connection['cert'] = $pubKey;
            SquizSuite::registerProduct($systemid, $systemType, $connection);
            SquizSuite::updateProduct($systemid, 'status', 'pending');
            $success = SquizSuite::sendMessage($systemid, 'CON_REQUEST_ACK');

            DAL::commit();
            self::_sendHeader(200);
        } catch (Exception $e) {
            self::_logErrorMessage($e->getMessage());
            DAL::rollback();
            self::_sendHeader(500);
            exit;
        }

    }//end suiteConnect()


    /**
     *
     * @since  4.0.0
     * @return void
     * @api    read
     */
    public static function suiteConnectAck()
    {
        try {
            DAL::beginTransaction();


            DAL::commit();
            self::_sendHeader(200);
        } catch (Exception $e) {
            self::_logErrorMessage($e->getMessage());
            DAL::rollback();
            self::_sendHeader(500);
            exit;
        }

    }//end suiteConnectAck()


    /**
     *
     * @since  4.0.0
     * @return void
     * @api    read
     */
    public static function suiteConnectAckAck()
    {
        try {
            DAL::beginTransaction();


            DAL::commit();
            self::_sendHeader(200);
        } catch (Exception $e) {
            self::_logErrorMessage($e->getMessage());
            DAL::rollback();
            self::_sendHeader(500);
            exit;
        }

    }//end suiteConnectAckAck()


    // Process the incoming SquizSuite message.
    public static function processMessage()
    {
        try {
            DAL::beginTransaction();
            $systemType = self::_getsystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
            if ($systemType === FALSE) {
                $errMsg = 'User-agent string can not be parsed.';
                throw new ChannelException($errMsg);
            }

            include_once 'Libs/Security/Security.inc';
            $xml        = Security::getRequestData('xml', NULL, FALSE, 'post');
            $password   = Security::getRequestData('password', NULL, FALSE, 'post');
            $conRequest = Security::getRequestData('con_request', NULL, FALSE, 'post');
            if ($conRequest === NULL) {
                // Only the connection request comes unencrypted.
                $xml = SquizSuite::decryptMessage($xml, $password);
                if ($xml === FALSE) {
                    $errMsg = 'Squiz Suite message can not be decrypted.';
                    throw new ChannelException($errMsg);
                }
            } else {
                // It comes as non-encrypted, HTML entity encoded format.
                $xml = html_entity_decode($xml);
            }

            self::_processMessage($systemType, $xml);
            DAL::commit();
            self::_sendHeader(200);
        } catch (Exception $e) {
            self::_logErrorMessage($e->getMessage());
            DAL::rollback();
            self::_sendHeader(500);
            exit;
        }

    }//end processSuiteMessage()


    private static function _processMessage($systemType, $xml)
    {
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $sysidEl   = $doc->getElementsByTagName('systemid')->item(0);
        $sysurlEl  = $doc->getElementsByTagName('system_url')->item(0);
        $systemid  = $sysidEl->nodeValue;
        $systemUrl = $sysurlEl->nodeValue;

        $msgTypeEl = $doc->getElementsByTagName('message_type')->item(0);
        $msgType   = $msgTypeEl->nodeValue;
        self::_logReceivedMessage($systemid, $msgType);
        switch ($msgType) {
            case 'CON_REQUEST':
                // TODO: Maximum daily request exceeded?
                // TODO: System already exists?

                // Add the system and set the status as 'pending'.

                // TODO: What if pub key is not given?
                // Anyway to validate the integrity of Public Key????
                $pubKeyEl = $doc->getElementsByTagName('cert')->item(0);
                $pubKey   = $pubKeyEl->nodeValue;

                $connection              = array();
                $connection['url']       = $systemUrl;
                $connection['cert'] = $pubKey;
                SquizSuite::registerProduct($systemid, $systemType, $connection);
                SquizSuite::updateProduct($systemid, 'status', 'pending');
                $success = SquizSuite::sendMessage($systemid, 'CON_REQUEST_ACK');
            break;

            case 'CON_REQUEST_ACK':
                $productInfo = SquizSuite::getProduct($systemid);
                if (empty($productInfo) === TRUE) {
                    // Who r u ? and y you sending this to me?
                }

                $pubKeyEl = $doc->getElementsByTagName('cert')->item(0);
                $pubKey   = $pubKeyEl->nodeValue;

                // TODO: What if pub key is not gived?
                $productInfo['connection']['cert'] = $pubKey;

                // Connection Request accepted.
                SquizSuite::updateProduct($systemid, 'connection', $productInfo['connection']);
                SquizSuite::updateProduct($systemid, 'status', 'live');
                $success = SquizSuite::sendMessage($systemid, 'CON_REQUEST_ACK_ACK');
            break;

            case 'CON_REQUEST_ACK_ACK':
                $productInfo = SquizSuite::getProduct($systemid);
                if (empty($productInfo) === TRUE) {
                    // Who r u ? and y you sending this to me?
                    // TODO: Do i have pending system waiting for the response from 
                    // this system?
                }

                SquizSuite::updateProduct($systemid, 'status', 'live');
            break;

            default:
            break;
        }//end switch

    }//end _processMessage()


    // Send HTTP header.
    private static function _sendHeader($responseCode)
    {
        switch ($responseCode) {
            case 200:
                header('HTTP/1.1 200 OK', FALSE, 200);
                /*
                    Content-Type: text/xml; charset=utf-8
                    Content-Length: length
                */
            break;

            case 500:
                header('HTTP/1.1 500 Internal Server Error', FALSE, 500);
            break;

            default:
            break;
        }

    }//end _sendHeader()


    // Returns the current system's User Agent string.
    private static function _getSystemUserAgent()
    {
        $productInfo  = SquizSuite::getProduct();
        $userAgentStr = self::$_userAgentPrefix.$productInfo['type'].'/4.0';
        return $userAgentStr;

    }//end _getSystemUserAgent()


    // Parse the incoming SquizSuite User Agent string.
    private static function _getsystemTypeFromUserAgent($userAgentStr)
    {
        $matches = array();
        $pattern = '/'.self::$_userAgentPrefix.'([^\/]+)\/4\.0/';
        preg_match_all($pattern, $userAgentStr, $matches);
        if (empty($matches) === TRUE) {
            return FALSE;
        }

        return $matches[1][0];

    }//end _parseSystemUserAgent()


    private static function _logReceivedMessage($systemid, $msgType)
    {
        /*
            e.g)
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

    }//end _logMessage()


    private static function _logSentMessage($systemid, $msgType)
    {
        /*
            e.g)
            [RECEIVED 02:23:18] CON_REQUEST from search (http://squiz-search.net) (172.293.283.281)
            [SENT 02:23:18] CON_REQUEST to search (http://squiz-search.net) (172.293.283.281)
        */

        include_once 'Libs/String/String.inc';
        $log  = '[SENT '.String::easyDatetime(time()).'] '.$msgType;
        $log .= ' to '.$systemid;
        $productInfo = SquizSuite::getProduct($systemid);
        if (empty($productInfo) === FALSE) {
            $log .= ' ('.$productInfo['connection']['url'].')';
        }

        $log .= "\n";

        file_put_contents(self::_getLogFilePath(), $log, FILE_APPEND);

    }//end _logMessage()


    private static function _logErrorMessage($errorMessage='')
    {
        /*
            e.g)
            [RECEIVED 02:23:18] CON_REQUEST from search (http://squiz-search.net) (172.293.283.281)
            [SENT 02:23:18] CON_REQUEST to search (http://squiz-search.net) (172.293.283.281)
        */
        include_once 'Libs/String/String.inc';
        $log  = '[ERROR '.String::easyDatetime(time()).'] '.$errorMessage;
        $log .= "\n";

        file_put_contents(self::_getLogFilePath(), $log, FILE_APPEND);

    }//end _logMessage()


    private static function _getLogFilePath()
    {
        $logFilePath  = BaseSystem::getDataDir('SquizSuite');
        $logFilePath .= '/message_log.txt';
        return $logFilePath;

    }

    // TODO: Replace this with openssl_encrypt once PHP installation
    // is upgrade to > 5.3.0
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


    // TODO: Replace this with openssl_decrypt once PHP installation
    // is upgrade to > 5.3.0
    /**
     *
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
