<?php
/**
 * Library functions.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Network
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Web/Web.inc';

/**
 * Library functions for authenticating with the OAuth 1.0 protocol.
 *
 * Terminology in this library conforms to IETF RFC 5849, the currently prevailing
 * standard for OAuth 1.0 at the time of writing.
 *
 * Previous community versions of the standard, such as the superceded "OAuth Core
 * 1.0a", used different terminology for some terms (for instance, reference to
 * "consumer/provider" instead of the RFC's "client/server"). Please refer to section
 * 1.1 of RFC 5849 for further details. Some old terms (notably "consumer key")
 * remain in the auth headers, for backward compatibility with the previous community
 * standard.
 *
 * The process to get an access token:
 * 1. Gain access to a set of "client credentials", consisting of an identifier and
 *    shared secret. (Previously "consumer key" and "consumer secret".) This needs to
 *    be done before any calls are made. Some services offer anonymous client
 *    credentials, others *must* to be registered beforehand. (Example of former:
 *    Google Data Protocol. Example of latter: Twitter's API.)
 * 2. Request a set of "temporary credentials" from the server. (Previously the
 *    "request token".) Use getTemporaryCredentials() for this. Along with your
 *    client credentials, you also pass a callback URL at this stage, which is where
 *    the authorisation page will redirect back to if/when approved.
 * 3. Request authorisation from the resource owner. Call getResourceOwnerAuth()
 *    with the temporary credentials identifier, which will REDIRECT you to the
 *    service's authorisation page.
 * 4. If approved, a token and verifier will be returned as GET variables to your
 *    callback page. Use getToken() to get your access token. You will need your
 *    client credentials, temporary credentials, and the returned verifier value.
 *
 * The process of getting temporary credentials, and the token, as well as all
 * PLAINTEXT-"signed" requests, MUST be made through a secure connection (eg. HTTPS)
 * according to RFC 5849. (In the older standard, this was merely a recommendation.)
 * This is due to authentication details (such as tokens) being passed in clear text.
 *
 * If authorisation is denied, there is no requirement to let the requesting client
 * know. Indeed, many services will not redirect automatically at all, on the premise
 * that you should not return a user to a non-trustworthy client site - although most
 * will at least provide a manual link. Some services (eg. Twitter API) will also
 * return information about the token denied, but this is not required, and probably
 * rare.
 *
 * The process of revoking a token programmatically is not defined by the OAuth
 * standard. It is usually left up to the resource owner to revoke access. Sometimes,
 * a method of revoking a token will be provided (Google Data does this, overlaying
 * its AuthSub "revoke token" endpoint to accept OAuth token requests). These can
 * likely be called as normal doRequest() calls using the token to revoke.
 *
 * @since 4.0.0
 */
class OAuth
{


    /**
     * Get the temporary credentials (previously known as a "request token").
     *
     * @param string $endpointUrl    The REST endpoint that issues the temporary
     *                               credentials.
     * @param string $clientKey      The "consumer key" part of client credentials.
     * @param string $clientSecret   The shared secret part of client credentials.
     * @param string $callbackUrl    The URL to return to after the resource owner
     *                               approves the client.
     * @param array  $extraAuthItems Extra header items to go into the Authorization
     *                               header.
     * @param string $signMethod     The method used to sign the request. Understands
     *                               "PLAINTEXT" and "HMAC-SHA1".
     *
     * @since  4.0.0
     * @return array
     */
    public static function getTemporaryCredentials(
        $endpointUrl,
        $clientKey,
        $clientSecret,
        $callbackUrl,
        array $extraAuthItems=array(),
        $signMethod='HMAC-SHA1'
    ) {
        // Add callback to the Authorization header first, then call the request.
        $extraAuthItems['oauth_callback'] = $callbackUrl;

        $result = self::doRequest(
            $endpointUrl,
            $clientKey,
            $clientSecret,
            '',
            '',
            'POST',
            $extraAuthItems,
            array(),
            $signMethod
        );

        // Was the request good? If so, break out the result so the temporary
        // credentials can be more easily accessed.
        if ($result['status'] === 200) {
            parse_str($result['output'], $returnedToken);
            $result['tempToken']  = $returnedToken['oauth_token'];
            $result['tempSecret'] = $returnedToken['oauth_token_secret'];
        }

        return $result;

    }//end getTemporaryCredentials()


