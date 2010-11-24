<?php
/**
 * Actions for the SquizSuite System.
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
 * @subpackage SquizSuite
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * SquizSuite Actions Class.
 */
class SquizSuiteActions
{

    /**
     * User Agent String prefix.
     *
     * @var string
     */
    private static $_userAgentPrefix = 'SquizSuite-';

    /**
     * Encryption algorithm.
     *
     * @var string
     */
    private static $_opensslEncryptionAlgorithm = 'blowfish';

    /**
     * Local cache variable for product information.
     *
     * @var string
     */
    private static $_productCache = array();

    /**
     * Flag used for indicating that the system is running as system administrator.
     *
     * @var boolean
     */
    private static $_suiteSystemUser = FALSE;


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Returns the name of this product.
     *
     * Other systems need to refine this channel
     * to provide their own data.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function getProductTitle()
    {
        $title = 'Squiz Framework';
        return $title;

    }//end getProductTitle()


    /**
     * Returns the logo of this product.
     *
     * Other systems need to refine this channel
     * to provide their own logo.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function getProductLogo()
    {
        $logoPath = '/__web/Systems/GUI/Widgets/Toolbar/Web/logo.png';
        return $logoPath;

    }//end getProductLogo()


    /**
     * Returns the summary of this product.
     *
     * Other systems need to plug in this channel
     * to provide their own data to send out to
     * other systems.
     *
     * @return array
     *
     * @api            read
     * @api-permission superuser
     */
    public static function getProductSummary()
    {
        $summary = array();
        return $summary;

    }//end getProductSummary()


    /**
     * Returns the type of the current product.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function getProductType()
    {
        $currProduct = SquizSuite::getProduct();
        return $currProduct['type'];

    }//end getProductType()


    /**
     * Returns the default systemid for the type passed.
     *
     * @param string $productType Type of the product.
     *
     * @return string
     */
    public static function getDefaultIDForType($productType)
    {
        $id = '';
        switch ($productType) {
            case 'Squiz CMS':
                $id = 'cms_demo';
            break;

            case 'Squiz Analytics':
                $id = 'analytics_demo';
            break;

            case 'Squiz Search':
                $id = 'search_demo';
            break;

            case 'Squiz Update':
                $id = 'update_demo';
            break;

            default:
                // No default ID for unknown product.
            break;
        }//end switch

        return $id;

    }//end getDefaultIDForType()


    /**
     * Returns the public key of the current product.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function getPublicKey()
    {
        $product = SquizSuite::getCurrentProduct();
        return $product['connection']['cert'];

    }//end getPublicKey()


    /**
     * Returns TRUE if the user doesnt have permission to get product summary.
     *
     * @return boolean
     */
    public static function hideProductSummary()
    {
        Channels::includeSystem('User');
        $summary    = array();
        $currUserid = User::getCurrentUserid();
        if (User::isSystemAdministrator($currUserid) === FALSE) {
            return TRUE;
        }

        return FALSE;

    }//end hideProductSummary()


    /**
     * Returns an empty array for product summary.
     *
     * @return array
     */
    public static function getNoProductSummary()
    {
        $summary = array();
        return $summary;

    }//end getNoProductSummary()


    /**
     * Returns TRUE if the current product is a demo product.
     *
     * @return boolean
     */
    public static function isDemoProduct()
    {
        $isDemoSystem = FALSE;
        $demoIds      = array(
                         'sales_demo',
                         'cms_demo',
                         'analytics_demo',
                         'search_demo',
                         'update_demo',
                        );

        $currProduct = SquizSuite::getProduct();
        if (in_array($currProduct['systemid'], $demoIds) === TRUE) {
            $isDemoSystem = TRUE;
        }

        return $isDemoSystem;

    }//end isDemoProduct()


    /**
     * Return TRUE if suite system user mode is on.
     *
     * When it is TRUE, the system runs as a system administrator.
     * This can ONLY be turned on when the encrypted API request is
     * received from the connected live suite system. Check
     * decryptAPIData() action.
     *
     * @return boolean
     */
    public static function isSuiteSystemUser()
    {
        return self::$_suiteSystemUser;

    }//end isSuiteSystemUser()


    /**
     * Add a new Update Product for activation.
     *
     * If the system hasn't been activated yet, its ID does not
     * exist in the update server. In this instance, we can not
     * proceed the normal connection handshake sequence to connect.
     *
     * This action will set the appropriate setting for the update server
     * to connect and the rest will be handled by the real activation
     * in Patching System.
     *
     * @param string $apiURL The API Url of the Update Server.
     *
     * @return void
     */
    public static function addUpdateServerForActivation($apiURL)
    {
        Channels::includeSystem('API');
        $apiURL   = rtrim($apiURL, '/');
        $cert     = '';
        $message  = array();
        $systemid = NULL;
        try {
            $response = API::sendCURLAPIRequest(
                $apiURL.'/json/SquizSuite/getPublicKey',
                $message
            );
        } catch (Exception $e) {
            $response['curlInfo'] = array('http_code' => NULL);
        }

        if ($response['curlInfo']['http_code'] === 200) {
            $decoded = json_decode($response['result'], TRUE);
            $cert    = $decoded['result'];

            $systemid   = 'update-au-01';
            $systemName = 'Squiz Update';
            $systemType = 'Squiz Update';
            $connection = array(
                           'url'  => $apiURL,
                           'name' => $systemName,
                           'cert' => $cert,
                          );

            SquizSuite::registerProduct(
                $systemid,
                $systemType,
                $connection
            );

            SquizSuite::updateProduct($systemid, 'current', FALSE);
            SquizSuite::updateProduct($systemid, 'status', 'pending');
        }//end if

    }//end addUpdateServerForActivation()


    /**
     * Get the list of all suite products.
     *
     * @param boolean $includeCurrent If TRUE, the result includes the current product.
     * @param boolean $includeDeleted If TRUE, the result includes the products
     *                                marked for deletion.
     * @param array   $excludeTypes   If specified, the types will be excluded from the result.
     * @param array   $includeStatus  If specified, only the statuses specified will be returned.
     *
     * @return array
     */
    public static function getProducts(
        $includeCurrent=TRUE,
        $includeDeleted=TRUE,
        array $excludeTypes=array(),
        array $includeStatus=array()
    ) {
        $query    = DAL::getDALQuery('SquizSuite', 'getAllProducts');
        $products = DAL::getAssoc($query);

        // Process serialized array first.
        foreach ($products as &$product) {
            $product['connection'] = unserialize($product['connection']);

            // Adding to asset cache.
            $systemid = $product['systemid'];
            if (array_key_exists($systemid, self::$_productCache) === FALSE) {
                self::$_productCache[$systemid] = $product;
            }
        }//end foreach

        $result = array();
        foreach ($products as $p) {
            if ($includeCurrent === FALSE
                && $p['current'] === TRUE
            ) {
                continue;
            }

            if ($includeDeleted === FALSE
                && $p['sync_status'] === 'D'
            ) {
                continue;
            }

            if (empty($excludeTypes) === FALSE
                && in_array($p['type'], $excludeTypes) === TRUE
            ) {
                continue;
            }

            if (empty($includeStatus) === FALSE
                && in_array($p['status'], $includeStatus) === FALSE
            ) {
                continue;
            }

            $result[] = $p;
        }//end foreach

        return $result;

    }//end getProducts()


    /**
     * Get the list of all live suite products.
     *
     * The product should be live, not marked for deletion
     * and not the current one.
     *
     * @return array
     */
    public static function getLiveProducts()
    {
        $query  = DAL::getDALQuery('SquizSuite', 'getLiveProducts');
        $result = DAL::getAssoc($query);
        foreach ($result as &$product) {
            $product['connection'] = unserialize($product['connection']);

            // Adding to asset cache.
            $systemid = $product['systemid'];
            if (array_key_exists($systemid, self::$_productCache) === FALSE) {
                self::$_productCache[$systemid] = $product;
            }
        }//end foreach

        return $result;

    }//end getLiveProducts()


    /**
     * Get the product information for the given systemid.
     *
     * If NULL is given for the systemid, then the current product
     * will be returned.
     *
     * @param mixed $systemid Unique ID of the product.
     *
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
     * Returns the current system ID.
     *
     * @return string
     */
    public static function getSystemId()
    {
        $currProduct = SquizSuite::getCurrentProduct();
        return $currProduct['systemid'];

    }//end getSystemId()


    /**
     * Returns the attribute of the product.
     *
     * @param string $systemid System ID of the product.
     * @param string $attrName Attriute name to get.
     *
     * @return mixed
     */
    public static function getProductAttribute($systemid, $attrName)
    {
        $product = SquizSuite::getProduct($systemid);
        if (empty($product) === FALSE) {
            if (array_key_exists($attrName, $product) === TRUE) {
                return $product[$attrName];
            } else if (array_key_exists($attrName, $product['connection']) === TRUE) {
                return $product['connection'][$attrName];
            }
        }

        return NULL;

    }//end getProductAttribute()


    /**
     * Return the current product details.
     *
     * @return array
     */
    public static function getCurrentProduct()
    {
        $products = SquizSuite::getProducts();
        foreach ($products as $product) {
            if ($product['current'] === TRUE) {
                return $product;
            }
        }

    }//end getCurrentProduct()


