<?php
/**
 * Actions for the Bar Graph Widget (GUIGraphBar) System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUIGraphBar
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Actions Class for the Bar Graph Widget (GUIGraphBar) System.
 *
 * @since 4.0.0
 */
class GUIGraphBarActions
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
        list($providerSystem, $providerMethod) = explode(
            '::',
            $settings['dataProvider']
        );

        // Get the total height of the graph.
        $graphHeight = Util::getArrayIndex($settings, 'height', '500');
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
            if (array_key_exists('colour', $groupData) === TRUE) {
                $groupData['style'] = Util::getArrayIndex(
                    $groupData,
                    'style',
                    'diagonal'
                );

                $groupKey = $groupData['colour'].','.$groupData['style'];

                if (array_key_exists($groupKey, $usedStyles) === FALSE) {
                    $usedStyles[$groupKey] = 0;
                }

                $usedStyles[$groupKey]++;
            }//end if colour exists
        }//end foreach data group

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

                    $thisColour = $fixedColours[$colourIndex];
                    $thisStyle  = $fixedStyles[$styleIndex];
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
                }//end while colour not yet found

                // We found a free colour.
                $groupData['colour'] = $thisColour;
                $groupData['style']  = $thisStyle;
            }//end if colour exists
        }//end foreach data group

        // Unset value after having run foreach by reference with it.
        unset($groupData);

        $flags = Util::getArrayIndex($settings, 'flags', array());

        $content  = '<div id="%id%" class="%class%">';
        $content .= '<ul class="%class%-outer-list" style="height: ';
        $content .= $graphHeight.'px;">';
        $content .= '<?php ';
        $content .= 'Channels::includeSystem(\'GUIGraphBar\');';
        $content .= 'Channels::includeSystem(\''.$providerSystem.'\');';
        $content .= '$groups = '.var_export($groups, TRUE).'; ';
        $content .= '$flags  = '.var_export($flags, TRUE).'; ';
        $content .= '$data   = '.$settings['dataProvider'].'(); ';
        $content .= '$height = '.$graphHeight.'; ';
        $content .= 'GUIGraphBar::paintGraph($data, $groups, $flags, $height,';
        $content .= ' \'%id%\', \'%class%\'); ';
        $content .= '?>';
        $content .= '</ul>';
        $content .= '</div>';
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
     * @param array   $data   The data points sent from the data provider.
     * @param array   $groups Details of the data groups.
     * @param array   $flags  Details of the data flags.
     * @param integer $height Overall height of the graph (in pixels).
     * @param string  $id     HTML ID of the overall widget.
     * @param string  $class  Class prefix to use for CSS elements.
     *
     * @since  4.0.0
     * @return void
     */
    public static function paintGraph(
        array $data,
        array $groups,
        array $flags,
        $height,
        $id,
        $class
    ) {
        // Required size of a top label.
        $topLabelSize = 13;

        // Required height for the x-axis when full, and when blank.
        $xAxisHeight   = 18;
        $noXAxisHeight = 3;

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
            $dataRow['data'] = array_slice($dataRow['data'], 0, count($groups));
            $rowTotal        = array_sum($dataRow['data']);
            $maxValue        = max($maxValue, $rowTotal);

            if (array_key_exists('label', $dataRow) === TRUE) {
                if ($dataRow['label'] !== '') {
                    if ($maxValueWithLabel === NULL) {
                        $maxValueWithLabel = $rowTotal;
                    } else {
                        $maxValueWithLabel = max($maxValueWithLabel, $rowTotal);
                    }
                }
            }//end if

            $maxValue = max($maxValue, $rowTotal);
            $data[$dataIndex] = $dataRow;

            // Do we need to print an x-axis?
            if (array_key_exists('x-axis', $dataRow) === TRUE) {
                if ($dataRow['x-axis'] !== '') {
                    $hasXAxis = TRUE;
                }
            }
        }//end foreach data row

        // Allow room for the flags at the bottom of the screen.
        $flagHeight = 0;

        foreach ($flags as $flagIndex => $flag) {
            $flags[$flagIndex]['height'] = (int) Util::getArrayIndex(
                $flag,
                'height',
                8
            );

            $flagHeight += ($flags[$flagIndex]['height'] + 2);
        }

        $heightDiff = 0;

        // If we could not have a label on our tallest stacks without stretching the
        // graph, shrink the stacks until we can.
        if (($maxValueWithLabel !== NULL) && ($maxValue > 0)) {
            $labelPct  = ($maxValueWithLabel / $maxValue);
            $maxHeight = ceil(($labelPct * ($height - $flagHeight) + $topLabelSize));

            if ($maxHeight > ($height - $flagHeight)) {
                $heightDiff = ($maxHeight - ($height - $flagHeight));
            }
        }

        // Now paint it.
        $rowNumber = 0;
        foreach ($data as $dataRow) {
            $rowNumber++;

            // Calculate how high our bar is allowed to be in total.
            $rowTotal     = array_sum($dataRow['data']);
            $maxBarHeight = ($height - $flagHeight - $heightDiff);
            $totalHeight  = floor(($rowTotal / $maxValue * $maxBarHeight));

            if ($hasXAxis === TRUE) {
                $totalHeight -= $xAxisHeight;
            } else {
                $totalHeight -= $noXAxisHeight;
            }

            $barHeight = $totalHeight;

            // Make room for a label.
            if (array_key_exists('label', $dataRow) === TRUE) {
                if ($dataRow['label'] !== '') {
                    $barHeight += $topLabelSize;
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
            echo '%; height: ';
            echo $barHeight;
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

            echo '<ul style="display: block">';

            // Start stacking the graph up.
            foreach ($dataRow['data'] as $groupIndex => $dataPoint) {
                $groupUnits = Util::getArrayIndex($groups[$groupIndex], 'units', '');

                $pointTitle  = $groups[$groupIndex]['name'];
                $pointTitle .= ': '.$dataPoint.$groupUnits;

                $oldHeight  = $cumHeight;
                $cumHeight += ($dataPoint / $rowTotal * $totalHeight);

                ob_start();
                    echo '<li class="'.$class.'-data-point';
                    echo ' '.$class.'-bar-colour-'.$groups[$groupIndex]['colour'];
                    echo ' '.$class.'-bar-style-'.$groups[$groupIndex]['style'];
                    echo '" title="'.$pointTitle.'"';
                    echo ' style="height: ';
                    echo (round($cumHeight) - round($oldHeight)).'px">';
                    echo '<span class="'.$class.'-data-point-alt-text">';
                    echo $pointTitle;
                    echo '</span>';
                    echo '</li>';
                    $liTexts[] = ob_get_contents();
                ob_end_clean();
            }//end foreach data point

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
            }//end if has X axis

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
                    }//end if type of flag value

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
                        }//end if type of flag value

                        echo '</li>';
                        $liTexts[] = ob_get_contents();
                    ob_end_clean();
                }//end foreach flag

                echo implode('', $liTexts);
                echo '</ul>';
            }//end if there were flags existing

            echo '</ul>';
        }//end foreach data row

    }//end paintGraph()


}//end class

?>
