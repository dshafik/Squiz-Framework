<?php
/**
 * This script resets a users' password.
 *
 * You need to pass in both a username and password passed to the script.
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

if ($argv[0] !== 'Scripts/'.basename(__FILE__)) {
    echo 'You need to run this script from '.dirname(dirname(__FILE__))."\n";
    echo 'Then call it like: php Scripts/'.basename(__FILE__)."\n";
    exit(1);
}

if (isset($argv[1]) === FALSE || isset($argv[2]) === FALSE) {
    echo 'You need to supply the username to reset, ';
    echo 'along with the new password.'."\n";
    echo $argv[0].' username newpassword'."\n";
    exit(1);
}

$username = $argv[1];
$password = $argv[2];

require dirname(dirname(__FILE__)).'/Channels/Channels.inc';
Channels::includeSystem('User');

$userid = User::getUserIdByUsername($username);
if ($userid === FALSE) {
    echo "That username doesn't exist. Try again.\n";
    exit(2);
}

$userinfo = array(
             'userid'   => $userid,
             'password' => $password,
             'hashed'   => FALSE,
            );

try {
    User::setUserPassword($userinfo);
} catch (ChannelException $e) {
    echo 'Unable to change the password: '.$e->getMessage()."\n";
    exit(3);
}

echo 'The password has been reset successfully.'."\n";

?>
