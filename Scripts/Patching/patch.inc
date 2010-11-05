<?php
/**
 * Contains the settings for the patch.
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
 * @package    CMS
 * @subpackage Scripts
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/*
    from:   FROM revision.
    to:     TO revision.
    block:  DEPRECATED
            TRUE if updating the Patching system, no further patches in queue
            will be applied (will be unscheduled). It is also recommended not to
            include any upgrade script, especially if you are changing
            PatchingCron::applyPatch() method.
    reboot: TRUE if you want to issue a system reboot command after patching.
*/
$patchInfo = array(
              'from'   => 9049,
              'to'     => 'unknown',
              'block'  => FALSE,
              'reboot' => TRUE,
             );

/**
 * These files are always removed during the installation and
 * patch generation.
 */
$paths_to_remove = array(
                    '/Systems/Developer',
                    '/Scripts/Patching',
                    '/install.php',
                   );

/**
 * These files are removed before TO system is baked.
 */
$paths_to_remove_before_baking_TO_system = array();

/**
 * These files are removed after TO system is baked.
 */
$paths_to_remove_after_baking_TO_system = array();

/**
 * These files are removed before FROM system is baked.
 */
$paths_to_remove_before_baking_FROM_system = array();

/**
 * These files are removed after FROM system is baked.
 */
$paths_to_remove_after_baking_FROM_system = array();

?>