    /**
     * Get the product list of the given type.
     *
     * @param string $type Type of product to get.
     *
     * @return array
     */
    public static function getProductsByType($type)
    {
        $query  = DAL::getDALQuery('SquizSuite', 'getProductsByType');
        $result = DAL::getAssoc($query);

        foreach ($result as &$product) {
            $product['connection'] = unserialize($product['connection']);
            self::$_productCache[$product['systemid']] = $product;
        }

        return $result;

    }//end getProductsByType()


    /**
     * Update the product information for the given systemid.
     *
     * @param string $systemid  Unique ID of the product.
     * @param string $attrName  Attribute name to update.
     * @param mixed  $attrValue Attribute value to update.
     *
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
     * @param string $knowsMeAs  The systemid which the other product think
     *                           the current product has.
     *
     * @return boolean
     */
    public static function registerProduct(
        $systemid,
        $type,
        $connection,
        $knowsMeAs=NULL
    ) {
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

        // Also update the current product connection information with public key in it.
        $currProduct['connection']['cert'] = $certStr;
        SquizSuite::updateProduct($currProduct['systemid'], 'connection', $currProduct['connection']);

    }//end generateSystemKeyPair()


    /**
     * Send suite specific message to other system.
     *
     * @param string $targetid System ID to send the message to OR
     *                         the API URL when sendConnect message is being
     *                         sent.
     * @param string $msgType  Type of message to send.
     * @param array  $options  Optional variables to pass.
     *
     * @return mixed
     * @throws ChannelException Error occurred.
     */
    public static function sendMessage($targetid, $msgType, array $options=array())
    {
        if ($msgType !== 'suiteConnect') {
            $productInfo = SquizSuite::getProduct($targetid);
            if (empty($productInfo) === TRUE) {
                $errMsg = $targetid.' is not known. Cannot send '.$msgType.' to it.';
                throw new ChannelException($errMsg);
            }
        }

        $dom = self::_createSuiteMessageDOMTemplate($targetid);
        self::_createSuiteMessageDOM($dom, $msgType, $options);
        $xml = $dom->saveXML();

        // Do not encrypt suiteConnect message. In fact, it can not encrypt as
        // the current system does not know the public key of the destined system.
        $msg = array();
        if ($msgType === 'suiteConnect') {
            $msg['xml']         = $xml;
            $msg['con_request'] = 1;
        } else {
            $msg['_enc']            = array('xml' => $xml);
            $msg['_pubKeySystemid'] = $targetid;
        }

        $result = SquizSuite::sendRequest(
            $targetid,
            'xml',
            'SquizSuite',
            $msgType,
            $msg
        );

        if ($result['curlInfo']['http_code'] !== 200) {
            $errMsg  = 'HTTP Response '.$result['curlInfo']['http_code']."\n";
            $errMsg .= var_export($result, 1)."\n";
        }

        return $result;

    }//end sendMessage()


    /**
     * Send a suite request to other system.
     *
     * @param string $targetid The targetid of the destination system. 
     * @param string $format   The format of the results. 
     * @param string $system   The system on the destination system.
     * @param string $action   The function to call on the destination system.
     * @param array  $message  The message to send.
     *
     * @return array
     */
    public static function sendRequest($targetid, $format, $system, $action, array $message)
    {
        $currProductInfo = SquizSuite::getProduct();
        $userAgentStr    = self::$_userAgentPrefix.$currProductInfo['type'].'/4.0';
        $productInfo     = SquizSuite::getProduct($targetid);
        $systemidKnownAs = SquizSuite::productKnowsMeAs($targetid);

        // Get the target API url.
        $apiURL = '';
        if ($action === 'suiteConnect') {
            $apiURL = $targetid;
        } else {
            $apiURL = $productInfo['connection']['url'];
        }

        if (self::_isMatrixSystem($targetid) === TRUE) {
            // Matrix needs a special case to handle.
            $message['_format'] = $format;
            $message['_system'] = $system;
            $message['_action'] = $action;
            if ($action !== 'suiteConnect') {
                // Only set the username/password if the _enc array isset.
                if (isset($message['_enc']) === FALSE || is_array($message['_enc']) === TRUE) {
                    if (isset($message['_pubKeySystemid']) === FALSE) {
                        $message['_pubKeySystemid'] = $targetid;
                    }

                    if (isset($message['_enc']['_systemid']) === FALSE) {
                        $message['_enc']['_systemid'] = $systemidKnownAs;
                    }

                    if (isset($productInfo['connection']['username']) === TRUE) {
                        if (isset($message['_enc']['matrix']) === FALSE) {
                            $message['_enc']['matrix'] = array();
                        }

                        $message['_enc']['matrix']['username'] = $productInfo['connection']['username'];
                    }

                    if (isset($productInfo['connection']['password']) === TRUE) {
                        $message['_enc']['matrix']['password'] = $productInfo['connection']['password'];
                    }
                }//end if
            }//end if
        } else {
            // Everybody else.
            $apiURL  = rtrim($apiURL, '/');
            $apiURL .= '/'.$format;
            $apiURL .= '/'.$system;
            $apiURL .= '/'.$action;
            if ($action !== 'suiteConnect') {
                // Set the encryption for all requests except suiteConnect,
                // so the target system treats this request as a superuser.
                if (isset($message['_enc']) === FALSE) {
                    $message['_enc'] = array();
                    $message['_pubKeySystemid'] = $targetid;
                }

                if (isset($message['_enc']['_systemid']) === FALSE) {
                    $message['_enc']['_systemid'] = $systemidKnownAs;
                }
            }//end if
        }//end if

        // Perform the request.
        Channels::includeSystem('API');
        $response = API::sendCURLAPIRequest(
            $apiURL,
            $message,
            $userAgentStr
        );

        return $response;

    }//end sendRequest()


    /**
     * Returns the systemid of the other system know me as.
     *
     * @param string $systemid The systemid to query.
     *
     * @return string
     */
    public static function productKnowsMeAs($systemid)
    {
        $currProd = SquizSuite::getProduct();
        $product  = SquizSuite::getProduct($systemid);
        if (empty($product) === FALSE
            && $product['knows_me_as'] !== NULL
            && $currProd['systemid'] !== $product['knows_me_as']
        ) {
            return $product['knows_me_as'];
        }

        return $currProd['systemid'];

    }//end productKnowsMeAs()


    /**
     * Return TRUE if the System is a Matrix System.
     *
     * @param string $systemid The systemid to test.
     *
     * @return boolean
     */
    private static function _isMatrixSystem($systemid)
    {
        $isMatrix = FALSE;

        $product = SquizSuite::getProduct($systemid);
        if (empty($product) === FALSE) {
            if ($product['type'] === 'Squiz Matrix') {
                // This is a Matrix system.
                $isMatrix = TRUE;
            }
        } else {
            // Unregistered system, work it out from the URL (aka systemid) instead.
            if (strpos($systemid, '__api') === FALSE) {
                $isMatrix = TRUE;
            }
        }

        return $isMatrix;

    }//end _isMatrixSystem()


    /**
     * Return the template message XML in DOM document format.
     *
     * Every suite messages are based on this template. It includes
     * systemid, system type and the URL of the system.
     *
     * @param string $targetid System ID to send the message to OR
     *                         the API URL when sendConnect message is being
     *                         sent.
     *
     * @return object
     */
    private static function _createSuiteMessageDOMTemplate($targetid)
    {
        Channels::includeSystem('API');
        $dom     = new DomDocument('1.0', 'utf-8');
        $msgElem = $dom->createElement('message');
        $dom->appendChild($msgElem);

        // Get the product information about itself.
        $currProduct   = SquizSuite::getProduct();
        $targetProduct = SquizSuite::getProduct($targetid);

        // System and Action requested.
        $sysidEl   = $dom->createElement('system_id');
        $systypeEl = $dom->createElement('system_type');
        $sysurlEl  = $dom->createElement('system_url');
        $sysNameEl = $dom->createElement('system_name');

        // Note that this system is identifying itself as the target
        // system thinks who I am!
        API::buildResponseXMLDom($sysidEl, SquizSuite::productKnowsMeAs($targetid));

        API::buildResponseXMLDom($systypeEl, $currProduct['type']);
        API::buildResponseXMLDom($sysurlEl, $currProduct['connection']['url']);
        API::buildResponseXMLDom($sysNameEl, $currProduct['connection']['name']);
        $msgElem->appendChild($sysidEl);
        $msgElem->appendChild($systypeEl);
        $msgElem->appendChild($sysurlEl);
        $msgElem->appendChild($sysNameEl);

        return $dom;

    }//end _createSuiteMessageDOMTemplate()


