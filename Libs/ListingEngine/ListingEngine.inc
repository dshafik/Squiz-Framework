<?php
/**
 * ListingEngine related methods.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Web
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Listing Engine related methods.
 *
 * @since 4.0.0
 */
class ListingEngine
{

    /**
     * List of listings.
     *
     * Each element contains information about a listing.
     *
     * @var   array $_listings
     * @since 4.0.0
     */
    private static $_listings = array();


    /**
     * Constructor.
     *
     * Private to prevent instantiation.
     *
     * @since 4.0.0
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Initialises the asset listing.
     *
     * This method should be called before using keywordReplacements.
     *
     * @param integer $listingid       Id of the asset listing.
     * @param array   $assetids        Array of assetids.
     * @param string  $defaultFormat   The default format to use.
     * @param array   $typeFormats     List of asset type formats.
     *                                 (typeid => format).
     * @param array   $positionFormats List of position formats.
     *                                 (pos1, pos2, ...).
     * @param integer $assetsPerPage   Number of assets to display per page.
     * @param array   $settings        Extra settings for the listing engine.
     *
     * @since  4.0.0
     * @return void
     */
    public static function initialise(
        $listingid,
        array $assetids,
        $defaultFormat,
        array $typeFormats=array(),
        array $positionFormats=array(),
        $assetsPerPage=NULL,
        array $settings=array()
    ) {
        if (Util::getArrayIndex(self::$_listings, $listingid, NULL) === NULL) {
            self::$_listings[$listingid] = array();
            self::_setInfo($listingid, 'assetsPerPage', $assetsPerPage);
            $totalAssets = count($assetids);
            self::_setInfo($listingid, 'totalAssets', $totalAssets);
            $totalPages = 1;
            if ($assetsPerPage > 0 && $totalAssets > 0) {
                $totalPages = ceil(($totalAssets / $assetsPerPage));
            }

            self::_setInfo($listingid, 'totalPages', $totalPages);
            self::_setInfo($listingid, 'defaultFormat', $defaultFormat);
            self::_setInfo($listingid, 'typeFormats', $typeFormats);
            self::_setInfo($listingid, 'assetids', $assetids);
            self::_setInfo($listingid, 'positionFormats', $positionFormats);

            foreach ($settings as $setting => $value) {
                self::_setInfo($listingid, $setting, $value);
            }
        }

    }//end initialise()


    /**
     * Clears a listing from the internal cache.
     *
     * @param integer $listingid Id of the asset listing.
     *
     * @since  4.0.0
     * @return void
     */
    public static function clearListing($listingid)
    {
        if (isset(self::$_listings[$listingid]) === TRUE) {
            unset(self::$_listings[$listingid]);
        }

    }//end clearListing()


    /**
     * Returns the specified content with its listing engine keywords replaced.
     *
     * @param integer $listingid       Id of the asset listing.
     * @param string  $content         Content to use.
     * @param array   $assetids        Array of assetids.
     * @param string  $defaultFormat   The default format to use.
     * @param array   $typeFormats     List of asset type formats.
     *                                 (typeid => format).
     * @param array   $positionFormats List of position formats.
     *                                 (pos1, pos2, ...).
     * @param integer $assetsPerPage   Number of assets to display per page.
     * @param array   $settings        Extra settings for the listing engine.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getAssetList(
        $listingid,
        $content,
        array $assetids,
        $defaultFormat,
        array $typeFormats=array(),
        array $positionFormats=array(),
        $assetsPerPage=NULL,
        array $settings=array()
    ) {
        self::initialise(
            $listingid,
            $assetids,
            $defaultFormat,
            $typeFormats,
            $positionFormats,
            $assetsPerPage,
            $settings
        );

        $content = self::replaceKeywords($listingid, $content);
        return $content;

    }//end getAssetList()


    /**
     * Returns the list of assets for the current offset.
     *
     * @param integer $listingid     Id of the asset listing.
     * @param array   $assetids      Array of assetids.
     * @param integer $assetsPerPage Number of assets to display per page.
     *
     * @since  4.0.0
     * @return array
     */
    private static function _getPageAssets(
        $listingid,
        array $assetids,
        $assetsPerPage
    ) {
        // Get the current offset.
        include_once 'Libs/Security/Security.inc';
        $offset = Security::getRequestData($listingid.'_p', 1);
        if ($offset < 1) {
            $offset = 1;
        } else {
            $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');
            $totalPages    = self::_getInfo($listingid, 'totalPages');
            if ($offset > $totalPages) {
                $offset = $totalPages;
            }
        }

        $assetids = array_slice(
            $assetids,
            (($offset - 1) * $assetsPerPage),
            $assetsPerPage,
            TRUE
        );

        return $assetids;

    }//end _getPageAssets()


