<?php
/**
 * Util methods.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Util
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Util methods.
 *
 * @since 4.0.0
 */
class Util
{


    /**
     * Returns the specified index of an array.
     *
     * If $var is not an array or $index not found then the $default value
     * will be returned.
     *
     * Note that if the value of the array index is NULL then this method will
     * return FALSE even if the index exists.
     *
     * @param mixed $var     The array.
     * @param mixed $index   The array index.
     * @param mixed $default Default value to return.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function getArrayIndex($var, $index, $default=NULL)
    {
        // Using isset instead of array_key_exists due to performance issues.
        if (is_array($var) === TRUE && isset($var[$index]) === TRUE) {
            return $var[$index];
        }

        return $default;

    }//end getArrayIndex()


    /**
     * Replace stylesheet class name and identifiers for the backend use.
     *
     * @param string $cssContents CSS to replace.
     * @param string $prefixId    ID prefix to replace (Optional).
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function replaceCSSPrefix($cssContents, $prefixId='EditingAsset')
    {
        $startIndex = 0;
        $endIndex   = 0;
        $iemac      = FALSE;
        $preserve   = FALSE;
        $i          = 0;

        // Remove all comment blocks...
        while (($startIndex = strpos($cssContents, '/*', $startIndex)) !== FALSE) {
            $preserve = strlen($cssContents) > ($startIndex + 2) && $cssContents[($startIndex + 2)] === '!';
            $endIndex = strpos($cssContents, '*/', ($startIndex + 2));
            if ($endIndex < 0) {
                if ($preserve === FALSE) {
                    $cssContents = substr($cssContents, 0, $startIndex);
                }
            } else if ($endIndex >= ($startIndex + 2)) {
                if ($cssContents[($endIndex - 1)] === '\\') {
                    // Looks like a comment to hide rules from IE Mac.
                    // Leave this comment, and the following one, alone...
                    $startIndex = ($endIndex + 2);
                    $iemac      = TRUE;
                } else if ($iemac === TRUE) {
                    $startIndex = ($endIndex + 2);
                    $iemac      = FALSE;
                } else if ($preserve === FALSE) {
                    $cssContents = substr($cssContents, 0, $startIndex).substr($cssContents, ($endIndex + 2));
                } else {
                    $startIndex = ($endIndex + 2);
                }
            }
        }//end while

        $cssContents = str_replace("\n\r", "\n", $cssContents);
        $cssContents = str_replace("\r", "\n", $cssContents);
        $cssContents = preg_replace('/\/\*[^\/]+\*\//', '', $cssContents);
        $matches     = array();
        $cssContents = preg_replace('/\n\{/', '{', $cssContents);

        $cssContents = str_replace("\n", ' ', $cssContents);
        $cssContents = str_replace('}', "}\n", $cssContents);
        $cssContents = str_replace('*/', "*/\n", $cssContents);

        preg_match_all('/^(.*)\{/m', $cssContents, $matches);

        $prefix = '#'.$prefixId;
        foreach ($matches[1] as $origMatch) {
            $match = trim($origMatch);

            $subTags      = explode(',', $match);
            $replacements = array();

            foreach ($subTags as $tag) {
                $tag = trim($tag);

                if ($tag === 'body' || $tag === 'html') {
                    $replacements[] = $prefix.' ';
                } else if ($tag === '*') {
                    $replacements[] = $prefix.' * ';
                } else {
                    $replacements[] = $prefix.' '.$tag;
                }
            }

            $replace = implode(',', $replacements);

            // Match string will be wrapped into regex expression.
            // Escapt regex special characters first.
            $match       = str_replace('*', '\*', $origMatch);
            $match       = str_replace('+', '\+', $match);
            $match       = str_replace('.', '\.', $match);
            $match       = str_replace('@', '\@', $match);
            $match       = str_replace('?', '\?', $match);
            $match       = str_replace('/', '\/', $match);
            $match       = str_replace('(', '\(', $match);
            $match       = str_replace(')', '\)', $match);
            $cssContents = preg_replace(
                '/^'.$match.'\{/m',
                $replace.' {',
                $cssContents,
                1
            );
        }//end foreach

        return $cssContents;

    }//end replaceCSSPrefix()


    /**
     * Wrapper for Javascript minification funtion.
     *
     * @param string $jsContents JS to minify.
     *
     * @since  4.0.0
     * @return string
     */
    public static function minifyJavascript($jsContents)
    {
        if (file_exists('Resources/jsmin-1.1.1.php') === TRUE) {
            include_once 'Resources/jsmin-1.1.1.php';
            // PHPCS exception for external library file.
            // @codingStandardsIgnoreStart
            return JSMin::minify($jsContents);
            // @codingStandardsIgnoreEnd
        } else {
            return $jsContents;
        }

    }//end minifyJavascript()


    /**
     * Wrapper for CSS minification funtion.
     *
     * @param string  $cssContents  CSS to minify.
     * @param integer $linebreakpos Line wrapping point.
     *
     * @since  4.0.0
     * @return string
     */
    public static function minifyCSS($cssContents, $linebreakpos=0)
    {
        if (file_exists('Resources/cssmin.php') === TRUE) {
            include_once 'Resources/cssmin.php';
            // PHPCS exception for external library file.
            // @codingStandardsIgnoreStart
            return CSSMin::minify($cssContents, $linebreakpos);
            // @codingStandardsIgnoreEnd
        } else {
            return $cssContents;
        }

    }//end minifyCSS()


    /**
     * This function returns the content between the given tag.
     *
     * @param string $tag      Tag to get the content.
     * @param string $contents HTML content.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTagContent($tag, $contents)
    {
        $startMatch = array();
        $endMatch   = array();
        preg_match(
            '/<'.$tag.'[^>]*>/i',
            $contents,
            $startMatch,
            PREG_OFFSET_CAPTURE
        );
        preg_match(
            '/<\/\s*'.$tag.'\s*>/i',
            $contents,
            $endMatch,
            PREG_OFFSET_CAPTURE
        );

        if (empty($startMatch) === TRUE) {
            $start = 0;
        } else {
            $start = $startMatch[0][1];
        }

        if (empty($endMatch) === TRUE) {
            $end = strlen($contents);
        } else {
            $end = $endMatch[0][1];
        }

        $html = substr($contents, $start, ($end - $start));
        $html = preg_replace('/<'.$tag.'[^>]*>/i', '', $html);
        $html = str_replace('</'.$tag.'>', '', $html);

        return $html;

    }//end getTagContent()


    /**
     * This function returns the list of script tag details.
     *
     * The script can take 2 types, 'external' or 'embeded'. External script tag
     * also include the source URL unlike embeded script includes Javascript source
     * code.
     *
     * @param string $html HTML content.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getScripts($html)
    {
        $scripts = array();
        $matches = array();
        $offset  = 0;
        while (preg_match('/<script[^>]*>/', $html, $matches, PREG_OFFSET_CAPTURE, $offset) !== 0) {
            $offset      = ($matches[0][1] + strlen($matches[0][0]));
            $closeTagPos = strpos($html, '</script>', $offset);
            $openTag     = $matches[0][0];

            $typeReg     = '/type="([^"]+)"/';
            $typeMatches = array();
            // Make sure the type is 'text/javascript'.
            if (preg_match($typeReg, $openTag, $typeMatches) !== 0
                && $typeMatches[1] === 'text/javascript'
            ) {
                $srcReg     = '/src="([^"]+)"/';
                $srcMatches = array();
                if (preg_match($srcReg, $openTag, $srcMatches) !== 0) {
                    // External Javascript.
                    $scripts[] = array(
                                  'type'     => 'external',
                                  'url'      => $srcMatches[1],
                                  'openTag'  => $matches[0][0],
                                  'openPos'  => $matches[0][1],
                                  'closeTag' => '</script>',
                                  'closePos' => $closeTagPos,
                                 );
                } else {
                    // Embeded Javascript.
                    $scripts[] = array(
                                  'type'     => 'embeded',
                                  'code'     => substr($html, $offset, ($closeTagPos - $offset)),
                                  'openTag'  => $matches[0][0],
                                  'openPos'  => $matches[0][1],
                                  'closeTag' => '</script>',
                                  'closePos' => $closeTagPos,
                                 );
                }//end if
            }//end if

            $matches = array();
        }//end while

        return $scripts;

    }//end getScripts()


    /**
     * This function returns the content of the linked external CSS.
     *
     * @param string $html HTML content of <head> section.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getExternalCSSContents($html)
    {
        $css   = '';
        $links = array();
        preg_match_all('/<(link[^>]*)>/', $html, $links);

        if (empty($links) === FALSE) {
            foreach ($links[1] as $link) {
                $cssLink = FALSE;
                $link    = strtolower($link);
                if (preg_match('/type=[\'"]{1}type\/css[\'"]{1}/', $link) !== 0) {
                    $cssLink = TRUE;
                }

                if ($cssLink === FALSE
                    && preg_match('/rel=[\'"]{1}stylesheet[\'"]{1}/', $link) !== 0
                ) {
                    $cssLink = TRUE;
                }

                if ($cssLink === FALSE
                    && preg_match('/rev=[\'"]{1}stylesheet[\'"]{1}/', $link) !== 0
                ) {
                    $cssLink = TRUE;
                }

                if ($cssLink === TRUE) {
                    // Check the CSS link and if the media type is neither of all or screen
                    // then dont include it if there is any.
                    $match   = array();
                    $pattern = '/media="([^"]+)"/';
                    preg_match($pattern, $link, $match);
                    if (empty($match) === FALSE) {
                        if ($match[1] !== 'all' && $match[1] !== 'screen') {
                            $cssLink = FALSE;
                        }
                    }
                }

                if ($cssLink === TRUE) {
                    $href = array();
                    preg_match('/href=[\'"]{1}([^\'"]+)[\'"]{1}/', $link, $href);
                    include_once 'Libs/Web/HTTPRequest.inc';

                    $http = new HTTPRequest($href[1]);
                    try {
                        $http->sendGetRequest();
                        $http->sendRequest();
                    } catch (Exception $e) {
                        // If this does not work, it doesn't work.
                        // We wouldn't log anything at this time.
                    }

                    $response = $http->getData();
                    if (strpos($response, '@import url(../?stylesheet=1);') !== FALSE) {
                        // This is a style from Design Customisation. We need to include
                        // the parent design together.
                        $parentDesignCssUrl = preg_replace('/\/[^\/]+\/\?stylesheet=1$/', '/?stylesheet=1', $href[1]);
                        $http               = new HTTPRequest($parentDesignCssUrl);
                        try {
                            $http->sendGetRequest();
                            $http->sendRequest();
                        } catch (Exception $e) {
                            // If this does not work, it doesn't work.
                            // We wouldn't log anything at this time.
                        }

                        $designCSS = $http->getData();
                        $css      .= $designCSS."\n";

                        // Remove @import from the parent design CSS.
                        $response = str_replace('@import url(../?stylesheet=1);', '', $response);
                    }

                    $css .= $response."\n";
                }//end if
            }//end foreach
        }//end if

        return $css;

    }//end getExternalCSSContents()


    /**
     * This function does pre-processing for the submitted HTML content.
     *
     * @param string $value The HTML content to save.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getHTML($value)
    {
        $dom     = new DomDocument('1.0', 'UTF-8');
        $content = '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
                    <html><head></head><body>'.$value.'</body></html>';

        @$dom->loadHtml($content);

        // Create XPath object.
        $xpath = new DomXpath($dom);

        // Convert the keywords that are in node attributes.
        $attributes = $xpath->query('//@keywords');
        if ($attributes->length > 0) {
            foreach ($attributes as $attribute) {
                $nodeAttrVals = json_decode($attribute->value, TRUE);
                $node         = $attribute->ownerElement;
                $node->removeAttribute('keywords');
                foreach ($nodeAttrVals as $attrName => $attrVal) {
                    $node->setAttribute($attrName, $attrVal);
                }
            }
        }//end if

        // Need to decode href and src attributes because browsers encode these
        // attributes and they might contain keywords (e.g. chars % ~).
        $linkReferences = array();
        $attributes     = $xpath->query('//@href | //@src');
        $ampersands     = array();
        if ($attributes->length > 0) {
            foreach ($attributes as $attribute) {
                $attrName     = $attribute->name;
                $ampersands[] = $attribute->name.'="'.$attribute->value.'"';
            }
        }//end if

        // Convert the keyword nodes in content.
        $keywordNodes = $xpath->query('//span[@class="_my4_keyword"]');
        if ($keywordNodes->length > 0) {
            foreach ($keywordNodes as $node) {
                $textNode = $dom->createTextNode($node->getAttribute('keyword'));
                $node->parentNode->insertBefore($textNode, $node);
                $node->parentNode->removeChild($node);
            }
        }

        $content = $dom->saveHTML();
        $value   = substr($content, 134, -16);

        // Okay. Painful handling for '&' character in remoteid type of asset.
        // Above the list of & included href or src has been collected.
        // It now needs to go back to raw '&' char to make keyword work.
        if (empty($ampersands) === FALSE) {
            foreach ($ampersands as $ampStr) {
                $value = str_replace($ampStr, rawurldecode($ampStr), $value);
            }
        }

        // Replace nonbraking space char.
        $value = str_replace(chr(194).chr(160), '&nbsp;', $value);

        // Fix stub element tags this is also done in ViperSourceView plugin.
        $r     = '/<(img|br|hr)((\s+\w+(\s*=\s*(?:".*?"|\'.*?\'|[^\'">\s]+))?)+)?\s*>/';
        $value = preg_replace($r, '<\\1\\2 />', $value);

        return $value;

    }//end getHTML()


    /**
     * Return TRUE if an array is an associative array.
     *
     * @param array $array Array to determine.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function isAssoc(array $array)
    {
        if (is_array($array) === FALSE) {
            return FALSE;
        }

        $keyComp      = array_keys(array_keys($array));
        $diffKey      = array_diff_key($array, $keyComp);
        $diffKeyCount = count($diffKey);
        if ($diffKeyCount !== 0) {
            return TRUE;
        }

        return FALSE;

    }//end isAssoc()


}//end class

?>
