<?php
/**
 * String Lib.
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
 * Library for general file system operations.
 *
 * Methods for adding, deleting and modifying directories and files should
 * reside here.
 *
 */
class String
{


    /**
     * Chops a string at a certain maximum length and appends an ellipsis.
     *
     * @param string  $string The string to manipulate.
     * @param integer $length The length of the chopped string.
     *
     * @return string
     */
    public static function ellipsisize($string, $length)
    {
        // Type verification.
        if (is_string($string) === FALSE || is_int($length) === FALSE) {
            return '';
        }

        // Length needs to be at least zero.
        if ($length < 0) {
            return '';
        }

        // If the string is not long enough, don't change it.
        if (strlen($string) <= $length) {
            return $string;
        }

        $string = substr($string, 0, $length);
        $string = trim($string);

        // Figure out how many dots are on the end of the
        // string so we don't add too many.
        $end       = substr($string, -3);
        $endNoDots = rtrim($end, '.');
        $numDots   = (strlen($end) - strlen($endNoDots));

        $string .= str_repeat('.', (3 - $numDots));
        return $string;

    }//end ellipsisize()


    /**
     * Changes a timestamp to ISO 8601 format e.g. 2008-01-15 12:00:00).
     *
     * @param integer $timestamp The timestamp to process.
     *
     * @return string
     */
    public static function tsIso8601($timestamp)
    {
        $isoDate = date('c', $timestamp);
        return $isoDate;

    }//end tsIso8601()


    /**
     * Converts a number of seconds to a readable form.
     *
     * If you pass in a negative value, the format will
     * always be in seconds, so best not to do it.
     *
     * @param integer $time Number of seconds.
     *
     * @return string
     */
    public static function readableSeconds($time)
    {
        $result = '';
        if ($time > 31536000) {
            $unit   = floor(($time / 31536000));
            $result = sprintf(ngettext('%d year', '%d years', $unit), $unit);
        } else if ($time > 2419200) {
            $unit   = floor(($time / 2419200));
            $result = sprintf(ngettext('%d month', '%d months', $unit), $unit);
        } else if ($time > 604800) {
            $unit   = floor(($time / 604800));
            $result = sprintf(ngettext('%d week', '%d weeks', $unit), $unit);
        } else if ($time > 86400) {
            $unit   = floor(($time / 86400));
            $result = sprintf(ngettext('%d day', '%d days', $unit), $unit);
        } else if ($time > 3600) {
            $unit   = floor(($time / 3600));
            $result = sprintf(ngettext('%d hour', '%d hours', $unit), $unit);
        } else if ($time > 60) {
            $unit   = floor(($time / 60));
            $result = sprintf(ngettext('%d minute', '%d minutes', $unit), $unit);
        } else {
            $result = sprintf(ngettext('%d second', '%d seconds', $time), $time);
        }//end if

        return $result;

    }//end readableSeconds()


    /**
     * Returns TRUE if the given string ends with $endStr.
     *
     * @param string $string String to check.
     * @param string $endStr The end string.
     *
     * @return boolean
     */
    public static function endsWith($string, $endStr)
    {
        if (substr($string, (strlen($string) - strlen($endStr))) === $endStr) {
            return TRUE;
        }

        return FALSE;

    }//end endsWith()


    /**
     * Returns the ordinal suffix for a given number.
     *
     * Returns FALSE if argument is not a valid number.
     *
     * @param integer $number A valid numeric value.
     *
     * @return string
     */
    public static function getOrdinalSuffix($number)
    {
        if (is_numeric($number) === FALSE) {
            return FALSE;
        }

        $suffix = '';
        $tmp    = ($number % 100);

        if ($tmp >= 4 && $tmp <= 20) {
            $suffix = 'th';
        } else {
            switch (($number % 10)) {
                case 1:
                    $suffix = 'st';
                break;

                case 2:
                    $suffix = 'nd';
                break;

                case 3:
                    $suffix = 'rd';
                break;

                default:
                    $suffix = 'th';
                break;
            }//end switch
        }//end if

        return $suffix;

    }//end getOrdinalSuffix()


    /**
     * Prints date time (long).
     *
     * @param string $timestamp The timestamp to print.
     *
     * @return string
     */
    public static function easyDatetime($timestamp)
    {
        $value = date('M jS Y \a\t g:ia', $timestamp);
        return $value;

    }//end easyDatetime()


    /**
     * Prints readable date time.
     *
     * @param string $timestamp The timestamp to print.
     *
     * @return string
     */
    public static function readableDatetime($timestamp)
    {
        $value = date('jS M Y g:ia', $timestamp);
        return $value;

    }//end readableDatetime()


    /**
     * Converts specified seconds to H:m:s format.
     *
     * @param integer $seconds Seconds to convert.
     *
     * @return string
     */
    public static function secondsToHms($seconds)
    {
        $hms = '';

        if ($seconds <= 0) {
            return '0:00';
        }

        if ($seconds < 60) {
            $hms .= '0';
        } else {
            $hours = intval(intval($seconds) / 3600);
            if ($hours > 0) {
                $hms .= $hours.':';
            }

            $minutes = intval(($seconds / 60) % 60);
            if ($hours > 0) {
                $hms .= str_pad($minutes, 2, '0', STR_PAD_LEFT);
            } else {
                $hms .= $minutes;
            }
        }

        $secs = intval($seconds % 60);
        $hms .= ':'.str_pad($secs, 2, '0', STR_PAD_LEFT);

        return $hms;

    }//end secondsToHms()


    /**
     * Converts a camel caps name into a readable string.
     *
     * @param string $name The camel caps name to convert.
     *
     * @return string
     */
    public static function expandCamelCaps($name)
    {
        if (is_string($name) === FALSE) {
            return '';
        }

        if ($name === '') {
            return '';
        }

        $friendlyName = '';
        $length       = strlen($name);

        $lastWasUpper   = FALSE;
        $lastWasNumeric = FALSE;
        for ($i = 0; $i < $length; $i++) {
            if (is_numeric($name[$i]) === TRUE) {
                if ($lastWasNumeric === FALSE) {
                    $friendlyName .= ' ';
                }

                $lastWasUpper   = FALSE;
                $lastWasNumeric = TRUE;
            } else {
                $lastWasNumeric = FALSE;

                $char = strtolower($name[$i]);
                if ($char === $name[$i]) {
                    // Lowercase.
                    $lastWasUpper = FALSE;
                } else {
                    // Uppercase.
                    if ($lastWasUpper === FALSE) {
                        $friendlyName .= ' ';
                        $next = $name[($i + 1)];
                        if (strtolower($next) === $next) {
                            // Next char is lowercase so it is a word boundary.
                            $name[$i] = strtolower($name[$i]);
                        }
                    }

                    $lastWasUpper = TRUE;
                }
            }//end if

            $friendlyName .= $name[$i];
        }//end for

        $friendlyName    = trim($friendlyName);
        $friendlyName[0] = strtoupper($friendlyName[0]);

        return $friendlyName;

    }//end expandCamelCaps()


}//end class

?>