    /**
     * Returns a string of a asset listing.
     *
     * @param integer $listingid       Id of the asset listing.
     * @param array   $assetids        Array of assetids.
     * @param string  $defaultFormat   The default format to use.
     * @param array   $typeFormats     List of asset type formats.
     *                                 (typeid => format).
     * @param array   $positionFormats List of position formats.
     *                                 (pos1, pos2, ...).
     * @param integer $assetsPerPage   Number of assets to display per page.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getAssetListing(
        $listingid,
        array $assetids,
        $defaultFormat,
        array $typeFormats=array(),
        array $positionFormats=array(),
        $assetsPerPage=NULL
    ) {
        $content = '';
        if (empty($assetids) === TRUE) {
            return $content;
        }

        // Check if the list is paginated.
        if ($assetsPerPage !== NULL && empty($assetsPerPage) === FALSE) {
            self::_setInfo($listingid, 'assetsPerPage', $assetsPerPage);
            $assetids = self::_getPageAssets(
                $listingid,
                $assetids,
                $assetsPerPage
            );
        }

        if (empty($assetids) === TRUE) {
            return $content;
        }

        // Get the asset types in the list if there are type formats.
        $useTypeFormats = FALSE;
        if (empty($typeFormats) === FALSE) {
            $useTypeFormats = TRUE;
            Channels::includeSystem('Asset');
            $assetTypes = DAL::getGroupedAssoc(
                Asset::batchGetAssetType($assetids, TRUE)
            );
        }

        // Check if there are any position formats.
        $usePositionFormats = FALSE;
        if (empty($positionFormats) === FALSE) {
            $usePositionFormats = TRUE;
        }

        $formatKeywords = array();
        Channels::includeSystem('Keyword');
        $formatKeywords['defaultFormat'] = Keyword::getKeywordInfoList(
            $defaultFormat
        );

        foreach ($assetids as $index => $assetid) {
            $format   = $defaultFormat;
            $keywords = $formatKeywords['defaultFormat'];

            // If there are type formats then use the specified type format
            // else use the default.
            if ($useTypeFormats === TRUE) {
                // Determine the type format.
                $assetType = Util::getArrayIndex($assetTypes, $assetid, NULL);
                if ($assetType !== NULL) {
                    $assetType = $assetType[0]['typeid'];

                    $format = Util::getArrayIndex(
                        $typeFormats,
                        $assetType,
                        $defaultFormat
                    );

                    if (isset($formatKeywords['typeFormat'.$assetType]) === FALSE) {
                        // Get the keywords of this type format.
                        $formatKeywords['typeFormat'.$assetType] = Keyword::getKeywordInfoList($format);
                    }

                    $keywords = $formatKeywords['typeFormat'.$assetType];
                }
            }//end if

            if ($usePositionFormats === TRUE) {
                $position = (($index + 1) - ($assetsPerPage * (self::_getCurrentPage($listingid) - 1)));
                $format   = Util::getArrayIndex(
                    $positionFormats,
                    '1:'.$position,
                    $format
                );
                $position = ($index + 1);
                $format   = Util::getArrayIndex(
                    $positionFormats,
                    '0:'.$position,
                    $format
                );

                if (isset($formatKeywords['position_'.$index]) === FALSE) {
                    // Get the keywords of this position format.
                    $formatKeywords['position_'.$index] = Keyword::getKeywordInfoList($format);
                }

                $keywords = $formatKeywords['position_'.$index];
            }//end if

            self::_setInfo($listingid, 'currentAsset', $assetid);
            self::_setInfo($listingid, 'currentAssetIndex', $index);
            $content .= self::_getAssetString(
                $listingid,
                $assetid,
                $format,
                $keywords
            );
        }//end foreach

        return $content;

    }//end getAssetListing()


    /**
     * Returns the contents of a single asset in the specified listing.
     *
     * @param integer $listingid   Id of the asset listing.
     * @param integer $assetid     The id of the asset.
     * @param string  $format      The format to use for the content.
     * @param array   $keywordList List of keywords that needs scope.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getAssetString(
        $listingid,
        $assetid,
        $format,
        array $keywordList
    ) {
        // Replace asset_listing keyword with nothing to stop recursion.
        $format = str_replace('%asset_listing%', '', $format);
        $format = self::replaceKeywords($listingid, $format);

        if (empty($keywordList) === FALSE) {
            Channels::includeSystem('Keyword');
            foreach ($keywordList as $keywordInfo) {
                if (empty($keywordInfo['scope']) === TRUE) {
                    $replacement = Keyword::insertKeywordScope(
                        $keywordInfo,
                        $assetid,
                        FALSE
                    );

                    $format = str_replace(
                        $keywordInfo['keyword'],
                        $replacement,
                        $format
                    );
                }
            }
        }

        return $format;

    }//end _getAssetString()


    /**
     * Replace asset type related keywords in given content.
     *
     * @param integer $listingid Id of the listing.
     * @param string  $content   Content to search & replace keywords.
     *
     * @since  4.0.0
     * @return string
     */
    public static function replaceKeywords($listingid, $content)
    {
        $keywords = self::getListingEngineKeywords($content);
        if (empty($keywords) === FALSE) {
            foreach ($keywords as $keyword) {
                $keyData = NULL;

                $key = str_replace(
                    ' ',
                    '',
                    ucwords(str_replace('_', ' ', trim($keyword, '%')))
                );

                $dash = strpos($key, '-');
                if ($dash !== FALSE) {
                    $keyData = substr($key, ($dash + 1));
                    $key     = substr($key, 0, $dash);
                }

                $func = array(
                         'ListingEngine',
                         '_get'.$key.'Replacement',
                        );

                $replacement = '';
                if (is_callable($func) === TRUE) {
                    $replacement = call_user_func($func, $listingid, $keyData);
                }

                $content = str_replace($keyword, $replacement, $content);
            }//end foreach
        }//end if

        return $content;

    }//end replaceKeywords()


