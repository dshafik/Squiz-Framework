<?php
/**
 * Actions for the ImageExport System.
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
 * @subpackage ImageExport
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * ImageExport Actions Class.
 */
class ImageExportActions
{


    /**
     * Takes a screenshot of a URL.
     *
     * @param string  $url    URL to take a screenshot of.
     * @param string  $dest   Where the screenshot will be saved.
     * @param integer $width  The width of the screenshot to take.
     * @param integer $height The height of the screenshot to take.
     *
     * @return boolean
     */
    public static function generateFromURL($url, $dest, $width=NULL, $height=NULL)
    {
        $command = dirname(__FILE__).'/Scripts/url.sh png '.$dest.'.tmp '.$url;

        ImageExport::startServer();
        $success = ImageExport::generateScreenshot($command, $dest.'.tmp');
        ImageExport::stopServer();

        if ($success === FALSE) {
            return FALSE;
        }

        try {
            if ($width === NULL || $height === NULL) {
                // No resize needed, just a file type conversion.
                include_once 'Libs/FileSystem/FileSystem.inc';
                $ext = FileSystem::getExtension($dest);
                if ($ext === 'png') {
                    // No conversion required as PNG is the native
                    // format we produce.
                    exec('mv '.$dest.'.tmp '.$dest);
                } else {
                    ImageExport::generateFromImage($dest.'.tmp', $dest);
                }
            } else {
                ImageExport::generateFromImage($dest.'.tmp', $dest, $width, $height, TRUE);
                exec('rm '.$dest.'.tmp');
            }

            $success = TRUE;
        } catch (ChannelException $e) {
            echo $e->getMessage();
            $success = FALSE;
            exec('rm '.$dest.'.tmp');
        }//end try

        return $success;

    }//end generateFromURL()


