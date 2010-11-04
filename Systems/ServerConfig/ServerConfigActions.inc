<?php
/**
 * Actions for the ServerConfig System.
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
 * @package    Analytics
 * @subpackage SquizAnalytics
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * ServerConfig Actions Class.
 *
 * @since 4.0.0
 */
class ServerConfigActions
{

    /**
     * File path of hostname file on the server.
     *
     * @var string
     */
    private static $_hostnameFilePath = '/etc/hostname';

    /**
     * File path of mailname file on the server.
     *
     * @var string
     */
    private static $_mailnameFilePath = '/etc/mailname';

    /**
     * File path of interfaces file on the server.
     *
     * @var string
     */
    private static $_interfacesFilePath = '/etc/network/interfaces';

    /**
     * File path of resolv.conf file on the server.
     *
     * @var string
     */
    private static $_resolvConfFilePath = '/etc/resolv.conf';

    /**
     * Network Configuration types.
     *
     * @var array
     */
    private static $_networkConfTypes = array(
                                         'ip',
                                         'netmask',
                                         'broadcast,',
                                         'gateway',
                                         'namespace1',
                                         'namespace2',
                                        );

    /**
     * Network Configuration cache file.
     *
     * @var array
     */
    private static $_networkConfCache = array();


    /**
     * Returns the hostname on the server.
     *
     * @return string
     */
    public static function getHostname()
    {
        if (isset(self::$_networkConfCache['hostname']) === TRUE) {
            $hostname = self::$_networkConfCache['hostname'];
        } else {
            include_once 'Libs/FileSystem/FileSystem.inc';
            $hostname = 'localhost';
            if (file_exists(self::$_hostnameFilePath) === TRUE) {
                $hostname = FileSystem::removeShellCommentLines(
                    trim(file_get_contents(self::$_hostnameFilePath))
                );
            }

            self::$_networkConfCache['hostname'] = $hostname;
        }

        return $hostname;

    }//end getHostname()


    /**
     * Returns the mailname for exim service on the server.
     *
     * @return string
     */
    public static function getMailname()
    {
        if (isset(self::$_networkConfCache['mailname']) === TRUE) {
            $mailname = self::$_networkConfCache['mailname'];
        } else {
            include_once 'Libs/FileSystem/FileSystem.inc';
            $mailname = '';
            if (file_exists(self::$_mailnameFilePath) === TRUE) {
                $mailname = FileSystem::removeShellCommentLines(
                    trim(file_get_contents(self::$_mailnameFilePath))
                );
            }

            self::$_networkConfCache['mailname'] = $mailname;
        }

        return $mailname;

    }//end getMailname()