    /**
     * Returns the internal keywords from the given content.
     *
     * @param string $content Content to search for keywords.
     *
     * @since  4.0.0
     * @return array
     */
    public static function getListingEngineKeywords($content)
    {
        $pattern = '/%[a-zA-Z_\-0-9]+%/';
        $matches = array();
        preg_match_all($pattern, $content, $matches);
        $matches = array_unique($matches[0]);
        return $matches;

    }//end getListingEngineKeywords()


    /**
     * Returns the asset_listing keyword replacement.
     *
     * @param integer $listingid Id of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getAssetListingReplacement($listingid)
    {
        $assets          = self::_getInfo($listingid, 'assetids');
        $defaultFormat   = self::_getInfo($listingid, 'defaultFormat');
        $typeFormats     = self::_getInfo($listingid, 'typeFormats');
        $positionFormats = self::_getInfo($listingid, 'positionFormats');
        $assetsPerPage   = self::_getInfo($listingid, 'assetsPerPage');

        $content = self::getAssetListing(
            $listingid,
            $assets,
            $defaultFormat,
            $typeFormats,
            $positionFormats,
            $assetsPerPage
        );

        return $content;

    }//end _getAssetListingReplacement()


    /**
     * Replace the current page keyword.
     *
     * @param integer $listingid Id of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getPageNumberReplacement($listingid)
    {
        include_once 'Libs/Security/Security.inc';
        $currentPage = Security::getRequestData($listingid.'_p', 1);
        if ($currentPage < 1) {
            $currentPage = 1;
        } else {
            $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');
            if ($assetsPerPage > 0) {
                $totalPages = self::_getInfo($listingid, 'totalPages');
                if ($currentPage > $totalPages) {
                    $currentPage = $totalPages;
                }
            } else {
                $currentPage = 1;
            }
        }

        return $currentPage;

    }//end _getPageNumberReplacement()


    /**
     * Replace the total_assets keyword.
     *
     * @param integer $listingid Id of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getTotalAssetsReplacement($listingid)
    {
        $totalAssets = self::_getInfo($listingid, 'totalAssets');
        return $totalAssets;

    }//end _getTotalAssetsReplacement()


    /**
     * Replace the total pages keyword.
     *
     * @param integer $listingid Id of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getTotalPagesReplacement($listingid)
    {
        $totalPages    = 1;
        $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');
        if ($assetsPerPage > 0) {
            $totalPages = self::_getInfo($listingid, 'totalPages');
        }

        return $totalPages;

    }//end _getTotalPagesReplacement()


    /**
     * Returns the replacement of position keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getPositionReplacement($listingid)
    {
        $position = '';
        $index    = self::_getInfo($listingid, 'currentAssetIndex');
        if ($index !== NULL) {
            $position = ($index + 1);
        }

        return $position;

    }//end _getPositionReplacement()


    /**
     * Returns the replacement of odd_even keyword.
     *
     * This uses the position on the page and not
     * the asset's positions in the list, so every
     * page begins with an odd number.
     *
     * @param integer $listingid The ID of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getOddEvenReplacement($listingid)
    {
        $oddEven = '';
        $offset  = self::_getOffset($listingid);
        $index   = self::_getInfo($listingid, 'currentAssetIndex');
        $index   = ($offset + $index - 1);
        if ($index !== NULL) {
            $oddEven = 'odd';
            if ((($index + 1) % 2) === 0) {
                $oddEven = 'even';
            }
        }

        return $oddEven;

    }//end _getOddEvenReplacement()


    /**
     * Returns the replacement of asset_next_link keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     * @param boolean $hrefOnly  If true then only the URL will be returned.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getNextPageLinkReplacement(
        $listingid,
        $hrefOnly=FALSE
    ) {
        $text          = '';
        $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');
        if ($assetsPerPage !== NULL) {
            $offset = self::_getCurrentPage($listingid);

            $totalPages = self::_getInfo($listingid, 'totalPages');
            if ($offset < $totalPages && $totalPages > 1) {
                $offset++;
                $url = self::_getListingPageUrl($listingid, $offset);
                if ($hrefOnly === TRUE) {
                    $text = $url;
                } else {
                    $text = self::_getInfo($listingid, 'nextLinkText', '');
                    $text = '<a href="'.$url.'">'.$text.'</a>';
                }
            } else if ($offset >= $totalPages) {
                $text = self::_getInfo($listingid, 'nextLinkText', '');
            }
        }

        return $text;

    }//end _getNextPageLinkReplacement()


    /**
     * Returns the replacement of prev_page_link keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     * @param boolean $hrefOnly  If true then only the URL will be returned.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getPrevPageLinkReplacement(
        $listingid,
        $hrefOnly=FALSE
    ) {
        $text          = '';
        $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');
        if ($assetsPerPage !== NULL) {
            $offset = self::_getCurrentPage($listingid);

            $totalPages = self::_getInfo($listingid, 'totalPages');
            if ($offset > 1 && $totalPages > 1) {
                $offset--;
                $url = self::_getListingPageUrl($listingid, $offset);
                if ($hrefOnly === TRUE) {
                    $text = $url;
                } else {
                    $text = self::_getInfo($listingid, 'prevLinkText', '');
                    $text = '<a href="'.$url.'">'.$text.'</a>';
                }
            } else if ($offset === 1) {
                $text = self::_getInfo($listingid, 'prevLinkText', '');
            }
        }

        return $text;

    }//end _getPrevPageLinkReplacement()


    /**
     * Returns the replacement of prev_page_href keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getPrevPageHrefReplacement($listingid)
    {
        $content = self::_getPrevPageLinkReplacement($listingid, TRUE);
        return $content;

    }//end _getPrevPageHrefReplacement()


    /**
     * Returns the replacement of prev_page_href keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getNextPageHrefReplacement($listingid)
    {
        $content = self::_getNextPageLinkReplacement($listingid, TRUE);
        return $content;

    }//end _getNextPageHrefReplacement()


    /**
     * Returns the URL for the specified listing page.
     *
     * @param integer $listingid The ID of the asset listing.
     * @param integer $page      Page of the listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getListingPageUrl($listingid, $page)
    {
        $url = '';
        if (is_numeric($page) === TRUE) {
            include_once 'Libs/Web/Web.inc';
            $url = Web::getCurrentUrl(FALSE).'?'.$listingid.'_p='.$page;
        }

        return $url;

    }//end _getListingPageUrl()


    /**
     * Returns the replacement of page_list keyword.
     *
     * @param integer $listingid The ID of the asset listing.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getPageListReplacement($listingid)
    {
        $content     = '';
        $totalPages  = self::_getInfo($listingid, 'totalPages');
        $currentPage = self::_getCurrentPage($listingid);
        $pages       = array();

        $content = self::_listPages($listingid, 1, $totalPages);
        return $content;

    }//end _getPageListReplacement()


    /**
     * Returns the replacement of sliding_page_list keyword.
     *
     * @param integer $listingid  The ID of the asset listing.
     * @param integer $shownPages The number of pages to show in the list.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getSlidingPageListReplacement(
        $listingid,
        $shownPages
    ) {
        if ($shownPages === NULL || $shownPages <= 1) {
            $shownPages = 5;
        }

        $content     = '';
        $totalPages  = self::_getInfo($listingid, 'totalPages');
        $currentPage = self::_getCurrentPage($listingid);
        $pages       = array();

        // How many pages to show on each side of the current page.
        $offset = floor($shownPages / 2);
        if (($shownPages % 2) === 0) {
            $offset--;
        }

        if ($shownPages > $totalPages) {
            $start = 1;
            $end   = $totalPages;
        } else if (($currentPage + $offset) >= $totalPages) {
            // We need to include some pages before the
            // current page to make up the numbers.
            $diff  = ($totalPages - $currentPage);
            $start = ($currentPage - $shownPages + $diff + 1);
            $end   = $totalPages;
        } else {
            $start = ($currentPage - $offset);
            if ($start <= 0) {
                $start = 1;
            }

            $end = ($start + $shownPages - 1);
        }

        $content = self::_listPages($listingid, $start, $end);

        return $content;

    }//end _getSlidingPageListReplacement()


    /**
     * Returns a string with page numbers linked.
     *
     * @param integer $listingid The ID of the asset listing.
     * @param integer $start     Page # to start from.
     * @param integer $end       Page # to stop.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _listPages($listingid, $start, $end)
    {
        $currentPage = self::_getCurrentPage($listingid);
        for ($i = (int) $start; $i <= $end; $i++) {
            if ($currentPage === $i) {
                $pages[] = $i;
            } else {
                $url     = self::_getListingPageUrl($listingid, $i);
                $pages[] = '<a href="'.$url.'">'.$i.'</a>';
            }
        }

        $content = implode(' ', $pages);
        return $content;

    }//end _listPages()


    /**
     * Sets the specified info for the specified listing.
     *
     * @param integer $listingid Id of the asset listing.
     * @param string  $name      Name of the setting.
     * @param mixed   $value     Value of the setting.
     *
     * @since  4.0.0
     * @return void
     */
    private static function _setInfo($listingid, $name, $value)
    {
        self::$_listings[$listingid][$name] = $value;

    }//end _setInfo()