    /**
     * Takes a screenshot of a PDF.
     *
     * @param string  $source The location of the PDF.
     * @param string  $dest   Where the screenshot will be saved.
     * @param integer $width  The width of the screenshot to take.
     * @param integer $height The height of the screenshot to take.
     *
     * @return boolean
     */
    public static function generateFromPDF($source, $dest, $width, $height)
    {
        $command = dirname(__FILE__).'/Scripts/pdf.sh "'.$source.'" "'.$dest.'" '.$width.' '.$height;
        if (ImageExport::generateScreenshot($command, $dest) === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end generateFromPDF()


    /**
     * Resizes an image.
     *
     * @param string  $src     The source file.
     * @param string  $dest    The destination path.
     * @param mixed   $width   The width of the image.
     * @param mixed   $height  The height of the image.
     * @param boolean $crop    Optionally it can crop the image instead
     *                         of resizing.
     * @param boolean $dynamic Optionally it can dynamically resize based
     *                         on dimensions.
     *
     * @return void
     * @throws ChannelException If the image is an invalid type.
     */
    public static function generateFromImage(
        $src,
        $dest,
        $width=NULL,
        $height=NULL,
        $crop=FALSE,
        $dynamic=FALSE
    ) {
        if (file_exists($src) === FALSE) {
            throw new ChannelException('Source Image does not exist: '.$src);
        }

        $imgInfo = @getimagesize($src);
        if ($imgInfo === FALSE) {
            throw new ChannelException('Failed to get image size: '.$src);
        }

        if (is_array($crop) === FALSE) {
            $crop = array(
                     'width'  => $crop,
                     'height' => $crop,
                    );
        }

        // Source image height and width.
        $owidth  = $imgInfo[0];
        $oheight = $imgInfo[1];
        $swidth  = $width;
        $sheight = $height;
        $srcImg  = NULL;

        if ($width === NULL) {
            $width = $owidth;
        }

        if ($height === NULL) {
            $height = $oheight;
        }

        if ($dynamic === TRUE) {
            if ($owidth > $oheight) {
                // The height will now be dynamically determined from the width
                // and we need to make sure the image is not cropped based on
                // width as we know the width is now exactly what we want.
                $height        = '%';
                $crop['width'] = FALSE;
            } else {
                // Substitute "width" for "height" in the comment above.
                $width          = '%';
                $crop['height'] = FALSE;
            }
        }

        if ($height === '%'
            || ($crop['width'] === FALSE
            && $crop['height'] === TRUE
            && $owidth > $oheight)
        ) {
            $height = ($oheight / ($owidth / $width));
            if ($sheight !== '%' && $height > $sheight) {
                // The height is still bigger than what they asked for, so
                // now we may have to scale the width down, or crop the image.
                if ($crop['height'] === FALSE) {
                    $width = ($width / ($height / $sheight));
                }

                $height = $sheight;
            }
        } else if ($width === '%'
            || ($crop['width'] === TRUE
            && $crop['height'] === FALSE
            && $oheight > $owidth)
        ) {
            $width = ($owidth / ($oheight / $height));
            if ($swidth !== '%' && $width > $swidth) {
                // The width is still bigger than what they asked for, so
                // now we may have to scale the height down, or crop the image.
                if ($crop['width'] === FALSE) {
                    $height = ($height / ($width / $swidth));
                }

                $width = $swidth;
            }
        }//end if

        // If the new size is larger than the original then we should not
        // do any resizing or processing but create a copy of the image.
        // Image functions should be used to apply the image quality
        // settings etc.
        if ($width > $owidth) {
            $width = $owidth;
        }

        if ($height > $oheight) {
            $height = $oheight;
        }

        // Create an image resource for the new image.
        $newImg = imagecreatetruecolor($width, $height);

        // Create an image resource for the source image.
        switch($imgInfo['mime']) {
            case 'image/png':
                $srcImg = imagecreatefrompng($src);

                // For transparency.
                imagealphablending($newImg, FALSE);
                imagesavealpha($newImg, TRUE);
            break;

            case 'image/gif':
                $srcImg = imagecreatefromgif($src);

                // For transparency.
                imagealphablending($newImg, FALSE);
                imagesavealpha($newImg, TRUE);
            break;

            case 'image/jpeg':
                $srcImg = imagecreatefromjpeg($src);
            break;

            default:
                $msg = 'Invalid image type: '.$imgInfo['mime'];
                throw new ChannelException($msg);
            break;
        }//end switch

        // Resize and Crop.
        if ($crop['width'] === TRUE || $crop['height'] === TRUE) {
            $wm = ($owidth / $width);
            $hm = ($oheight / $height);

            if ($crop['width'] === TRUE
                && ($wm > $hm || $crop['height'] === FALSE)
            ) {
                // The width was adjusted more than the height.
                $adjustedWidth = ($owidth / $hm);
                imagecopyresampled(
                    $newImg,
                    $srcImg,
                    0,
                    0,
                    0,
                    0,
                    $adjustedWidth,
                    $height,
                    $owidth,
                    $oheight
                );
            } else {
                // The height was adjusted more than the width.
                $adjustedHeight = ($oheight / $wm);
                imagecopyresampled(
                    $newImg,
                    $srcImg,
                    0,
                    0,
                    0,
                    0,
                    $width,
                    $adjustedHeight,
                    $owidth,
                    $oheight
                );
            }//end if
        } else {
            if ($width > $height) {
                $nwidth = ($owidth / ($oheight / $height));
                if ($nwidth < $width) {
                    $nwidth = $width;
                }

                imagecopyresampled(
                    $newImg,
                    $srcImg,
                    0,
                    0,
                    0,
                    0,
                    $nwidth,
                    $height,
                    $owidth,
                    $oheight
                );
            } else {
                $nheight = ($oheight / ($owidth / $width));
                if ($nheight < $height) {
                    $nheight = $height;
                }

                imagecopyresampled(
                    $newImg,
                    $srcImg,
                    0,
                    0,
                    0,
                    0,
                    $width,
                    $nheight,
                    $owidth,
                    $oheight
                );
            }//end if
        }//end if

        // Save the created image.
        include_once 'Libs/FileSystem/FileSystem.inc';
        $ext = FileSystem::getExtension($dest);
        if (strlen($ext) !== 3) {
            // If extension is not valid then use source image mime type.
            $ext = $imgInfo['mime'];
        }

        switch ($ext) {
            case 'image/jpeg':
            case 'jpg':
                imagejpeg($newImg, $dest, 80);
            break;

            case 'image/png':
            case 'png':
                imagepng($newImg, $dest, 8);
            break;

            case 'image/gif':
            case 'gif':
                imagegif($newImg, $dest);
            break;

            default:
                throw new ChannelException('Invalid image type: '.$ext);
            break;
        }

        imagedestroy($newImg);
        imagedestroy($srcImg);

    }//end generateFromImage()


    /**
     * Starts the virtual X server.
     *
     * @return void
     */
    public static function startServer()
    {
        $start = dirname(__FILE__).'/Scripts/start_server.sh';
        exec($start.' > /dev/null');

    }//end startServer()


    /**
     * Stops the virtual X server.
     *
     * @return void
     */
    public static function stopServer()
    {
        $stop = dirname(__FILE__).'/Scripts/stop_server.sh';
        exec($stop.' > /dev/null');

    }//end stopServer()


    /**
     * Executes a screenshot script and waits until it is done.
     *
     * @param string $command The command that will generate the screenshot.
     * @param string $dest    Where the screenshot will be saved.
     *
     * @return boolean
     */
    public static function generateScreenshot($command, $dest)
    {
        // If the image already exists, remove it first or else the
        // while loop will exit early.
        if (file_exists($dest) === TRUE) {
            unlink($dest);
        }

        // Make sure dest folder exists.
        if (file_exists(dirname($dest)) === FALSE) {
            mkdir(dirname($dest), 0755, TRUE);
        }

        exec($command.' > /dev/null &');

        // We need to wait until a file is created.
        $created  = FALSE;
        $count    = 0;
        $waitSecs = 25;

        while ($created === FALSE) {
            sleep(1);
            if ($waitSecs <= $count) {
                // Failed to create screenshot, continue.
                break;
            }

            // Check for the screenshot file.
            if (file_exists($dest) === TRUE) {
                $created = TRUE;
            }

            $count++;
        }//end while

        return $created;

    }//end generateScreenshot()


}//end class

?>
