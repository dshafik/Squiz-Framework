<?php
/**
 * Actions for the PDFExport System.
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
 * @subpackage PDFExport
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * PDFExport Actions Class.
 */
class PDFExportActions
{


    /**
     * Generate a PDF from a URL.
     *
     * @param string $url  URL to generate.
     * @param string $dest Where the PDF will be saved.
     *
     * @return boolean
     */
    public static function generateFromURL($url, $dest)
    {
        $command = dirname(__FILE__).'/Scripts/url.sh pdf '.$dest.' '.$url;

        Channels::includeSystem('ImageExport');
        ImageExport::startServer();
        $success = ImageExport::generateScreenshot($command, $dest);
        ImageExport::stopServer();

        return $success;

    }//end generateFromURL()


}//end class

?>
