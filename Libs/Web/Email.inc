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
     * This only handles basic text emails.
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


    /**
     * Sends html email.
     *
     * The body should be full html already, this does not construct it.
     *
     * The data passed in will contain an array of image attachments
     * which are included inline.
     *
     * In the body of the message there are placeholders for these to go in:
     * %attachment_0%, %attachment_1% etc
     * and these are automatically replaced.
     *
     * @param string $to      The email address to send to.
     * @param string $subject Subject of the email.
     * @param string $body    The main contents.
     * @param array  $data    Settings for the message.
     *
     * @return void
     */
    public static function sendHtmlEmail($to, $subject, $body, array $data=array())
    {
        include_once 'Libs/Security/Security.inc';

        $rand         = md5(time());
        $mimeBoundary = $rand;

        $headers  = 'From: '.$to;
        $headers .= "\nMIME-Version: 1.0\n";
        $headers .= 'Content-Type: multipart/alternative; boundary="'.$mimeBoundary."\"\n";

        // Text Content.
        $message  = '--'.$mimeBoundary."\n";
        $message .= "Content-Type: text/plain; charset=\"iso-8859-1\"\n";
        $message .= "Content-Transfer-Encoding: 7bit\n\n";
        $message .= $subject."\n\n";

        // HTML Content.
        $htmlMimeBoundary = Security::createUID();
        $message .= '--'.$mimeBoundary."\n";
        $message .= 'Content-Type: multipart/related; ';
        $message .= 'boundary="'.$htmlMimeBoundary.'"; ';
        $message .= 'type="text/html"'."\n\n";

        $message .= '--'.$htmlMimeBoundary."\n";
        $message .= "Content-Type: text/html; charset=\"iso-8859-1\"\n";
        $message .= "Content-Transfer-Encoding: 7bit\n\n";
        $message .= $body."\n\n";

        // Image attachments.
        if (isset($data['attachments']) === TRUE) {
            $attachments = '';
            foreach ($data['attachments'] as $index => $attch) {
                $fileName = basename($attch);
                $cid      = Security::createUID().'@squiz.net';
                $data     = chunk_split(base64_encode(file_get_contents($attch)));

                $message = str_replace('%attachment_'.$index.'%', $cid, $message);

                $attachments .= '--'.$htmlMimeBoundary."\n";
                $attachments .= "Content-Disposition: inline;\n";
                $attachments .= ' filename="'.$fileName."\"\n";
                $attachments .= 'Content-Transfer-Encoding: base64'."\n";
                $attachments .= 'Content-Type: image/jpeg;'."\n";
                $attachments .= ' name="'.$fileName."\"\n";
                $attachments .= 'Content-Id: <'.$cid.">\n\n";
                $attachments .= $data;
            }

            $message .= $attachments;
        }//end if

        $message .= '--'.$htmlMimeBoundary."--\n";
        $message .= '--'.$mimeBoundary."--\n";

        $res = mail($to, $subject, $message, $headers);

    }//end sendHtmlEmail()


    /**
     * Returns the header content for email.
     *
     * @param string $title Title of the message.
     * @param string $url   Url to the logo.png and background.png files.
     *
     * @return string
     */
    public static function getHtmlHeader($title='', $url='')
    {
        $header  = '';
        $header .= '<html><head><title></title></head><body style="margin:0px;padding:0px;">';
        $header .= '<table style="font-size:0px;" border="0" cellpadding="0" cellspacing="0" width="100%" background="'.$url.'/background.png">';
        $header .= '<tr><td style="padding-left:10px;" width="150px"><img src="'.$url.'/logo.png" /></td><td style="color:#FFF;font: 17px arial;">- '.$title.'</td></tr></table>';
        $header .= '<!-- main content !--><div style="padding:15px;background-color:#FFF;">';
        return $header;

    }//end getHtmlHeader()


    /**
     * Returns the footer contents for email.
     *
     * @param string $message Message to include in the footer.
     *
     * @return string
     */
    public static function getHtmlFooter($message)
    {
        $footer  = '</div><!-- end main content !-->';
        $footer .= '<table width="100%" height="100%" bgcolor="#EEEEEE"><tr><td valign="top">';
        $footer .= '<div style="color:#666;padding:15px;padding-bottom:50px;">';
        $footer .= $message;
        $footer .= '</div></td></tr></table>';
        $footer .= '</body></html>';
        return $footer;

    }//end _getFooter()


}//end class

?>
