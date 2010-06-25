<?php
/**
 * Debug Library.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Library for development and debugging.
 *
 * @since 4.0.0
 */
class Debug
{

    /**
     * Stores the last time recording from speedCheck().
     *
     * @var   float $_speedCheckLastHit Time Total.
     * @since 4.0.0
     */
    private static $_speedCheckLastHit = NULL;

    /**
     * Stores the overall total from all speedCheck() calls.
     *
     * @var   float $_speedCheckTotal Time Total.
     * @since 4.0.0
     */
    private static $_speedCheckTotal = NULL;

    /**
     * Used by mem_check to store the last memory usage recording.
     *
     * @var   float $_memCheckLastHit Time Total.
     * @since 4.0.0
     */
    private static $_memCheckLastHit = NULL;

    /**
     * Used by mem_check to store the first memory usage recording.
     *
     * @var   float $_memCheckFirstHit Time Total.
     * @since 4.0.0
     */
    private static $_memCheckFirstHit = NULL;


    /**
     * Recursively prints array contents.
     *
     * @param mixed   $array      The array to create the contents of.
     * @param integer $maxDepth   The maximum depth to decend into the array.
     * @param boolean $ignoreInts Whether to ignore integer keys or not.
     * @param integer $depth      The current depth in the array.
     *
     * @since  4.0.0
     * @return string
     */
    public static function arrayContents(
        $array,
        $maxDepth=0,
        $ignoreInts=FALSE,
        $depth=0
    ) {
        $string = '';
        $indent = '';
        for ($i = 0; $i < $depth; $i++) {
            $indent .= "\t";
        }

        if ((empty($maxDepth) === FALSE) && ($depth >= $maxDepth)) {
            return $indent."[Max Depth Reached]\n";
        }

        if (count($array) === 0) {
            return $indent."[Empty]\n";
        }

        reset($array);
        while (list($key,$value) = each($array)) {
            $printKey = str_replace(
                "\n",
                "\\n",
                str_replace("\r", "\\r", str_replace("\t", "\\t", addslashes($key)))
            );
            if (($ignoreInts === TRUE) && (gettype($key) === 'integer')) {
                continue;
            }

            $type = gettype($value);
            if ($type === 'array' || $type === 'object') {
                $string .= $indent;

                if (is_string($key) === TRUE) {
                    $string .= '"'.$printKey.'"';
                } else {
                    $string .= $key.' => ';
                }

                if ($type === 'array') {
                    $string .= "array (\n";
                } else {
                    $string .= '';
                }

                if ($type === 'object') {
                    $string .= 'new '.get_class($value)." Object (\n";
                } else {
                    $string .= '';
                }

                $string .= self::arrayContents(
                    $value,
                    $maxDepth,
                    $ignoreInts,
                    ($depth + 1)
                );
                $string .= $indent."),\n";
            } else {
                if (is_string($value) === TRUE) {
                    $value = str_replace(
                        "\n",
                        "\\n",
                        str_replace(
                            "\r",
                            "\\r",
                            str_replace("\t", "\\t", addslashes($value))
                        )
                    );
                }

                $string .= $indent;
                if (is_string($key) === TRUE) {
                    $string .= '"'.$printKey.'"';
                } else {
                    $string .= $key;
                }

                $string .= ' => ';

                if (is_string($value) === TRUE) {
                    $string .= '"'.$value.'"';
                } else {
                    $string .= $value;
                }

                $string .= ",\n";
            }//end if
        }//end while

        $string[(strlen($string) - 2)] = ' ';
        return $string;

    }//end arrayContents()


    /**
     * Alias for {@link preEcho()}.
     *
     * @param mixed   $x        Var to print.
     * @param integer $maxDepth Max depth.
     * @param mixed   $style    CSS style to use.
     *
     * @since  4.0.0
     * @see    preEcho()
     * @return void
     */
    public static function bam($x='BAM!', $maxDepth=0, $style='')
    {
        self::preEcho($x, $maxDepth, $style);

    }//end bam()


