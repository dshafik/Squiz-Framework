<?php
/**
 * DebugExecutionPath Library.
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
 * Library for development and debugging.
 *
 */
class DebugExecutionPath
{

    /**
     * Channel call stack.
     *
     * @var   array
     */
    private static $_stack = array();

    /**
     * Stack element counter.
     *
     * @var   integer
     */
    private static $_stackCounter = 0;

    /**
     * Completed array.
     *
     * @var   array
     */
    private static $_completed = array();

    /**
     * Array containing stack indexes to be closed.
     *
     * Its done in this way so that the array contains the real execution order
     * It is possible to add the poped elements to another array but this gives
     * random exec path.
     *
     * @var   array $_toEnd
     */
    private static $_toEnd = array();

    /**
     * Path to the report directory.
     *
     * @var   string
     */
    private static $_reportDir = '';

    /**
     * Channel type.
     *
     * @var   string
     */
    private static $_channelType = '';


    /**
     * Creates a new stack element.
     *
     * @param string $action The action name.
     *
     * @return void
     */
    public static function start($action)
    {
        $time        = microtime(TRUE);
        $callerIndex = end(self::$_toEnd);
        $caller      = 'index.php';
        if (isset(self::$_stack[$callerIndex]) === TRUE) {
            $caller = self::$_stack[$callerIndex]['action'];
        }

        $queries = DAL::getLoggedQueries();

        self::$_toEnd[] = self::$_stackCounter;
        self::$_stackCounter++;
        self::$_stack[] = array(
                           'action'  => $action,
                           'start'   => $time,
                           'caller'  => $caller,
                           'type'    => self::$_channelType,
                           'queries' => $queries,
                          );

        DAL::clearLoggedQueries();

    }//end start()


    /**
     * Stores the time taken for the current channel.
     *
     * @return void
     * @throws Exception If file cannot be found or stack is empty.
     */
    public static function end()
    {
        self::$_channelType = '';
        $endTime            = microtime(TRUE);
        $index              = array_pop(self::$_toEnd);
        $current            = self::$_stack[$index];

        // Find the time taken.
        $startTime = $current['start'];
        $timeTaken = ($endTime - $startTime);

        // Find the query time.
        $queries = DAL::getLoggedQueries();
        $prevQueries = $current['queries'];
        self::$_stack[$index]['queryTime']  = 0;
        self::$_stack[$index]['queryCount'] = 0;
        foreach ($queries as $query) {
            self::$_stack[$index]['queryTime'] += $query[0];
            self::$_stack[$index]['queryCount']++;
        }

        // Reset the queries for the parent.
        DAL::setLoggedQueries($prevQueries);
        unset(self::$_stack[$index]['queries']);

        self::$_stack[$index]['end']       = $endTime;
        self::$_stack[$index]['timeTaken'] = $timeTaken;

        $lastAction = '';
        if (empty(self::$_toEnd) === TRUE) {
            $lastAction = 'index.php';
            include_once 'Libs/FileSystem/FileSystem.inc';

            self::$_reportDir = dirname(dirname(dirname(__FILE__))).'/report/';
            $pid              = self::getId();
            if (empty($pid) === TRUE) {
                return;
            }

            $file = self::$_reportDir.'report_'.$pid.'.inc';
            file_put_contents($file, print_r(self::$_stack, 1));
            self::process($pid, $file);
        }

    }//end end()


