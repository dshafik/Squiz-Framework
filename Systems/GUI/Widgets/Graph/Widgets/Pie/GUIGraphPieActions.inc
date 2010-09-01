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
require_once 'Libs/FileSystem/FileSystem.inc';

/**
 * Actions Class for the Pie Graph Widget (GUIGraphPie) System.
 */
class GUIGraphPieActions
{

    /**
     * Physical size of the pie graph, in pixels.
     *
     * Size is presumed to be a square (equal height and width), since the pie graph
     * is always a circle.
     *
     * @var string
     */
    protected static $size;

    /**
     * Data ring configuration.
     *
     * Each ring is defined from inner ring to outer ring, with the following
     * settings:
     * - radius:  Size of the ring, in relative units. These do not have to add up
     *            to the "size" setting, since these are in the logical units of the
     *            SVG document, rather than physical.
     * - opacity: Opacity of all the data points in this ring [0.0, 1.0] - default
     *            1.0. If a data point has an opacity setting too, it will be
     *            multiplied by the ring value.
     *
     * @var array
     */
    protected static $rings;

    /**
     * Data point configuration.
     *
     * Each data point is defined in order of the first to be defined in the data
     * provider result, with the following settings:
     * - colour:  Predefined named colour, as defined in this widget's CSS file.
     * - opacity: Opacity of this data point in all rings [0.0, 1.0] - default 1.0.
     *            If a ring also has opacity set, it will be multiplied by this
     *            value to get an overall opacity for that piece of data.
     *
     * @var array
     */
    protected static $dataPoints;

    /**
     * Data provider action.
     *
     * The data provider should return values in a 2-D array ordered by ring, then
     * data point - inner ring first.
     *
     * @var string
     */
    protected static $dataProvider;

    /**
     * Border configuration.
     *
     * Optionally set a border around the whole pie chart, using the following
     * settings:
     * - radius: Radius of the border, in the same relative units as the ring radii.
     * - colour: Predefined named colour, as defined in this widget's CSS file.
     *
     * @var array
     */
    protected static $border;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        Channels::includeSystem('GUI');

        // Size of the graph on the page, in pixels. Height and width will be the
        // same size.
        $size = (int) Util::getArrayIndex($settings, 'size');

        // Border settings.
        $border = Util::getArrayIndex($settings, 'border');
        if (is_array($border) === FALSE) {
            $settings['border'] = array(
                                   'radius' => 0,
                                  );
        }

        // Generate a new random file name.
        $fileDir   = BaseSystem::getDataDir('GUIGraphPie').'/svg-temp';
        $hashValue = mt_rand().microtime();
        $baseName  = $settings['template']['system'].'_'.$settings['template']['name'].'_'.$settings['widget']['id'];
        $baseName .= '_'.sha1($hashValue);
        $fileName  = $fileDir.'/'.$baseName.'.svg';

        $radius = (int) Util::getArrayIndex($settings['border'], 'radius', 0);
        foreach ($settings['rings'] as $ring) {
            $radius += (int) Util::getArrayIndex($ring, 'radius', 0);
        }

        // Build the SVG file.
        ob_start();
            echo '<?xml version="1.0" encoding="utf-8" ?>';
            echo '<?xml-stylesheet href="/__web/'.GUI::getSystemWebPath('GUIGraphPie').'/GUIGraphPie.css" type="text/css" ?>';

            echo '<svg:svg xmlns:svg="http://www.w3.org/2000/svg" class="GUIGraphPie"';
            echo ' viewBox="'.(-$radius).' '.(-$radius);
            echo ' '.($radius * 2).' '.($radius * 2).'">'."\n";

            GUIGraphPie::paintData($settings);

            echo '</svg:svg>';

            $fileContents = ob_get_contents();
        ob_end_clean();

        // Create the directory (if necessary) and the file.
        if (is_dir($fileDir) === FALSE) {
            mkdir($fileDir, 0755, TRUE);
        }

        FileSystem::filePutContents($fileName, $fileContents, 0644);

        include_once 'Libs/Web/Web.inc';

        $params = Web::getQueryStringParams();
        unset($params['_callback']);
        $url = '/__api/raw/GUIGraphPie/getSVGContents';

        Channels::includeSystem('API');
        $userToken = API::getUserToken();

        $params['fileName']    = $baseName;
        $params['_api_token'] = $userToken;
        $url = Web::appendQueryString($params, $url);

        echo '<span class="GUIGraphPie-container"  style="width: '.$size.'px; height: '.$size.'px;">';
        echo '<object data="'.$url.'"';
        echo ' style="width: '.$size.'px; height: '.$size.'px;"';
        echo ' type="image/svg+xml">'."\n";
        echo '</object>'."\n";
        echo '</span>'."\n";