    /**
     * Add message specific element to the passed DOM object.
     *
     * @param object &$dom    DOM object to work with.
     * @param string $msgType Type of the message to send.
     * @param array  $options Optional variables to pass.
     *
     * @return object
     */
    private static function _createSuiteMessageDOM(
        &$dom,
        $msgType,
        array $options=array()
    ) {
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

            case 'suiteProductSync':
                $incCurrent = FALSE;
                $incDeleted = FALSE;
                $excTypes   = array('Squiz Update');
                $products   = SquizSuite::getProducts($incCurrent, $incDeleted, $excTypes);

                $productsEl = $dom->createElement('products');
                $productsEl->setAttribute('type', 'array');
                $msgEl->appendChild($productsEl);
                foreach ($products as $product) {
                    if ($product['status'] === 'live') {
                        $addSystem = TRUE;
                        if ($product['type'] === 'Squiz Matrix') {
                            $username = Util::getArrayIndex(
                                $product['connection'],
                                'username',
                                ''
                            );
                            $password = Util::getArrayIndex(
                                $product['connection'],
                                'password',
                                ''
                            );

                            if (empty($username) === TRUE
                                && empty($password) === TRUE
                            ) {
                                $addSystem = FALSE;
                            }
                        }

                        if ($addSystem === TRUE) {
                            $productEl = $dom->createElement('product');
                            API::buildResponseXMLDom($productEl, $product);
                            $productsEl->appendChild($productEl);
                        }
                    }//end if
                }//end foreach

                if (array_key_exists('sync_direction', $options) === TRUE) {
                    $syncDir = $dom->createElement('sync_direction');
                    API::buildResponseXMLDom($syncDir, $options['sync_direction']);
                    $msgEl->appendChild($syncDir);
                }
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
     * @return array
     * @throws ChannelException Error occurred.
     *
     * @api            read
     * @api-permission superuser
     */
    public static function suitePing()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            self::_parseUserAgentString();

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            $xml = self::_getDecryptedXML();

            // If XML parsing fails, it will throw an exception.
            $required = array(
                         'system_id',
                         'system_url',
                         'system_name',
                        );
            $xml      = html_entity_decode($xml);
            $message  = self::_parseSuiteXML($xml, $required);

            $systemid   = $message['system_id'];
            $systemUrl  = $message['system_url'];
            $systemName = $message['system_name'];

            // If the sender is a sync pending product,
            // make it live.
            $product = SquizSuite::getProduct($systemid);
            if (empty($product) === FALSE
                && $product['status'] === 'sync pending'
            ) {
                SquizSuite::updateProduct($systemid, 'status', 'live');
            }

            $response = 'ok';
            return $response;
        } catch (Exception $e) {
            $errMsg  = 'suiteConnect failed'."\n";
            $errMsg .= $e->getMessage()."\n";
            DAL::rollback();

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw new ChannelException($e->getMessage());
        }//end try

    }//end suitePing()


    /**
     * Suite system connection request handler.
     *
     * Other suite system will make a request to connect by sharing
     * the security certificate that includes public key information.
     * The submitter's key certificate will be registered together and
     * it will send suiteConnectAck message back as a response.
     *
     * @return array
     * @throws ChannelException Error occurred.
     *
     * @api            read
     * @api-permission public
     */
    public static function suiteConnect()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            self::_parseUserAgentString();

            $currProduct = SquizSuite::getProduct();
            if (empty($currProduct['connection']['url']) === TRUE) {
                // The current system does not have any API URL set and
                // is not ready to accept any connection request.
                $errMsg = 'The system is not ready to accept any connection request.';
                throw new ChannelException($errMsg);
            }

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            include_once 'Libs/Security/Security.inc';
            $required = array(
                         'system_id',
                         'system_url',
                         'system_name',
                         'system_type',
                         'cert',
                        );
            $xml      = Security::getRequestData('xml', NULL, FALSE, 'post');
            $xml      = html_entity_decode($xml);
            $message  = self::_parseSuiteXML($xml, $required);

            $systemid    = $message['system_id'];
            $systemUrl   = $message['system_url'];
            $systemName  = $message['system_name'];
            $systemType  = $message['system_type'];
            $certificate = $message['cert'];

            DAL::beginTransaction();

            // The systemid already exists?
            $product = SquizSuite::getProduct($systemid);
            if (empty($product) === FALSE) {
                if ($product['status'] === 'live'
                    && $product['connection']['url'] === $systemUrl
                    && $product['connection']['cert'] === $certificate
                    && $product['connection']['name'] === $systemName
                ) {
                    // Okay. The exact system already exists and it's even live.
                    // Why the suiteConnect request comes again?
                    // Try to ping the sender.
                    $pingRes = SquizSuite::sendMessage($systemid, 'suitePing');
                    if (strpos($pingRes['result'], '<?xml') !== 0) {
                        // Ping didn't even reach the sender's URL.
                        // Something went wrong.
                        $errMsg = 'The requested system is already registered.';
                        throw new ChannelException($errMsg);
                    } else {
                        if ($pingRes['curlInfo']['http_code'] === 200) {
                            if ($product['sync_status'] === 'D') {
                                // The product has been removed and the message hasn't
                                // been propagated to the all the connected systems.
                                // So it can't accept the suite request yet.
                                $errMsg = 'The requested system has been removed.';
                                throw new ChannelException($errMsg);
                            } else {
                                // The (supposed to be) sender might not be the sender.
                                // The current ping message worked fine.
                                // Do not accept this request.
                                $errMsg = 'The requested system is already registered.';
                                throw new ChannelException($errMsg);
                            }
                        }
                    }//end if
                } else {
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
                }//end if
            } else {
                $connection         = array();
                $connection['url']  = $systemUrl;
                $connection['cert'] = $certificate;
                $connection['name'] = $systemName;
                SquizSuite::registerProduct(
                    $systemid,
                    $systemType,
                    $connection,
                    $currProduct['systemid']
                );
            }//end if

            // Now we are all good. Register the system and set the status
            // as pending. Then send acknowlegement message back.
            SquizSuite::updateProduct($systemid, 'status', 'pending approval');

            DAL::commit();

            $response = 'ok';
            return $response;
        } catch (Exception $e) {
            $errMsg  = 'suiteConnect failed'."\n";
            $errMsg .= $e->getMessage()."\n";
            DAL::rollback();

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw new ChannelException($e->getMessage());
        }//end try

    }//end suiteConnect()


    /**
     * Suite system connection acknowledgment request handler.
     *
     * Once the initial connect message has been accepted by the
     * other system and the returning acknowledgment will be handled
     * here. The incoming message is encrypted with the current system's
     * public key.
     *
     * @return void
     * @throws ChannelException Error occurred.
     *
     * @api            read
     * @api-permission public
     */
    public static function suiteConnectAck()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            self::_parseUserAgentString();

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            $xml = self::_getDecryptedXML();

            $required = array(
                         'system_id',
                         'system_url',
                         'system_name',
                         'system_type',
                         'cert',
                        );
            $message  = self::_parseSuiteXML($xml, $required);

            $systemid    = $message['system_id'];
            $systemUrl   = rtrim($message['system_url'], '/');
            $systemName  = $message['system_name'];
            $systemType  = $message['system_type'];
            $certificate = $message['cert'];

            $allProducts = SquizSuite::getProducts();
            $productInfo = array();
            foreach ($allProducts as $product) {
                if ($product['status'] === 'pending'
                    && rtrim($product['connection']['url'], '/') === $systemUrl
                ) {
                    $productInfo = $product;
                    break;
                }
            }

            if (empty($productInfo) === TRUE) {
                $errMsg = 'Unknown system has sent suiteConnectAck message.';
                throw new ChannelException($errMsg);
            }

            $duplicated = SquizSuite::getProduct($systemid);
            if (empty($duplicated) === FALSE) {
                $errMsg = 'The requested system already exists.';
                throw new ChannelException($errMsg);
            }

            DAL::beginTransaction();
            // Connection Request accepted by the other system.
            // Remove the temporary product entry first.
            $knowsMeAs = $productInfo['knows_me_as'];
            SquizSuite::removeProduct($productInfo['systemid']);

            $connection = array(
                           'url'  => $systemUrl,
                           'cert' => $certificate,
                           'name' => $systemName,
                          );
            SquizSuite::registerProduct($systemid, $systemType, $connection, $knowsMeAs);
            SquizSuite::updateProduct($systemid, 'status', 'live');
            SquizSuite::updateProduct($systemid, 'sync_status', 'A');
            $response = SquizSuite::sendMessage($systemid, 'suiteConnectAckAck');
            $response = $response['result'];
            if (strpos($response, '<?xml') !== 0) {
                $errMsg = 'suiteConnectAckAck message did not reach to '.$systemUrl;
                throw new ChannelException($errMsg);
            } else {
                Channels::includeSystem('API');
                $dom = new DomDocument();
                $dom->loadXML($response);

                $responseDom = $dom->getElementsByTagName('result')->item(0);
                $response    = API::getResponseFromXMLDom($responseDom);
                if ($response !== 'ok') {
                    $errMsg = 'suiteConnectAckAck message did not reach to '.$systemUrl;
                    throw new ChannelException($errMsg);
                }
            }

            DAL::commit();
        } catch (Exception $e) {
            $errMsg  = 'suiteConnectAck failed.'."\n";
            $errMsg .= $e->getMessage()."\n";
            DAL::rollback();

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw new ChannelException($e->getMessage());
        }//end try

    }//end suiteConnectAck()


    /**
     * Suite system connection acknowlegement acknowlegement request handler.
     *
     * The final acknowlegement for suite connection request.
     *
     * @return string
     * @throws ChannelException Error occurred.
     *
     * @api            read
     * @api-permission public
     */
    public static function suiteConnectAckAck()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            self::_parseUserAgentString();

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            $xml = self::_getDecryptedXML();

            $required  = array(
                          'system_id',
                          'system_url',
                         );
            $message   = self::_parseSuiteXML($xml, $required);
            $systemid  = $message['system_id'];
            $systemUrl = $message['system_url'];

            // The final acknowledgment has been arrived.
            // Now two systems are connected.
            $productInfo = SquizSuite::getProduct($systemid);
            if (empty($productInfo) === TRUE) {
                $errMsg = 'Unknown system has been requested.';
                throw new ChannelException($errMsg);
            }

            if ($productInfo['status'] === 'pending approval') {
                SquizSuite::updateProduct($systemid, 'status', 'live');
                SquizSuite::updateProduct($systemid, 'sync_status', 'A');
            }//end if

            $response = 'ok';
            return $response;
        } catch (Exception $e) {
            $errMsg  = 'suiteConnectAckAck failed.'."\n";
            $errMsg .= $e->getMessage()."\n";

            // Re-throw exception here so that API system can send
            // the proper error HTTP header.
            throw new ChannelException($e->getMessage());
        }//end try

    }//end suiteConnectAckAck()


    /**
     * Suite product sync request handler.
     *
     * The connected system can send suiteProductSync message
     * to deliver the information about its the connected products.
     * Once it's receive, it automatically adds then as a live connected system.
     *
     * @return string
     * @throws ChannelException Error occurred.
     *
     * @api            read
     * @api-permission superuser
     */
    public static function suiteProductSync()
    {
        try {
            // Verify the User Agent string. Can we get the system type out of it?
            self::_parseUserAgentString();

            // Parse the posted XML file. Exception will be thrown in case error occurs.
            $xml = self::_getDecryptedXML();

            $required  = array(
                          'system_id',
                          'system_url',
                          'products',
                          'sync_direction',
                         );
            $message   = self::_parseSuiteXML($xml, $required);
            $systemid  = $message['system_id'];
            $systemUrl = $message['system_url'];
            $products  = $message['products'];
            foreach ($products as $product) {
                // Does this product already exist?
                $p = SquizSuite::getProduct($product['systemid']);
                if (empty($p) === FALSE) {
                    continue;
                }

                $connection = $product['connection'];
                SquizSuite::registerProduct(
                    $product['systemid'],
                    $product['type'],
                    $connection
                );

                SquizSuite::updateProduct($product['systemid'], 'status', 'live');
            }//end foreach

            // Sync Direction is 'broadcast' so send out the message
            // to all the connected systems.
            if ($message['sync_direction'] === 'broadcast') {
                $productsToSync = self::_getProductsToSync();
                foreach ($productsToSync as $product) {
                    $options  = array('sync_direction' => 'one');
                    $response = SquizSuite::sendMessage(
                        $product['systemid'],
                        'suiteProductSync',
                        $options
                    );
                }
            }

            $response = 'ok';
            return $response;
        } catch (Exception $e) {
            $errMsg  = 'suiteProductSync failed.'."\n";
            $errMsg .= $e->getMessage()."\n";
        }//end try

    }//end suiteProductSync()


    /**
     * Return the list of products to send suiteProductSync message.
     *
     * @param array $excludeSystemids Optional list of systemid to exclude.
     *
     * @return array
     */
    private static function _getProductsToSync(array $excludeSystemids=array())
    {
        $result      = array();
        $allProducts = SquizSuite::getProducts();
        foreach ($allProducts as $product) {
            if ($product['current'] === TRUE
                || in_array($product['systemid'], $excludeSystemids) === TRUE
                || $product['status'] !== 'live'
                || $product['type'] === 'Squiz Update'
            ) {
                continue;
            }

            $result[] = $product;
        }

        return $result;

    }//end _getProductsToSync()


    /**
     * Returns the suite information this system knows.
     *
     * It includes the information about itself and all the
     * systems it knows.
     *
     * @throws ChannelException Error occurred.
     *
     * @return array
     *
     * @api            read
     * @api-permission superuser
     */
    public static function suiteProductInfo()
    {
        // Verify the User Agent string. Can we get the system type out of it?
        self::_parseUserAgentString();

        // Parse the posted XML file. Exception will be thrown in case error occurs.
        $xml = self::_getDecryptedXML();

        $required   = array(
                       'system_id',
                       'system_url',
                       'system_name',
                       'system_type',
                      );
        $message    = self::_parseSuiteXML($xml, $required);
        $systemid   = $message['system_id'];
        $systemUrl  = rtrim($message['system_url'], '/');
        $systemName = $message['system_name'];
        $systemType = $message['system_type'];

        $currProd  = SquizSuite::getProduct();
        $knowsMeAs = SquizSuite::productKnowsMeAs($systemid);
        if ($currProd['systemid'] !== $knowsMeAs) {
            // The requesting system thought my systemid was different.
            // Now it knows the correct systemid as it requested my info,
            // so it's time to update.
            SquizSuite::updateProduct($systemid, 'knows_me_as', $currProd['systemid']);
        }

        $result = array(
                   'systemid'  => $currProd['systemid'],
                   'name'      => $currProd['connection']['name'],
                   'apiurl'    => $currProd['connection']['url'],
                   'cert'      => $currProd['connection']['cert'],
                   'added'     => array(),
                   'deleted'   => array(),
                   'connected' => array(),
                  );

        // Get the information about the connected systems.
        $incCurrent = FALSE;
        $incDeleted = TRUE;
        $excTypes   = array('Squiz Update');
        $products   = SquizSuite::getProducts($incCurrent, $incDeleted, $excTypes);
        foreach ($products as $p) {
            if ($p['status'] !== 'live') {
                // Only send the message about the live systems.
                continue;
            }

            if ($p['sync_status'] === 'D') {
                $result['deleted'][] = $p;
            } else {
                $result['connected'][] = $p;
            }
        }

        return $result;

    }//end suiteProductInfo()


    /**
     * Pull other suite system's information and sync itself.
     *
     * This action runs by SquizSuite cron job. It requests suite info
     * to all live connected products and update the information accordingly
     * such as systemid/name changes, new/deleted products and so on.
     *
     * @return void
     */
    public static function syncWithLiveProducts()
    {
        $liveProducts = SquizSuite::getLiveProducts();

        $incCur      = FALSE;
        $incDel      = TRUE;
        $excTypes    = array('Squiz Update');
        $allProducts = SquizSuite::getProducts($incCur, $incDel, $excTypes);

        // First collect the systems marked for deletion or addition.
        // Once it pulls from all live systems, and if everyone has
        // the same addition/deletion, then we can remove sync_status.
        // If any of the system hasn't got the message, then we will turn
        // the flag to be FALSE, and sync_status will remain.
        $markedForDeletion = array();
        $markedForAddition = array();
        foreach ($allProducts as $product) {
            if ($product['sync_status'] === 'D') {
                $markedForDeletion[$product['systemid']] = TRUE;
            }

            if ($product['sync_status'] === 'A') {
                $markedForAddition[$product['systemid']] = TRUE;
            }
        }//end foreach

        // Let's loop through each connected live systems and pull their information.
        $connectionFailed = FALSE;
        foreach ($allProducts as $product) {
            if ($product['status'] !== 'live' || $product['sync_status'] === 'D') {
                continue;
            }

            $response = SquizSuite::sendMessage($product['systemid'], 'suiteProductInfo');
            $response = self::_getSuiteMessageResult($response);
            if ($response === 'failed') {
                $connectionFailed = TRUE;

                // Send out warning email about the lost connection.
                self::_sendConnectionFailEmail($product);
                continue;
            }

            // We are passing $markedForDeletion and $markedForAddition via reference,
            // and each private function will modify the value so that we can determine
            // whether remove/add sync_status or not.
            self::_syncProductDetails($product, $response);
            self::_syncDeletedProducts(
                $response['deleted'],
                $markedForDeletion,
                $markedForAddition
            );

            self::_syncConnectedProducts(
                $product['systemid'],
                $response['connected'],
                $markedForDeletion,
                $markedForAddition
            );
        }//end foreach

        // Once tt has contacted EVERY connected systems, then it can process
        // sync_status.
        if ($connectionFailed === FALSE) {
            foreach ($markedForDeletion as $sysid => $canBeRemoved) {
                if ($canBeRemoved === TRUE) {
                    SquizSuite::removeProduct($sysid);
                }
            }

            foreach ($markedForAddition as $sysid => $canBeAdded) {
                if ($canBeAdded === TRUE) {
                    SquizSuite::updateProduct($sysid, 'sync_status', '');
                }
            }
        }//end if

    }//end syncWithLiveProducts()


    /**
     * Sync the product details like product name, URL.
     *
     * @param array $product  Product information to sync with.
     * @param array $response Response message from the product.
     *
     * @return void
     */
    private static function _syncProductDetails(array $product, array $response)
    {
        if ($product['systemid'] !== $response['systemid']) {
            // The product's systemid it returns is not the same as
            // the systemid it is using. The other system must been updated
            // its systemid. Let's update our record as well.
            SquizSuite::updateProduct($product['systemid'], 'systemid', $response['systemid']);
        }

        $updated = FALSE;
        if ($product['connection']['cert'] !== $response['cert']
            || $product['connection']['name'] !== $response['name']
        ) {
            $product['connection']['cert'] = $response['cert'];
            $product['connection']['name'] = $response['name'];
            $updated = TRUE;
        }

        if ($updated === TRUE) {
            SquizSuite::updateProduct(
                $systemid,
                'connection',
                $product['connection']
            );
        }

    }//end _syncProductDetails()


    /**
     * Sync the deleted products.
     *
     * This is a helper function to fill up $markedForDeletion and
     * $markedForAddition array for later processing.
     *
     * @param array $deleted            List of deleted products.
     * @param array &$markedForDeletion List of deleted products to process later.
     * @param array &$markedForAddition List of added products to process later.
     *
     * @return void
     */
    private static function _syncDeletedProducts(
        array $deleted,
        array &$markedForDeletion,
        array &$markedForAddition
    ) {
        // Okay. This product has deleted some other products.
        // Let's see what we can do.
        foreach ($deleted as $p) {
            $res = SquizSuite::getProduct($p['systemid']);
            if (empty($res) === TRUE) {
                // Good. I also deleted this system.
                continue;
            } else if ($res['sync_status'] === 'D') {
                // Also good. I'm going to remove this too.
                continue;
            } else if ($res['status'] === 'live') {
                // Okay. Im going to delete this.
                SquizSuite::removeProduct($p['systemid']);

                // Even though I just added, since you have deleted it
                // I trust you more.
                unset($markedForAddition[$p['systemid']]);
            }
        }

    }//end _syncDeletedProducts()


    /**
     * Sync the deleted products.
     *
     * This is a helper function to fill up $markedForDeletion and
     * $markedForAddition array for later processing.
     *
     * @param string $systemid           Current product's systemid.
     * @param array  $connected          List of connected products.
     * @param array  &$markedForDeletion List of deleted products to process later.
     * @param array  &$markedForAddition List of added products to process later.
     *
     * @return void
     */
    private static function _syncConnectedProducts(
        $systemid,
        array $connected,
        array &$markedForDeletion,
        array &$markedForAddition
    ) {
        // Okay. This product has added some other products.
        // Let's see what we can do.
        $connectedIds = array();
        foreach ($connected as $p) {
            $res = SquizSuite::getProduct($p['systemid']);
            if (empty($res) === TRUE) {
                // Hmmmm... I think I need to add this one.
                SquizSuite::registerProduct(
                    $p['systemid'],
                    $p['type'],
                    $p['connection']
                );
                SquizSuite::updateProduct($p['systemid'], 'status', 'live');
            } else if ($res['sync_status'] === 'D') {
                // You have added something, but the system is marked for
                // deletion here. I wouldn't do anything and I can't remove
                // the entry yet.
                $markedForDeletion[$p['systemid']] = FALSE;
            } else if ($res['status'] !== 'live') {
                // I have the same system, and the status is not live.
                // But you have the system! so I trust you and will set
                // my flag as live.
                SquizSuite::updateProduct($p['systemid'], 'status', 'live');
            }

            $connectedIds[] = $p['systemid'];
        }//end foreach

        // Get all systemid which have been added to this system,
        // but not existing in the other one.
        $notAddedYet = array_diff(array_keys($markedForAddition), $connectedIds);
        foreach ($notAddedYet as $notAddedId) {
            if ($systemid !== $notAddedId) {
                $markedForAddition[$notAddedId] = FALSE;
            }
        }

    }//end _syncConnectedProducts()


    /**
     * Send out the connection fail warning email.
     *
     * If the Patching system has recipient list, then it will send to them.
     * If it is not defined, then it will use send out to the super users by
     * default.
     *
     * @param array $product Product information failed to connect to.
     *
     * @return void
     */
    private static function _sendConnectionFailEmail(array $product)
    {
        $currProduct = SquizSuite::getProduct();
        $isCMS       = FALSE;
        if ($currProduct['type'] === 'Squiz CMS') {
            $isCMS = TRUE;
        }

        Channels::includeSystem('Patching');
        $config = Patching::getPatchingConfig();

        $recipientIds = array();
        if (empty($config['recipient']) === 'FALSE') {
            $recipientIds = $config['recipient'];
        } else {
            Channels::includeSystem('User');
            if ($isCMS === TRUE) {
                $recipientIds = User::getSystemAdministrators();
            } else {
                $recipientIds = User::getSuperUsers();
            }
        }

        if (empty($recipientIds) === TRUE) {
            // Nobody to send.
            return;
        }

        $emails = array();
        foreach ($recipientIds as $userid) {
            $email = User::getEmailRecipientAddress($userid);
            if (empty($email) === FALSE) {
                $emails[] = $email;
            }
        }

        if (empty($emails) === TRUE) {
            // No emails have been set for recipients.
            return;
        }

        $to        = implode(', ', $emails);
        $subject   = 'Failed to connect to '.$product['systemid'].' ('.$product['type'].').';
        $mainText  = '<p>'.$currProduct['connection']['name'].' ('.$currProduct['systemid'].')';
        $mainText .= ' failed to connect to ';
        $mainText .= $product['connection']['name'].' ('.$product['systemid'].')';
        $mainText .= ' via '.$product['connection']['url'].'</p>';

        include_once 'Libs/Web/Email.inc';
        $body = '<table width="100%">
                 <tr><td colspan="2" style="height:22px;font: 16px arial; border-bottom:1px dotted #CCC; padding-bottom:10px;">'.$subject.'</td></tr>
                 <tr>
                   <td style="font: 12px arial;padding-top:10px;padding-right:10px;" valign="top">
                  <p style="padding-top:15px;margin:0px;">'.$mainText.'</p>
                  <br />
                  </td>
                </tr>
                </table>';
        Email::sendHtmlEmail($to, $subject, $body);

    }//end _sendConnectionFailEmail()


    /**
     * Returns TRUE if product sync is scheduled for next cron run.
     *
     * @return boolean
     */
    public static function isProductSyncScheduled()
    {
        Channels::includeSystem('SystemConfig');
        $config = SystemConfig::getConfig('SquizSuite');
        return $config['check_asap'];

    }//end isProductSyncScheduled()


    /**
     * Returns TRUE if there are products to sync.
     *
     * @return boolean
     */
    public static function productsToSyncExist()
    {
        $incCur    = FALSE;
        $incDel    = FALSE;
        $excTypes  = array('Squiz Update');
        $incStatus = array('live');

        $productsToSync = SquizSuite::getProducts($incCur, $incDel, $excTypes, $incStatus);
        if (empty($productsToSync) === TRUE) {
            return FALSE;
        }

        return TRUE;

    }//end productsToSyncExist()


    /**
     * Returns _enc field from $_POST var.
     *
     * If _enc field doesn't exist in $_POST, it will
     * throw an exception.
     *
     * @return string
     * @throws ChannelException If _enc field does not exist.
     */
    private static function _getDecryptedXML()
    {
        include_once 'Libs/Security/Security.inc';
        $enc = Security::getRequestData('_enc', array(), FALSE, 'post');
        $xml = Util::getArrayIndex($enc, 'xml', array());
        if ($xml === NULL) {
            $errMsg = 'Non encrypted message received.';
            throw new ChannelException($errMsg);
        }

        $xml = html_entity_decode($xml);
        return $xml;

    }//end _getDecryptedXML()


    /**
     * Helper function to parse user agent string for suite message.
     *
     * On failure, it will throw an exception.
     *
     * @return void
     * @throws ChannelException If the user agent string parsing fails.
     */
    private static function _parseUserAgentString()
    {
        // Verify the User Agent string. Can we get the system type out of it?
        $systemType = self::_getSystemTypeFromUserAgent($_SERVER['HTTP_USER_AGENT']);
        if ($systemType === FALSE) {
            $errMsg = 'User-agent string can not be parsed.';
            throw new ChannelException($errMsg);
        }

    }//end _parseUserAgentString()


    /**
     * Helper function to get the result from suite message response string.
     *
     * If the response string is not a XML string, it will return FALSE.
     *
     * @param string $response Suite message response string.
     *
     * @return mixed
     */
    private static function _getSuiteMessageResult($response)
    {
        $result   = $response['result'];
        $curlInfo = $response['curlInfo'];
        if (strpos($result, '<?xml') !== 0) {
            return 'failed';
        } else {
            if ($curlInfo['http_code'] !== 200) {
                return 'failed';
            }

            Channels::includeSystem('API');
            $dom = new DomDocument();
            $dom->loadXML($result);

            $responseDom = $dom->getElementsByTagName('result')->item(0);
            $response    = API::getResponseFromXMLDom($responseDom);
            return $response;
        }

    }//end _getSuiteMessageResult()


    /**
     * Parse the passed XML file and return.
     *
     * @param string  $xml    SquizSuite specific XML message string.
     * @param array   $fields Optional list of fields to extract from XML.
     * @param boolean $silent When FALSE, it will throw an exception if the
     *                        fields does not exist.
     *
     * @return array
     * @throws ChannelException Invalid XML caused DomDocument() error.
     */
    private static function _parseSuiteXML(
        $xml,
        array $fields=array(),
        $silent=FALSE
    ) {
        $doc    = new DomDocument();
        $result = $doc->loadXML($xml);
        if ($result === FALSE) {
            $errMsg = 'Failed to load the message XML.';
            throw new ChannelException($errMsg);
        }

        if (empty($fields) === TRUE) {
            $fields = array(
                       'system_id',
                       'system_url',
                       'system_name',
                       'system_type',
                       'cert',
                      );
        }

        $result = array();

        Channels::includeSystem('API');
        foreach ($fields as $field) {
            $fieldEl = $doc->getElementsByTagName($field)->item(0);
            if ($fieldEl === NULL) {
                $result[$field] = NULL;
                if ($silent === FALSE) {
                    $errMsg = $field.' has not been specified.';
                    throw new ChannelException($errMsg);
                }
            } else {
                if ($fieldEl->nodeType === XML_ELEMENT_NODE) {
                    $result[$field] = API::getResponseFromXMLDom($fieldEl);
                } else {
                    $result[$field] = $fieldEl->nodeValue;
                }
            }
        }

        return $result;

    }//end _parseSuiteXML()


    /**
     * Parse the passed user agent string and return the system type.
     *
     * @param string $userAgentStr User agent string to parse.
     *
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
     * @return void
     */
    public static function decryptAPIData()
    {
        include_once 'Libs/Security/Security.inc';
        $encrypted = Security::getRequestData('_enc', NULL, FALSE, 'post');
        $password  = Security::getRequestData('_password', NULL, FALSE, 'post');
        if ($encrypted === NULL || $password === NULL) {
            return;
        }

        $encrypted = SquizSuite::decryptData($encrypted, $password);
        if ($encrypted === FALSE) {
            // @codingStandardsIgnoreStart
            $_POST['_enc'] = FALSE;
            // @codingStandardsIgnoreEnd
        } else if ($encrypted !== FALSE) {
            // @codingStandardsIgnoreStart
            $encrypted     = json_decode($encrypted, TRUE);
            $_POST['_enc'] = $encrypted;
            // @codingStandardsIgnoreEnd

            $systemid = NULL;
            if (is_array($encrypted) === TRUE) {
                $systemid = $encrypted['_systemid'];
            } else if (strpos($encrypted, '<?xml') === 0) {
                Channels::includeSystem('API');
                $dom = new DomDocument();
                $dom->loadXML($encrypted);
                $systemidDom = $dom->getElementsByTagName('system_id')->item(0);
                $systemid    = API::getResponseFromXMLDom($systemidDom);
            }

            if ($systemid !== NULL) {
                $product = SquizSuite::getProduct($systemid);
                if (empty($product) === FALSE
                    && $product['status'] === 'live'
                    && $product['sync_status'] !== 'D'
                ) {
                    // This is the API request from the connected
                    // SquizSuite system.
                    // Setting suite system user TRUE will make this
                    // API call run as system administrator.
                    self::$_suiteSystemUser = TRUE;
                }
            }
        }//end if

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


    /**
     * Returns the current product column array for SquizSuite screen.
     *
     * @return array
     */
    public static function getCurrentProductRow()
    {
        Channels::includeSystem('GUI');
        $currProduct = SquizSuite::getProduct();

        $typeC    = str_replace(' ', '', $currProduct['type']);
        $typeIcon = '<div class="SquizSuiteScreen-productIconWrap '.$typeC.'">&nbsp;</div>';

        // Name of the product.
        $name  = '<strong>'._('Name').'</strong>&nbsp;&nbsp;';
        $name .= '<span class="SquizSuiteScreen-currentPrductNameDesc">('._('this name will appear in all connected products').')</span><br />';
        $name .= GUI::createWidget(
            'squizSuite-currProductName',
            'GUITextBox',
            array(
             'value' => $currProduct['connection']['name'],
             'size'  => 50,
            )
        );

        // URL of the product.
        $url  = '<strong>'._('URL').'</strong>&nbsp;&nbsp;';
        $url .= '<span class="SquizSuiteScreen-currentPrductNameDesc">('._('this URL is required to add a new product').')</span><br />';
        $url .= GUI::createWidget(
            'squizSuite-currProductURL',
            'GUITextBox',
            array(
             'value' => $currProduct['connection']['url'],
             'size'  => 50,
            )
        );

        // Product summary information.
        $summary        = '<strong>'._('Summary').'</strong><br /><div class="SquizSuiteScreen-summaryWrap">';
        $productSummary = SquizSuite::getProductSummary();
        foreach ($productSummary as $field) {
            $summary .= '<span class="SquizSuiteScreen-summaryLabel">'._($field['label']).'</span>';
            $summary .= '<span class="SquizSuiteScreen-summaryValue">'._($field['value']).'</span>';
        }

        $summary .= '</div>';

        $rows   = array();
        $rows[] = array(
                   'currTypeIcon'   => $typeIcon,
                   'currName'       => $name.$url,
                   'currStatusIcon' => '&nbsp;',
                   'currSummary'    => $summary,
                  );

        return $rows;

    }//end getCurrentProductRow()


    /**
     * Returns the connected products rows array for SquizSuite screen.
     *
     * @return array
     */
    public static function getConnectedProductRows()
    {
        Channels::includeSystem('GUI');
        $rows = array();

        $count       = 0;
        $incCurrent  = FALSE;
        $incDeleted  = FALSE;
        $allProducts = SquizSuite::getProducts($incCurrent, $incDeleted);
        foreach ($allProducts as $product) {
            $systemid = $product['systemid'];

            $productName = _('Unknown').' ('.$product['systemid'].')';
            if (array_key_exists('name', $product['connection']) === TRUE) {
                $productName = $product['connection']['name'];
            }

            $productUrl = '';
            if (array_key_exists('url', $product['connection']) === TRUE) {
                $productUrl = $product['connection']['url'];
            }

            // Product Type Icon.
            $typeC    = str_replace(' ', '', $product['type']);
            $typeIcon = '<div class="SquizSuiteScreen-productIconWrap '.$typeC.'">&nbsp;</div>';

            // Product Name.
            if ($product['status'] === 'pending approval' || $product['status'] === 'pending') {
                $name  = _('Unknown').' ('.$product['systemid'].')';
                $name .= '<span class="SquizSuiteScreen-productExpander" rowid="'.$count.'">&nbsp;</span>';
            } else {
                $name  = $productName;
                $name .= '<span class="SquizSuiteScreen-productExpander" rowid="'.$count.'">&nbsp;</span>';
            }

            // Product Summary Status and Summary.
            $statucC = 'live';
            $summary = '<div id="squizSuite-'.$systemid.'-summary">';
            switch ($product['status']) {
                case 'live':
                    $summary .= '<span class="SquizSuiteScreen-pendingApprovalMsg">'._('Loading ...').'</span><br /><div class="SquizSuiteScreen-summaryWrap"></div>';
                    $statusC  = 'loading';
                break;

                case 'pending':
                    $summary .= '<span class="SquizSuiteScreen-pendingApprovalMsg">'._('Connection Pending ...').'</span>';
                    $statusC  = 'pending';
                break;

                case 'pending approval':
                    $setting  = array(
                                 'value'       => 'Approve Connection',
                                 'click'       => 'SquizSuiteSquizSuiteScreen.approveConnection(this)',
                                 'customClass' => 'approveConnection',
                                );
                    $summary .= '<span class="SquizSuiteScreen-pendingApprovalMsg">'._('Connection Pending ...').'</span>';
                    $summary .= GUI::createWidget('squizSuite-'.$product['systemid'].'-approve-btn', 'GUIButton', $setting);
                    $statusC  = 'pending';
                break;

                default:
                    // No default status.
                    $statusC = 'pending';
                break;
            }//end switch

            $statusIcon = '<div id="squizSuite-'.$systemid.'-statusWrap" class="SquizSuiteScreen-statusWrap '.$statusC.'">&nbsp;</div>';
            $summary   .= '</div>';

            $deleteIcon = '<div class="SquizSuiteScreen-deleteColBtn" systemid="'.$systemid.'">&nbsp;</div>';

            // Product Row.
            $rows[] = array(
                       'productTypeIcon'   => $typeIcon,
                       'productName'       => $name,
                       'productStatusIcon' => $statusIcon,
                       'productSummary'    => $summary,
                       'delIcon'           => $deleteIcon,
                      );

            // Product Details Row.
            $setting = array(
                        'value'       => $productUrl,
                        'size'        => 40,
                        'customClass' => 'apiURL',
                       );

            if (strpos($product['status'], 'pending') === 0) {
                $setting['readonly'] = TRUE;
            }

            $colContent  = '<div class="SquizSuiteScreen-productDetailWrap">';
            $colContent .= '<strong>'._('API URL').'</strong><br />';
            $colContent .= GUI::createWidget('squizSuite-'.$product['systemid'].'-apiURL', 'GUITextBox', $setting);
            $colContent .= '</div>';
            if ($product['type'] === 'Squiz Matrix') {
                $username = '';
                if (array_key_exists('username', $product['connection']) === TRUE) {
                    $username = $product['connection']['username'];
                }

                $password = '';
                if (array_key_exists('password', $product['connection']) === TRUE) {
                    $password = $product['connection']['password'];
                }

                // Add an extra username/password for Matrix connections.
                $usernameSetting = array(
                                    'value' => $username,
                                    'size'  => 15,
                                   );
                $passwordSetting = array(
                                    'value' => $password,
                                    'size'  => 15,
                                    'type'  => 'password',
                                   );

                $colContent .= '<div class="SquizSuiteScreen-productDetailWrap">';
                $colContent .= '<strong>'._('Username').'</strong><br />';
                $colContent .= GUI::createWidget('squizSuite-'.$product['systemid'].'-username', 'GUITextBox', $usernameSetting);
                $colContent .= '</div>';
                $colContent .= '<div class="SquizSuiteScreen-productDetailWrap">';
                $colContent .= '<strong>'._('Password').'</strong><br />';
                $colContent .= GUI::createWidget('squizSuite-'.$product['systemid'].'-password', 'GUITextBox', $passwordSetting);
                $colContent .= '</div>';
            }//end if

            $rows[] = array(
                       'productTypeIcon' => array(
                                             'colspan'    => 5,
                                             'colContent' => $colContent,
                                            ),
                      );

            $count += 2;
        }//end foreach

        $rows[] = array(
                   'productTypeIcon' => array(
                                         'colspan'    => 5,
                                         'colContent' => self::_getNewProductRowContent(),
                                        ),
                  );
        return $rows;

    }//end getConnectedProductRows()


    /**
     * Returns the HTML code to create a new product row.
     *
     * @return string
     */
    private static function _getNewProductRowContent()
    {
        Channels::includeSystem('GUI');

        // New product field.
        $typeIcon         = '<div class="SquizSuiteScreen-productIconWrap unknown">&nbsp;</div>';
        $newProductLabel  = 'New product connection - ';
        $newProductLabel .= '<span class="SquizSuiteScreen-newProductHeadDesc">';
        $newProductLabel .= 'Name and summary information will be available after the initial connection is made</span>';
        $newProductLabel .= '</span>';

        $setting = array(
                    'allowDelete' => FALSE,
                    'hideHeader'  => TRUE,
                    'columns'     => array(
                                      'currTypeIcon' => array(
                                                         'name'  => _('Type'),
                                                         'align' => 'left',
                                                         'width' => '60px',
                                                        ),
                                      'currName'     => array(
                                                         'name'  => _('Name'),
                                                         'align' => 'left',
                                                        ),
                                     ),
                   );

        $setting['rows'][] = array(
                              'currTypeIcon' => $typeIcon,
                              'currName'     => $newProductLabel,
                             );

        $textSetting = array(
                        'value' => '',
                        'size'  => 45,
                       );
        $colContent  = '<div class="SquizSuiteScreen-productDetailWrap">';
        $colContent .= '<strong>API URL</strong><br />';
        $colContent .= GUI::createWidget('squizSuite-new-apiURL', 'GUITextBox', $textSetting);
        $colContent .= '</div>';

        $setting['rows'][] = array(
                              'currTypeIcon' => array(
                                                 'colspan'    => 3,
                                                 'colContent' => $colContent,
                                                ),
                             );

        $newTable = GUI::createWidget('SquizSuiteScreen-newProductTable', 'GUITable', $setting);
        return $newTable;

    }//end _getNewProductRowContent()


    /**
     * Returns the JS code to initialise the Roles Screen.
     *
     * @return string
     */
    public static function getSquizSuiteScreenInitCode()
    {
        $products = SquizSuite::getProducts();
        $data     = array(
                     'currentProduct'    => array(),
                     'connectedProducts' => array(),
                     'refreshScheduled'  => SquizSuite::isProductSyncScheduled(),
                    );

        foreach ($products as $p) {
            if ($p['current'] === TRUE) {
                $data['currentProduct'] = $p;
            } else {
                $data['connectedProducts'][] = $p;
            }
        }

        $data = json_encode($data);
        return '<script>SquizSuiteSquizSuiteScreen.initScreen('.$data.');</script>';

    }//end getSquizSuiteScreenInitCode()


    /**
     * Save the SuperUsers screen settings.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     * @throws ChannelException If the user in not a super user.
     */
    public static function saveSquizSuiteScreen(array $data)
    {
        Channels::includeSystem('User');
        if (User::isSystemAdministrator() === FALSE) {
            throw new ChannelException(_('You must be a super user to access this screen'));
        }

        include_once 'Libs/Util/Util.inc';
        $retVal = array();
        $type   = Util::getArrayIndex($data, 'type', NULL);
        switch ($type) {
            // Get the live product summary.
            case 'getProductSummary':
                $retVal = self::_saveGetProductSummary($data);
                if (is_string($retVal) === TRUE) {
                    $retVal = array('errors' => $retVal);
                }
            break;

            // Approve the pending approval system.
            case 'approveConnection':
                $retVal = self::_saveApproveConnection($data);
                if (is_string($retVal) === TRUE) {
                    $retVal = array('errors' => $retVal);
                }
            break;

            // Refresh product status ASAP.
            case 'refreshNow':
                $schedule = Util::getArrayIndex($data, 'schedule', TRUE);
                if ($schedule === FALSE) {
                    // Do not even use the cron and update every information right now.
                    SquizSuite::syncWithLiveProducts();
                } else {
                    // Schedule the next cron run to update product info.
                    Channels::includeSystem('SystemConfig');
                    $config = SystemConfig::getConfig('SquizSuite');
                    if ($config['check_asap'] === FALSE) {
                        $config['check_asap'] = TRUE;
                        SystemConfig::setConfig('SquizSuite', $config);
                    }
                }

                $retVal = TRUE;
            break;

            // Save the connected system form fields.
            default:
                $retVal = self::_saveSquizSuiteScreen($data);
                if (is_string($retVal) === TRUE) {
                    $retVal = array('errors' => $retVal);
                }
            break;
        }//end switch

        if (isset($retVal['errors']) === FALSE) {
            $retVal = array('success' => $retVal);
        }

        return $retVal;

    }//end saveSquizSuiteScreen()


    /**
     * Save the SuperUsers screen settings.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _saveSquizSuiteScreen(array $data)
    {
        include_once 'Libs/Util/Util.inc';
        $retVal = TRUE;

        try {
            // Current product name.
            $currProductName = Util::getArrayIndex($data, 'squizSuite-currProductName', NULL);
            if ($currProductName !== NULL) {
                $nameUpdated = FALSE;
                $currProduct = SquizSuite::getProduct();
                if ($currProduct['connection']['name'] !== $currProductName) {
                    $nameUpdated        = TRUE;
                    $connection         = $currProduct['connection'];
                    $connection['name'] = $currProductName;
                    SquizSuite::updateProduct($currProduct['systemid'], 'connection', $connection);
                }
            }

            // Current product URL.
            $currProductURL = Util::getArrayIndex($data, 'squizSuite-currProductURL', '');
            if ($currProductURL !== NULL) {
                $newURL      = '';
                $oldURL      = '';
                $urlUpdated  = FALSE;
                $currProduct = SquizSuite::getProduct();

                Channels::includeSystem('API');
                $apiSuffix      = API::getAPISuffix();
                $currProductURL = rtrim($currProductURL, '/');
                if (empty($currProductURL) === FALSE
                    && strpos($currProductURL, $apiSuffix) !== (strlen($currProductURL) - 5)
                ) {
                    $currProductURL .= '/'.$apiSuffix;
                }

                if ($currProduct['connection']['url'] !== $currProductURL) {
                    $urlUpdated        = TRUE;
                    $connection        = $currProduct['connection'];
                    $oldURL            = $connection['url'];
                    $connection['url'] = $currProductURL;
                    $newURL            = $connection['url'];
                    SquizSuite::updateProduct($currProduct['systemid'], 'connection', $connection);
                }
            }//end if

            // Delete the connected product.
            self::_saveDeleteProduct($data);

            $includeCurrent = FALSE;
            $products       = SquizSuite::getProducts($includeCurrent);
            foreach ($products as $product) {
                $systemid = $product['systemid'];
                if ($product['status'] === 'live') {
                    $updated    = FALSE;
                    $apiURL     = Util::getArrayIndex($data, 'squizSuite-'.$systemid.'-apiURL', NULL);
                    $connection = $product['connection'];
                    if ($apiURL !== NULL
                        && $connection['url'] !== $apiURL
                    ) {
                        $updated           = TRUE;
                        $connection['url'] = $apiURL;
                    }

                    if ($updated === TRUE) {
                        SquizSuite::updateProduct($systemid, 'connection', $connection);
                    }
                }//end if

                // Save Usernames and Passwords for Matrix products.
                if ($product['type'] === 'Squiz Matrix') {
                    // Is Matrix continue...
                    $systemid     = $product['systemid'];
                    $upConnection = $product['connection'];
                    $newUsername  = Util::getArrayIndex($data, 'squizSuite-'.$systemid.'-username', NULL);
                    $newPassword  = Util::getArrayIndex($data, 'squizSuite-'.$systemid.'-password', NULL);
                    if ($newUsername !== NULL
                        && (array_key_exists('username', $product['connection']) === FALSE
                        || $newUsername !== $product['connection']['username'])
                    ) {
                        $upConnection['username'] = $newUsername;
                        SquizSuite::updateProduct($systemid, 'connection', $upConnection);
                    }

                    if ($newPassword !== NULL
                        && (array_key_exists('password', $product['connection']) === FALSE
                        || $newPassword !== $product['connection']['password'])
                    ) {
                        $upConnection['password'] = $newPassword;
                        SquizSuite::updateProduct($systemid, 'connection', $upConnection);
                    }
                }//end if
            }//end foreach

            // Add a new product.
            self::_saveAddNewProduct($data);
        } catch (Exception $e) {
            $retVal = $e->getMessage();
        }//end try

        return $retVal;

    }//end _saveSquizSuiteScreen()


    /**
     * Helper function to process deleting a connected product.
     *
     * @param array $data Submitted data to process.
     *
     * @return void
     */
    private static function _saveDeleteProduct(array $data)
    {
        // Deleted Product Connection.
        // Are there any live products left after the deletion?
        // If so, mark the sync_status as 'D' for syncing. If there is
        // no live products left to sync, just remove the products.
        $templateData    = Util::getArrayIndex($data, 'templateData', array());
        $deletedProducts = Util::getArrayIndex($templateData, 'deletedProducts', array());

        $deletedProductids = array();
        foreach ($deletedProducts as $systemid => $delete) {
            if ($delete === TRUE) {
                $deletedProductids[] = $systemid;
            }
        }

        $incCurrent  = FALSE;
        $incDeleted  = FALSE;
        $excTypes    = array('Squiz Update');
        $incStatus   = array('live');
        $liveProdIds = array();
        $result      = SquizSuite::getProducts($incCurrent, $incDeleted, $excTypes, $incStatus);
        foreach ($result as $p) {
            $liveProdIds[] = $p['systemid'];
        }

        $updateSyncStatus    = TRUE;
        $productsToSyncAfter = array_diff($liveProdIds, $deletedProductids);
        if (empty($productsToSyncAfter) === TRUE) {
            $updateSyncStatus = FALSE;
        }

        foreach ($deletedProducts as $systemid => $delete) {
            if ($delete === TRUE) {
                if ($updateSyncStatus === TRUE) {
                    SquizSuite::updateProduct($systemid, 'sync_status', 'D');
                } else {
                    SquizSuite::removeProduct($systemid);
                }
            }
        }

    }//end _saveDeleteProduct()


    /**
     * Helper function to process saving a new product.
     *
     * @param array $data Submitted data to process.
     *
     * @return mixed
     * @throws ChannelException Occurs when the processing failed.
     */
    private static function _saveAddNewProduct(array $data)
    {
        // New Product.
        include_once 'Libs/Security/Security.inc';
        $newProductURL = Security::getRequestData('squizSuite-new-apiURL', '', TRUE, $data);
        $newProductURL = trim($newProductURL);
        if (empty($newProductURL) === FALSE) {
            Channels::includeSystem('API');
            $apiSuffix = API::getAPISuffix();

            $url = trim($newProductURL);
            $url = rtrim($url, '/');
            if (strpos($url, $apiSuffix) !== (strlen($url) - 5)) {
                $url .= '/'.$apiSuffix;
            }

            if (strpos($url, 'http://') !== 0 && strpos($url, 'https://') !== 0) {
                $url = 'http://'.$url;
            }

            // Check whether Update Product already exists.
            // If the update server already exists, do not add the second one.
            $productType     = '';
            $getProdTypeURL  = $url;
            $getProdTypeURL .= '/json/SquizSuite/getProductType';
            $response        = API::sendCURLAPIRequest($getProdTypeURL, array());
            if ($response['curlInfo']['http_code'] === 200) {
                $productType = json_decode($response['result'], TRUE);
                $productType = $productType['result'];
                if ($productType === 'Squiz Update') {
                    // It is a Squiz Update system.
                    $updateProducts = SquizSuite::getProductsByType('Squiz Update');
                    if (empty($updateProducts) === FALSE) {
                        $errMsg = _('Update Product already exists');
                        throw new ChannelException($errMsg);
                    }
                }
            }

            $incCurrent = FALSE;
            $incDeleted = TRUE;
            $excTypes   = array('Squiz Update');
            $products   = SquizSuite::getProducts($incCurrent, $incDeleted, $excTypes);
            foreach ($products as $product) {
                if ($product['connection']['url'] === $url) {
                    if ($product['sync_status'] === 'D') {
                        $errMsg = _('The product is marked for deletion. Please wait until the live connected products sync.');
                        throw new ChannelException($errMsg);
                    } else {
                        $errMsg = _('Product with the same URL already exists.');
                        throw new ChannelException($errMsg);
                    }
                }
            }

            $currProduct   = SquizSuite::getProduct();
            $currTypeDefID = SquizSuite::getDefaultIDForType(SquizSuite::getProductType());
            if ($currProduct['systemid'] === $currTypeDefID
                && $productType === 'Squiz Update'
            ) {
                // This is not an activated system and tries to connect to
                // the update server.
                SquizSuite::addUpdateServerForActivation($url);
                return;
            }

            $response   = SquizSuite::sendMessage($url, 'suiteConnect');
            $matrixTest = FALSE;
            if ($response['curlInfo']['http_code'] === 404) {
                // This may be a Matrix connection if a 404 is received, try again.
                $url        = substr($url, 0, -6);
                $response   = SquizSuite::sendMessage($url, 'suiteConnect');
                $matrixTest = TRUE;
            }

            Channels::includeSystem('API');
            if ($response['curlInfo']['http_code'] !== 200) {
                $retVal   = _($response['curlInfo']['http_code'].' response returned.');
                $response = $response['result'];
                if (strpos($response, '<?xml') === 0) {
                    $dom = new DomDocument();
                    $dom->loadXML($response);
                    $exceptionDom = $dom->getElementsByTagName('exception')->item(0);
                    $exception    = API::getResponseFromXMLDom($exceptionDom);
                    $retVal      .= ' '.$exception;
                    throw new ChannelException($retVal);
                }
            } else {
                $response = $response['result'];
                if (strpos($response, '<?xml') !== 0) {
                    $retVal = _('Non XML response returned.');
                    throw new ChannelException($retVal);
                } else {
                    $dom = new DomDocument();
                    $dom->loadXML($response);

                    $responseDom = $dom->getElementsByTagName('result')->item(0);
                    $response    = API::getResponseFromXMLDom($responseDom);
                    if ($response === 'ok') {
                        $tempid = uniqid();
                        if ($matrixTest === TRUE) {
                            $systemType = 'Squiz Matrix';
                        } else {
                            $systemType = $productType;
                        }

                        $connection = array('url' => $url);
                        SquizSuite::registerProduct(
                            $tempid,
                            $systemType,
                            $connection,
                            $currProduct['systemid']
                        );
                        SquizSuite::updateProduct($tempid, 'status', 'pending');
                    }
                }//end if
            }//end if
        }//end if

    }//end _saveAddNewProduct()


    /**
     * Returns the given product summary.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _saveGetProductSummary(array $data)
    {
        include_once 'Libs/Util/Util.inc';
        $retVal   = TRUE;
        $systemid = Util::getArrayIndex($data, 'systemid', NULL);

        $currProduct = SquizSuite::getProduct();
        $product     = SquizSuite::getProduct($systemid);
        if (empty($product['connection']) === FALSE) {
            $msg      = array('_pubKeySystemid' => $systemid);
            $response = SquizSuite::sendRequest(
                $systemid,
                'json',
                'SquizSuite',
                'getProductSummary',
                $msg
            );

            if ($response['curlInfo']['http_code'] === 200) {
                $response = json_decode($response['result'], TRUE);
                $retVal   = $response['result'];
            } else {
                $retVal = $response['curlInfo']['http_code'].' response returned.';
            }
        }//end if

        return $retVal;

    }//end _saveGetProductSummary()


    /**
     * Approve the pending approval product connection.
     *
     * @param array $data Settings to save.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _saveApproveConnection(array $data)
    {
        include_once 'Libs/Util/Util.inc';
        $retVal   = TRUE;
        $systemid = Util::getArrayIndex($data, 'systemid', NULL);
        if ($systemid !== NULL) {
            $response = SquizSuite::sendMessage($systemid, 'suiteConnectAck');
            if ($response['curlInfo']['http_code'] !== 200) {
                $dom = new DomDocument();
                $dom->loadXML($response['result']);
                $exceptionDom = $dom->getElementsByTagName('exception')->item(0);
                if (isset($exceptionDom) === TRUE) {
                    $retVal = $exceptionDom->nodeValue;
                }
            } else {
                // Sending suiteConnectAck message was successful, which means now
                // the other system know my real systemid.
                // I can safely update its knows_me_as field with my real systemid.
                $currProduct = SquizSuite::getProduct();
                SquizSuite::updateProduct($systemid, 'knows_me_as', $currProduct['systemid']);
            }
        }//end if

        return $retVal;

    }//end _saveApproveConnection()


}//end class

?>
