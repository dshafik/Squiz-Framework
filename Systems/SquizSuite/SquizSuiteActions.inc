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
     * Activate a product to the Suite.
     *
     * @param string $systemid The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function activateProduct($systemid)
    {
        return TRUE;

    }//end activateProduct()


    /**
     * Collect the Product Statistics.
     *
     * @param string $systemid The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function collectProductStatistic($systemid)
    {
        return TRUE;

    }//end collectProductStatistic()


    // Get all products.
    public static function getAllProducts()
    {
        $query  = DAL::getDALQuery('SquizSuite', 'getAllProducts');
        $result = DAL::getAssoc($query);
        return $result;

    }//end getAllProducts()


    // Get specific product information. If systemid is null, the current
    // product information will be returned.
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
     * Modify a project in the Suite.
     *
     * @param string $systemid The ID of the Product.
     * @param string $attrName  The name of the field to change.
     * @param string $attrValue The new value.
     *
     * @since  4.0.0
     * @return boolean
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

    }//end modifyProduct()


    /**
     * Register a product.
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
     * Remove a product from the Suite.
     *
     * @param string $systemid The ID of the Product.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function removeProduct($systemid)
    {
        $result = DAL::executeQuery('SquizSuite', 'removeProduct');
        return $result;

    }//end removeProduct()


    // Encrypt the passed data with the given system's public key.
    public static function encryptMessage($systemid, $data)
    {
        $result  = NULL;
        $product = SquizSuite::getProduct($systemid);
        if (isset($product['connection']['publicKey']) === TRUE) {
            openssl_public_encrypt($data, $result, $product['connection']['publicKey']);
            return $result;
        }

        return FALSE;

    }//end encryptMessage()


    /*
    TODO: PASSPHRASE NEEDS TO BE FIXED!
     */
    // Decrypt the passed data with the current system's private key.
    public static function decryptMessage($data)
    {
        // Get the current product information.
        $currProduct = SquizSuite::getProduct();
        $priKeyPath  = BaseSystem::getDataDir('SquizSuite').'/privatekey';
        $priKey      = openssl_pkey_get_private('file://'.$priKeyPath, $currProduct['systemid']);

        $decrypted = '';
        openssl_private_decrypt(
            $data,
            $decrypted,
            $priKey
        );

        return $decrypted;

    }//end decrypeMessage()


    /*
    TODO: PASSPHRASE NEEDS TO BE FIXED!
     */
    // Generate the pub/pri key pair for the current system.
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
        openssl_pkey_export_to_file($keyGenerated, $suiteDir.'/privatekey', $currProduct['systemid']);

        $pubKey = openssl_pkey_get_details($keyGenerated);
        file_put_contents($suiteDir.'/publickey', $pubKey['key']);

        // Also update the current product connection information with public key in it.
        $currProduct['connection']['publicKey'] = $pubKey['key'];
        SquizSuite::updateProduct($currProduct['systemid'], 'connection', $currProduct['connection']);

        return $pubKey['key'];

    }//end generateKeyPair()


    // Returns true if the request is for SquizSuite system messages.
    public static function isSuiteRequest()
    {
        //'SquizSuite-Analytics/4.0 (http://squiz-analytics.net; systemidcomeshere)';
        if (isset($_SERVER['HTTP_USER_AGENT']) === TRUE) {
            if (strpos($_SERVER['HTTP_USER_AGENT'], self::$_userAgentPrefix) === 0) {
                return TRUE;
            }
        }

        return FALSE;

    }//end 


    // Send SquizSuite message to the target system.
    // TODO: $data?
    public static function sendMessage($systemid, $msgType, $data=NULL)
    {
        $productInfo = SquizSuite::getProduct($systemid);
        if (empty($productInfo) === TRUE) {
            // No PRODUCT INFO!.
        }

        $dom = self::_createSuiteMessageDOMTemplate();
        self::_createSuiteMessageDOM($dom, $systemid, $productInfo, $msgType);
        $xml = $dom->saveXML();

        $msg        = array();
        $msg['xml'] = $xml;
        if ($msgType === 'CON_REQUEST') {
            $msg['con_request'] = 1;
        } else {
            $msg['xml'] = SquizSuite::encryptMessage($systemid, $msg['xml']);
            $msg['xml'] = base64_encode($msg['xml']);
        }

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $productInfo['connection']['url']);
        curl_setopt($ch, CURLOPT_POST, TRUE);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER  ,1);  // RETURN THE CONTENTS OF THE CALL
        curl_setopt($ch, CURLOPT_HTTPHEADER, array('Expect:'));
        curl_setopt($ch, CURLOPT_USERAGENT, self::_getSystemUserAgent()); 

        $result   = curl_exec($ch);
        $curlinfo = curl_getinfo($ch);
        curl_close($ch);

    }//end sendSuiteMessage()


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
                // Send a request to other SquizSuite system to be registered.
                $currProduct = SquizSuite::getProduct();
                $pubKey      = $currProduct['connection']['publicKey'];
                $pubkeyEl    = $dom->createElement('publicKey');
                API::buildResponseXMLDom($pubkeyEl, $pubKey);
                $msgEl->appendChild($pubkeyEl);
            break;

            case 'CON_REQUEST_ACK':
            break;

            case 'CON_CONFIRM':
            break;

            case 'CON_CONFIRM_ACK':
            break;

            default:
            break;
        }//end switch

        return $dom;

    }//end _getSuiteMessageDOMTemplate()


    // Process the incoming SquizSuite message.
    public static function processMessage()
    {
        $uaParts = self::_parseSystemUserAgent($_SERVER['HTTP_USER_AGENT']);
        if ($uaParts === FALSE) {
            self::_sendHeader(500);
            exit;
        }

        $systemType = $uaParts['systemType'];
        $systemUrl  = $uaParts['systemUrl'];
        $systemid   = $uaParts['systemid'];

        include_once 'Libs/Security/Security.inc';
        $xml        = Security::getRequestData('xml', NULL, FALSE, 'post');
        $conRequest = Security::getRequestData('con_request', NULL, FALSE, 'post');
        if ($conRequest === NULL) {
            // Only the connection request comes unencrypted.
            try {
                $xml = base64_decode($xml);
                $xml = SquizSuite::decryptMessage($xml);
            } catch (Exception $e) {
                // CAN NOT DECRYPT THE MESSAGE.
                // SOMETHING WENT WRONG!
                self::_sendHeader(500);
                exit;
            }
        }

        try {
            self::_processMessage($systemid, $systemUrl, $systemType, $xml);
            self::_sendHeader(200);
        } catch (Exception $e) {
            self::_sendHeader(500);
            exit;
        }

    }//end processSuiteMessage()


    private static function _processMessage($systemid, $systemUrl, $systemType, $xml)
    {
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $msgTypeEl = $doc->getElementsByTagName('message_type')->item(0);
        $msgType   = $msgTypeEl->nodeValue;
        switch ($msgType) {
            case 'CON_REQUEST':
                // TODO: Maximum daily request exceeded?
                // TODO: System already exists?

                // Add the system and set the status as 'pending'.

                // Anyway to validate the integrity of Public Key????
                $pubKeyEl = $doc->getElementsByTagName('pubKey')->item(0);
                $pubKey   = $pubKeyEl->nodeValue;

                $connection              = array();
                $connection['url']       = $systemUrl;
                $connection['publicKey'] = $pubKey;

                SquizSuite::registerProduct($systemid, $systemType, $connection);
                SquizSuite::sendMessage($systemid, 'CON_REQUEST_ACK');
            break;

            case 'CON_REQUEST_ACK':
                // Request accepted.
                // Set the status as 'waiting'.
            break;

            case 'CON_CONFIRM':
                // Request accepted.
                // Set the status as 'live'.
            break;

            case 'CON_CONFIRM_ACK':
                // Request accepted.
                // Set the status as 'live'.
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

            default:
            break;
        }

    }//end _sendHeader()


    // Returns the current system's User Agent string.
    private static function _getSystemUserAgent()
    {
        $productInfo   = SquizSuite::getProduct();
        $userAgentStr  = self::$_userAgentPrefix.$productInfo['type'].'/4.0 ';
        $userAgentStr .= '('.$productInfo['connection']['url'].'; ';
        $userAgentStr .= $productInfo['systemid'].')';
        return $userAgentStr;

    }//end _getSystemUserAgent()


    // Parse the incoming SquizSuite User Agent string.
    private static function _parseSystemUserAgent($userAgentStr)
    {
        $matches = array();
        $pattern = '/'.self::$_userAgentPrefix.'([^\/]+)\/4.0 \(http:\/\/([^ ]+); ([^\)]+)\)/';
        preg_match_all($pattern, $userAgentStr, $matches);
        if (empty($matches) === TRUE) {
            return FALSE;
        }

        $result = array(
                   'systemType' => $matches[1][0],
                   'systemUrl'  => $matches[2][0],
                   'systemid'   => $matches[3][0],
                  );
        return $result;

    }//end _parseSystemUserAgent()


}//end class

?>
