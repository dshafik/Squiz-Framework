<?php
/**
 * Email related methods.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Web
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Email related methods.
 *
 * @since 4.0.0
 */
class Email
{


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     *
     * @since 4.0.0
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Validate an email address using regular expression.
     *
     * @param string $email The email address to validate.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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