<?php
/**
 * Email related methods.
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
 * Email related methods.
 */
class Email
{


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Validate an email address using regular expression.
     *
     * @param string $email The email address to validate.
     *
     * @return boolean
     */
    public static function validate($email)
    {
        if (is_string($email) === FALSE) {
            return FALSE;
        }

        // Dot character cannot be the first/last character in the local-part.
        $local       = '\d0-9a-zA-Z-_+';
        $localMiddle = $local.'.\w';

        // Matches a normal email address.
        $pattern  = '/^(['.$local.'](['.$localMiddle.'\']*['.$local.']){0,1}';
        $pattern .= '@(((?:[\da-zA-Z]|[\da-zA-Z][\'-\w]*[\da-zA-Z])\.)+';
        $pattern .= '[a-zA-Z]{2,7}))$/';
        if (preg_match($pattern, $email) === 1) {
            return TRUE;
        }

        // Email with display name, e.g. 'Someone <some.one@example.com>'.
        $pattern  = '/^[a-zA-Z]+(([\'\,\.\- ][a-zA-Z ])?[a-zA-Z]*)*\s+';
        $pattern .= '<(['.$local.'](['.$localMiddle.']*['.$local.']){0,1}';
        $pattern .= '@(((?:[\da-zA-Z]|[\da-zA-Z][\'-\w]*[\da-zA-Z])\.)+';
        $pattern .= '[a-zA-Z]{2,7}))>$/';
        if (preg_match($pattern, $email) === 1) {
            return TRUE;
        }

        return FALSE;

    }//end validate()


    /**
     * Sends an email using PHP mail() method.
     *
     * @param string $to      Recipient email address.
     * @param string $from    Sender email address.
     * @param string $subject Email subject.
     * @param string $message Email content.
     * @param string $headers Additional email headers.
     * @param string $params  Additional email parameters.
     *
     * @return boolean
     */
    public static function send(
        $to,
        $from,
        $subject,
        $message,
        $headers='',
        $params=''
    ) {
        if (self::validate($to) === FALSE || self::validate($from) === FALSE) {
            return FALSE;
        }

        $message  = wordwrap($message, 70);
        $headers .= 'From: '.$from."\r\n";
        $ret      = mail($to, $subject, $message, $headers, $params);
        return $ret;

    }//end send()


}//end class

?>