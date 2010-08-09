<?php
/**
 * Actions for the Pie Graph Widget (GUIGraphPie) System.
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
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';
require_once 'Libs/Util/Util.inc';
require_once 'Libs/Web/Web.inc';

/**
 * Actions Class for the Pie Graph Widget (GUIGraphPie) System.
 *
 */
class GUIGraphPieActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        Channels::includeSystem('GUI');
        $currentTemplate = GUI::getBakeTimeIncludingTemplate();
        // Size of the graph on the page, in pixels. Height and width will be the
        // same size.
        $size = (int) Util::getArrayIndex($settings, 'size');

        $params = Web::getQueryStringParams();
        unset($params['_callback']);

        $url = '/__api/raw/GUIGraphPie/getSVGContents';

        $content  = '<span>';
        $content .= '&lt;?php'."\n";
        $content .= 'include_once \'Libs/Web/Web.inc\';'."\n\n";
        $content .= '$url    = \''.addslashes($url).'\';'."\n";
        $content .= '$params = Web::getQueryStringParams();'."\n";
        $content .= 'unset($params[\'_callback\']);'."\n\n";
        $content .= '$params[\'system\']   = \''.addslashes($currentTemplate['system']).'\';'."\n";
        $content .= '$params[\'template\'] = \''.addslashes($currentTemplate['template']).'\';'."\n";
        $content .= '$params[\'widgetId\'] = \''.addslashes($settings['id']).'\';'."\n\n";
        $content .= '$url = Web::appendQueryString($params, $url);';
        $content .= '?&gt;'."\n";
        $content .= '<object data="&lt;?php echo $url; ?&gt;"';
        $content .= ' style="width: '.$size.'px; height: '.$size.'px;"';
        $content .= ' type="image/svg+xml">'."\n";
        $content .= '</object>'."\n";
        $content .= '</span>';

        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl  The HTML content of the widget.
     * @param array   $settings The list of settings for this widget.
     *
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl, array $settings=array())
    {
        return $htmlTpl;

    }//end prepareBake()


    /**
     * Paint the graph data for the widget.
     *
     * @param array  $data          The provided data.
     * @param array  $ringConf      Ring configuration settings.
     * @param array  $borderConf    Border configuration settings.
     * @param array  $dataPointConf Data point configuration settings.
     * @param string $id            ID of the widget.
     * @param string $class         Class (prefix) of the widget.
     *
     * @return void
     */
    public static function paintData(
        array $data,
        array $ringConf,
        array $borderConf,
        array $dataPointConf,
        $id,
        $class
    ) {
        $dataPct = array();

        // Calculate the total of the inner ring - this is the basis of all our
        // proportions.
        $total = array_sum($data[0]);

        if ($total <= 0) {
            return;
        }



        if (empty($borderConf) === FALSE) {
            $radius = (int) Util::getArrayIndex($borderConf, 'radius', 0);

            foreach ($ringConf as $ring) {
                $radius += (int) $ring['radius'];
            }

            if ($radius > 0) {
                echo '<svg:circle cx="0" cy="0" ';
                echo 'r="'.($radius).'" ';
                echo ' class="colour-'.$borderConf['colour'].'" />';
            }
        }

        // Then normalise the data points to a [0..1] value.
        foreach ($data as $dataRingIndex => $dataRing) {
            $dataPct[$dataRingIndex] = array();

            foreach ($dataRing as $dataPointIndex => $dataPoint) {
                if ($dataPoint > $data[0][$dataPointIndex]) {
                    $dataPoint = $data[0][$dataPointIndex];
                }

                $dataPct[$dataRingIndex][$dataPointIndex] = ($dataPoint / $total);
            }//end foreach
        }//end foreach

        // Total angle provided by a circle.
        $circleAngle = (2 * M_PI);

        // Array of positions where the data points should start.
        $startAngles = array();
        $cumAngle    = $circleAngle;
        $outerRadius = 0;

        // Work on a ring-by-ring basis.
        foreach ($dataPct as $dataRingIndex => $dataRing) {
            if ($dataRingIndex >= count($ringConf)) {
                break;
            }

            $innerRadius = $outerRadius;
            $outerRadius = ($innerRadius + $ringConf[$dataRingIndex]['radius']);

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

                $thisAngle = ($dataPoint * $circleAngle);

                // Rationalise angle to a 0..2Pi range.
                $thisAngle -= (floor(($thisAngle / $circleAngle)) * $circleAngle);

                $cumAngle = ($oldAngle - $thisAngle);

                if ($thisAngle > 0) {
                    // We need to negate the Y points... normally when converting
                    // points on a circl to an XY-graph co-ordinate, positive Y is
                    // going UP on the graph. However in SVG, positive Y is going
                    // DOWN on the graph, so we need to flip the values.
                    $points = array(
                               array(
                                'x' => round(($innerRadius * cos($oldAngle)), 6),
                                'y' => round(($innerRadius * (-sin($oldAngle))), 6),
                               ),
                               array(
                                'x' => round(($innerRadius * cos($cumAngle)), 6),
                                'y' => round(($innerRadius * (-sin($cumAngle))), 6),
                               ),
                               array(
                                'x' => round(($outerRadius * cos($cumAngle)), 6),
                                'y' => round(($outerRadius * (-sin($cumAngle))), 6),
                               ),
                               array(
                                'x' => round(($outerRadius * cos($oldAngle)), 6),
                                'y' => round(($outerRadius * (-sin($oldAngle))), 6),
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
                }//end if
            }//end foreach
        }//end foreach

    }//end paintData()


    /**
     * Get the contents of the SVG file.
     *
     * @param string $system   The system that the template is a part of.
     * @param string $template The template that contains the widget.
     * @param string $widgetId The widget ID to draw the contents for.
     *
     * @return string
     * @throws ChannelException If the widget settings are not well-formed.
     * @api             read
     * @api-contentType image/svg+xml
     */
    public static function getSVGContents($system, $template, $widgetId)
    {
        ob_start();
            // Include the dependencies file of the template, and see if that widget
            // exists.
            $filename = BaseSystem::getDataDir().'/'.$system.'/Templates/'.$template.'.tpl.deps.php';

            if (file_exists($filename) === TRUE) {
                include $filename;
                if (array_key_exists('GUIGraphPie:GUIGraphPie', $dependentFiles['widgets']) === TRUE) {
                    $found = FALSE;

                    foreach ($dependentFiles['widgets']['GUIGraphPie:GUIGraphPie'] as $pieWidget) {
                        if ($pieWidget['id'] === $widgetId) {
                            $found    = TRUE;
                            $settings = $pieWidget['settings'];

                            // Data provider channel.
                            $dataProvider = (string) Util::getArrayIndex($settings, 'dataProvider');
                            if ($dataProvider === NULL) {
                                $msg = 'Widget parse error: No data provider for pie chart.';
                                throw new ChannelException($msg);
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

                            /*
                                Ring definitions:
                                - radius:  Radius of the ring, in logical units.
                                - opacity: Opacity of data points on this ring.
                            */

                            $rings = (array) Util::getArrayIndex($settings, 'rings', array());
                            foreach ($rings as $ringIndex => &$ring) {
                                if (array_key_exists('radius', $ring) === FALSE) {
                                    $msg = 'Widget parse error: Pie chart ring '.($ringIndex + 1).' must have a radius set.';
                                    throw new ChannelException($msg);
                                }

                                $totalRadius += ($ring['radius']);
                            }

                            // Unset by-ref foreach value.
                            unset($ring);

                            /*
                                Border definitions:
                                - radius:  Radius of the outer border.
                                - colour:  Pre-defined colour.
                            */

                            $border = (array) Util::getArrayIndex($settings, 'border', array());

                            if (count($border) > 0) {
                                $border = $border[0];
                            }

                            if (array_key_exists('colour', $border) === FALSE) {
                                $border = array();
                            }

                            $totalRadius += Util::getArrayIndex($border, 'radius', 0);

                            // Unset by-ref foreach value.
                            unset($ring);

                            /*
                                Data point definitions:
                                - label:   Label to show when mousing over the
                                           data point.
                                - colour:  Pre-defined colour of the data point.
                                - opacity: Opacity of this data point (in all rings).

                                Where an opacity is specified on both ring and
                                data point, they will be multiplied together.
                            */

                            $dpConf = (array) Util::getArrayIndex($settings, 'dataPoints', array());

                            echo '<svg:svg xmlns:svg="http://www.w3.org/2000/svg"';
                            echo ' class="GUIGraphPie"';
                            echo ' viewBox="'.(-$totalRadius).' '.(-$totalRadius);
                            echo ' '.($totalRadius * 2).' '.($totalRadius * 2).'">'."\n";

                            GUIGraphPie::paintData($data, $rings, $border, $dpConf, $settings['id'], 'GUIGraphPie');

                            echo '</svg:svg>';

                            break;
                        }//end if
                    }//end foreach
                }//end if
            }//end if

            $contents = ob_get_contents();
        ob_end_clean();

        return $contents;

    }//end getSVGContents()


}//end class

?>
