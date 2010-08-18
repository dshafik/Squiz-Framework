<?php
/**
 * Actions for the Bar Graph Widget (GUIGraphBar) System.
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

/**
 * Actions Class for the Bar Graph Widget (GUIGraphBar) System.
 */
class GUIGraphBarActions
{

    /**
     * The height of the graph.
     *
     * @var string
     */
    protected static $height = '500';

    /**
     * Groups.
     *
     * @var array
     */
    protected static $groups = array();

    /**
     * Flags.
     *
     * @var array
     */
    protected static $flags = array();

    /**
     * Major tick.
     *
     * @var string
     */
    protected static $majorTick = '';

    /**
     * DataProvider.
     *
     * @var string
     */
    protected static $dataProvider = '';

    /**
     * Max.
     *
     * @var string
     */
    protected static $max = '';

    /**
     * Minor tick.
     *
     * @var string
     */
    protected static $minorTick = '';

    /**
     * Default height of the graph.
     *
     * Overridden using 'height' setting.
     *
     * @var string
     */
    private static $_defaultGraphHeight = '500';

    /**
     * Default height of top-of-bar labels, in pixels.
     *
     * @var integer
     */
    private static $_topLabelHeight = 13;

    /**
     * Default height of a flag.
     *
     * @var integer
     */
    private static $_defaultFlagHeight = 8;

    /**
     * Height of an X-axis that contains at least one value.
     *
     * @var integer
     */
    private static $_xAxisHeight = 15;

    /**
     * Height of spacing between flags.
     *
     * @var integer
     */
    private static $_flagSpacing = 2;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        // Get the total height of the graph.
        $graphHeight = Util::getArrayIndex(
            $settings,
            'height',
            self::$_defaultGraphHeight
        );
        $graphHeight = (int) $graphHeight;

        // Set a fixed set of colours that we can automatically assign
        // to a data point if the settings don't specify it.
        $fixedColours = array(
                         'black',
                         'blue',
                         'gold',
                         'red',
                         'green',
                        );

        // Ditto with the styles.
        $fixedStyles = array(
                        'diagonal',
                        'plain',
                        'reverse',
                        'fuzz',
                       );

        // Take a list of the styles we've used.
        $usedStyles = array();
        $fixedIndex = 0;

        /*
            Find the styles we've actually used.
            If there is no colour, then the colour/style combination is invalid,
            and auto-generate it.
            If there is a colour but no style, then set to the default of 'diagonal'.
            If there is a colour, track what styles have been used and
            how many times.
        */

        $groups = $settings['groups'];
        foreach ($groups as $groupIndex => &$groupData) {
            $groupData['units'] = (string) Util::getArrayIndex(
                $groupData,
                'units',
                ''
            );

            if (array_key_exists('colour', $groupData) === TRUE) {
                $groupData['style'] = (string) Util::getArrayIndex(
                    $groupData,
                    'style',
                    'diagonal'
                );

                $groupKey  = (string) $groupData['colour'];
                $groupKey .= ','.$groupData['style'];

                if (array_key_exists($groupKey, $usedStyles) === FALSE) {
                    $usedStyles[$groupKey] = 0;
                }

                $usedStyles[$groupKey]++;
            }//end if
        }//end foreach

        // Unset value after having run foreach by reference with it.
        unset($groupData);

        /*
            Fill those groups that have an empty colour, with a colour and style.
            The algorithm is to step through all available colours and styles.
            If a colour/style combination hasn't been used by a manually-set colour,
            it will be used.
            If it has, it will not be skipped over until it has been through the same
            number of loops as manually-set colours. This is to keep the usage of
            colours/styles balanced, and also to not make it spin into an infinite
            loop if we were just tracking usage/non-usage, once all combinations were
            used up.
            (ie. If a colour/style is manually set twice for some reason, it will
            not use that as an auto-selection until other combinations are used
            twice.)
        */