    /**
     * Returns the value of IP address part for each type.
     *
     * @param string  $type Type of network config to get.
     * @param integer $part Part index number of IP address.
     *
     * @return integer
     */
    public static function getNetworkConfigValue($type, $part)
    {
        if (isset(self::$_networkConfCache[$type.$part]) === TRUE) {
            return self::$_networkConfCache[$type.$part];
        }

        $ipRegex = '\s+(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})';
        $regexes = array(
                    'ip'        => array(
                                    'reg' => '/address'.$ipRegex.'/',
                                    'def' => array(
                                              127,
                                              0,
                                              0,
                                              1,
                                             ),
                                   ),
                    'netmask'   => array(
                                    'reg' => '/netmask'.$ipRegex.'/',
                                    'def' => array(
                                              255,
                                              255,
                                              255,
                                              0,
                                             ),
                                   ),
                    'broadcast' => array(
                                    'reg' => '/broadcast'.$ipRegex.'/',
                                    'def' => array(
                                              255,
                                              255,
                                              255,
                                              0,
                                             ),
                                   ),
                    'gateway'   => array(
                                    'reg' => '/(gateway|up\s+route\s+add\s+default\s+gw)'.$ipRegex.'/',
                                    'def' => array(
                                              127,
                                              0,
                                              0,
                                              1,
                                             ),
                                   ),
                   );

        include_once 'Libs/FileSystem/FileSystem.inc';
        switch ($type) {
            case 'ip':
            case 'netmask':
            case 'broadcast':
            case 'gateway':
                // Load the /etc/network/interfaces file.
                $interfaces = FileSystem::removeShellCommentLines(
                    trim(file_get_contents(self::$_interfacesFilePath))
                );

                $matches = array();
                preg_match($regexes[$type]['reg'], $interfaces, $matches);
                if (empty($matches) === FALSE) {
                    self::$_networkConfCache[$type.'1'] = (int) $matches[1];
                    self::$_networkConfCache[$type.'2'] = (int) $matches[2];
                    self::$_networkConfCache[$type.'3'] = (int) $matches[3];
                    self::$_networkConfCache[$type.'4'] = (int) $matches[4];
                } else {
                    self::$_networkConfCache[$type.'1'] = $regexes[$type]['def'][0];
                    self::$_networkConfCache[$type.'2'] = $regexes[$type]['def'][1];
                    self::$_networkConfCache[$type.'3'] = $regexes[$type]['def'][2];
                    self::$_networkConfCache[$type.'4'] = $regexes[$type]['def'][3];
                }
            break;

            case 'nameserver1':
            case 'nameserver2':
                // Load the /etc/resolv.conf file.
                $matches     = array();
                $resolvRegex = '/(nameserver'.$ipRegex.')/';
                if (file_exists(self::$_resolvConfFilePath) === TRUE) {
                    $resolvConf = FileSystem::removeShellCommentLines(
                        trim(file_get_contents(self::$_resolvConfFilePath))
                    );

                    preg_match_all($resolvRegex, $resolvConf, $matches);
                    if (empty($matches[0][0]) === FALSE) {
                        self::$_networkConfCache[$type.'1'] = (int) $matches[2][0];
                        self::$_networkConfCache[$type.'2'] = (int) $matches[3][0];
                        self::$_networkConfCache[$type.'3'] = (int) $matches[4][0];
                        self::$_networkConfCache[$type.'4'] = (int) $matches[5][0];
                    } else {
                        self::$_networkConfCache[$type.'1'] = 127;
                        self::$_networkConfCache[$type.'2'] = 0;
                        self::$_networkConfCache[$type.'3'] = 0;
                        self::$_networkConfCache[$type.'4'] = 1;
                    }
                }//end if
            break;

            default:
                // No default.
            break;
        }//end switch

        return self::$_networkConfCache[$type.$part];

    }//end getNetworkConfigValue()


    /**
     * Returns the JS code to initialise the Roles Screen.
     *
     * @return string
     */
    public static function getNetworkConfigurationInitCode()
    {
        $data = array(
                 'currentProduct'    => 1,
                 'connectedProducts' => 2,
                );

        $data = json_encode($data);
        return '<script>NetworkConfiguration.initScreen('.$data.');</script>';

    }//end getNetworkConfigurationInitCode()


    /**
     * Returns TRUE if the IP address is used to access the system.
     *
     * @return boolean
     */
    public static function isIPRequest()
    {
        include_once 'Libs/Web/Web.inc';
        $url   = Web::getCurrentURL();
        $parts = parse_url($url);

        $pattern = '/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/';
        if (preg_match($pattern, $parts['host']) === 1) {
            return TRUE;
        }

        return FALSE;

    }//end isIPRequest()


    /**
     * Paints the Network Configuration template.
     *
     * @return void
     */
    public static function paintNetworkConfiguration()
    {
        Channels::includeSystem('GUI');
        GUI::printTemplate('ServerConfig', 'NetworkConfiguration.tpl');

    }//end paintNetworkConfiguration()


    /**
     * Process the Network Configuration template.
     *
     * @return string
     *
     * @api            write
     * @api-permission superuser
     */
    public static function saveNetworkConfiguration()
    {
        include_once 'Libs/Security/Security.inc';
        $action = Security::getRequestData('action', '');
        switch ($action) {
            case 'saveNetworkConfig':
                return self::_saveNetworkConfigChanges();
            break;

            case 'checkNetworkConfigUpdate':
                return self::_checkNetworkConfigUpdate();
            break;

            default:
                // No default.
            break;
        }

    }//end saveNetworkConfiguration()


