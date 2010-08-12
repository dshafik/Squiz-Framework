<?php
/**
 * Security Library.
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

/**
 * Library for security.
 */
class Security
{


    /**
     * Returns a unique challenge string.
     *
     * The challenge string is used for
     * the user authentication.
     *
     * @return string
     */
    public static function createChallengeString()
    {
        $result = md5(uniqid(rand(), TRUE));
        return $result;

    }//end createChallengeString()


    /**
     * Validates the user password.
     *
     * @param string $hashedPassword The hashed password from the client.
     * @param string $password       The stored passowrd.
     * @param string $challenge      The challenge string to compare with.
     *
     * @return boolean
     */
    public static function validateUserPassword(
        $hashedPassword,
        $password,
        $challenge
    ) {
        if ($hashedPassword === md5(hash_hmac('sha1', $password, $challenge))) {
            return TRUE;
        } else if ($hashedPassword === $password) {
            // TODO: Use hash above when custom form for password field is ready.
            return TRUE;
        }

        return FALSE;

    }//end validateUserPassword()


    /**
     * Returns the hashed version of the password.
     *
     * @param string $password The stored passowrd.
     *
     * @return string
     */
    public static function hashUserPassword($password)
    {
        $result = sha1($password);
        return $result;

    }//end hashUserPassword()


    /**
     * Creates a unique ID that can be used to identify a resource.
     *
     * @return string
     */
    public static function createUID()
    {
        $uid = strtoupper(md5(uniqid(rand(), TRUE)));

        $uuid  = substr($uid, 0, 8).'-';
        $uuid .= substr($uid, 8, 4).'-';
        $uuid .= substr($uid, 12, 4).'-';
        $uuid .= substr($uid, 16, 4).'-';
        $uuid .= substr($uid, 20, 12);

        return $uuid;

    }//end createUID()


    /**
     * Filter the raw form data ($_POST, $_GET, $_REQUEST).
     *
     * @param string  $key        The key of the data in the array.
     * @param mixed   $default    Default value when it's not found.
     * @param boolean $jsonDecode TRUE, the value will be json decoded.
     * @param mixed   $dataType   Specifies $_REQUEST, $_POST, $_GET or
     *                            user's own array.
     *
     * @return mixed
     */
    public static function getRequestData(
        $key,
        $default=NULL,
        $jsonDecode=FALSE,
        $dataType='request'
    ) {
        include_once 'Libs/Util/Util.inc';

        $value = $default;
        switch ($dataType)
        {
            case 'request':
                $value = Util::getArrayIndex($_REQUEST, $key, $default);
            break;

            case 'get':
                $value = Util::getArrayIndex($_GET, $key, $default);
            break;

            case 'post':
                $value = Util::getArrayIndex($_POST, $key, $default);
            break;

            case 'files':
                $value = Util::getArrayIndex($_FILES, $key, $default);
            break;

            default:
                if (is_array($dataType) === TRUE) {
                    $value = Util::getArrayIndex($dataType, $key, $default);
                } else {
                    return $default;
                }
            break;
        }//end switch

        if ($jsonDecode === TRUE && $value !== $default) {
            // JSON decode will return NULL if the specified string is
            // not a valid JSON string. If this is the case return the value
            // but do NOT use htmlentities. So we need to handle string 'null'
            // separately to distinguish it from the error case.
            if (strtolower($value) === 'null') {
                $value = NULL;
            } else {
                $decodedVal = json_decode($value, TRUE);
                if ($decodedVal !== NULL) {
                    $value = $decodedVal;
                }
            }
        } else {
            if (is_string($value) === TRUE) {
                $value = htmlentities($value);
            }
        }

        return $value;

    }//end getRequestData()


    /**
     * Generates random password.
     *
     * @param integer $length Password length.
     *
     * @return string
     */
    public static function generatePassword($length=8)
    {
        $password = '';
        $sources  = array(
                     'abcdefghijklmnopqrstuvwxyz',
                     'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                     '0123456789',
                     '!@#$%^*()-=_+.',
                    );

        $max         = ceil($length / 4);
        $frequencies = array(
                        0,
                        0,
                        0,
                        0,
                       );
        for ($i = 0; $i < $length; $i++) {
            $type = rand(0, 3);
            if ($frequencies[$type] >= $max) {
                $i--;
                continue;
            }

            $charIdx   = rand(0, (strlen($sources[$type]) - 1));
            $password .= $sources[$type][$charIdx];
            $frequencies[$type]++;
        }

        return $password;

    }//end generatePassword()


}//end class

?>