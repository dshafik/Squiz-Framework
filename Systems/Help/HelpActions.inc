<?php
/**
 * Actions for the Help System.
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
 * @subpackage Help
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Help System Actions Class.
 */
class HelpActions
{

    /**
     * Array that keeps track of converted files during parsing.
     *
     * @var array
     */
    private static $_convertedFiles = array();

    /**
     * Array that keeps track of imported files during parsing.
     *
     * This array is used to detect import tag recursion. Where A includes B and B
     * includes A, etc.
     *
     * @var array
     */
    private static $_importing = array();

    /**
     * List of glossary terms and the articles that reference them.
     *
     * @var array
     */
    private static $_glossaryUsage = array();

    /**
     * List of artile titles.
     *
     * @var array
     */
    private static $_articleTitles = array();

    /**
     * Infromation about the documentation files.
     *
     * @var array
     */
    private static $_docsInfo = array();

    /**
     * Sections that are displayed on the template index page.
     *
     * @var array
     */
    private static $_sections = array();

    /**
     * Path to the general articles file.
     *
     * @var string
     */
    private static $_generalArticlesFilePath = NULL;

    /**
     * Widget id and class references for glossary terms.
     *
     * @var string
     */
    private static $_widgetReferences = NULL;

    /**
     * List of general terms that appear in all glossary index pages.
     *
     * @var array
     */
    private static $_generalGlossaryTerms = array();


    /**
     * Returns the path to the documentation's directory.
     *
     * @return string
     */
    public static function getDocsDirectory()
    {
        $dir = BaseSystem::getDataDir('Help').'/Docs/';
        return $dir;

    }//end getDocsDirectory()


    /**
     * Create documentation.
     *
     * @return void
     */
    public static function createDocs()
    {
        // Set the valid sections for template index page.
        self::$_sections = array(
                            'tasks'           => _('Tasks'),
                            'troubleshooting' => _('Troubleshooting'),
                           );

        self::$_widgetReferences = array(
                                    'elemid'    => array(),
                                    'elemclass' => array(),
                                   );

        // Get the general articles file.
        self::$_generalArticlesFilePath = self::_getGeneralArticleFilePath();

        include_once 'Libs/FileSystem/FileSystem.inc';

        // Create the base help data dir.
        $dataDir = Help::getDocsDirectory();
        if (file_exists($dataDir) === FALSE) {
            FileSystem::mkdir($dataDir, 0755, TRUE);
        } else {
            FileSystem::clearDirectory($dataDir, FALSE);
        }

        $helpDirs = array();
        $dom      = Channels::getSystemXml();
        $xpath    = new DOMXPath($dom);

        // Get the exclusive systems and move the implementor docs to
        // exclusive system.
        $systems = $xpath->query('/systems/system');
        foreach ($systems as $system) {
            $helpDirs[$system->getAttribute('name')] = $system->getAttribute('path').'/Docs';
        }

        $systems = $xpath->query('/systems/system[@exclusive="true"]');
        foreach ($systems as $system) {
            $impPath = $helpDirs[$system->getAttribute('implementor')];

            $helpDirs[$system->getAttribute('name')] = $impPath;
            unset($helpDirs[$system->getAttribute('implementor')]);
        }

        $systemsWithDocs = array();
        foreach ($helpDirs as $system => $dir) {
            if (file_exists($dir) === TRUE) {
                $systemsWithDocs[$system] = $dir;
            }
        }

        // Sort by system name.
        ksort($systemsWithDocs);

        // Put list of all HTML files in all systems in to a structured array.
        $docs = array();
        foreach ($systemsWithDocs as $systemName => $docsPath) {
            $mainDocs = FileSystem::listDirectory($docsPath, array('.html'), FALSE, FALSE);
            $docs[$systemName] = array(
                                  'path'         => $docsPath,
                                  'mainDocs'     => $mainDocs,
                                  'templateDocs' => array(),
                                 );

            $templates = FileSystem::listDirectories($docsPath.'/Templates');
            foreach ($templates as $templatePath) {
                $templateDocs = FileSystem::listDirectory($templatePath, array('.html'), FALSE, FALSE);
                $docs[$systemName]['templateDocs'][basename($templatePath)] = $templateDocs;
            }
        }

        self::_parseDocs($docs);
        self::_addGlossaryUsageToGlossaryFiles();
        self::_createIndexPages();
        self::_createHelpSystemsMenu();

    }//end createDocs()


    /**
     * Prints the Help doc page for the specified page.
     *
     * @param string $pageid          Id of the page to load.
     * @param array  $templateLineage Lineage of the active screen. Used to determine
     *                                if user needs to change screens to view article.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function getPage($pageid, array $templateLineage=array())
    {
        $filePath = Help::getDocsDirectory().'/'.$pageid;
        if (strpos($pageid, '.html') === FALSE) {
            $filePath .= '.html';
        }

        if (file_exists($filePath) === FALSE) {
            Channels::includeSystem('GUI');
            $cssHref   = self::_getPageCSSPath();
            $contents  = GUI::getDocType();
            $contents .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
            $contents .= '</head><body class="Help-iframe pageNotFound">';

            ob_start();
            $msg = _('There are no help articles for the screen that you are currently viewing.');
            Help::printMessageBox($msg, '', 'info', TRUE);
            $contents .= ob_get_contents();
            ob_end_clean();

            $contents .= '</html></body>';
            return $contents;
        } else {
            $contents = file_get_contents($filePath);

            if (strpos($pageid, 'article-') === 0) {
                // Article might belong to a different template and use may need to
                // switch to that screen, show the steps to get to the correct template.
                // Get the articles template.
                $parts           = explode('-', $pageid);
                $articleTemplate = $parts[1].':'.$parts[2];

                // Reverse the lineage so we start from bottom.
                $templateLineage = array_reverse($templateLineage);

                $path = FALSE;
                foreach ($templateLineage as $tpl) {
                    // Get the lineage for the current active screen/mode up to the parent.
                    $path = Help::getTemplateLineages($articleTemplate, $tpl);
                    if ($path !== FALSE) {
                        break;
                    }
                }

                if ($path !== NULL && empty($path) === FALSE) {
                    $parentTemplate = array_pop($path);
                    $path           = array_reverse($path);
                    $steps          = array();
                    foreach ($path as $template) {
                        $info    = self::_getTemplateInfo($template);
                        $steps[] = array(
                                    'title'         => $info['label'],
                                    'template'      => $template,
                                    'ownerWidgetid' => $info['parents'][$parentTemplate],
                                   );

                        $parentTemplate = $template;
                    }

                    if (empty($steps) === FALSE) {
                        // Found valid path to the template, add template switching code.
                        $msg = _('In order to understand this article, please perform the following steps');
                        ob_start();
                            self::_printStepsBox($msg, _('Hide this message'), $steps);
                        $boxContent = ob_get_contents();
                        ob_end_clean();
                        $contents = str_replace('<h1', $boxContent.'<h1', $contents);
                    }
                }//end if
            }//end if
        }//end if

        return $contents;

    }//end getPage()


    /**
     * Returns the contents of the Help popup systems menu dropdown.
     *
     * @return string
     */
    public static function getSystemsMenu()
    {
        $filePath = Help::getDocsDirectory().'/_systems_menu.html';
        return file_get_contents($filePath);

    }//end getSystemsMenu()