    /**
     * Same as bam, but initially hidden.
     *
     * Useful when you're bamming lots of stuff and it's hard to keep track of
     * what's what, or when you want your bam to have a low profile.
     *
     * @param mixed  $contents The thing to bam, can be almost any data type.
     * @param string $title    Title for the block (optional).
     *
     * @since  4.0.0
     * @return void
     */
    public static function miniBam($contents, $title='BAM')
    {
        ob_start();
            self::preEcho($contents);
            $contents = ob_get_contents();
        ob_end_clean();

        $id = rawurlencode(strtolower($title)).'_'.microtime();
        $c  = '<table border="1" cellspacing="0" cellpadding="2" style="background-color: #FFDDDD">'."\n";
        $c .= '  <tr>'."\n";
        $c .= '    <td style="cursor: pointer; cursor: hand; text-decoration: underline; font-weight: bold" onclick="elt = document.getElementById(\''.$id.'\'); elt.style.display = (elt.style.display === \'none\') ? \'\' : \'none\';">';
        $c .= $title.'</td>'."\n";
        $c .= '  </tr>'."\n";
        $c .= '  <tr>'."\n";
        $c .= '    <td id="'.$id.'" style="display: none">';
        $c .= $contents.'</td>'."\n";
        $c .= '  </tr>'."\n";
        $c .= '</table>'."\n";

        echo $c;

    }//end miniBam()


    /**
     * Take any kind of variable and tries to print it out.
     *
     * @param mixed   $x        Var to print.
     * @param integer $maxDepth Max depth.
     * @param mixed   $style    CSS style to use.
     *
     * @since  4.0.0
     * @return void
     */
    public static function preEcho($x, $maxDepth=0, $style='')
    {
        if (php_sapi_name() === 'cli') {
            echo "\n------------------------------------\n";
        } else {
            echo "\n", '<div align="left"><pre style="', $style, 'font-family: courier, monospace;">';
        }

        $type = gettype($x);
        if ($type === 'object' && $maxDepth === FALSE) {
            print_r($x);
        } else {
            if ($type === 'object' || $type === 'array') {
                // Get the contents, then recursively call this method.
                if ($maxDepth === 0) {
                    $maxDepth = 10;
                }

                $x = self::arrayContents($x, $maxDepth);
            }

            $x = str_replace("\t", '    ', $x);

            if (php_sapi_name() === 'cli') {
                $toEcho = $x;
            } else {
                $toEcho = htmlspecialchars($x);
            }

            echo $toEcho;
        }//end if

        if (php_sapi_name() === 'cli') {
            echo "\n------------------------------------\n";
        } else {
            echo '</pre></div>', "\n";
        }

    }//end preEcho()


    /**
     * For optimisaiton, this little function lets you track down the slow bits.
     *
     * @param string  $str    A comment that you want to have displayed with the
     *                        speed result.
     * @param boolean $return Whether to return the plain result.
     *
     * @since  4.0.0
     * @return string
     */
    public static function speedCheck($str='', $return=FALSE)
    {
        list($msec, $sec) = explode(' ', microtime());
        // Record time (note we ignore time spent IN this function).
        $bam = (((double) $sec) + ((double) $msec));

        if (self::$_speedCheckLastHit !== NULL) {
            $elapsed = ($bam - self::$_speedCheckLastHit);
            self::$_speedCheckTotal += $elapsed;

            $c  = '<table cellpadding=0 cellspacing=0 border=0 bgcolor="#ffff88">'."\n";
            $c .= '  <tr>'."\n";
            $c .= '    <td>'."\n";
            $c .= '      <p style="color:red;font-weight:bold;font-family:sans-serif;font-size:13px;">';
            $c .= round($elapsed, 5).'&nbsp;/&nbsp;'.round(self::$_speedCheckTotal, 5).'&nbsp;/&nbsp;';
            $c .= $str.'</p>'."\n";
            $c .= '    </td>'."\n";
            $c .= '  </tr>'."\n";
            $c .= '</table>'."\n";

            if ($return === FALSE) {
                echo $c;
            } else {
                $res  = round($elapsed, 5).' / ';
                $res .= round(self::$_speedCheckTotal, 5).' / '.$str;
                return $res;
            }
        }//end if

        list($msec, $sec) = explode(' ', microtime());
        // Initialize.
        self::$_speedCheckLastHit = (((double) $sec) + ((double) $msec));
        return '';

    }//end speedCheck()


