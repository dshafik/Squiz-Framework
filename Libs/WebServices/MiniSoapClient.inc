<?php
/**
 * Extended SOAP Client.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Web
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

// @codingStandardsIgnoreStart
/**
 * Mini SOAP Client.
 *
 * @since 4.0.0
 */
class MiniSoapClient extends SoapClient
{

    /**
     * List of unwantedQString.
     *
     * @var   array
     * @since 4.0.0
     */
    private static $_unwantedQString = array('modeType', 'a');


    /**
     * Overriding doRequest method of SoapClient.
     *
     * @param string $request  XML Request.
     * @param string $location URL Location for request.
     * @param string $action   SOAP Action.
     * @param string $version  SOAP Version.
     *
     * @since  4.0.0
     * @return string
     */
    function __doRequest($request, $location, $action, $version)
    {
        // The location should always have a ?wsdl at the end.
        $pattern     = '/wsdl$/';
        $location    = preg_replace($pattern, '', $location);
        $queryString = self::appendQueryString(self::$_unwantedQString, array()); 
        $location    = $location.$queryString;
        $response    = parent::__doRequest($request, $location, $action, $version);

        return $response;

    }//end __doRequest()


    /**
     * Append existing server query string to the URL.
     *
     * @since  4.0.0
     * @return string
     */
    public static function appendQueryString($unwantedQString, $wantedQString)
    {
        $queryString     = $_SERVER['QUERY_STRING'];
        $finalComponents = array();
        $qsComponents    = explode('&', $queryString);
        foreach ($qsComponents as $component) {
            $qsParts = explode('=', $component);
            if (isset($qsParts[1]) === TRUE && in_array($qsParts[0], $unwantedQString) === FALSE) {
                if (empty($wantedQString) === TRUE) {
                    $finalComponents[$qsParts[0]] = $qsParts[1];
                } else {
                    if (in_array($qsParts[0], $wantedQString) === TRUE) {
                        $finalComponents[$qsParts[0]] = $qsParts[1];
                    }//end if
                }//end if
            }//end if
        }//end foreach

        $finalQueryString = '';
        $pieces           = array();
        foreach ($finalComponents as $qkey => $qval) {
            $pieces[] = $qkey.'='.$qval;
        }//end foreach
        $finalQueryString = implode('&', $pieces);

        return $finalQueryString;

    }//end appendQueryString()


}//end class
// @codingStandardsIgnoreEnd

?>