        return $settings;

    }//end paint()


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
    public static function paintData(array $settings)
    {
        // Get our settings
        $data          = $settings['dataProvider'];
        $ringConf      = $settings['rings'];
        $borderConf    = $settings['border'];
        $dataPointConf = $settings['dataPoints'];
        $id            = $settings['widget']['id'];
        $class         = $settings['widget']['type'];

        $dataPct  = array();
        // Calculate the total of the inner ring - this is the basis of all our
        // proportions.

        $firstRow = reset($data);
        $total    = array_sum($firstRow);

        if ($total <= 0) {
            return;
        }

        if (empty($borderConf) === FALSE) {
            $radius = (int) Util::getArrayIndex($borderConf, 'radius', 0);

            if ($radius > 0) {
                foreach ($ringConf as $ring) {
                    $radius += (int) $ring['radius'];
                }

                echo '<svg:circle cx="0" cy="0" ';
                echo 'r="'.($radius).'" ';
                echo ' class="colour-'.$borderConf['colour'].'" />';
            }
        }

        // Then normalise the data points to a [0..1] value.
        foreach ($ringConf as $ringIndex => $ring) {
            $dataPct[$ringIndex] = array();
            $dataRing = Util::getArrayIndex($data, $ringIndex, array());

            foreach ($dataPointConf as $pointIndex => $point) {
                $dataPoint  = Util::getArrayIndex($dataRing, $pointIndex, 0);
                $firstPoint = Util::getArrayIndex($firstRow, $pointIndex, 0);

                if ($dataPoint > $firstPoint) {
                    $dataPoint = $firstPoint;
                }

                $dataPct[$ringIndex][$pointIndex] = ($dataPoint / $total);
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
            if (array_key_exists($dataRingIndex, $ringConf) === FALSE) {
                continue;
            }

            $innerRadius = $outerRadius;
            $outerRadius = ($innerRadius + $ringConf[$dataRingIndex]['radius']);

            foreach ($dataRing as $dataPointIndex => $dataPoint) {
                if (array_key_exists($dataPointIndex, $dataPointConf) === FALSE) {
                    continue;
                }

                if (array_key_exists($dataPointIndex, $startAngles) === TRUE) {
                    $oldAngle = $startAngles[$dataPointIndex];
                } else {
                    $startAngles[$dataPointIndex] = $cumAngle;
                    $oldAngle = $cumAngle;
                }

                $thisAngle = ($dataPoint * $circleAngle);
                $cumAngle  = ($oldAngle - $thisAngle);

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

                    // If this is 100% of the circle, drawing an arc from one point
                    // to itself won't work - it draws a 0% angle instead. However,
                    // drawing two semi-circle arcs does. So, do that.
                    // If not 100%, just draw the arc.
                    if ($thisAngle === $circleAngle) {
                        echo 'A'.$innerRadius.','.$innerRadius.' 0 1,1 '.(-$points[1]['x']).','.$points[1]['y'].' ';
                        echo 'A'.$innerRadius.','.$innerRadius.' 0 1,1 '.$points[1]['x'].','.$points[1]['y'].' ';
                    } else {
                        echo 'A'.$innerRadius.','.$innerRadius.' 0 ';

                        // If greater than a semi-circle, we have to state we want the
                        // "large" angle arc.
                        if ($thisAngle > M_PI) {
                            echo '1';
                        } else {
                            echo '0';
                        }

                        echo ',1 '.$points[1]['x'].','.$points[1]['y'].' ';
                    }

                    echo 'L'.$points[2]['x'].','.$points[2]['y'].' ';

                    // Draw the outer arc - again, two semi-circle arcs if we are
                    // drawing a full circle arc.
                    if ($thisAngle === $circleAngle) {
                        echo 'A'.$outerRadius.','.$outerRadius.' 0 1,0 '.(-$points[3]['x']).','.$points[3]['y'].' ';
                        echo 'A'.$outerRadius.','.$outerRadius.' 0 1,0 '.$points[3]['x'].','.$points[3]['y'].' ';
                    } else {
                        echo 'A'.$outerRadius.','.$outerRadius.' 0 ';

                        if ($thisAngle > M_PI) {
                            echo '1';
                        } else {
                            echo '0';
                        }

                        echo ',0 '.$points[3]['x'].','.$points[3]['y'].' ';
                    }

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
     * Get the contents of an SVG file.
     *
     * Returns an empty SVG document if it doesn't exist.
     *
     * @param string $fileName The base name of the SVG file to server.
     *
     * @return string
     *
     * @api             read
     * @api-contentType image/svg+xml
     */
    public static function getSVGContents($fileName)
    {
        // Generate a new random file name.
        $fileDir  = BaseSystem::getDataDir('GUIGraphPie').'/svg-temp';
        $fileName = $fileDir.'/'.$fileName.'.svg';

        if (file_exists($fileName) === TRUE) {
            $contents = file_get_contents($fileName);
        } else {
            $contents  = '<svg:svg xmlns:svg="http://www.w3.org/2000/svg">';
            $contents .= '</svg:svg>';
        }

        return $contents;

    }//end getSVGContents()


    /**
     * Clean the SVG file temp folder.
     *
     * System maintenance script.
     *
     * @return void
     */
    public static function cleanSVGTempFiles()
    {
        $fileDir  = BaseSystem::getDataDir('GUIGraphPie').'/svg-temp';

    }//end cleanSVGTempFiles()


}//end class

?>