    /**
     * Seek authorisation from the resource owner by redirection.
     *
     * @param string $endpointUrl The URL to redirect to authorise the token.
     * @param string $tempToken   The identifier part of the temporary credentials.
     *
     * @since  4.0.0
     * @return void
     */
    public static function getResourceOwnerAuth($endpointUrl, $tempToken)
    {
        $locationHeader  = 'Location: '.$endpointUrl;
        $locationHeader .= '?oauth_token='.$tempToken;
        header($locationHeader);

    }//end getResourceOwnerAuth()


    /**
     * Get the access token.
     *
     * @param string $endpointUrl  The endpoint URL of the "get token" web service.
     * @param string $clientKey    The "consumer key" part of client credentials.
     * @param string $clientSecret The shared secret part of client credentials.
     * @param string $tempToken    The identifier of the temporary credentials.
     * @param string $tempSecret   The shared secret of the temporary credentials.
     * @param string $verifier     The verifier provided from the auth redirect.
     * @param string $signMethod   The method used to sign the request. Understands
     *                             "PLAINTEXT" and "HMAC-SHA1".
     *
     * @since  4.0.0
     * @return array
     */
    public static function getToken(
        $endpointUrl,
        $clientKey,
        $clientSecret,
        $tempToken,
        $tempSecret,
        $verifier,
        $signMethod='HMAC-SHA1'
    ) {
        // Add the verifier to the Authorization header.
        $extraAuthItems = array();
        $extraAuthItems['oauth_verifier'] = $verifier;

        $result = self::doRequest(
            $endpointUrl,
            $clientKey,
            $clientSecret,
            $tempToken,
            $tempSecret,
            'POST',
            $extraAuthItems,
            array(),
            $signMethod
        );

        // Was the request good? If so, break out the result so the token
        // credentials can be more easily accessed.
        if ($result['status'] === 200) {
            parse_str($result['output'], $returnedToken);
            $result['token']       = $returnedToken['oauth_token'];
            $result['tokenSecret'] = $returnedToken['oauth_token_secret'];
        }

        return $result;

    }//end getToken()


    /**
     * Do a signed OAuth request.
     *
     * A timestamp value will be generated to add to the request. Some servers (eg.
     * Google Data protocol) will reject a timestamp value from the client if it is
     * too different from the server's time. Ensure time is synchronised using eg.
     * NTP if relying on this. Also, be aware some older server implementations
     * require that timestamps be equal or greater than previous timestamps (no
     * longer required by RFC 5849).
     *
     * @param string $endpointUrl      The endpoint URL we are accessing.
     * @param string $clientKey        The "consumer key" part of client
     *                                 credentials.
     * @param string $clientSecret     The shared secret part of client credentials.
     * @param string $token            The access token identifier.
     * @param string $tokenSecret      The access token shared secret.
     * @param string $method           The method used to call the request.
     * @param array  $authHeaderParams Extra header items to go into the
     *                                 "Authorization" header.
     * @param array  $postFields       If POST, fields to be posted to the request.
     * @param string $signMethod       The method used to sign the request.
     *                                 Understands "PLAINTEXT" and "HMAC-SHA1".
     *
     * @since  4.0.0
     * @return array
     */
    public static function doRequest(
        $endpointUrl,
        $clientKey,
        $clientSecret,
        $token,
        $tokenSecret,
        $method='GET',
        array $authHeaderParams=array(),
        array $postFields=array(),
        $signMethod='HMAC-SHA1'
    ) {
        /*
            The getQueryStringParams() function uses parse_str() to decode the query
            string, and this automatically decodes the params, so this is OK to do.

            We are dumping the parameters into an array with key and value on an
            equal level. This is because duplicate keys in the base string are
            allowed where they exist more than once (eg. in both the GET and POST
            data).
        */

        foreach (Web::getQueryStringParams($endpointUrl) as $key => $value) {
            $baseStringParams[] = array(
                                   $key,
                                   $value,
                                  );
        }

        foreach ($postFields as $key => $value) {
            $baseStringParams[] = array(
                                   $key,
                                   $value,
                                  );
        }

        // Strip the query string to get the base URL, for the base string.
        $baseUrl = Web::stripUrlQueryString($endpointUrl);

        // Authentication header parameters.
        $authHeaderParams['oauth_consumer_key']     = $clientKey;
        $authHeaderParams['oauth_token']            = $token;
        $authHeaderParams['oauth_signature_method'] = strtoupper($signMethod);
        $authHeaderParams['oauth_version']          = '1.0';
        $authHeaderParams['oauth_nonce']            = self::generateNonce();
        $authHeaderParams['oauth_timestamp']        = time();

        foreach ($authHeaderParams as $key => $value) {
            // Realm part of Authorization header is excluded [RFC sect. 3.4.1.3.1].
            if ($key !== 'realm') {
                $baseStringParams[] = array(
                                       $key,
                                       $value,
                                      );
            }
        }

        // Build the base string, and the request's signature.
        $baseString = self::getBaseString($baseUrl, $method, $baseStringParams);
        $signature  = self::getSignature(
            $baseString,
            $clientSecret,
            $tokenSecret,
            $signMethod
        );

        $authHeaderParams['oauth_signature'] = $signature;

        // Build the "Authorization" header.
        $authString = array();

        foreach ($authHeaderParams as $key => $value) {
            $newAuthString  = self::encodeValue($key);
            $newAuthString .= '="'.self::encodeValue($value).'"';
            $authString[]   = $newAuthString;
        }

        $authString = implode(', ', $authString);
        $authString = 'Authorization: OAuth '.$authString;

        // Start building the CURL request.
        $curl = curl_init($endpointUrl);

        if ($method === 'POST') {
            curl_setopt($curl, CURLOPT_POST, 1);
            curl_setopt($curl, CURLOPT_POSTFIELDS, $postFields);
        }

        curl_setopt($curl, CURLOPT_HTTPHEADER, array($authString));
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
        $output = curl_exec($curl);

        $curlinfo = curl_getinfo($curl);

        $retval = array(
                   'status' => (int) $curlinfo['http_code'],
                   'output' => $output,
                  );

        return $retval;

    }//end doRequest()


