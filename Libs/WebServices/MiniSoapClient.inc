<?php
/**
 * Extended SOAP Client.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

// @codingStandardsIgnoreStart
/**
 * Mini SOAP Client.
 *
 */
class MiniSoapClient extends SoapClient
{

    /**
     * List of unwantedQString.
     *
     * @var   array
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