    /**
     * Returns or prints out the memory usage for the current script.
     *
     * @param array|NULL $print       An array of parts to print
     *                                (current, change, total).
     * @param boolean    $returnValue Whether to return the value or just
     *                                print it.
     * @param boolean    $startOver   Clear current memory stats & start fresh.
     *
     * @since  4.0.0
     * @return string
     */
    public static function memCheck(
        $print=NULL,
        $returnValue=FALSE,
        $startOver=FALSE
    ) {
        if ($startOver === TRUE) {
            self::$_memCheckFirstHit = 0;
            self::$_memCheckLastHit  = 0;
        }

        $hit = (int) (memory_get_usage() / 1024);
        $mem = '';
        if ($print === NULL || in_array('current', $print) === TRUE) {
            $mem = $hit.' KB';
        }

        if (self::$_memCheckLastHit !== NULL) {
            if (($print === NULL) || (in_array('change', $print) === TRUE)) {
                if ($hit > self::$_memCheckLastHit) {
                    $mem .= ' (+ '.($hit - self::$_memCheckLastHit).' KB)';
                }

                if ($hit < self::$_memCheckLastHit) {
                    $mem .= ' (- '.(self::$_memCheckLastHit - $hit).' KB)';
                }
            }
        }

        if (self::$_memCheckFirstHit !== NULL) {
            if (($print === NULL) || (in_array('total', $print) === TRUE)) {
                if ($hit > self::$_memCheckFirstHit) {
                    $mem .= ' [+ '.($hit - self::$_memCheckFirstHit).' KB TOTAL]';
                }

                if ($hit < self::$_memCheckFirstHit) {
                    $mem .= ' [- '.(self::$_memCheckFirstHit - $hit).' KB TOTAL]';
                }
            }
        } else {
            self::$_memCheckFirstHit = $hit;
        }

        if ($returnValue === FALSE) {
            ?>
            <br>
            <table cellpadding=0 cellspacing=0 border=0 bgcolor=#ffff88>
                <tr>
                <td>
                    <p style=\"color:red;font-weight:bold;font-family:sans-serif;font-size:13px;\">
                    Mem: <?php echo $mem; ?>
                </td>
                </tr>
            </table>
            <br>
            <?php
        }

        self::$_memCheckLastHit = $hit;
        if ($returnValue === TRUE) {
            return $mem;
        }

    }//end memCheck()


    /**
     * Get the details of the function that called the current function.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCallingFunc()
    {
        $bt   = debug_backtrace();
        $file = substr($bt[1]['file'], (strrpos($bt[1]['file'], '/') + 1));
        $res  = '';
        if (isset($bt[2]) === TRUE) {
            if (isset($bt[2]['class']) === TRUE) {
                $res = $bt[2]['class'].'::'.$bt[2]['function'];
            } else {
                $res = $bt[2]['function'];
            }
        }

        $res .= ' ('.$file.' line '.$bt[1]['line'].')';
        return $res;

    }//end getCallingFunc()


    /**
     * Dump a variable to error log.
     *
     * @param mixed $var Variable to dump.
     *
     * @since  4.0.0
     * @return void
     */
    public static function logDump($var=NULL)
    {
        static $count = 0;
        if ($var === NULL) {
            $var = '---------- Debugging ----------';
        }

        $count++;
        error_log($count.': '.self::getCallingFunc().': '.print_r($var, 1));

    }//end logDump()


    /**
     * Gets the functions executed previously to this one.
     *
     * Returns a string representation of the function calls that have been
     * executed to the current point in the script.
     *
     * @param boolean $includeArgs If TRUE arguments will be included in
     *                             the stack.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCallStack($includeArgs=TRUE)
    {
        $bt = debug_backtrace();
        // Remove the call to this function.
        array_shift($bt);
        $functions = array();
        $i         = count($bt);

        foreach ($bt as $call) {
            $function = '';
            if (isset($call['class']) === TRUE) {
                $function = $i.': '.$call['class'].$call['type'];
            }

            $function .= $call['function'].'(';
            if ($includeArgs === TRUE) {
                $args = array();
                // Find an approprite string to represent this argument so that
                // its not too long.
                foreach ($call['args'] as $arg) {
                    switch (gettype($arg)) {
                        case 'array':
                            $args[] = 'array()';
                        break;

                        case 'string':
                            $args[] = "'".$arg."'";
                        break;

                        case 'integer':
                        case 'float':
                            $args[] = $arg;
                        break;

                        case 'boolean':
                            if ($arg === TRUE) {
                                $args[] = 'true';
                            } else {
                                $args[] = 'false';
                            }
                        break;

                        default:
                            $args[] = 'object '.get_class($arg);
                        break;
                    }//end switch
                }//end foreach

                $function .= implode(', ', $args);
            }//end if

            if (isset($call['file']) === TRUE) {
                $function .= ') '.$call['file'].' (line:'.$call['line'].')';
            } else {
                $function .= ') ';
            }

            $function    = str_repeat(' ', ($i + 2)).$function;
            $functions[] = $function;

            $i--;
        }//end foreach

        return "\n".implode("\n", array_reverse($functions))."\n";

    }//end getCallStack()


}//end class

?>