    /**
     * Returns id for the request.
     *
     * @return string
     */
    public static function getId()
    {
        $id = str_replace('/', '-', $_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI']);
        return $id;

    }//end getId()


    /**
     * Sets the channel type.
     *
     * @param string $type Channel type.
     *
     * @return void
     */
    public static function setChannelType($type)
    {
        self::$_channelType = $type;

    }//end setChannelType()


    /**
     * Process.
     *
     * @param string $id   ID.
     * @param string $file File.
     *
     * @return void
     * @throws Exception If file cannot be found or stack is empty.
     */
    public static function process($id, $file)
    {
        if (file_exists($file) === FALSE) {
            throw new Exception('File not found: '.$file);
        }

        include $file;

        if (empty($stack) === TRUE) {
            throw new Exception('Stack is empty');
        }

        $data            = array();
        $data['fnCalls'] = self::_getChannelCallInfo($stack);

        $dotFile = self::_createDotFile($id, $stack, $data);
        $svgFile = self::createSVG($dotFile, $dotFile.'.svg');
        self::_createSVGHTML($svgFile, $svgFile.'.html');

    }//end process()


    /**
     * Retrieves channel call information..
     *
     * @param array $stack Channel call stack array.
     *
     * @return array
     */
    private static function _getChannelCallInfo(array $stack)
    {
        $calls = array();
        foreach ($stack as $action) {
            if (isset($calls[$action['action']]) === FALSE) {
                $calls[$action['action']] = array(
                                             'numCalls'   => 1,
                                             'numQueries' => $action['queryCount'],
                                             'functions'  => array(),
                                            );
            } else {
                $calls[$action['action']]['numCalls']++;
                $calls[$action['action']]['numQueries'] += $action['queryCount'];
            }

            $caller = $action['caller'];
            if (array_key_exists($caller, $calls) === FALSE) {
                $calls[$caller]              = array();
                $calls[$caller]['functions'] = array();
            }

            $calls[$caller]['functions'][] = array(
                                              'action'    => $action['action'],
                                              'time'      => $action['timeTaken'],
                                              'queries'   => $action['queryCount'],
                                              'queryTime' => $action['queryTime'],
                                              'type'      => $action['type'],
                                             );
        }//end foreach

        // TODO: Should we merge all same method calls from an action into 1
        // with total time?
        // Now get the % of each call and find the highest and lowest times.
        foreach ($calls as $caller => $action) {
            $totalTime      = 0;
            $totalQueryTime = 0;
            if (isset($action['functions']) === TRUE) {
                $functions = $action['functions'];
                foreach ($functions as $fn) {
                    $totalTime      += $fn['time'];
                    $totalQueryTime += $fn['queryTime'];
                }
            }

            $calls[$caller]['fnTimes']    = $totalTime;
            $calls[$caller]['queryTimes'] = $totalQueryTime;
        }

        return $calls;

    }//end _getChannelCallInfo()


    /**
     * Retrieves channel call information..
     *
     * @param string $id    ID.
     * @param array  $stack Channel call stack array.
     * @param array  $data  Data array.
     *
     * @return string
     */
    private static function _createDotFile($id, array $stack, array $data)
    {
        // Load template file.
        $template  = file_get_contents(self::$_reportDir.'/template.inc');
        $template .= '//'.$_SERVER['REQUEST_URI']."\n";
        $pid       = self::getId();
        $outFile   = self::$_reportDir.'report_'.$pid.'.dot';

        $actionInfo = array();

        $contents     = '';
        $totalCalls   = 0;
        $totalQueries = 0;
        foreach ($data['fnCalls'] as $action => $info) {
            if (isset($info['numCalls']) === FALSE) {
                $info['numCalls'] = 0;
            } else {
                $totalCalls += $info['numCalls'];
            }

            if (isset($info['numQueries']) === FALSE) {
                $info['numQueries'] = 0;
            } else {
                $totalQueries += $info['numQueries'];
            }

            if (isset($info['functions']) === FALSE) {
                continue;
            }

            $fnCount = count($info['functions']);
            if ($fnCount === 0) {
                continue;
            }

            foreach ($info['functions'] as $fn) {
                $calledFn = $fn['action'];
                if ($info['fnTimes'] === 0) {
                    $pc = 100;
                } else {
                    $pc = (number_format(($fn['time'] / $info['fnTimes']), 2) * 100);
                }

                $time  = number_format($fn['time'], 5).'('.$pc.'%)';
                $color = '#000000';
                $type  = $fn['type'];
                $style = 'solid';

                if ($fnCount > 1 && $pc > 30) {
                    $style = 'bold';
                }

                switch ($type) {
                    case 'gatherer':
                        $color = 'blue';
                    break;

                    case 'runner':
                        $color = 'gray';
                    break;

                    case 'switcher':
                        $color = 'DarkGoldenRod';
                    break;

                    case 'blocker':
                        $color = 'brown';
                    break;

                    case 'refiner':
                        $color = 'orange';
                    break;

                    case 'modifier':
                        $color = 'purple';
                    break;

                    default:
                        $color = 'black';
                    break;
                }//end switch

                $content = '"'.$action.'" -> "'.$calledFn.'" [label="'.$time.'", style="'.$style.'", color="'.$color.'"];'."\n";

                if (isset($actionInfo[$calledFn]) === FALSE) {
                    $actionInfo[$calledFn] = array(
                                              'time'       => 0,
                                              'queryTime'  => 0,
                                              'numCalls'   => 0,
                                              'numQueries' => 0,
                                             );
                }

                $actionInfo[$calledFn]['time']       += $fn['time'];
                $actionInfo[$calledFn]['queryTime']  += $fn['queryTime'];
                $actionInfo[$calledFn]['numQueries'] += $fn['queries'];
                $actionInfo[$calledFn]['numCalls']++;

                $contents .= $content;
            }//end foreach

            $contents .= '"'.$action.'" [label="['.$info['numCalls'].'/'.$info['numQueries'].'] '.$action.'\n('.number_format($info['fnTimes'], 5).')", URL=""];'."\n";
        }//end foreach

        foreach ($actionInfo as $calledFn => $info) {
            $contents .= '"'.$calledFn.'" [label="['.$info['numCalls'].'/'.$info['numQueries'].'] '.$calledFn.'\n('.number_format($info['time'], 5).')", URL=""];'."\n";
        }

        $contents .= '"index.php" [label="['.$totalCalls.'/'.$totalQueries.'] index.php\n('.number_format($data['fnCalls']['index.php']['fnTimes'], 5).')", URL=""];'."\n";

        $contents = str_replace('%graph%', $contents, $template);

        file_put_contents($outFile, $contents);

        return $outFile;

    }//end _createDotFile()


    /**
     * Creates SVG file.
     *
     * @param string  $dotFile  The source dot file.
     * @param string  $destFile The svg file to create.
     * @param boolean $onlySVG  If FALSE then a wrapper HTML file is created.
     *
     * @return string
     * @throws Exception If source dot file is not found.
     */
    public static function createSVG($dotFile, $destFile, $onlySVG=FALSE)
    {
        if (file_exists($dotFile) === FALSE) {
            throw new Exception('Dot file not found: '.$dotFile);
        }

        // Using dot command create the SVG file.
        $cmd  = 'dot -T svg -o '.$destFile;
        $cmd .= ' '.$dotFile;

        $output = '';
        system($cmd, $output);

        // Now wait for SVG creation for at least 5 seconds.
        $seconds = 5;

        $exists = FALSE;
        while ($seconds-- >= 0) {
            // Check if file was created.
            if (file_exists($destFile) === TRUE) {
                $exists = TRUE;
                break;
            }

            sleep(1);
        }

        if ($exists === FALSE) {
            throw new Exception('Failed to create SVG file: '.$output);
        }

        // Insert required JS stuff in to SVG file.
        // First add onload event.
        $svgContents = file_get_contents($destFile);
        $jsOnLoad    = 'onload="parent.SvgDebugger.add(this);"';
        $svgContents = str_replace('<svg', '<svg '.$jsOnLoad, $svgContents);

        file_put_contents($destFile, $svgContents);

        return $destFile;

    }//end createSVG()


    /**
     * Creates wrapper HTML file for the SVG.
     *
     * @param string $svgFile  The svg file to wrap.
     * @param string $destFile The destination HTML file.
     *
     * @return string
     */
    private static function _createSVGHTML($svgFile, $destFile)
    {
        $svgSrc     = basename($svgFile);
        $embedCode  = '<embed src="'.$svgSrc.'" type="image/svg+xml"></embed>';
        $jsFile     = 'Libs/Debug/ChannelsSvgDebugger/svgDebugger.js';
        $jsContents = file_get_contents($jsFile);

        $html = '<?xml version="1.0" encoding="UTF-8"?>
                <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
                <html xmlns="http ://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
                <head>
                    <title>MySource Mini Channels SVG Debugger</title>
                     <script type="text/javascript">
                     '.$jsContents.'
                     </script>
                </head>
                <body>
                '.$embedCode.'
                </body>
                </html>';

        file_put_contents($destFile, $html);
        return $destFile;

    }//end _createSVGHTML()


}//end class

?>