    /**
     * Build a normalised base string for signing OAuth requests with.
     *
     * The base string is defined as:
     * - the uppercased method of the request (eg. GET, POST),
     * - the base URL of the request (excluding query string), including port if it
     *   is non-standard (ie. not port 80 if HTTP, port 443 if HTTPS),
     * - all parameters passed to the request. See also normaliseParams() for
     *   details of sources.
     *
     * These are each urlencoded, and concatenated (in this order) with a "&".
     * This may result in double-encoded values since parameters are urlencoded in
     * the normalisation process - this is normal.
     *
     * @param string $baseUrl Base URL of request.
     * @param string $method  HTTP request method.
     * @param array  $params  Full parameter list (to be normalised).
     *
     * @since  4.0.0
     * @return string
     */
    public static function getBaseString(
        $baseUrl,
        $method,
        array $params=array()
    ) {
        $baseString  = self::encodeValue(strtoupper($method)).'&';
        $baseString .= self::encodeValue($baseUrl).'&';
        $baseString .= self::encodeValue(self::normaliseParams($params));

        return $baseString;

    }//end getBaseString()


    /**
     * Return passed request parameters in normalised order.
     *
     * For creating the base string for signing, the standard requires that the
     * request parameters be placed in a particular order.
     *
     * Parameters are to be sourced from:
     * - the query string,
     * - fields to be placed into the "Authorization" header,
     * - post fields if the body is of the correct format;
     * Except that the "oauth_signature" and "realm" parameters of the Authorization
     * header needs to be excluded.
     * Values to be urldecoded where necessary before normalising.
     *
     * Parameters are then handled in the following order:
     * - Encode name and value according to RFC 3986.
     * - Sort by encoded name, then by encoded value where necessary. Sorting is by
     *     byte order.
     * - Then place '=' sign between name and value (including empty values).
     * - Implode the parameters using '&' to separate them.
     *
     * Note, when being put back into the base string, the whole request string needs
     * to be encoded again by the calling function. When looking at the base string
     * when debugging, double-urlencoded characters (space => '%20' => '%2520') in
     * the parameter list are normal.
     *
     * @param array $params Parameters of the request.
     *
     * @since  4.0.0
     * @return string
     */
    public static function normaliseParams(array $params)
    {
        // Encode the values we have.
        foreach ($params as &$param) {
            $param[0] = self::encodeValue($param[0]);
            $param[1] = self::encodeValue($param[1]);
        }

        // Unset foreach-by-reference param.
        unset($param);

        // Sort them using our user-defined function that sorts by keys, then values.
        usort(
            $params,
            array(
             'self',
             '_normaliseParamsFn',
            )
        );

        // Now put it all together.
        $paramStr = array();

        foreach ($params as $param) {
            $paramStr[] = $param[0].'='.$param[1];
        }

        return implode('&', $paramStr);

    }//end normaliseParams()