    /**
     * Returns the specified info for the given listing.
     *
     * @param integer $listingid Id of the asset listing.
     * @param string  $name      Name of the setting.
     * @param mixed   $default   The default value to return.
     *
     * @since  4.0.0
     * @return mixed
     */
    private static function _getInfo($listingid, $name, $default=NULL)
    {
        $value = $default;
        if (isset(self::$_listings[$listingid]) === TRUE) {
            $value = Util::getArrayIndex(
                self::$_listings[$listingid],
                $name,
                $default
            );
        }

        return $value;

    }//end _getInfo()


    /**
     * Returns the current page of the listing.
     *
     * @param integer $listingid The id of the asset listing.
     *
     * @since  4.0.0
     * @return integer
     */
    private static function _getCurrentPage($listingid)
    {
        include_once 'Libs/Security/Security.inc';
        $currentPage = Security::getRequestData($listingid.'_p', 1);
        if ($currentPage < 1) {
            $currentPage = 1;
        } else {
            $totalPages = self::_getInfo($listingid, 'totalPages');
            if ($currentPage > $totalPages) {
                $currentPage = $totalPages;
            }
        }

        return (int) $currentPage;

    }//end _getCurrentPage()


    /**
     * Returns the asset the current page starts at.
     *
     * @param integer $listingid The id of the asset listing.
     *
     * @since  4.0.0
     * @return integer
     */
    private static function _getOffset($listingid)
    {
        $currentPage   = self::_getCurrentPage($listingid);
        $assetsPerPage = self::_getInfo($listingid, 'assetsPerPage');

        $offset = (($currentPage - 1) * $assetsPerPage);
        $offset++;

        return $offset;

    }//end _getOffset()