    /**
     * Saves the changes made to the Network Configuration.
     *
     * @return string
     */
    private static function _saveNetworkConfigChanges()
    {
        include_once 'Libs/Security/Security.inc';
        $values   = array();
        $formData = Security::getRequestData('values', NULL);

        $valid = FALSE;
        $types = array(
                  'ip',
                  'netmask',
                  'broadcast',
                  'gateway',
                  'nameserver1',
                  'nameserver2',
                 );

        // Make sure all the IP addressed entered are valid ones.
        foreach ($types as $type) {
            $ipAddress  = $formData[$type.'1'].'.';
            $ipAddress .= $formData[$type.'2'].'.';
            $ipAddress .= $formData[$type.'3'].'.';
            $ipAddress .= $formData[$type.'4'];

            $values[$type] = $ipAddress;

            $valid = ip2long($ipAddress);
            if ($valid !== FALSE) {
                $valid = long2ip($valid);
                if ($valid !== FALSE) {
                    $valid = TRUE;
                }
            }

            if ($valid !== TRUE) {
                // The IP address entered is not valid.
                break;
            }
        }//end foreach

        if ($valid === FALSE) {
            return 'failed';
        }

        $interfaces  = 'auto lo'."\n";
        $interfaces .= 'iface lo inet loopback'."\n\n";
        $interfaces .= 'auto eth0'."\n";
        $interfaces .= 'iface eth0 inet static'."\n";
        $interfaces .= 'address '.$values['ip']."\n";
        $interfaces .= 'netmask '.$values['netmask']."\n";
        $interfaces .= 'broadcast '.$values['broadcast']."\n";
        $interfaces .= 'gateway '.$values['gateway']."\n";
        $interfaces .= 'post-up /sbin/iptables-restore < /etc/network/iptables-save'."\n";
        $interfaces .= 'pre-down /sbin/iptables-save > /etc/network/iptables-save'."\n";

        $resolveFile  = 'nameserver '.$values['nameserver1']."\n";
        $resolveFile .= 'nameserver '.$values['nameserver2']."\n";

        $mailnameFile = $formData['mailname']."\n";
        $hostnameFile = $formData['hostname']."\n";

        include_once 'Libs/FileSystem/FileSystem.inc';
        $success = FALSE;
        $result  = FileSystem::filePutContents(
            BaseSystem::getDataDir().'/interfaces',
            $interfaces
        );

        if ($result !== FALSE) {
            $result = FileSystem::filePutContents(
                BaseSystem::getDataDir().'/resolv.conf',
                $resolveFile
            );
            if ($result !== FALSE) {
                $result = FileSystem::filePutContents(
                    BaseSystem::getDataDir().'/hostname',
                    $hostnameFile
                );
                if ($result !== FALSE) {
                    $result = FileSystem::filePutContents(
                        BaseSystem::getDataDir().'/mailname',
                        $mailnameFile
                    );
                    if ($result !== FALSE) {
                        $success = TRUE;
                    }
                }
            }
        }//end if

        if ($success === TRUE) {
            return 'success';
        } else {
            return 'failed';
        }

    }//end _saveNetworkConfigChanges()


    /**
     * Checks whether new network config setting has been successful or not.
     *
     * @return string
     */
    private static function _checkNetworkConfigUpdate()
    {
        $dataDirPath = BaseSystem::getDataDir();
        if (file_exists($dataDirPath.'/network_changes_failed') === TRUE) {
            $command   = 'rm '.$dataDirPath.'/network_changes_failed';
            $output    = array();
            $returnVal = NULL;
            exec($command, $output, $returnVal);
            return 'failed';
        } else if (file_exists($dataDirPath.'/network_changes_applied') === FALSE) {
            // Cron hasn't run yet.
            return 'false';
        } else {
            // Okay, the changes applied and the script got here!
            // Let's remove old settings now so that cron won't put the
            // original settings back.
            $command   = 'rm '.$dataDirPath.'/hostname_old ';
            $command  .= $dataDirPath.'/mailname_old ';
            $command  .= $dataDirPath.'/interfaces_old ';
            $command  .= $dataDirPath.'/resolv.conf_old ';
            $command  .= $dataDirPath.'/network_changes_applied';
            $output    = array();
            $returnVal = NULL;
            exec($command, $output, $returnVal);

            if ($returnVal === 0) {
                return 'true';
            } else {
                return 'false';
            }
        }//end if

    }//end _checkNetworkConfigUpdate()


    /**
     * Print API Token hidden field for manual processing.
     *
     * @return void
     */
    public static function printAPIToken()
    {
        Channels::includeSystem('User');
        Channels::includeSystem('API');
        $currUserid = User::getCurrentUserid();
        if ($currUserid !== NULL) {
            $currToken = API::getUserToken();
            if ($currToken === NULL) {
                $currToken = API::getUniqueToken();
                API::setUserToken($currToken);
            }
        }

        echo '<input id="__api_token" type="hidden" value="'.$currToken.'" />';

    }//end printAPIToken()


}//end class

?>