    /**
     * Returns info about the glossary term for specified element ids and/or class names.
     *
     * If a glossary term is found then the return value will be in following format:
     *    termid|matched-element-id|matched-element-className
     * And if there is no match then a message is returned.
     *
     * @param array $ids        List of DOMElement ids.
     * @param array $classNames List of DOMElement class names.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function findArticleForElement(array $ids, array $classNames)
    {
        $docsDir      = Help::getDocsDirectory();
        $glossaryid   = NULL;
        $matchedId    = NULL;
        $matchedClass = NULL;

        // Need to search each id individually so that we get the most relevant result.
        foreach ($ids as $index => $id) {
            if (empty($id) === FALSE) {
                $id             = preg_replace('/[^a-z0-9_-\s]/i', '', $id);
                $cmd            = 'grep -liE ".*<h1 elemid=\"'.$id.'\".*" '.$docsDir.'* | grep -E ".*/glossary-.*"';
                $contentResults = array();
                exec($cmd, $contentResults);
                if (empty($contentResults) === FALSE) {
                    // Found the term.
                    $glossaryid = basename(str_replace('.html', '', $contentResults[0]));
                    $matchedId  = $id;
                    break;
                }
            }

            $className = Util::getArrayIndex($classNames, $index, '');
            $className = preg_replace('/[^a-z0-9_-\s]/i', '', $className);
            if (empty($className) === TRUE) {
                continue;
            }

            // Search for the class names.
            $cmd            = 'grep -liE ".*<h1 elemclass=\".*'.$className.'.*\".*" '.$docsDir.'* | grep -E ".*/glossary-.*"';
            $contentResults = array();
            exec($cmd, $contentResults);
            if (empty($contentResults) === FALSE) {
                // Found the term.
                $glossaryid   = basename(str_replace('.html', '', $contentResults[0]));
                $matchedClass = $className;
                break;
            } else {
                // Search for each class name individually.
                $pipedClasses   = str_replace(' ', '|', $className);
                $cmd            = 'grep -liE ".*<h1 elemclass=\".*('.$pipedClasses.').*\".*" '.$docsDir.'* | grep -E ".*/glossary-.*"';
                $contentResults = array();
                exec($cmd, $contentResults);
                if (empty($contentResults) === FALSE) {
                    // Found the term.
                    $glossaryid   = basename(str_replace('.html', '', $contentResults[0]));
                    $matchedClass = $className;
                    break;
                }
            }
        }//end foreach

        if ($glossaryid === NULL) {
            return _('Not found');
        }

        $resultStr = $glossaryid.'|'.$matchedId.'|'.$matchedClass;

        return $resultStr;

    }//end findArticleForElement()


    /**
     * Prints Help message box.
     *
     * @param string  $message         The message to display.
     * @param string  $closeButtonText The text displayed next to close icon.
     * @param string  $messageType     The message type.
     * @param boolean $inIframe        If TRUE then the message will be added to iframe body.
     *
     * @return void
     */
    public static function printMessageBox($message, $closeButtonText, $messageType='info', $inIframe=FALSE)
    {
        echo '<div class="Help-message"';

        if ($inIframe === TRUE) {
            echo ' iniframe="true"';
        }

        echo '>';

        // Top message section, displays message type icon and the message.
        echo '<div class="Help-message-top type-'.$messageType.'">';
        echo '<div class="Help-message-top-msg">';
        echo $message;
        echo '</div>';
        echo '</div>';

        if (empty($closeButtonText) === FALSE) {
            // Message footer (shows the close icon).
            echo '<div class="Help-message-footer">';
            echo '<a class="Help-message-close" href="javascript:window.parent.Help.hideMessages('.var_export($inIframe, TRUE).');" title="'.$closeButtonText.'">'.$closeButtonText.'</a>';
            echo '</div>';
        }

        echo '</div>';

    }//end printMessageBox()


    /**
     * Prints Help message box.
     *
     * @param string $message         The message to display.
     * @param string $closeButtonText The text displayed next to close icon.
     * @param array  $steps           List of steps to perform.
     *
     * @return void
     */
    private static function _printStepsBox($message, $closeButtonText, array $steps)
    {
        echo '<div class="Help-message stepsBox Help-iframe-msg"';
        echo ' iniframe="true"';
        echo '>';

        // Top message section, displays message type icon and the message.
        echo '<div class="Help-message-top type-info">';
        echo '<div class="Help-message-top-msg">';
        echo $message;
        echo '</div>';

        echo '<div class="Help-message-stepsList">';
        foreach ($steps as $i => $step) {
            echo '<div class="Help-message-step step-'.($i + 1).'" template="'.$step['template'].'">';
            printf(_('Select %s'), $step['title']);
            echo '<a href="javascript:window.parent.Help.showMe(\''.$step['ownerWidgetid'].'\', \''.$step['template'].'\');">'._('Show me').'</a>';
            echo '</div>';
        }

        echo '</div>';
        echo '</div>';

        if (empty($closeButtonText) === FALSE) {
            // Message footer (shows the close icon).
            echo '<div class="Help-message-footer">';
            echo '<a class="Help-message-close" href="javascript:window.parent.Help.hideMessages(true);" title="'.$closeButtonText.'">'.$closeButtonText.'</a>';
            echo '</div>';
        }

        echo '</div>';

    }//end _printStepsBox()


    /**
     * Searches docs and returns the results as a sarch result page.
     *
     * @param string  $searchString String to search for (space separated).
     * @param integer $startOffset  The search result offset.
     *
     * @return string
     *
     * @api            read
     * @api-permission public
     */
    public static function searchDocs($searchString, $startOffset=0)
    {
        $docsDir = Help::getDocsDirectory();

        $wordList          = array_unique(explode(' ', $searchString));
        $originalWordsList = array();
        foreach ($wordList as $index => $word) {
            $word = preg_replace('/[^a-z0-9_-]/i', '', $word);
            $len  = strlen($word);
            if ($len > 3) {
                // Only keep words that has more than 3 characters and remove the
                // last character from the word. This is a simple way to match plurals
                // (e.g. "goals" will match "goal" and "test" will match "tes").
                $originalWordsList[] = $word;
                $wordList[$index]    = substr($word, 0, -1);
            } else if ($len < 3) {
                unset($wordList[$index]);
            } else {
                $originalWordsList[] = $word;
            }
        }

        if (empty($wordList) === TRUE) {
            return self::_getSearchResultsPage($wordList, $originalWordsList);
        }

        $words    = implode('|', $wordList);
        $sedWords = implode('\|', $wordList);

        // Search articles and glossary file contents.
        $cmd = 'grep -iE ".*('.$words.').*" '.$docsDir.'* | sed -n -e "s/<[^>]*>//g" -e "/'.$sedWords.'/p" | awk -F : \'{print $1}\' | uniq | grep -E ".*/(article|glossary)-.*"';

        $contentResults = array();
        exec($cmd, $contentResults);

        // Search articles and glossary file names.
        $cmd = 'find '.$docsDir.' -regextype posix-egrep -iregex ".*/(article|glossary)-.*('.$words.').*" -print';

        $fileNames = array();
        exec($cmd, $fileNames);

        $rawSearchResults = array();
        foreach ($contentResults as $filePath) {
            $matches    = array();
            $contents   = file_get_contents($filePath);
            $contents   = strip_tags($contents);
            $numMatches = preg_match_all('/.*\b('.$words.').*/imU', $contents, $matches);
            $numWords   = str_word_count($contents);
            $score      = ((ceil($numMatches / ($numWords * 100)) * 2) + $numMatches);

            $rawSearchResults[basename($filePath)] = $score;
        }

        $fileScore = 5;
        foreach ($fileNames as $filePath) {
            $fileName   = basename($filePath);
            $matches    = array();
            $numMatches = preg_match_all('/.*('.$words.').*/iU', $fileName, $matches);

            if (isset($rawSearchResults[$fileName]) === FALSE) {
                $rawSearchResults[$fileName] = ($numMatches * $fileScore);
            } else {
                $rawSearchResults[$fileName] += ($numMatches * $fileScore);
            }
        }

        if (empty($rawSearchResults) === TRUE) {
            return self::_getSearchResultsPage($wordList, $originalWordsList);
        }

        arsort($rawSearchResults);

        $contents = self::_getSearchResultsPage($searchString, $originalWordsList, $rawSearchResults, $startOffset);
        return $contents;

    }//end searchDocs()


    /**
     * Prints the search results page using the given results list.
     *
     * @param string  $searchString String to search for (space separated).
     * @param array   $words        List of searched words.
     * @param array   $results      List of search results.
     * @param integer $startOffset  The search result offset.
     *
     * @return string
     */
    private static function _getSearchResultsPage($searchString, array $words, array $results=array(), $startOffset=0)
    {
        $startOffset    = (int) $startOffset;
        $maxResults     = 10;
        $totalResults   = count($results);
        $results        = array_slice($results, $startOffset, $maxResults, TRUE);
        $hasMoreResults = FALSE;

        if (($totalResults - ($startOffset + $maxResults)) > 0) {
            $hasMoreResults = TRUE;
        }

        // For each result file open it and retrieve the article details.
        $dom     = new DomDocument();
        $docsDir = Help::getDocsDirectory();

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();

        if ($startOffset === 0) {
            $contents  = GUI::getDocType();
            $contents .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
            $contents .= '</head><body class="Help-iframe searchResultsPage">';
        }

        if (empty($results) === TRUE) {
            if ($startOffset === 0) {
                $contents .= '<h3 class="Help-noSearchResults">'._('No search results').'</h3>';
            }
        } else {
            // Options for the 2nd argument of Help.loadPage function.
            $opts = urlencode(json_encode(array('searchedWords' => $words)));

            if ($startOffset === 0) {
                // Show number of results.
                $contents .= sprintf(
                    _('<h3>%s results found for <strong>%s</strong></h3>'),
                    $totalResults,
                    $searchString
                );
            }

            $contents .= '<ol id="Help-searchResults-list" class="Help-searchResults-list">';

            foreach ($results as $fileName => $score) {
                $dom->loadHTMLFile($docsDir.$fileName);

                $id      = str_replace('.html', '', basename($fileName));
                $title   = $dom->getElementsByTagName('h1')->item(0)->nodeValue;
                $summary = '';

                $articleType = 'article';
                if (strpos($id, 'glossary-') === 0) {
                    $articleType = 'glossary';
                    $termDesc    = $dom->getElementsByTagName('div')->item(0);
                    $summary     = strip_tags($termDesc->nodeValue);
                } else {
                    $metaTags = $dom->getElementsByTagName('meta');
                    foreach ($metaTags as $metaTag) {
                        if ($metaTag->getAttribute('name') === 'summary') {
                            $summary = $metaTag->getAttribute('content');
                        }
                    }
                }

                include_once 'Libs/String/String.inc';
                $summary = String::ellipsisize($summary, 100);

                $href      = 'javascript:parent.window.Help.loadPage(\''.$id.'\', '.$opts.');';
                $contents .= '<li class="Help-searchResult '.$articleType.'">';
                $contents .= '<h4><a href="'.$href.'">'.$title.'</a></h4>';
                $contents .= '<div class="Help-searchResult-summary">'.$summary.'</div>';
                $contents .= '</li>';
            }//end foreach
        }//end if

        $contents .= '</ol>';

        if ($hasMoreResults === TRUE) {
            $href      = 'javascript:parent.window.Help.getSearchResults(\''.$searchString.'\', '.($startOffset + $maxResults).');';
            $contents .= '<p class="Help-searchResults-moreLink"><a href="'.$href.'">'._('Show more results').'</a></p>';
        }

        if ($startOffset === 0) {
            $contents .= '</body></html>';
        }

        return $contents;

    }//end _getSearchResultsPage()


    /**
     * Returns template information.
     *
     * @param string $template Name of the template (e.g. System:template).
     *
     * @return array
     */
    private static function _getTemplateInfo($template)
    {
        $templates = array();
        $tplPath   = BaseSystem::getDataDir('Help').'/tplPaths.inc';
        if (file_exists($tplPath) === TRUE) {
            include $tplPath;
        }

        if (isset($templates[$template]) === TRUE) {
            return $templates[$template];
        }

        return array();

    }//end _getTemplateInfo()


    /**
     * Returns the parent templates for a specified template.
     *
     * @param string $template Name of the template (e.g. System:template).
     *
     * @return array
     */
    private static function _getTemplateParents($template)
    {
        $templates = array();
        $tplPath   = BaseSystem::getDataDir('Help').'/tplPaths.inc';
        if (file_exists($tplPath) === TRUE) {
            include $tplPath;
        }

        if (isset($templates[$template]) === TRUE) {
            return array_keys($templates[$template]['parents']);
        }

        return array();

    }//end _getTemplateParents()


    /**
     * Returns template lineage between two specified templates.
     *
     * @param string $template         Name of the template (e.g. System:template).
     * @param string $topLevelTemplate Name of the template (e.g. System:template).
     *
     * @return array
     */
    public static function getTemplateLineages($template, $topLevelTemplate)
    {
        $lineage = array($template);
        if ($template === $topLevelTemplate) {
            return $lineage;
        }

        $parents = self::_getTemplateParents($template);

        if (empty($parents) === TRUE) {
            return FALSE;
        }

        $before = count($lineage);
        foreach ($parents as $parent) {
            $parentLin = Help::getTemplateLineages($parent, $topLevelTemplate);
            if ($parentLin !== FALSE) {
                $lineage = array_merge($lineage, $parentLin);
                return $lineage;
            }
        }

        if (empty($lineage) === TRUE || $before === count($lineage)) {
            return FALSE;
        }

        return $lineage;

    }//end getTemplateLineages()


    /**
     * Adds specified template to template lineage list.
     *
     * Note: template lineage related methods are used to display screen switching
     * steps that appear in article pages.
     *
     * @param string $childTemplate  Name of the template (e.g. System:template).
     * @param string $parentTemplate The name of the template parent template.
     * @param string $widgetid       Id of the parent template widget.
     * @param string $label          Label for the template.
     *
     * @return void
     */
    public static function addTemplateToLineage($childTemplate, $parentTemplate=NULL, $widgetid='', $label='')
    {
        $templates = array();
        $tplPath   = BaseSystem::getDataDir('Help').'/tplPaths.inc';
        if (file_exists($tplPath) === TRUE) {
            include $tplPath;
        }

        if (isset($templates[$childTemplate]) === FALSE) {
            $templates[$childTemplate] = array(
                                          'label'   => $label,
                                          'parents' => array(),
                                         );
        }

        if ($parentTemplate !== NULL) {
            $templates[$childTemplate]['parents'][$parentTemplate] = $widgetid;
        }

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::arrayToFile($templates, 'templates', $tplPath);

    }//end addTemplateToLineage()


    /**
     * Parses specified Help articles.
     *
     * @param array $docs Array of Help files.
     *
     * @return void
     * @throws ChannelException If cannot be parsed.
     */
    private static function _parseDocs(array $docs)
    {
        // First convert docs in Systems directories and place the new files in
        // data/Help/Docs/.
        $files = array();
        foreach ($docs as $system => $info) {
            $path     = $info['path'];
            $idPrefix = $system;

            $parentGlossaryFile = NULL;
            foreach ($info['mainDocs'] as $fileName) {
                $srcFile = $path.'/'.$fileName;
                if ($fileName === 'glossary.html') {
                    $parentGlossaryFile = $path.'/'.$fileName;
                    self::_parseGlossaryFile('glossary-'.$idPrefix.'-', $srcFile);
                } else {
                    self::_parseArticleFile('article-'.$idPrefix.'-', $srcFile);
                }
            }

            // Templates.
            foreach ($info['templateDocs'] as $templateName => $templates) {
                foreach ($templates as $fileName) {
                    $idPrefix = $system.'-'.$templateName;
                    $tplPath  = $path.'/Templates/'.$templateName;
                    $srcFile  = $tplPath.'/'.$fileName;
                    if ($fileName === 'glossary.html') {
                        self::_parseGlossaryFile('glossary-'.$idPrefix.'-', $srcFile, $parentGlossaryFile);
                    } else {
                        self::_parseArticleFile('article-'.$idPrefix.'-', $srcFile, TRUE);
                    }
                }
            }
        }//end foreach

        // Now convert all special Help tags in the new files recursively.
        try {
            include_once 'Libs/FileSystem/FileSystem.inc';
            $dataDir = Help::getDocsDirectory();
            $files   = FileSystem::listDirectory($dataDir, array('.html'));
            $doc     = new DomDocument();
            foreach ($files as $file) {
                @$doc->loadHTMLFile($file);
                self::_convertTags($doc, basename($file));
                $doc->saveHTMLFile($file);
                self::$_convertedFiles[] = $file;
            }
        } catch (ChannelException $e) {
            $msg = sprintf(_('%s (Exception while parsing %s)'), $e->getMessage(), $file);
            throw new ChannelException($msg);
        }

    }//end _parseDocs()


    /**
     * Parse a single glossary file.
     *
     * @param string $idPrefix           The prefix to use (e.g. <articleType>-<system>-<tamplte>).
     * @param string $glossaryFile       The glossary file to parse.
     * @param string $parentGlossaryFile If specified then the glossary terms from this
     *                                   file will be imported.
     *
     * @return void
     * @throws ChannelException If cannot be parsed.
     */
    private static function _parseGlossaryFile($idPrefix, $glossaryFile, $parentGlossaryFile=NULL)
    {
        $fileContents = trim(file_get_contents($glossaryFile));
        if (empty($fileContents) === TRUE) {
            $msg = sprintf(_('Empty glossary file: %s'), $glossaryFile);
            throw new ChannelException($msg);
        }

        if ($parentGlossaryFile !== NULL) {
            // Import parent glossary file.
            $fileContents .= file_get_contents($parentGlossaryFile);
        }

        $idParts = explode('-', $idPrefix);

        $doc = new DomDocument();
        @$doc->loadHTML($fileContents);

        $titles = array();
        $terms  = $doc->getElementsByTagName('h1');
        foreach ($terms as $term) {
            $glossaryid    = NULL;
            $glossaryTitle = trim($term->nodeValue);

            // The defualt id is the title of the glossary term.
            if ($term->hasAttribute('id') === TRUE) {
                $glossaryid = trim($term->getAttribute('id'));
            } else {
                $glossaryid = $glossaryTitle;
            }

            // If the glossaryid is empty then throw an exception.
            if (empty($glossaryid) === TRUE) {
                $msg = '';
                if ($parentGlossaryFile !== NULL) {
                    $msg = _('Invalid glossary ID in: %s or %s');
                    throw new ChannelException(sprintf($msg, $glossaryFile, $parentGlossaryFile));
                } else {
                    $msg = _('Invalid glossary ID in: %s');
                    throw new ChannelException(sprintf($msg, $glossaryFile));
                }
            }

            // Create a glossary term file in data dir which will be shown to the user.
            $termPath = '';
            $termDoc  = new DomDocument();
            Channels::includeSystem('GUI');
            $cssHref = self::_getPageCSSPath();
            $html    = GUI::getDocType();
            $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
            $html   .= '</head><body class="Help-iframe glossaryPage">';
            $html   .= self::_getSetCurrentSystemScript($idParts[1]);
            $html   .= '</body></html>';
            @$termDoc->loadHTML($html);
            $bodyTag = $termDoc->getElementsByTagName('body')->item(0);

            $contentWrapper = NULL;
            $node           = $term;
            do {
                $importedNode = $termDoc->importNode($node, TRUE);

                // Inside the H1 element we need to add the element locator.
                if ($importedNode->tagName === 'h1') {
                    $elemLocator = self::_createElementLocator(
                        $termDoc,
                        $term->getAttribute('elemid'),
                        $term->getAttribute('elemclass')
                    );

                    if ($elemLocator !== NULL) {
                        $importedNode->appendChild($elemLocator);
                    }

                    $bodyTag->appendChild($importedNode);

                    // Create the content contentWrapper.
                    $contentWrapper = $termDoc->createElement('div');
                    $contentWrapper->setAttribute('id', 'help-article-wrapper');
                    $bodyTag->appendChild($contentWrapper);
                } else if ($contentWrapper === NULL) {
                    throw new ChannelException(_('Found content before H1 element'));
                } else {
                    $contentWrapper->appendChild($importedNode);
                }//end if

                // Loop until another term is found (h1 tag).
                $node = $node->nextSibling;
            } while ($node && ($node->nodeType !== XML_ELEMENT_NODE || $node->tagName !== 'h1'));

            // Term file destination.
            $dataDir  = Help::getDocsDirectory();
            $fileName = $idPrefix.strtolower(preg_replace('/[^a-zA-Z0-9_]/', '_', $glossaryid));
            $destFile = $dataDir.'/'.$fileName.'.html';

            if ($term->hasAttribute('elemid') === TRUE) {
                self::$_widgetReferences['elemid'][$fileName] = $term->getAttribute('elemid');
            } else if ($term->hasAttribute('elemclass') === TRUE) {
                self::$_widgetReferences['elemclass'][$fileName] = $term->getAttribute('elemclass');
            }

            $glossaryInfo = array(
                             'id'    => $fileName,
                             'title' => $glossaryTitle,
                            );
            self::_addGlossaryToIndex($fileName, $glossaryInfo);

            // Is it a general term?
            if ($term->getAttribute('general') === 'true') {
                self::$_generalGlossaryTerms[$glossaryTitle] = $fileName;
            }

            if (file_exists($destFile) === TRUE) {
                throw new ChannelException(sprintf(_('Duplicate glossary term found in: %s and id: %s (May be in parent glossary file?)'), $glossaryFile, $glossaryid));
            }

            $termDoc->saveHTMLFile($destFile);
        }//end foreach

    }//end _parseGlossaryFile()


    /**
     * Parse a single Help article file.
     *
     * @param string  $idPrefix          The prefix to use (e.g. <articleType>-<system>-<tamplte>).
     * @param string  $articleFile       The article file to parse.
     * @param boolean $isTemplateArticle If TRUE then article is inside a template Docs folder.
     *
     * @return void
     * @throws ChannelException If the article cannot be parsed.
     */
    private static function _parseArticleFile($idPrefix, $articleFile, $isTemplateArticle=FALSE)
    {
        $fileContents = trim(file_get_contents($articleFile));
        if (empty($fileContents) === TRUE) {
            $msg = sprintf(_('Empty article file: %s'), $articleFile);
            throw new ChannelException($msg);
        }

        $isGeneralPage     = FALSE;
        $isIndexPage       = FALSE;
        $isSystemIndexPage = FALSE;
        if (basename($articleFile) === 'index.html') {
            $isIndexPage = TRUE;
            if ($isTemplateArticle === FALSE) {
                $isSystemIndexPage = TRUE;
            }
        } else if ($articleFile === self::$_generalArticlesFilePath) {
            $isGeneralPage = TRUE;
        }

        $idParts = explode('-', $idPrefix);

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html    = '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" /></head>';
        $html   .= '<body class="Help-iframe articlePage">';
        $html   .= self::_getSetCurrentSystemScript($idParts[1]);
        $html   .= '<div id="help-article-wrapper">';
        $html   .= $fileContents;
        $html   .= '</div></body></html>';

        $doc = new DomDocument();
        @$doc->loadHTML($html);

        // Get the title.
        $titleTags = $doc->getElementsByTagName('h1');
        if ($titleTags->length === 0) {
            throw new ChannelException(sprintf(_('Article does not have a title (h1 tag): %s.'), $articleFile));
        }

        $titleTag = $titleTags->item(0);

        // Move the title tag outside of the help-article-wrapper (for importing).
        $wrapperElement = $doc->getElementsByTagName('div')->item(0);
        $wrapperElement->parentNode->insertBefore($titleTag, $wrapperElement);

        $title     = $titleTag->nodeValue;
        $articleid = strtolower(preg_replace('/[^a-zA-Z0-9_]/', '_', str_replace('.html', '', basename($articleFile))));

        // Get the info tag.
        $infoTags = $doc->getElementsByTagName('info');
        if ($isSystemIndexPage === FALSE
            && $isGeneralPage === FALSE
            && $infoTags->length === 0
        ) {
            throw new ChannelException(sprintf(_('Article file does not have "info" tag: %s.'), $articleFile));
        }

        // If not an index page then it must have show-in tag.
        $showIn = array();
        if ($isIndexPage === FALSE && $isGeneralPage === FALSE) {
            self::$_articleTitles[$idPrefix.$articleid] = $title;

            $showInTag = $doc->getElementsByTagName('show-in');
            if ($showInTag->length === 0) {
                throw new ChannelException(sprintf(_('Article file does not have "show-in" tag: %s.'), $articleFile));
            }

            $showIn = explode(',', $showInTag->item(0)->nodeValue);

            // Validate show locations.
            if (empty($showIn) === TRUE) {
                throw new ChannelException(sprintf(_('Article does not have show-in values: %s'), $articleFile));
            }

            foreach ($showIn as $index => $showLocation) {
                $showLocation = trim(strtolower($showLocation));
                if (isset(self::$_sections[$showLocation]) === TRUE) {
                    $showIn[$index] = $showLocation;
                } else {
                    throw new ChannelException(sprintf(_('Invalid show-in value "%s" in %s'), $showLocation, $articleFile));
                }
            }
        }//end if

        // Get the summary if there is one.
        $summary     = '';
        $summaryTags = $doc->getElementsByTagName('summary');
        if ($summaryTags->length > 1) {
            throw new ChannelException(sprintf(_('Article has more than 1 summary tag: %s.'), $articleFile));
        } else if ($summaryTags->length === 1) {
            $summary = trim($summaryTags->item(0)->nodeValue);
        } else {
            //throw new ChannelException(sprintf(_('Article must have a summary tag: %s.'), $articleFile));
        }

        $screenTitle     = '';
        $screenTitleTags = $doc->getElementsByTagName('screen_title');
        if ($screenTitleTags->length > 0) {
            $screenTitle = trim($screenTitleTags->item(0)->nodeValue);
        }

        // Check if this System should be shown in the menu.
        $showInMenu = FALSE;
        if ($isSystemIndexPage === TRUE) {
            $showInMenu     = TRUE;
            $showInMenuTags = $doc->getElementsByTagName('show-in-menu');
            if ($showInMenuTags->length > 0 && $showInMenuTags->item(0)->nodeValue === 'false') {
                $showInMenu = FALSE;
            }
        }

        // Save the file in data dir.
        $dataDir  = Help::getDocsDirectory();
        $destFile = '';

        if ($isIndexPage === TRUE) {
            // The index pages are prefixed with index-.
            $destFile = $dataDir.trim(str_replace('article-', 'index-', $idPrefix), '-').'.html';
        } else if ($isGeneralPage === TRUE) {
            $destFile = $dataDir.'general.html';
        } else {
            $destFile = $dataDir.'/'.$idPrefix.$articleid.'.html';
        }

        $articleInfo = array(
                        'id'          => $idPrefix.$articleid,
                        'title'       => $title,
                        'showIn'      => $showIn,
                        'summary'     => $summary,
                        'screenTitle' => $screenTitle,
                        'showInMenu'  => $showInMenu,
                       );
        self::_addArticleToIndex($idPrefix.$articleid, $articleInfo, $isIndexPage);

        if (file_exists($destFile) === TRUE) {
            throw new ChannelException(sprintf(_('Duplicate article found in: %s.'), $articleFile));
        }

        // Remove the info tag.
        if ($infoTags->length !== 0) {
            $infoTags->item(0)->parentNode->removeChild($infoTags->item(0));
        }

        // Add the summary to HTML doc as a meta tag.
        $summaryMetaTag = $doc->createElement('meta');
        $summaryMetaTag->setAttribute('name', 'summary');
        $summaryMetaTag->setAttribute('content', $summary);
        $doc->getElementsByTagName('head')->item(0)->appendChild($summaryMetaTag);

        $doc->saveHTMLFile($destFile);

    }//end _parseArticleFile()


    /**
     * Returns the element that points to a widget when clicked.
     *
     * @param DomDocument $dom       The DOMDocument object that is going to use the new element.
     * @param string      $elemId    The id of the widget to point to.
     * @param string      $elemClass The id of the widget to point to.
     *
     * @return DomNode
     */
    private static function _createElementLocator(DomDocument $dom, $elemId=NULL, $elemClass=NULL)
    {
        $iconEl = $dom->createElement('img');
        $iconEl->setAttribute('title', _('Click to show location on screen'));

        if (empty($elemId) === FALSE) {
            $iconEl->setAttribute('elemid', $elemId);
        } else if (empty($elemClass) === FALSE) {
            $iconEl->setAttribute('elemclass', $elemClass);
        } else {
            return NULL;
        }

        $iconEl->setAttribute(
            'class',
            'Help-locator-img'
        );

        Channels::includeSystem('GUI');
        $iconEl->setAttribute(
            'src',
            '/__web/'.GUI::getSystemWebPath('Help').'/Templates/Help/link_locator.png'
        );

        $iconEl->setAttribute(
            'onclick',
            'parent.window.Help.pointer.pointTo(this.getAttribute(\'elemid\'), this.getAttribute(\'elemclass\'));return false;'
        );

        $iconEl->setAttribute(
            'onload',
            'parent.window.Help.pointer.updateLinkIconState(this);'
        );

        return $iconEl;

    }//end _createElementLocator()


    /**
     * Converts the Help system specific tags inside the given document.
     *
     * @param DomDocument $dom      The document to parse.
     * @param string      $fileName The file that contains the tag.
     *
     * @return void
     */
    private static function _convertTags(DomDocument $dom, $fileName)
    {
        // Tag types.
        $tagTypes = array(
                     'import',
                     'glossary',
                     'article',
                    );

        foreach ($tagTypes as $tagName) {
            $tags     = $dom->getElementsByTagName($tagName);
            $function = array(
                         'self',
                         '_convert'.ucfirst($tagName).'Tag',
                        );

            // Add the DOMNodeList items to array since they will change and node
            // list will be updated.
            $tagsArray = array();
            foreach ($tags as $tag) {
                $tagsArray[] = $tag;
            }

            foreach ($tagsArray as $tag) {
                call_user_func($function, $tag, $fileName);
            }
        }

    }//end _convertTags()


    /**
     * Replaces given import tag with the actual article contents.
     *
     * @param DomNode $tag An import tag.
     *
     * @return void
     * @throws ChannelException If tag cannot be converted.
     */
    private static function _convertImportTag(DomNode $tag)
    {
        // Make sure import tag has no content.
        if (empty($tag->nodeValue) === FALSE) {
            throw new ChannelException(_('Import tag cannot have content'));
        }

        // Import tag requires glossaryid or articleid attribute.
        $hasGlossaryId = $tag->hasAttribute('glossaryid');
        $hasArticleId  = $tag->hasAttribute('articleid');
        if ($hasGlossaryId === FALSE
            && $hasArticleId === FALSE
        ) {
            throw new ChannelException(_('Import tag must have a glossaryid or articleid attribute'));
        } else if ($hasGlossaryId === TRUE
            && $hasArticleId === TRUE
        ) {
            throw new ChannelException(_('Import tag cannot have both glossaryid and articleid attribute'));
        }

        $id   = NULL;
        $type = NULL;
        if ($hasGlossaryId === TRUE) {
            $glossaryid = trim($tag->getAttribute('glossaryid'));
            if (empty($glossaryid) === TRUE) {
                throw new ChannelException(_('Found empty glossaryid attribute in import tag'));
            }

            $id   = $glossaryid;
            $type = 'glossary';
        } else {
            $articleid = trim($tag->getAttribute('articleid'));
            if (empty($articleid) === TRUE) {
                throw new ChannelException(_('Found empty articleid attribute in import tag'));
            }

            $id   = $articleid;
            $type = 'article';
        }

        // Check for import recursion.
        if (isset(self::$_importing[$id]) === TRUE) {
            $files = implode("\n", array_keys(self::$_importing));
            throw new ChannelException(sprintf(_('Import tag recursion detected. Check following files:'."\n%s"), $files));
        }

        // Add the id of this article to import tag array so that we can detect any
        // import recursion (e.g. A -(importing)> B, B -> C and C -> A).
        self::$_importing[$id] = TRUE;

        // Make sure the imported file is converted before importing.
        self::_convertTagsInFile($id, $type);

        $file = self::_getArticleFile($id, $type);
        $doc  = new DomDocument();
        @$doc->loadHTMLFile($file);
        $xpath       = new DOMXPath($doc);
        $articleNode = $xpath->query('//div[@id="help-article-wrapper"]')->item(0);
        if ($articleNode === NULL) {
            $msg = sprintf(_('Could not find the "help-article-wrapper" element in article file (%s)'), $file);
            throw new ChannelException($msg);
        }

        $importedNode = $tag->ownerDocument->importNode($articleNode, TRUE);
        while ($importedNode->firstChild) {
            $tag->parentNode->insertBefore($importedNode->firstChild, $tag);
        }

        $tag->parentNode->removeChild($tag);

        // Article imported.
        unset(self::$_importing[$id]);

    }//end _convertImportTag()


    /**
     * Converts a glossary tag.
     *
     * @param DomNode $tag      The glossary tag to convert.
     * @param string  $fileName The file that contains the tag.
     *
     * @return void
     * @throws ChannelException If tag cannot be converted.
     */
    private static function _convertGlossaryTag(DomNode $tag, $fileName)
    {
        $id = trim($tag->getAttribute('id'));
        if (empty($id) === TRUE) {
            throw new ChannelException(_('Found glossary tag with empty id'));
        }

        $content = trim($tag->nodeValue);
        if (empty($content) === TRUE) {
            throw new ChannelException(_('Found glossary tag with no content'));
        }

        // Check if glossary term exists.
        $dataDir  = Help::getDocsDirectory();
        $termFile = $dataDir.'glossary-'.$id.'.html';
        if (file_exists($termFile) === FALSE) {
            throw new ChannelException(sprintf(_('Referenced glossary term "%s" not found!'), $id));
        }

        $doc  = $tag->ownerDocument;
        $aTag = $doc->createElement('a');
        $aTag->setAttribute('href', 'javascript:parent.window.Help.loadPage(\'glossary-'.$id.'\');');

        $words = explode(' ', $content);
        $span  = NULL;
        foreach ($words as $word) {
            $span = $doc->createElement('span');
            $span->nodeValue = $word.' ';
            $aTag->appendChild($span);
        }

        if ($span !== NULL) {
            $span->nodeValue = rtrim($span->nodeValue);
        }

        // Get the element id or the class that is being referenced.
        $elemId    = NULL;
        $elemClass = NULL;
        if (isset(self::$_widgetReferences['elemid']['glossary-'.$id]) === TRUE) {
            $elemId = self::$_widgetReferences['elemid']['glossary-'.$id];
        } else if (isset(self::$_widgetReferences['elemclass']['glossary-'.$id]) === TRUE) {
            $elemClass = self::$_widgetReferences['elemclass']['glossary-'.$id];
        }

        // Create the element locator icon.
        $locator = self::_createElementLocator(
            $doc,
            $elemId,
            $elemClass
        );

        if ($locator !== NULL) {
            $aTag->appendChild($locator);
        }

        // Replace tag with the new one.
        $tag->parentNode->replaceChild($aTag, $tag);

        // Do not track glossary terms that refer to other terms or being refered by general article.
        if (strpos($fileName, 'glossary-') !== 0 && basename($fileName) !== 'general.html') {
            if (isset(self::$_glossaryUsage[$id]) === FALSE) {
                self::$_glossaryUsage[$id] = array();
            }

            self::$_glossaryUsage[$id][] = str_replace('.html', '', $fileName);
        }

    }//end _convertGlossaryTag()


    /**
     * Converts an article tag.
     *
     * @param DomNode $tag The article tag to convert.
     *
     * @return void
     * @throws ChannelException If tag cannot be converted.
     */
    private static function _convertArticleTag(DomNode $tag)
    {
        $id = trim($tag->getAttribute('id'));
        if (empty($id) === TRUE) {
            throw new ChannelException(_('Found article tag with empty id'));
        }

        $content = trim($tag->nodeValue);
        if (empty($content) === TRUE) {
            throw new ChannelException(_('Found article tag with no content'));
        }

        // Check if article exists.
        $dataDir     = Help::getDocsDirectory();
        $articleFile = $dataDir.'article-'.$id.'.html';
        if (file_exists($articleFile) === FALSE) {
            throw new ChannelException(sprintf(_('Referenced article "%s" not found!'), $id));
        }

        $doc  = $tag->ownerDocument;
        $aTag = $doc->createElement('a');
        $aTag->setAttribute('href', 'javascript:parent.window.Help.loadPage(\'article-'.$id.'\');');

        $words = explode(' ', $content);
        foreach ($words as $word) {
            $span = $doc->createElement('span');
            $span->nodeValue = $word.' ';
            $aTag->appendChild($span);
        }

        // Replace tag with the new one.
        $tag->parentNode->replaceChild($aTag, $tag);

    }//end _convertArticleTag()


    /**
     * Converts all the special Help tags in specified article file.
     *
     * @param string $id   The ID of the article.
     * @param string $type Type of the article (glossary, article).
     *
     * @return void
     * @throws ChannelException If article file does not exist.
     */
    private static function _convertTagsInFile($id, $type)
    {
        $fileName = self::_getArticleFile($id, $type, TRUE);

        if (in_array($fileName, self::$_convertedFiles) === TRUE) {
            // Its already converted.
            return;
        }

        $dataDir = Help::getDocsDirectory();
        $path    = $dataDir.'/'.$fileName;

        if (file_exists($path) === FALSE) {
            throw new ChannelException(sprintf(_('File does not exist: %s'), $path));
        }

        $doc = new DomDocument();
        @$doc->loadHTML(file_get_contents($path));
        self::_convertTags($doc, $fileName);
        $doc->saveHTMLFile($path);

        self::$_convertedFiles[] = $fileName;

    }//end _convertTagsInFile()


    /**
     * Returns the parth or name of the specified article ids file.
     *
     * @param string  $id       Id of the article.
     * @param string  $type     The type of the article.
     * @param boolean $nameOnly If TRUE then only the name of the file will be returned.
     *
     * @return string
     */
    private static function _getArticleFile($id, $type, $nameOnly=FALSE)
    {
        $fileName = $type.'-'.$id.'.html';
        if ($nameOnly === TRUE) {
            return $fileName;
        }

        $dataDir = Help::getDocsDirectory();
        $path    = $dataDir.'/'.$fileName;

        return $path;

    }//end _getArticleFile()


    /**
     * Adds a glossary term to the index array.
     *
     * @param string $docFile File name of the glossary term.
     * @param array  $info    Information about the glossary term.
     *
     * @return void
     */
    private static function _addGlossaryToIndex($docFile, array $info)
    {
        $parts = explode('-', $docFile);

        // Index 1 is the system name.
        $system = $parts[1];
        if (isset(self::$_docsInfo[$system]) === FALSE) {
            // Initialise docs info array.
            self::$_docsInfo[$system] = array(
                                         'glossary' => array(),
                                         'article'  => array(),
                                         'template' => array(),
                                        );
        }

        // Id of the glossary term is the title so that it can be sorted later on.
        $id = $info['title'];

        // If the parts count is 4 than there is a template name at index 2.
        if (count($parts) === 4) {
            $template = $parts[2];
            if (isset(self::$_docsInfo[$system]['template'][$template]) === FALSE) {
                self::$_docsInfo[$system]['template'][$template] = array(
                                                                    'glossary' => array(),
                                                                    'article'  => array(),
                                                                   );
            }

            self::$_docsInfo[$system]['template'][$template]['glossary'][$id] = $info['id'];
        } else {
            // Base system.
            self::$_docsInfo[$system]['glossary'][$id] = $info['id'];
        }

    }//end _addGlossaryToIndex()


    /**
     * Adds an article to the index array.
     *
     * @param string  $docFile     File name of the article.
     * @param array   $info        Information about the article.
     * @param boolean $isIndexPage If TRUE then the article is an index page.
     *
     * @return void
     */
    private static function _addArticleToIndex($docFile, array $info, $isIndexPage=FALSE)
    {
        $parts = explode('-', $docFile);

        // Index 1 is the system name.
        $system = $parts[1];
        if (isset(self::$_docsInfo[$system]) === FALSE) {
            // Initialise docs info array.
            self::$_docsInfo[$system] = array(
                                         'glossary'    => array(),
                                         'article'     => array(),
                                         'template'    => array(),
                                         'summary'     => '',
                                         'screenTitle' => '',
                                         'showInMenu'  => TRUE,
                                        );
        }

        // Id of the glossary term is the title so that it can be sorted later on.
        $id = $info['title'];

        // If the parts count is 4 than there is a template name at index 2.
        if (count($parts) === 4) {
            $template = $parts[2];
            if (isset(self::$_docsInfo[$system]['template'][$template]) === FALSE) {
                self::$_docsInfo[$system]['template'][$template] = array(
                                                                    'glossary'    => array(),
                                                                    'article'     => array(),
                                                                    'summary'     => '',
                                                                    'screenTitle' => '',
                                                                   );
            }

            foreach ($info['showIn'] as $showin) {
                if (isset(self::$_docsInfo[$system]['template'][$template]['article'][$showin]) === FALSE) {
                    self::$_docsInfo[$system]['template'][$template]['article'][$showin] = array();
                }

                self::$_docsInfo[$system]['template'][$template]['article'][$showin][$id] = $info;
            }

            if ($isIndexPage === TRUE && empty($info['summary']) === FALSE) {
                self::$_docsInfo[$system]['template'][$template]['summary'] = $info['summary'];
            }

            if ($isIndexPage === TRUE && empty($info['screenTitle']) === FALSE) {
                self::$_docsInfo[$system]['template'][$template]['screenTitle'] = $info['screenTitle'];
            }
        } else {
            if ($isIndexPage === TRUE && empty($info['screenTitle']) === FALSE) {
                self::$_docsInfo[$system]['screenTitle'] = $info['screenTitle'];
            }

            if ($isIndexPage === TRUE && empty($info['summary']) === FALSE) {
                self::$_docsInfo[$system]['summary'] = $info['summary'];
            }

            if ($isIndexPage === TRUE && $info['showInMenu'] === FALSE) {
                self::$_docsInfo[$system]['showInMenu'] = FALSE;
            }

            // Base system.
            foreach ($info['showIn'] as $showin) {
                if (isset(self::$_docsInfo[$system]['article'][$showin]) === FALSE) {
                    self::$_docsInfo[$system]['article'][$showin] = array();
                }

                self::$_docsInfo[$system]['article'][$showin][$id] = $info;
            }
        }//end if

    }//end _addArticleToIndex()


    /**
     * Adds links to articles that refer to glossary terms (related articles).
     *
     * @see    self::$_glossaryUsage
     * @return void
     */
    private static function _addGlossaryUsageToGlossaryFiles()
    {
        $dataDir = Help::getDocsDirectory();
        $doc     = new DomDocument();

        foreach (self::$_glossaryUsage as $term => $references) {
            $termFile = $dataDir.'/glossary-'.$term.'.html';
            if (file_exists($termFile) === FALSE) {
                continue;
            }

            @$doc->loadHTMLFile($termFile);
            $container = $doc->createElement('div');
            $container->setAttribute('id', 'Help-articleListWrapper');
            $containerTitle = $doc->createElement('h2');
            $containerTitle->nodeValue = _('Articles');
            $container->appendChild($containerTitle);

            $ulTag = $doc->createElement('ul');
            $ulTag->setAttribute('id', 'Help-articleList');
            $container->appendChild($ulTag);

            $references = array_unique($references);
            foreach ($references as $ref) {
                $liTag = $doc->createElement('li');
                $aTag  = $doc->createElement('a');
                $aTag->setAttribute('href', 'javascript:parent.window.Help.loadPage(\''.$ref.'\');');
                $aTag->nodeValue = self::$_articleTitles[$ref];
                $liTag->appendChild($aTag);
                $ulTag->appendChild($liTag);
            }

            $doc->getElementsByTagName('body')->item(0)->appendChild($container);
            $doc->saveHTMLFile($termFile);
        }//end foreach

    }//end _addGlossaryUsageToGlossaryFiles()


    /**
     * Creates the index pages like glossary, system and template.
     *
     * @return void
     */
    private static function _createIndexPages()
    {
        // For each system and its templates create its glossary index page.
        // For each system create its index page and then for each of its templates.
        foreach (self::$_docsInfo as $systemName => $docsInfo) {
            // Create the glossary index for this system.
            self::_createGlossaryIndexPage($docsInfo['glossary'], $systemName);

            // Create the index page for this system which has its description and
            // link to its screens.
            self::_createSystemIndexPage($docsInfo, $systemName);
            foreach ($docsInfo['template'] as $templateName => $tplDocsInfo) {
                // Create the glossary index for this template.
                self::_createGlossaryIndexPage($tplDocsInfo['glossary'], $systemName, $templateName);

                // Create the index page for the template which shows main article of
                // the template on top and rest of the articles of the template and
                // the mode.
                self::_createTemplateIndexPage($tplDocsInfo['article'], $docsInfo['article'], $systemName, $templateName);
            }
        }//end foreach

    }//end _createIndexPages()


    /**
     * Creates the glossary index page for a specified system and template.
     *
     * @param array  $glossaryTerms List of glossary terms.
     * @param string $systemName    Name of the system the terms defined in.
     * @param string $templateName  Name of the template the terms defined in.
     *
     * @return void
     */
    private static function _createGlossaryIndexPage(array $glossaryTerms, $systemName, $templateName=NULL)
    {
        // Include the general terms in the list.
        $glossaryTerms = array_merge($glossaryTerms, self::$_generalGlossaryTerms);
        ksort($glossaryTerms);

        $letters     = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $length      = strlen($letters);
        $contents    = '<div class="Help-glossary-letters">';
        $listContent = '<div class="Help-glossary-list">';

        for ($i = 0; $i < $length; $i++) {
            $hasArticle   = FALSE;
            $listContent .= '<h2 name="term'.$letters[$i].'" id="term'.$letters[$i].'">'.$letters[$i].'</h2>';
            $listContent .= '<ul class="Help-list">';

            $id    = current($glossaryTerms);
            $title = key($glossaryTerms);

            while ($title !== NULL) {
                $firstLt = strtoupper($title[0]);
                if ($firstLt === $letters[$i]) {
                    $url = 'javascript:window.parent.Help.loadPage(\''.$id.'\');';

                    $listContent .= '<li><a href="'.$url.'">'.$title.'</a></li>';
                    next($glossaryTerms);
                    $title      = key($glossaryTerms);
                    $id         = current($glossaryTerms);
                    $hasArticle = TRUE;
                } else {
                    break;
                }
            }

            $listContent .= '</ul>';

            // Link to sections.
            if ($hasArticle === TRUE) {
                $contents .= '<a href="#term'.$letters[$i].'">'.$letters[$i].'</a> ';
            } else {
                $contents .= $letters[$i].' ';
            }
        }//end for

        $listContent .= '</div>';
        $contents    .= '</div>';
        $contents    .= $listContent;

        $filePath = Help::getDocsDirectory().'/'.$systemName;
        if ($templateName !== NULL) {
            $filePath .= '-'.$templateName;
        }

        $filePath .= '-glossary-index.html';

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
        $html   .= '</head><body class="Help-iframe glossaryPage">';
        $html   .= self::_getSetCurrentSystemScript($systemName);
        $html   .= $contents;
        $html   .= '</body></html>';

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($filePath, $html);

    }//end _createGlossaryIndexPage()


    /**
     * Creates the system's index page.
     *
     * @param array  $docsInfo   Information about the doc articles.
     * @param string $systemName Name of the system.
     *
     * @return void
     */
    private static function _createSystemIndexPage(array $docsInfo, $systemName)
    {
         // Get the template document file and add it to the HTML (if exists).
        $indexFile = Help::getDocsDirectory().'/index-'.$systemName.'.html';

        $contents  = '';
        $contents .= '<div class="Help-iframe-systemInto">';

        if (file_exists($indexFile) === TRUE) {
            $contents .= file_get_contents($indexFile);
        }

        $contents .= '</div>';
        $contents .= '<div class="Help-iframe-templateList">';

        // Print each template and their summary.
        foreach ($docsInfo['template'] as $templateName => $tplInfo) {
            $tplHref = $systemName.'-'.$templateName.'-index.html';
            $desc    = '&nbsp;';
            if (empty($tplInfo['summary']) === FALSE) {
                $desc = $tplInfo['summary'];
            }

            $screenTitle = $templateName;
            if (empty($tplInfo['screenTitle']) === FALSE) {
                $screenTitle = $tplInfo['screenTitle'];
            }

            $contents .= '<h2>'.$screenTitle.' <a href="javascript:parent.window.Help.loadPage(\''.$tplHref.'\');">==&gt;</a></h2>';
            $contents .= '<p>'.$desc.'</p>';
        }

        $contents .= '</div>';

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
        $html   .= '</head><body class="Help-iframe systemIndexPage">';
        $html   .= self::_getSetCurrentSystemScript($systemName);
        $html   .= $contents;
        $html   .= '</body></html>';

        $filePath = Help::getDocsDirectory().'/'.$systemName.'-index.html';

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($filePath, $html);

    }//end _createSystemIndexPage()


    /**
     * Creates the index page for a template.
     *
     * @param array  $templateArticles List of articles for the template.
     * @param array  $parentArticles   List of articles for the system that the template is in.
     * @param string $systemName       Name of the system the articles belong to.
     * @param string $templateName     Name of the template the articles belong to.
     *
     * @return void
     * @throws ChannelException If the index page not found for specified template.
     */
    private static function _createTemplateIndexPage(array $templateArticles, array $parentArticles, $systemName, $templateName)
    {
        if (isset($templateArticles['tasks']) === FALSE) {
            $templateArticles['tasks'] = array();
        }

        if (isset($parentArticles['tasks']) === FALSE) {
            $parentArticles['tasks'] = array();
        }

        if (isset($templateArticles['troubleshooting']) === FALSE) {
            $templateArticles['troubleshooting'] = array();
        }

        if (isset($parentArticles['troubleshooting']) === FALSE) {
            $parentArticles['troubleshooting'] = array();
        }

        // Merge the current template articles with the system's articles.
        $templateArticles['tasks'] = array_merge($templateArticles['tasks'], $parentArticles['tasks']);
        $templateArticles['troubleshooting'] = array_merge($templateArticles['troubleshooting'], $parentArticles['troubleshooting']);

        // Sory by name.
        ksort($templateArticles['tasks']);
        ksort($templateArticles['troubleshooting']);

        // Get the template document file and add it to the HTML (if exists).
        $indexFile = Help::getDocsDirectory().'/index-'.$systemName.'-'.$templateName.'.html';

        if (file_exists($indexFile) === FALSE) {
            throw new ChannelException(_('Index file for template does not exist.'));
        }

        $contents  = '';
        $contents .= '<div class="Help-iframe-templateInto">';
        $contents .= file_get_contents($indexFile);
        $contents .= '</div>';
        $contents .= '<div class="Help-iframe-templateArticles">';

        $sections = self::$_sections;
        foreach ($sections as $i => $section) {
            if (count($templateArticles[strtolower($section)]) === 0) {
                unset($sections[$i]);
            }
        }

        // Print index page sections (e.g. Tasks, Troubleshooting).
        $totalSections = count($sections);
        foreach ($sections as $section) {
            if ($totalSections === 1
                && count($templateArticles[strtolower($section)]) === 1
            ) {
                foreach ($templateArticles[strtolower($section)] as $title => $info) {
                    ob_start();
                    Help::getPage($info['id']);
                    $contents .= ob_get_contents();
                    ob_clean();
                }
            } else {
                $contents .= '<h2>'.$section.'</h2>';
                $contents .= '<ul>';
                foreach ($templateArticles[strtolower($section)] as $title => $info) {
                    $contents .= '<li><a href="javascript:parent.window.Help.loadPage(\''.$info['id'].'\');">'.$title.'</a></li>';
                }

                $contents .= '</ul>';
            }
        }

        $contents .= '</div>';

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
        $html   .= '</head><body class="Help-iframe templateIndexPage">';
        $html   .= self::_getSetCurrentSystemScript($systemName);
        $html   .= $contents;
        $html   .= '</body></html>';

        $filePath  = Help::getDocsDirectory().'/'.$systemName.'-'.$templateName;
        $filePath .= '-index.html';

        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($filePath, $html);

    }//end _createTemplateIndexPage()


    /**
     * Returns the file path of the general articles.
     *
     * @return string
     * @throws ChannelException If there is a problem with retrieving current product.
     */
    private static function _getGeneralArticleFilePath()
    {
        Channels::includeSystem('SquizSuite');
        $currentProductInfo = SquizSuite::getCurrentProduct();
        if (isset($currentProductInfo['type']) === FALSE) {
            throw new ChannelException(_('Failed to retrieve product type'));
        }

        $productSystem = str_replace(' ', '', ucwords($currentProductInfo['type']));
        if (Channels::systemExists($productSystem) === FALSE) {
            throw new ChannelException(sprintf(_('Product type system (%s) not found!'), $productSystem));
        }

        // General articles path.
        $path = Channels::getSystemsPath().'/'.$productSystem.'/Docs/general.html';
        return $path;

    }//end _getGeneralArticleFilePath()


    /**
     * Creates the Systems menu for the dropdown list in Help dialog.
     *
     * @return void
     */
    private static function _createHelpSystemsMenu()
    {
        $html        = '<div class="Help-systemsMenu"><ul>';
        $systemInfos = array();

        foreach (self::$_docsInfo as $systemName => $docsInfo) {
            $systemTitle = $systemName;
            if (empty($docsInfo['screenTitle']) === FALSE) {
                $systemTitle = $docsInfo['screenTitle'];
            }

            $systemInfos[$systemName] = array('title' => $systemTitle);

            if ($docsInfo['showInMenu'] === FALSE) {
                // Not shown in the menu so continue.
                continue;
            }

            // Small icon for the system.
            $iconURL = '/__web/Systems/'.$systemName.'/Web/icon_'.strtolower($systemName).'_help.png';

            $href  = 'Help.closeMenu();Help.loadIndexPage(\''.$systemName.'\');';
            $html .= '<li onclick="'.$href.'" id="Help-dialog-sysMenuItem-'.$systemName.'" >';
            $html .= '<div style="background-image:url('.$iconURL.')" class="Help-systemsMenu-itemIcon">&nbsp;</div>';
            $html .= '<a href="javascript:'.$href.'">'.$systemTitle.'</a>';
            $html .= '<div class="Help-systemsMenu-selectedIcon"></div>';
            $html .= '</li>';
        }//end foreach

        $html .= '</ul></div>';

        // Also add script tag that will set the systemName => title  array in Help.
        $html .= '<script>Help.setSystems('.json_encode($systemInfos).')</script>';

        $filePath = Help::getDocsDirectory().'/_systems_menu.html';
        include_once 'Libs/FileSystem/FileSystem.inc';
        FileSystem::filePutContents($filePath, $html);

    }//end _createHelpSystemsMenu()


    /**
     * Returns the script tag HTML that sets the Help dialogs system info.
     *
     * The script tag is added to each page so that it can be executed when it loads.
     *
     * @param string $systemName Name of the system that the page belongs to.
     *
     * @return string
     */
    private static function _getSetCurrentSystemScript($systemName)
    {
        $script = '<script>parent.window.Help.setCurrentSystem("'.$systemName.'");</script>';
        return $script;

    }//end _getSetCurrentSystemScript()


    /**
     * Returns the Help CSS file web path.
     *
     * @return string
     */
    private static function _getPageCSSPath()
    {
        Channels::includeSystem('GUI');
        $cssPath = '/__web/'.GUI::getSystemWebPath('Help').'/Templates/Help/Help.css';
        return $cssPath;

    }//end _getPageCSSPath()


}//end class

?>