    /**
     * Filter assets based on permission and publishing rules.
     *
     * Only the assets the current user has acces will be returned.
     *
     * @param mixed $assetids List of assets to fileter.
     *
     * @since  4.0.0
     * @return array
     */
    public static function filterAssets($assetids)
    {
        // Do some filtering on the asset to list.
        Channels::includeSystem('Authentication');
        Channels::includeSystem('User');
        Channels::includeSystem('Filter');
        $userid = Authentication::getCurrentUserid();
        if ($userid === User::getPublicUserid()) {
            // The current user is a public user. They only should be able to
            // see the assets are public and published.
            $filters  = array(
                         'publicAssetFilter'    => array(),
                         'publishedAssetFilter' => array(),
                        );
            $filtered = Filter::filterAssets($assetids, $filters);
        } else {
            // This is a logged in user. If the user needs to have the privilege
            // to read unpublished to see all assets. Or not, filter out
            // unpublished assets.
            Channels::includeSystem('Role');
            Channels::includeSystem('Publishing');
            $filtered = array();
            foreach ($assetids as $childid) {
                // This checkPermission() returns a required privilegeid to read
                // this asset based on its publishing status.
                $privilegeid = Publishing::findPublishingReadEditPrivilegeid(
                    'asset.read.content',
                    $childid
                );

                $hasPerm = Role::checkPermission(
                    $childid,
                    $userid,
                    $privilegeid
                );

                if ($hasPerm === TRUE) {
                    $filtered[] = $childid;
                }
            }
        }//end if

        return $filtered;

    }//end filterAssets()


}//end class

?>
