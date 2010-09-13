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
            if ($system === 'Keyword') {
                // Skip keyword format for now.
                continue;
            }

            $helpDirs[$system->getAttribute('name')]
                = $system->getAttribute('path').'/Docs';
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

    }//end createDocs()


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
            $html   .= '</body></html>';
            @$termDoc->loadHTML($html);
            $bodyTag = $termDoc->getElementsByTagName('body')->item(0);

            $contentWrapper = NULL;
            $node           = $term;
            do {
                $importedNode = $termDoc->importNode($node, TRUE);

                // Inside the H1 element we need to add the element locator.
                if ($importedNode->tagName === 'h1') {
                    $importedNode->removeAttribute('elemid');
                    $importedNode->removeAttribute('elemclass');
                    $elemLocator = self::_createElementLocator(
                        $termDoc,
                        $term->getAttribute('elemid'),
                        $term->getAttribute('elemclass')
                    );
                    $importedNode->appendChild($elemLocator);
                    $bodyTag->appendChild($importedNode);

                    // Create the content contentWrapper.
                    $contentWrapper = $termDoc->createElement('div');
                    $contentWrapper->setAttribute('id', 'help-article-wrapper');
                    $bodyTag->appendChild($contentWrapper);
                } else if ($contentWrapper === NULL) {
                    throw new ChannelException(_('Found content before H1 element'));
                } else {
                    $contentWrapper->appendChild($importedNode);
                }

                // Loop until another term is found (h1 tag).
                $node = $node->nextSibling;
            } while ($node && ($node->nodeType !== XML_ELEMENT_NODE || $node->tagName !== 'h1'));

            // Term file destination.
            $dataDir  = Help::getDocsDirectory();
            $fileName = $idPrefix.strtolower(preg_replace('/[^a-zA-Z0-9_]/', '_', $glossaryid));
            $destFile = $dataDir.'/'.$fileName.'.html';

            $glossaryInfo = array(
                             'id'    => $fileName,
                             'title' => $glossaryTitle,
                            );
            self::_addGlossaryToIndex($fileName, $glossaryInfo);

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

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html    = '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" /></head>';
        $html   .= '<body class="Help-iframe articlePage">';
        $html   .= $fileContents;
        $html   .= '</body></html>';

        $doc = new DomDocument();
        @$doc->loadHTML($html);

        // Get the title.
        $titleTags = $doc->getElementsByTagName('h1');
        if ($titleTags->length === 0) {
            throw new ChannelException(sprintf(_('Article does not have a title (h1 tag): %s.'), $articleFile));
        }

        $title     = $titleTags->item(0)->nodeValue;
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
                        'id'      => $idPrefix.$articleid,
                        'title'   => $title,
                        'showIn'  => $showIn,
                        'summary' => $summary,
                       );
        self::_addArticleToIndex($idPrefix.$articleid, $articleInfo, $isIndexPage);

        if (file_exists($destFile) === TRUE) {
            throw new ChannelException(sprintf(_('Duplicate article found in: %s.'), $articleFile));
        }

        // Remove the info tag.
        if ($infoTags->length !== 0) {
            $infoTags->item(0)->parentNode->removeChild($infoTags->item(0));
        }

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
        $iconEl->setAttribute('system', $system);
        if (is_array($widgetRef) === TRUE) {
            $iconEl->setAttribute(
                'widgetclass',
                $widgetRef['val']
            );
            $iconEl->setAttribute('widget', '');
        } else {
            $iconEl->setAttribute('widget', $widgetRef);
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
            foreach ($tags as $tag) {
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

        // Create the element locator icon.
        $locator = self::_createElementLocator(
            $doc,
            $tag->getAttribute('elemid'),
            $tag->getAttribute('elemclass')
        );

        $aTag->appendChild($locator);

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
            $span->nodeValue = $word;
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
                                         'glossary' => array(),
                                         'article'  => array(),
                                         'template' => array(),
                                         'summary'  => array(),
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
                                                                    'summary'  => array(),
                                                                   );
            }

            foreach ($info['showIn'] as $showin) {
                if (isset(self::$_docsInfo[$system]['template'][$template]['article'][$showin]) === FALSE) {
                    self::$_docsInfo[$system]['template'][$template]['article'][$showin] = array();
                }

                self::$_docsInfo[$system]['template'][$template]['article'][$showin][$id] = $info;
            }

            if (empty($info['summary']) === FALSE) {
                self::$_docsInfo[$system]['template'][$template]['summary'] = $info['summary'];
            }
        } else {
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
     * Adds links to articles that refer to glossary terms.
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
            $hr = $doc->createElement('hr');
            $container->appendChild($hr);

            $ulTag = $doc->createElement('ul');
            $ulTag->setAttribute('id', 'Help-articleList');
            $container->appendChild($ulTag);

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
        // Need the title and the id of the article.
        asort($glossaryTerms);

        $letters     = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $length      = strlen($letters);
        $contents    = '<div class="Help-glossary-letters">';
        $listContent = '<div class="Help-glossary-list">';

        for ($i = 0; $i < $length; $i++) {
            $hasArticle   = FALSE;
            $listContent .= '<h2 name="term'.$letters[$i].'" id="term'.$letters[$i].'">'.$letters[$i].'</h2><hr />';
            $listContent .= '<ul class="Help-list">';

            $id    = current($glossaryTerms);
            $title = key($glossaryTerms);

            while ($title !== FALSE) {
                $firstLt = strtoupper($title[0]);
                if ($firstLt === $letters[$i]) {
                    $url = 'javascript:window.parent.Help.loadPage(\''.$id.'\');';

                    $listContent .= '<li><a href="'.$url.'">'.$title.'</a></li>';
                    $title        = next($glossaryTerms);
                    $hasArticle   = TRUE;
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

            $contents .= '<h1>'.$templateName.' <a href="javascript:parent.window.Help.loadPage(\''.$tplHref.'\');">==&gt;</a></h1>';
            $contents .= '<hr />';
            $contents .= '<p>'.$desc.'</p>';
        }

        $contents .= '</div>';

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
        $html   .= '</head><body class="Help-iframe systemIndexPage">';
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
     */
    private static function _createTemplateIndexPage(array $templateArticles, array $parentArticles, $systemName, $templateName)
    {
        // Merge the current template articles with the system's articles.
        $templateArticles['tasks'] = array_merge($templateArticles['tasks'], $parentArticles['tasks']);
        $templateArticles['troubleshooting'] = array_merge($templateArticles['troubleshooting'], $parentArticles['troubleshooting']);

        // Sory by name.
        ksort($templateArticles['tasks']);
        ksort($templateArticles['troubleshooting']);

        // Get the template document file and add it to the HTML (if exists).
        $indexFile = Help::getDocsDirectory().'/index-'.$systemName.'-'.$templateName.'.html';

        $contents  = '';
        $contents .= '<div class="Help-iframe-templateInto">';
        $contents .= file_get_contents($indexFile);
        $contents .= '</div>';
        $contents .= '<div class="Help-iframe-templateArticles">';

        // Print index page sections (e.g. Tasks, Troubleshooting).
        foreach (self::$_sections as $section) {
            $contents .= '<h1>'.$section.'</h1>';
            $contents .= '<hr />';
            $contents .= '<ul>';
            foreach ($templateArticles[strtolower($section)] as $title => $info) {
                $contents .= '<li><a href="javascript:parent.window.Help.loadPage(\''.$info['id'].'\');">'.$title.'</a></li>';
            }

            $contents .= '</ul></div>';
        }

        Channels::includeSystem('GUI');
        $cssHref = self::_getPageCSSPath();
        $html    = GUI::getDocType();
        $html   .= '<html><head><link href="'.$cssHref.'" type="text/css" rel="stylesheet" />';
        $html   .= '</head><body class="Help-iframe templateIndexPage">';
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
     * Prints the index page for the specified tamplate.
     *
     * @param string $pageid Id of the page to load.
     *
     * @return void
     *
     * @api read
     */
    public static function getPage($pageid)
    {
        $filePath = Help::getDocsDirectory().'/'.$pageid;
        if (strpos($pageid, '.html') === FALSE) {
            $filePath .= '.html';
        }

        if (file_exists($filePath) === FALSE) {
            printf(_('Page does not exist: #%s'), $pageid);
            return;
        }

        echo file_get_contents($filePath);

    }//end getPage()


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
