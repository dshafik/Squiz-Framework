<?php
/**
 * cssmin.php
 * Author: Scott Kim - http://scott-kim.net/
 * This is a PHP port of the CSS minification tool
 * distributed with YUICompressor, itself a port 
 * of the cssmin utility by Isaac Schlueter - http://foohack.com/ 
 * Permission is hereby granted to use the JavaScript version under the same
 * conditions as the YUICompressor (original YUICompressor note below).
 */
 
/*
* YUI Compressor
* Author: Julien Lecomte - http://www.julienlecomte.net/
* Copyright (c) 2009 Yahoo! Inc. All rights reserved.
* The copyrights embodied in the content of this file are licensed
* by Yahoo! Inc. under the BSD (revised) open source license.
*/
class CSSMin {

    public static function minify($css, $linebreakpos=0) {
        $startIndex = 0;
        $endIndex = 0;
        $iemac = false;
        $preserve = false;
        $i = 0;

        // TODO: Is this CORRECT?
        // Remove all comment blocks...
        while (($startIndex = strpos($css, '/*', $startIndex)) !== false) {
            $preserve = strlen($css) > $startIndex + 2 && $css[$startIndex + 2] === '!';

            $endIndex = strpos($css, "*/", $startIndex + 2);
            if ($endIndex < 0) {
                if (!$preserve) {
                    $css = substr($css, 0, $startIndex);
                }
            } else if ($endIndex >= $startIndex + 2) {
                if ($css[$endIndex - 1] === '\\') {
                    // Looks like a comment to hide rules from IE Mac.
                    // Leave this comment, and the following one, alone...
                    $startIndex = $endIndex + 2;
                    $iemac = true;
                } else if ($iemac) {
                    $startIndex = $endIndex + 2;
                    $iemac = false;
                } else if (!$preserve) {
                    $css = substr($css, 0, $startIndex).substr($css, $endIndex + 2);
                } else {
                    $startIndex = $endIndex + 2;
                }
            }
        }

        // TODO: /\s+/g regex, g is not recognized as modifier. howto?
        // Normalize all whitespace strings to single spaces. Easier to work with that way.
        $css = preg_replace('/\s+/', ' ', $css);

        // TODO: WTF is this regex?
        // Make a pseudo class for the Box Model Hack
        $css = preg_replace('/"\\"\}\\""/', '___PSEUDOCLASSBMH___', $css);

        // Remove the spaces before the things that should not have spaces before them.
        // But, be careful not to turn "p :link {...}" into "p:link{...}"
        // Swap out any pseudo-class colons with the token, and then swap back.
        $matches = array();
        preg_match_all('/(^|\})(([^\{:])+:)+([^\{]*\{)/', $css, $matches);
        $matchesLen = count($matches[0]);
        for ($i = 0; $i < $matchesLen; $i++) {
            $replacement = str_replace(':', '___PSEUDOCLASSCOLON___', $matches[0][$i]);
            $css = str_replace($matches[0][$i], $replacement, $css);
        }
        $css = preg_replace('/\s+([!{};:>+\(\)\],])/', '$1', $css);

        // If there is a @charset, then only allow one, and push to the top of the file.
        // This will always put the very first @charset found in the file to the top.
        $matches = array();
        preg_match_all('/(.*)(@charset "[^"]*";)/i', $css, $matches);
        if (empty($matches[0]) === FALSE) {
            foreach ($matches[2] as $match) {
                str_replace($match, '', $css);
            }
            $css = $matches[2][0].$css;
        }
        //$css = preg_replace('/^(\s*@charset [^;]+;\s*)+/i', '$1', $css);

        // Put the space back in some cases, to support stuff like
        // @media screen and (-webkit-min-device-pixel-ratio:0){
        $css = preg_replace('/(@media[^{]*[^\s])\(/i', '$1 (', $css);
        $css = preg_replace('/___PSEUDOCLASSCOLON___/', ":", $css);

        // Remove the spaces after the things that should not have spaces after them.
        $css = preg_replace('/([!{}:;>+\(\[,])\s+/', '$1', $css);

        // Add the semicolon where it's missing.
        $css = preg_replace('/([^;\}])\}/', "$1;}", $css);

        // Replace 0(px,em,%) with 0.
        $css = preg_replace('/([\s:])(0)(px|em|%|in|cm|mm|pc|pt|ex)/i', "$1$2", $css);

        // Replace 0 0 0 0; with 0.
        $css = preg_replace('/:0 0 0 0;/', ':0;', $css);
        $css = preg_replace('/:0 0 0;/', ':0;', $css);
        $css = preg_replace('/:0 0;/', ':0;', $css);

        // Replace background-position:0; with background-position:0 0;
        $css = preg_replace('/background-position:0;/i', "background-position:0 0;", $css);

        // Replace 0.6 to .6, but only when preceded by : or a white-space
        $css = preg_replace('/(:|\s)0+\.(\d+)/', "$1.$2", $css);

        // Shorten colors from rgb(51,102,153) to #336699
        // This makes it more likely that it'll get further compressed in the next step.
        $matches = array();
        preg_match_all('/rgb\s*\(\s*([0-9,\s]+)\s*\)/i', $css, $matches);
        $matchesLen = count($matches[0]);
        for ($i = 0; $i < $matchesLen; $i++) {
            $rgbcolors = explode(',', $matches[1][$i]);
            $hexcolors = array();
            foreach ($rgbcolors as $rgbcolor) {
                $rgbint = (int) $rgbcolor;
                $rgbhex = dechex($rgbint).'';
                if (strlen($rgbhex) === 1) {
                    $rgbhex = '0'.$rgbhex;
                }

                $hexcolors[] = $rgbhex;
            }

            $replace = '#'.implode('', $hexcolors);
            $css = str_replace($matches[0][$i], $replace, $css);
        }

        // Shorten colors from #AABBCC to #ABC. Note that we want to make sure
        // the color is not preceded by either ", " or =. Indeed, the property
        //     filter: chroma(color="#FFFFFF");
        // would become
        //     filter: chroma(color="#FFF");
        // which makes the filter break in IE.
        $matches = array();
        preg_match_all('/([^"\'=\s])(\s*)#([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])([0-9a-f])/i', $css, $matches);
        $matchesLen = count($matches[0]);
        for ($i = 0; $i < $matchesLen; $i++) {
            if ($matches[3][$i] === $matches[4][$i]
                && $matches[5][$i] === $matches[6][$i]
                && $matches[7][$i] === $matches[8][$i]) {
                $replace = strtolower($matches[1][$i].$matches[2][$i].'#'.$matches[3][$i].$matches[5][$i].$matches[7][$i]);
            } else {
                $replace = strtolower($matches[0][$i]);
            }

            $css = str_replace($matches[0][$i], $replace, $css);
        }

        // Remove empty rules.
        $css = preg_replace('/[^\}]+\{;\}/', '', $css);
        if ($linebreakpos > 0){
            // Some source control tools don't like it when files containing lines longer
            // than, say 8000 characters, are checked in. The linebreak option is used in
            // that case to split long lines after a specific column.
            $tmp = '';
            while (strlen($css) > $linebreakpos) {
                $semicolonIdx = strpos($css, ';');
                if ($semicolonIdx > $linebreakpos) {
                    $lastIdx  = $semicolonIdx;
                } else {
                    do {
                        $lastIdx      = $semicolonIdx;
                        $semicolonIdx = strpos($css, ';', $semicolonIdx + 1);
                    } while ($semicolonIdx !== FALSE && $semicolonIdx < $linebreakpos);
                }

                $tmp .= substr($css, 0, $lastIdx + 1)."\n";
                $css = substr($css, $lastIdx + 1, strlen($css) - $lastIdx);
            }

            if (empty($css) === FALSE) {
                $tmp .= $css;
            }

            $css = $tmp;
        }

        // Replace the pseudo class for the Box Model Hack
        $css = preg_replace('/___PSEUDOCLASSBMH___/', '"\\"}\\""', $css);

        // Replace multiple semi-colons in a row by a single one
        // See SF bug #1980989
        $css = preg_replace('/;;+/', ";", $css);

        // Trim the final string (for any leading or trailing white spaces)
        $css = preg_replace('/^\s+|\s+$/', "", $css);

        return $css;
    }

}

?>