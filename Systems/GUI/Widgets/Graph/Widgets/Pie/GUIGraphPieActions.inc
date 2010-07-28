<?php
/**
 * Actions for the Pie Graph Widget (GUIGraphPie) System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUIGraphPie
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';
require_once 'Libs/Util/Util.inc';

/**
 * Actions Class for the Pie Graph Widget (GUIGraphPie) System.
 *
 * @since 4.0.0
 */
class GUIGraphPieActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        Channels::includeSystem('GUI');
        $currentTemplate = GUI::getBakeTimeIncludingTemplate();
        // Size of the graph on the page, in pixels. Height and width will be the
        // same size.
        $size = (int) Util::getArrayIndex($settings, 'size');

        $content  = '<object data="/__api/raw/GUIGraphPie/getSVGContents?';
        $content .= 'system='.$currentTemplate['system'];
        $content .= '&amp;template='.$currentTemplate['template'];
        $content .= '&amp;widgetId='.$settings['id'];
        $content .= '" style="width: '.$size.'px; height: '.$size.'px;"';
        $content .= ' type="image/svg+xml">'."\n";
        $content .= '</object>'."\n";

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl  The HTML content of the widget.
     * @param array   $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl, array $settings=array())
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Paint the graph data for the widget.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintData(
        $data,
        $ringConf,
        $dataPointConf,
        $id,
        $class
    ) {
        $dataPct = array();

        // Calculate the total of the inner ring - this is the basis of all our
        // proportions.
        $total = array_sum($data[0]);

        // Then normalise the data points to a [0..1] value.
        foreach ($data as $dataRingIndex => $dataRing) {
            $dataPct[$dataRingIndex] = array();
            foreach ($dataRing as $dataPointIndex => $dataPoint) {
                if ($dataPoint > $data[0][$dataPointIndex]) {
                    $dataPoint = $data[0][$dataPointIndex];
                }
                $dataPct[$dataRingIndex][$dataPointIndex] = $dataPoint / $total;
            }
        }

        $circleAngle = 2 * M_PI;

        // Array of positions where the data points should start.
        $startAngles = array();
        $cumAngle    = $circleAngle;
        $outerRadius = 0;

        foreach ($dataPct as $dataRingIndex => $dataRing) {
            if ($dataRingIndex >= count($ringConf)) {
                break;
            }

            $innerRadius = $outerRadius;
            $outerRadius = $innerRadius + $ringConf[$dataRingIndex]['radius'];

            foreach ($dataRing as $dataPointIndex => $dataPoint) {
                if ($dataPointIndex >= count($dataPointConf)) {
                    break;
                }

                if ($dataRingIndex > 0) {
                    $oldAngle = $startAngles[$dataPointIndex];
                } else {
                    $startAngles[$dataPointIndex] = $cumAngle;
                    $oldAngle = $cumAngle;
                }

                $thisAngle = $dataPoint * $circleAngle;

                // Rationalise angle to a 0..2Pi range.
                $thisAngle -= (floor(($thisAngle / $circleAngle)) * $circleAngle);

                $cumAngle = $oldAngle - $thisAngle;

                if ($thisAngle > 0) {
                    // We need to negate the Y points... normally when converting
                    // points on a circle to an XY-graph co-ordinate, positive Y is
                    // going UP on the graph. However in SVG, positive Y is going
                    // DOWN on the graph, so we need to flip the values.
                    $points = array(
                               array(
                                'x' => round($innerRadius * cos($oldAngle), 6),
                                'y' => round($innerRadius * (-sin($oldAngle)), 6),
                               ),
                               array(
                                'x' => round($innerRadius * cos($cumAngle), 6),
                                'y' => round($innerRadius * (-sin($cumAngle)), 6),
                               ),
                               array(
                                'x' => round($outerRadius * cos($cumAngle), 6),
                                'y' => round($outerRadius * (-sin($cumAngle)), 6),
                               ),
                               array(
                                'x' => round($outerRadius * cos($oldAngle), 6),
                                'y' => round($outerRadius * (-sin($oldAngle)), 6),
                               ),
                              );

                    echo '<svg:path d="';
                    echo 'M'.$points[0]['x'].','.$points[0]['y'].' ';

                    echo 'A'.$innerRadius.','.$innerRadius.' 0 ';

                    // If greater than a semi-circle, we have to state we want the
                    // "large" angle arc.
                    if ($thisAngle > M_PI) {
                        echo '1';
                    } else {
                        echo '0';
                    }

                    echo ',1 '.$points[1]['x'].','.$points[1]['y'].' ';

                    echo 'L'.$points[2]['x'].','.$points[2]['y'].' ';

                    echo 'A'.$outerRadius.','.$outerRadius.' 0 ';

                    if ($thisAngle > M_PI) {
                        echo '1';
                    } else {
                        echo '0';
                    }

                    echo ',0 '.$points[3]['x'].','.$points[3]['y'].' ';

                    echo 'Z';
                    echo '" class="colour-'.$dataPointConf[$dataPointIndex]['colour'].'"';

                    if (array_key_exists('opacity', $dataPointConf[$dataPointIndex]) === FALSE) {
                        $dataPointConf[$dataPointIndex]['opacity'] = 1.0;
                    }

                    if (array_key_exists('opacity', $ringConf[$dataRingIndex]) === FALSE) {
                        $ringConf[$dataRingIndex]['opacity'] = 1.0;
                    }

                    $overallOpacity  = $ringConf[$dataRingIndex]['opacity'];
                    $overallOpacity *= $dataPointConf[$dataPointIndex]['opacity'];

                    echo ' title="';
                    echo $dataPointConf[$dataPointIndex]['name'].': ';
                    echo number_format($data[$dataRingIndex][$dataPointIndex]);
                    echo '"';

                    if ($overallOpacity < 1.0) {
                        echo ' style="opacity: ';
                        echo $overallOpacity;
                        echo '"';
                    }

                    echo ' />'."\n";
                }


            }//end foreach
        }//end foreach

    }//end paintGraph()


    /**
     * Get the contents of the SVG file.
     *
     * @api             read
     * @api-contentType image/svg+xml
     * @since           4.0.0
     * @return          string
     */
    public static function getSVGContents($system, $template, $widgetId)
    {
        ob_start();
            // Include the dependencies file of the template, and see if that widget
            // exists.
            $filename = BaseSystem::getDataDir().'/'.$system.'/Templates/'.$template.'.tpl.deps.php';
            //Debug::bam($filename);

            if (file_exists($filename) === TRUE) {
                include $filename;
                if (array_key_exists('GUIGraphPie:GUIGraphPie', $dependentFiles['widgets']) === TRUE) {
                    $found = FALSE;

                    foreach ($dependentFiles['widgets']['GUIGraphPie:GUIGraphPie'] as $pieWidget) {

                        if ($pieWidget['id'] === $widgetId) {
                            $found = TRUE;
                            $settings = $pieWidget['settings'];

                            // Data provider channel.
                            $dataProvider = (string) Util::getArrayIndex($settings, 'dataProvider');
                            if ($dataProvider === NULL) {
                                $msg = 'Widget parse error: No data provider for pie chart.';
                                throw new Exception($msg);
                            } else {
                                list($providerSystem, $method) = explode('::', $dataProvider);
                            }

                            Channels::includeSystem('GUI');
                            Channels::includeSystem('GUIGraphPie');
                            Channels::includeSystem($providerSystem);

                            echo '<?xml version="1.0" encoding="utf-8" ?>';
                            echo '<?xml-stylesheet href="/__web/'.GUI::getSystemWebPath('GUIGraphPie').'/GUIGraphPie.css" type="text/css" ?>';

                            $data = eval('return '.$dataProvider.'();');

                            $totalRadius = 0;

                            // Ring definitions.
                            // - radius: Radius of the ring, in logical units.
                            // - colour: The colour of the background of the ring.
                            //           If not specified, it's transparent.
                            $rings = (array) Util::getArrayIndex($settings, 'rings', array());
                            foreach ($rings as $ringIndex => &$ring) {
                                if (array_key_exists('radius', $ring) === FALSE) {
                                    $msg = 'Widget parse error: Pie chart ring '.($ringIndex + 1).' must have a radius set.';
                                    throw new Exception($msg);
                                }

                                $totalRadius += ($ring['radius']);
                            }
                            unset($ring);

                            // Data point definitions.
                            // - label:  Label to show when mousing over the data point.
                            // - colour: colour of the data point.
                            // - style:  style of the data point (defaulting to 'none').
                            $dpConf = (array) Util::getArrayIndex($settings, 'dataPoints', array());

                            echo '<svg:svg xmlns:svg="http://www.w3.org/2000/svg"';
                            echo ' class="GUIGraphPie"';
                            echo ' viewBox="'.(-$totalRadius).' '.(-$totalRadius);
                            echo ' '.($totalRadius * 2).' '.($totalRadius * 2).'">'."\n";

                            GUIGraphPie::paintData($data, $rings, $dpConf, $settings['id'], 'GUIGraphPie');

                            echo '</svg:svg>';

                            break;
                        }//end if
                    }//end foreach

                    if ($found === FALSE) {
                        // No Pie widget named this
                    }
                } else {
                    // No Pie widgets at all
                }
            } else {
                // Fail.
            }

            $contents = ob_get_contents();
        ob_end_clean();

        return $contents;

    }//end getSVGContents()


}//end class

?>