    /**
     * User-sort function for normalising the order of parameters.
     *
     * The OAuth normalising procedure requires parameters to be ordered in ascending
     * byte order of (encoded) parameter name, and then by (encoded) value if equal.
     * (Multiple instances of a parameter name is possible if specified in more than
     * one location - eg. as a GET and a POST variable.)
     *
     * The passed arrays should contain nested arrays of (name, value), as created
     * during doRequest().
     *
     * Returns the expected -1, 0, 1 result depending on which parameter is "greater"
     * (if not equal).
     *
     * @param array $a First parameter to compare.
     * @param array $b Second parameter to compare.
     *
     * @since  4.0.0
     * @return integer
     */
    private static function _normaliseParamsFn(array $a, array $b)
    {
        if (((string) $a[0]) > ((string) $b[0])) {
            return 1;
        } else if (((string) $a[0]) < ((string) $b[0])) {
            return -1;
        } else {
            // Keys equal. Use values instead.
            if (((string) $a[1]) > ((string) $b[1])) {
                return 1;
            } else if (((string) $a[1]) < ((string) $b[1])) {
                return -1;
            } else {
                return 0;
            }
        }

    }//end _normaliseParamsFn()


    /**
     * Calcuate the signature to use for a request.
     *
     * The signature is a digest of the generated base string with the passed
     * hash type.
     *
     * If the signature method is not supported or unknown, NULL is returned.
     * RSA-SHA1 is not currently supported due to requirement to pass a certificate
     * private key to sign.
     *
     * The $tokenSecret refers to the appropriate credentials being used for that
     * stage of the request:
     * - If getting temporary credentials, it should be an empty string ('').
     * - If getting a token, it should be the shared-secret of the temporary
     *   credentials.
     * - If doing a request with a token, it should be the shared-secret of
     *   the token.
     *
     * @param string $baseString   The generated base string.
     * @param string $clientSecret The shared secret part of client credentials.
     * @param string $tokenSecret  The shared secret part of the current credentials.
     * @param string $signMethod   The signature method.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSignature(
        $baseString,
        $clientSecret,
        $tokenSecret,
        $signMethod
    ) {
        $signatureHash  = self::encodeValue($clientSecret);
        $signatureHash .= '&'.self::encodeValue($tokenSecret);

        switch ($signMethod)
        {
            case 'HMAC-SHA1':
                // HMAC-SHA1 requires the HMAC digest to be generated (raw - not
                // hex string), and then base64-encoded before sending.
                $signature = hash_hmac('sha1', $baseString, $signatureHash, TRUE);
                $signature = base64_encode($signature);
            break;

            case 'PLAINTEXT':
                // In plain-text, the signature hash itself is just passed unchanged.
                $signature = $signatureHash;
            break;

            default:
                $signature = NULL;
            break;
        }//end switch

        return $signature;

    }//end getSignature()


    /**
     * Encode a value in accordance with RFC 3986.
     *
     * In PHP 5.2 and earlier, rawurlencode() was identical to RFC 3986 with one
     * exception: rawurlencode() encoded tildes ('~') which are to not be escaped in
     * RFC 3986.
     *
     * In PHP 5.3, rawurlencode() complies with RFC 3986. The replacement of the
     * encoded tilde should not have a effect, since it wouldn't be encoded to begin
     * with, and nothing else can replicate it in the encoded value.
     *
     * @param string $value The value to encode.
     *
     * @since  4.0.0
     * @return string
     */
    public static function encodeValue($value)
    {
        $value = rawurlencode($value);
        $value = str_replace('%7E', '~', $value);

        return $value;

    }//end encodeValue()


    /**
     * Generate a nonce value.
     *
     * Requests generally require a unique nonce + timestamp to mitigate the
     * potential for replay attacks.
     *
     * Although no longer required for PLAINTEXT-signed requests under RFC 5849,
     * older implementations may still require them, and otherwise, including them
     * does not hurt.
     *
     * @since  4.0.0
     * @return string
     */
    public static function generateNonce()
    {
        $mt   = microtime();
        $rand = mt_rand();

        return md5($mt.$rand);

    }//end generateNonce()


}//end class

?>