        foreach ($groups as $groupIndex => &$groupData) {
            if (array_key_exists('colour', $groupData) === FALSE) {
                $foundColour = FALSE;

                // Loop until we have found a colour/style combination we can use.
                while ($foundColour === FALSE) {
                    $colourIndex = ($fixedIndex % count($fixedColours));
                    $styleIndex  = (($fixedIndex / count($fixedColours)) %
                        count($fixedStyles));
                    $fixedIndex++;

                    $thisColour = (string) $fixedColours[$colourIndex];
                    $thisStyle  = (string) $fixedStyles[$styleIndex];
                    $groupKey   = $thisColour.','.$thisStyle;

                    // Do we still have a manually-set combination to match?
                    if (array_key_exists($groupKey, $usedStyles) === TRUE) {
                        $usedStyles[$groupKey]--;

                        // If none left, unset it so we only have to use the one IF.
                        if ($usedStyles[$groupKey] <= 0) {
                            unset($usedStyles[$groupKey]);
                        }
                    } else {
                        // We found our colour.
                        $foundColour = TRUE;
                    }
                }//end while

                // We found a free colour.
                $groupData['colour'] = $thisColour;
                $groupData['style']  = $thisStyle;
            }//end if
        }//end foreach

        // Unset value after having run foreach by reference with it.
        unset($groupData);

        $flags = Util::getArrayIndex($settings, 'flags', array());
        $max   = Util::getArrayIndex($settings, 'max', NULL);
        $tick  = array(
                  'major' => Util::getArrayIndex($settings, 'majorTick', NULL),
                  'minor' => Util::getArrayIndex($settings, 'minorTick', NULL),
                 );

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<div class="'.$class.'-outer-list" style="height: ';
        echo $graphHeight.'px;">';

        GUIGraphBar::paintGraph(
            $settings['dataProvider'],
            $groups,
            $flags,
            $graphHeight,
            $max,
            $tick,
            $settings['widget']['id'],
            $settings['widget']['type']
        );

