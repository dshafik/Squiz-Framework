<?php
/**
 * Actions for the GUI File Upload System.
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
 * GUI FileUpload Actions Class.
 */
class GUIFileUploadActions
{

    /**
     * Name attribute of the text field.
     *
     * @var string
     */
    protected static $name = 'GUIFileUpload';

    /**
     * Value attribute of the text field.
     *
     * @var string
     */
    protected static $value = '';

    /**
     * If TRUE then the container will have the style display:inline.
     *
     * @var boolean
     * @internal
     */
    protected static $inline = FALSE;


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings=array())
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div class="'.$class.' container';

        $inline = Util::getArrayIndex($settings, 'inline', FALSE);
        if ($inline === TRUE || strtolower($inline) === 'true') {
            echo ' inline';
        }

        echo '" id="'.$id.'">';

        $value = Util::getArrayIndex(
            $settings,
            'value',
            ''
        );

        $settings['widget']['apc_key'] = uniqid();

        // The input field.
        Channels::includeSystem('API');
        echo '<form enctype="multipart/form-data" id="'.$id.'-form" target="'.$id.'-iframe" method="POST" class="GUIFileUpload-upload-form">';
        echo '<input type="hidden" value="'.$settings['widget']['apc_key'].'" name="APC_UPLOAD_PROGRESS" />';
        echo '<input id="'.$id.'-input" type="file" class="'.$class.' input" name="'.$id.'-fileUpload" />';
        echo '<input id="'.$id.'-upload" type="submit" class="'.$class.' upload" value="'._('Upload').'" />';
        echo '<input name="id" type="hidden" value="'.$id.'" />';
        echo '<input name="_api_token" type="hidden" value="'.API::getUserToken().'" />';
        echo '</form>';

        echo '<div class="GUIFileUpload-progress-wrap hidden">';
        echo '<span class="GUIFileUpload-progress-msg">'._('Uploading...').'</span>';
        echo '</div>';

        echo '<div class="GUIFileUpload-complete-wrap hidden">';
        echo '<span class="GUIFileUpload-complete-filename"> </span>';
        echo '<span class="GUIFileUpload-complete-msg">'._(' has been uploaded.').'</span>';
        echo '</div>';

        echo '<iframe id="'.$id.'-iframe" name="'.$id.'-iframe" style="display: none;" > </iframe>';
        echo '</div>';

        return $settings;

    }//end paint()


    /**
     * Clean up the uploaded file.
     *
     * @param string $filename File to clean up.
     *
     * @return void
     */
    public static function cleanupFile($filename)
    {
        $targetFile  = BaseSystem::getDataDir('GUIFileUpload');
        $targetFile .= '/'.$filename;
        if (file_exists($targetFile) === TRUE) {
            unlink($targetFile);
        }

    }//end cleanupFile()


    /**
     * Move the uploaded file to the target location.
     *
     * @param string $filename   File to move.
     * @param string $targetPath The location to move the file to.
     *
     * @return boolean
     */
    public static function moveUploadedFile($filename, $targetPath)
    {
        $targetFile  = BaseSystem::getDataDir('GUIFileUpload');
        $targetFile .= '/'.$filename;
        if (file_exists($targetFile) === TRUE) {
            $res = copy($targetFile, $targetPath);
            if ($res === TRUE) {
                GUIFileUpload::cleanupFile($filename);
                return TRUE;
            }
        }

        return FALSE;

    }//end moveUploadedFile()


    /**
     * Prints this widgets content.
     *
     * @return boolean
     *
     * @api           read
     * @api-privilege public
     */
    public static function processFileUpload()
    {
        include_once 'Libs/Security/Security.inc';
        $id   = Security::getRequestData('id', '');
        $name = $id.'-fileUpload';

        $fileInfo = Security::getRequestData($name, array(), FALSE, 'files');
        if (empty($fileInfo) === FALSE) {
            $targetPath  = BaseSystem::getDataDir('GUIFileUpload');
            $targetPath .= '/'.basename($fileInfo['name']);
            if (move_uploaded_file($fileInfo['tmp_name'], $targetPath) === TRUE) {
                return TRUE;
            }
        }

        return FALSE;

    }//end processFileUpload()


    /**
     * Process widget AJAX call.
     *
     * @param string $progKey AFC Fetch key to retrieve the upload progress.
     *
     * @return integer
     *
     * @api           read
     * @api-privilege public
     */
    public static function getFileUploadProgress($progKey)
    {
        $prog = 0;
        if (function_exists('apc_fetch') === TRUE) {
            $status = apc_fetch('upload_'.$progKey);
            if ($status !== FALSE) {
                $prog = ($status['current'] / $status['total']);
                $prog = ($prog * 100);
                if ($status['current'] === $status['total']) {
                    $res = apc_delete('upload_'.$progKey);
                }
            }
        } else {
            $prog = 100;
        }

        return $prog;

    }//end getFileUploadProgress()


}//end class

?>