        echo '</div>';
        echo '</div>';

    }//end paint()


    /**
     * Paint the graph data for the widget.
     *
     * @param array   $data   The data points sent from the data provider.
     * @param array   $groups Details of the data groups.
     * @param array   $flags  Details of the data flags.
     * @param integer $height Overall height of the graph (in pixels).
     * @param float   $max    Maximum value of the graph (used only if it is not
     *                        exceeded by any other data point).
     * @param array   $tick   Configuration details for major and minor Y-axis tick
     *                        lines.
     * @param string  $id     HTML ID of the overall widget.
     * @param string  $class  Class prefix to use for CSS elements.
     *
     * @return void
     */
    public static function paintGraph(
        array $data,
        array $groups,
        array $flags,
        $height,
        $max,
        array $tick,
        $id,
        $class
    ) {
        /*
            Calculate the total amount of space we need below (and including)
            the X-axis:
            - Certain number of pixels for the X-axis legend, if one is required.
            - Certain number of pixels for each flag - either whatever was specified
              in the settings, or the default.
            - 2 pixels above each flag.

            Note that the X-axis line, being a border, is not counted as part of the
            box model and therefore doesn't count.
        */

        $belowAxisHeight = 0;

        foreach ($flags as $flagIndex => $flag) {
            $flags[$flagIndex]['height'] = (int) Util::getArrayIndex(
                $flag,
                'height',
                self::$_defaultFlagHeight
            );

            $belowAxisHeight += ($flags[$flagIndex]['height'] + self::$_flagSpacing);
        }//end foreach

        /*
            Calculate the maximum bar total overall, and the maximum bar total
            that contains a label.
            This is so we can adjust the size of the bars to fit labels on tall
            bars.
        */

        $maxValue          = 0;
        $maxValueWithLabel = NULL;
        $hasXAxis          = FALSE;

        foreach ($data as $dataIndex => $dataRow) {
            $rowTotal = array_sum($dataRow['data']);
            $maxValue = max($maxValue, $rowTotal);

            if (array_key_exists('label', $dataRow) === TRUE) {
                if ($dataRow['label'] !== '') {
                    if ($maxValueWithLabel === NULL) {
                        $maxValueWithLabel = $rowTotal;
                    } else {
                        $maxValueWithLabel = max($maxValueWithLabel, $rowTotal);
                    }
                }
            }//end if

            $maxValue = max($maxValue, $max, $rowTotal);
            $data[$dataIndex] = $dataRow;

            // Do we need to print an x-axis?
            if (array_key_exists('x-axis', $dataRow) === TRUE) {
                if ($dataRow['x-axis'] !== '') {
                    $hasXAxis = TRUE;
                }
            }
        }//end foreach

        if ($tick['major'] !== NULL) {
            if ($tick['major'] === 'auto') {
                if ($tick['minor'] !== NULL) {
                    $tickAmounts   = ceil($maxValue / $tick['minor']);
                    $tickAmounts   = max(2, round($tickAmounts / 5));
                    $tick['major'] = $tick['minor'] * $tickAmounts;
                } else {
                    // Auto tick!
                    $tickExp = pow(10, floor(log($maxValue, 10)));

                    if ((int) $tickExp === 0) {
                        $maxExp  = 1;
                        $tickExp = 2;
                    } else {
                        $maxExp = $maxValue / $tickExp;
                    }

                    if ($maxExp > 6) {
                        $tickExp *= 2;
                    } else if ($maxExp <= 3) {
                        $tickExp *= 0.5;
                    }

                    $tick['major'] = $tickExp;

                    if ($tick['minor'] === NULL) {
                        $tick['minor'] = $tick['major'] / 2;
                    }
                }//end if
            }//end if

            $maxValue = ceil($maxValue / $tick['major']) * $tick['major'];
        }//end if

        if ($hasXAxis === TRUE) {
            $belowAxisHeight += (self::$_xAxisHeight + 1);
        }

        $maxBarHeight = ($height - $belowAxisHeight);

        // If we could not have a label on our tallest stacks without stretching the
        // graph, shrink the stacks until we can.
        if (($maxValueWithLabel !== NULL) && ($maxValue > 0)) {
            $labelPct   = ($maxValueWithLabel / $maxValue);
            $barHeight  = ($labelPct * $maxBarHeight);
            $barHeight += self::$_topLabelHeight;
            $barHeight  = ceil($barHeight);

            // If the projected bar height is too high, shrink the maximum so it
            // will fit.
            if ($barHeight > $maxBarHeight) {
                $maxBarHeight -= ($barHeight - $maxBarHeight);
            }
        }//end if

        // Now paint it.
        if ($tick['minor'] !== NULL) {
            $listRight = '2.5%';
            $listWidth = '95%';
        } else {
            $listRight = 0;
            $listWidth = '100%';
        }

        echo '<ul class="'.$class.'-inner-list" style="position: absolute; ';
        echo 'display: block; width: '.$listWidth.'; top: 0; right: '.$listRight.';';
        echo ' height: '.$height.'px; vertical-align: bottom; z-index: 2">';

        $rowNumber = 0;
        foreach ($data as $dataRow) {
            $rowNumber++;

            // Calculate how high our bar is allowed to be in total.
            $rowTotal = array_sum($dataRow['data']);
            if ((int) $maxValue === 0) {
                $barHeight = 0;
            } else {
                $barHeight = round(($rowTotal / $maxValue * $maxBarHeight));
            }

            $totalHeight = ($barHeight + $belowAxisHeight);

            // Make room for a label.
            if (array_key_exists('label', $dataRow) === TRUE) {
                if ($dataRow['label'] !== '') {
                    $barHeight += self::$_topLabelHeight;
                }
            }

            echo '<li class="'.$class.'-data-point-alt-text">';
            echo 'Bar '.$rowNumber;
            echo '</li>';

            echo '<ul class="'.$class.'-data-row';

            // We have a bar style?
            if (array_key_exists('bar_style', $dataRow) === TRUE) {
                if ($dataRow['bar_style'] !== '') {
                    echo ' '.$dataRow['bar_style'];
                }
            }

            echo '" style="width: ';
            echo number_format((100 / count($data)), 3);
            echo '%; left: ';
            echo number_format((100 / count($data) * ($rowNumber - 1)), 3);
            echo '%; top: ';
            echo ($height - $totalHeight);
            echo 'px; height: ';
            echo $totalHeight;
            echo 'px">';

            // Do we have a label for this data row?
            if (array_key_exists('label', $dataRow) === TRUE) {
                echo '<li class="'.$class.'-data-label">';
                echo '<span class="'.$class.'-data-point-alt-text">';
                echo 'Bar label: ';
                echo '</span>';
                echo $dataRow['label'];
                echo '</li>';
            }

            $liTexts = array();

            // Silently omit any extra data points.
            $dataRow['flags'] = Util::getArrayIndex($dataRow, 'flags', array());
            $dataRow['flags'] = array_slice($dataRow['flags'], 0, count($flags));
            $cumHeight        = 0;

            // Separate the data block in a list.
            echo '<li class="'.$class.'-data-point-alt-text">';
            echo 'Data';
            echo '</li>';

            echo '<ul style="display: block;">';

            $rowLabels = Util::getArrayIndex($dataRow, 'pointLabel', array());

            // Start stacking the graph up.
            foreach (array_reverse($groups) as $groupIndex => $groupInfo) {
                $dataPoint = Util::getArrayIndex($dataRow['data'], $groupIndex, 0);
                $label = Util::getArrayIndex($rowLabels, $groupIndex);

                if ($dataPoint === 0) continue;

                $pointTitle  = $groups[$groupIndex]['name'].': ';

                if ($label !== NULL) {
                    $pointTitle .= $label;
                } else {
                    $pointTitle .= $dataPoint;
                }

                $oldHeight = $cumHeight;
                if ($rowTotal > 0) {
                    $cumHeight += ($dataPoint / $rowTotal * $barHeight);
                }

                $dataPointHeight = (round($cumHeight) - round($oldHeight));

                // If this is the first point of at least 1 pixel height, take one
                // pixel off it to add the top border (which is not considered part
                // of the box model).
                $firstPointStyle = '';
                if (round($cumHeight) === $barHeight) {
                    if ($dataPointHeight > 0) {
                        $firstPointStyle = ' first-point';
                        $dataPointHeight--;
                    }
                }

                ob_start();
                    echo '<li class="'.$class.'-data-point'.$firstPointStyle;
                    echo ' '.$class.'-bar-colour-'.$groups[$groupIndex]['colour'];
                    echo ' '.$class.'-bar-style-'.$groups[$groupIndex]['style'];
                    echo '" title="'.$pointTitle.'"';
                    echo ' style="height: ';
                    echo max($dataPointHeight, 0).'px">';
                    echo '<span class="'.$class.'-data-point-alt-text">';
                    echo $pointTitle;
                    echo '</span>';
                    echo '</li>';
                    $liTexts[] = ob_get_contents();
                ob_end_clean();
            }//end foreach

            // But we want the first group at the bottom, so reverse what we have
            // before printing it.
            $liTexts = array_reverse($liTexts);
            echo implode('', $liTexts);

            // Get an empty (hidden) x-axis text, since we need to use it
            // a few times.
            ob_start();
                echo '<span class="'.$class.'-data-point-alt-text">';
                echo 'X-axis: No label';
                echo '</span>';
                $emptyXAxisText = ob_get_contents();
            ob_end_clean();

            // Do we have to print the x-axis? If so, print only those for which we
            // have x-axis values, and the empty label otherwise.
            if ($hasXAxis === TRUE) {
                echo '<li class="'.$class.'-data-x-axis">';

                if (array_key_exists('x-axis', $dataRow) === TRUE) {
                    if ($dataRow['x-axis'] !== '') {
                        echo '<span class="'.$class.'-data-point-alt-text">';
                        echo 'X-axis: ';
                        echo '</span>';
                        echo $dataRow['x-axis'];
                    } else {
                        echo $emptyXAxisText;
                    }
                } else {
                    echo $emptyXAxisText;
                }

                echo '</li>';
            } else {
                echo '<li class="'.$class.'-data-x-axis-empty">';
                echo $emptyXAxisText;
                echo '</li>';
            }//end if

            echo '</ul>';

            // Now print the flags, if we have any set.
            // If boolean, it'll display if TRUE.
            // If numeric or string, it'll display if not an empty string.
            // If null or unset, it's hidden.
            if (count($flags) > 0) {
                echo '<li class="'.$class.'-data-point-alt-text">';
                echo 'Flags';
                echo '</li>';

                echo '<ul style="display: block">';

                $liTexts = array();

                foreach ($flags as $flagIndex => $flag) {
                    $flagValue = Util::getArrayIndex(
                        $dataRow['flags'],
                        $flagIndex
                    );

                    $flagTitle = $flag['name'];
                    $showFlag  = FALSE;

                    if ($flagValue === NULL) {
                        $showFlag = FALSE;
                    } else if (is_bool($flagValue) === TRUE) {
                        if ($flagValue === TRUE) {
                            $showFlag = TRUE;
                        }
                    } else {
                        $flagValue = (string) $flagValue;

                        if ($flagValue !== '') {
                            $showFlag = TRUE;
                        }
                    }//end if

                    ob_start();
                        echo '<li class="'.$class.'-data-flag';
                        if ($showFlag === TRUE) {
                            echo ' '.$class.'-bar-colour-'.$flag['colour'];
                        }

                        echo '"';

                        if ($showFlag === TRUE) {
                            echo ' title="'.$flagTitle.'"';
                        }

                        echo ' style="height: '.$flag['height'].'px;';
                        echo ' line-height: '.($flag['height']).'px;"';
                        echo '>';
                        echo '<span class="'.$class.'-data-point-alt-text">';
                        echo $flagTitle.': ';

                        if ($flagValue === NULL) {
                            echo _('Blank');
                        } else if (is_bool($flagValue) === TRUE) {
                            if ($showFlag === TRUE) {
                                echo _('Yes');
                            } else {
                                echo _('No');
                            }

                            echo '</span>';
                        } else {
                            echo '</span>';
                            echo $flagValue;
                        }//end if

                        echo '</li>';
                        $liTexts[] = ob_get_contents();
                    ob_end_clean();
                }//end foreach

                echo implode('', $liTexts);
                echo '</ul>';
            }//end if

            echo '</ul>';
        }//end foreach

        echo '</ul>';

        // Draw the ticks.
        if ($tick['major'] !== NULL) {
            echo '<div style="z-index: 1; display: block; position: absolute; top: 0;';
            echo 'right: 0; width: 100%; height: 100%;">';

            if ($tick['minor'] !== NULL) {
                for ($i = $maxValue; $i > 0; $i -= $tick['minor']) {
                    echo '<div style="border-top: 1px solid #F3F3F3; height: 0; top: ';
                    echo round($maxBarHeight * (($maxValue - $i) / ($maxValue)));
                    echo 'px; width: 100%; position: absolute;"></div>';
                }
            }

            for ($i = $maxValue; $i > 0; $i -= $tick['major']) {
                echo '<div style="border-top: 1px solid #E1E1E1; height: 0; top: ';
                echo round($maxBarHeight * (($maxValue - $i) / ($maxValue)));
                echo 'px; width: 100%; position: absolute;"></div>';

                echo '<div style="color: #888; font-size: 11px; left: 0; top: ';
                echo (2 + round($maxBarHeight * (($maxValue - $i) / ($maxValue))));
                echo 'px; position: absolute;">'.$i.'</div>';

                echo '<div style="color: #888; font-size: 11px; right: 0; top: ';
                echo (2 + round($maxBarHeight * (($maxValue - $i) / ($maxValue))));
                echo 'px; position: absolute;">'.$i.'</div>';
            }

            echo '<div style="border-top: 1px solid #bbb; height: 0; top: ';
            echo $maxBarHeight.'px; width: 100%; position: absolute;"></div>';

            echo '</div>';
        }//end if

    }//end paintGraph()


}//end class

?>
