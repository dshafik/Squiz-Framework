<?php
/**
 * Network methods.
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
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Network methods.
 */
class Network
{

    /**
     * Network interface definition file path.
     *
     * @var string
     */
    private static $_interfaceFilePath = '/etc/network/interfaces';

    /**
     * Directory path where PEM files locate.
     *
     * @var string
     */
    private static $_pemFileDirectoryPath = '/etc/lighttpd/ssl';

    /**
     * Directory path where Lighttpd configuration files locate.
     *
     * @var string
     */
    private static $_lighttpdConfDirectoryPath = '/etc/lighttpd/conf-enabled';

    /**
     * SSL lighttpd configuration file suffix.
     *
     * @var string
     */
    private static $_lighttpdConfFilePrefix = '99-ssl-';

    /**
     * OpenSSL command line tool path.
     *
     * @var string
     */
    private static $_openSSLCommandPath = '/usr/bin/openssl';

    /**
     * Lighttpd Web Server command line tool path.
     *
     * @var string
     */
    private static $_lighttpdCommandPath = '/usr/sbin/lighttpd';

    /**
     * Invoke RC command line tool path.
     *
     * @var string
     */
    private static $_invokeRCCommandPath = '/usr/sbin/invoke-rc.d';

    /**
     * Command line tool path of ifup.
     *
     * @var string
     */
    private static $_ifconfigCommandPath = '/sbin/ifconfig';

    /**
     * Command line tool path of ifup.
     *
     * @var string
     */
    private static $_ifupCommandPath = '/sbin/ifup';

    /**
     * Command line tool path of ifdown.
     *
     * @var string
     */
    private static $_ifdownCommandPath = '/sbin/ifdown';


    /**
     * Create PEM file for SSL support.
     *
     * @param string $fileName       PEM File name to create.
     * @param string $pemFileContent PEM File content to create.
     *
     * @return mixed
     */
    public static function createPemFile($fileName, $pemFileContent)
    {
        $result = FALSE;
        if (file_exists(self::$_pemFileDirectoryPath) === FALSE) {
            $result = mkdir(self::$_pemFileDirectoryPath);
            if ($result === FALSE) {
                return 'Creating '.self::$_pemFileDirectoryPath.' directory failed.';
            }
        }

        if (is_writable(self::$_pemFileDirectoryPath) === TRUE) {
            $filePath = self::$_pemFileDirectoryPath.'/'.$fileName;
            $result   = file_put_contents($filePath, $pemFileContent);
            if ($result !== FALSE) {
                $result = chown($filePath, 'root');
                if ($result === TRUE) {
                    $result = chgrp($filePath, 'root');
                    if ($result === TRUE) {
                        $result = chmod($filePath, 0400);
                        if ($result === TRUE) {
                            return TRUE;
                        } else {
                            return 'Pem file Permission change failed';
                        }
                    } else {
                        return 'Pem file Group ownership change failed';
                    }
                } else {
                    return 'Pem file Ownership change failed';
                }
            } else {
                return 'Pem file creation failed';
            }
        }//end if

        return $result;

    }//end createPemFile()


    /**
     * Return the PEM file path for the filename passed.
     *
     * @param string $fileName PEM file name to remove.
     *
     * @return string
     */
    public static function getPemFilePath($fileName)
    {
        $filePath = self::$_pemFileDirectoryPath.'/'.$fileName;
        return $filePath;

    }//end getPemFilePath()


    /**
     * Remove PEM file created.
     *
     * @param string $fileName PEM file name to remove.
     *
     * @return mixed
     */
    public static function removePemFile($fileName)
    {
        $result   = FALSE;
        $filePath = self::$_pemFileDirectoryPath.'/'.$fileName;
        if (file_exists($filePath) === TRUE
            && is_writable($filePath) === TRUE
        ) {
            $result = unlink($filePath);
        }

        return $result;

    }//end removePemFile()


    /**
     * Test PEM File integrity.
     *
     * @param string $fileName PEM file name to test.
     *
     * @return mixed
     */
    public static function testPemFileIntegrity($fileName)
    {
        $outputFilePath = self::_getCommandOutputFilePath();

        $command   = self::$_openSSLCommandPath.' x509 -noout -in ';
        $command  .= self::$_pemFileDirectoryPath.'/'.$fileName;
        $command  .= ' 1> '.$outputFilePath;
        $command  .= ' 2> '.$outputFilePath;
        $returnVal = NULL;
        system($command, $returnVal);

        $output = file_get_contents($outputFilePath);
        $output = trim($output);

        unlink($outputFilePath);
        if (empty($output) === TRUE) {
            return TRUE;
        }

        return $output;

    }//end testPemFileIntegrity()


    /**
     * Extract Private key from PEM file.
     *
     * @param string $filePath PEM file path to test.
     *
     * @return string
     */
    public static function getPrivateKeyFromPEMFile($filePath)
    {
        $privateKey = '';
        if (file_exists($filePath) === FALSE) {
            return FALSE;
        }

        $pemContent      = file_get_contents($filePath);
        $prvKeyHeader    = '-----BEGIN RSA PRIVATE KEY-----';
        $prvKeyFooter    = '-----END RSA PRIVATE KEY-----';
        $prvKeyHeaderLen = strlen($prvKeyHeader);
        $prvKeyFooterLen = strlen($prvKeyFooter);
        $prvKeyHeaderPos = strpos($pemContent, $prvKeyHeader);
        $prvKeyFooterPos = strpos($pemContent, $prvKeyFooter);

        if ($prvKeyHeaderPos === FALSE || $prvKeyFooterPos === FALSE) {
            return FALSE;
        }

        $keyBody = substr(
            $pemContent,
            ($prvKeyHeaderPos + $prvKeyHeaderLen),
            ($prvKeyFooterPos - ($prvKeyHeaderPos + $prvKeyHeaderLen))
        );

        $privateKey  = $prvKeyHeader."\n";
        $privateKey .= trim($keyBody)."\n";
        $privateKey .= $prvKeyFooter."\n";

        return $privateKey;

    }//end getPrivateKeyFromPEMFile()


    /**
     * Test whether the passed PEM file has a password on private key or not.
     *
     * @param string $filePath PEM file path.
     *
     * @return boolean
     */
    public static function testPemFilePrivateKeyPassword($filePath)
    {
        $pid = pcntl_fork();
        // @codingStandardsIgnoreStart
        if ($pid == -1) {
            return FALSE;
        } else if ($pid) {
        // @codingStandardsIgnoreEnd
            $count          = 0;
            $outputFilePath = self::_getCommandOutputFilePath();
            while (TRUE) {
                // Wait for 3 seconds and still does not have
                // completed message. Password inpu must be popped on the
                // command line.
                if ($count > 2) {
                    // Kill child process.
                    system('/bin/kill -9 '.$pid);

                    // Kill OpenSSL command process.
                    $openSSLPid = NULL;
                    $output     = array();
                    $command    = '/bin/ps ax | /bin/grep openssl';
                    exec($command, $output, $returnVar);
                    foreach ($output as $line) {
                        $comm = self::$_openSSLCommandPath.' rsa -in '.$filePath.' -noout';
                        if (strpos($line, $comm) !== FALSE) {
                            $pattern = '/^([\d]+) pts/';
                            $matches = array();
                            preg_match_all($pattern, $line, $matches);
                            if (empty($matches) === FALSE) {
                                $openSSLPid = $matches[1][0];
                                break;
                            }
                        }
                    }

                    if ($openSSLPid !== NULL) {
                        system('/bin/kill -9 '.$openSSLPid);
                    }

                    // Also remove output file if it exists somehow.
                    if (file_exists($outputFilePath) === TRUE) {
                        unlink($outputFilePath);
                    }

                    return FALSE;
                }//end if

                if (file_exists($outputFilePath) === TRUE) {
                    $output = file_get_contents($outputFilePath);
                    if ($output === 'done') {
                        unlink($outputFilePath);
                        return TRUE;
                    }
                }

                sleep(1);
                $count++;
            }//end while
        } else {
            // Child process will try to remove the password from the file.
            $command = self::$_openSSLCommandPath.' rsa -in '.$filePath.' -noout';

            $output    = array();
            $returnVal = NULL;
            exec($command, $output, $returnVal);

            $outputFilePath = self::_getCommandOutputFilePath();
            file_put_contents($outputFilePath, 'done');
            exit;
        }//end if

    }//end testPemFilePrivateKeyPassword()


    /**
     * Create Lighttpd SSL configuration file.
     *
     * @param string $fileName            Configuration file name to create.
     * @param string $lighttpdConfContent Configuration file content.
     *
     * @return mixed
     */
    public static function createLighttpdSSLConfFile($fileName, $lighttpdConfContent)
    {
        $result         = FALSE;
        $fileNamePrefix = '99-ssl-';
        if (is_writable(self::$_lighttpdConfDirectoryPath) === TRUE) {
            $filePath = self::$_lighttpdConfDirectoryPath.'/'.self::$_lighttpdConfFilePrefix.$fileName;
            $result   = file_put_contents($filePath, $lighttpdConfContent);
            if ($result !== FALSE) {
                $result = chown($filePath, 'root');
                if ($result === TRUE) {
                    $result = chgrp($filePath, 'root');
                    if ($result === TRUE) {
                        $result = chmod($filePath, 0755);
                        if ($result === TRUE) {
                            return TRUE;
                        } else {
                            return 'Lighttpd conf file Permission change failed';
                        }
                    } else {
                        return 'Lighttpd conf file Group ownership change failed';
                    }
                } else {
                    return 'Lighttpd conf file Ownership change failed';
                }
            } else {
                return 'Lighttpd conf file creation failed';
            }
        }//end if

        return $result;

    }//end createLighttpdSSLConfFile()


    /**
     * Remove Lighttpd SSL configuration file.
     *
     * @param string $fileName Configuration file name to remove.
     *
     * @return mixed
     */
    public static function removeLighttpdSSLConfFile($fileName)
    {
        $result   = FALSE;
        $filePath = self::$_lighttpdConfDirectoryPath.'/'.self::$_lighttpdConfFilePrefix.$fileName;
        if (file_exists($filePath) === TRUE
            && is_writable($filePath) === TRUE
        ) {
            $result = unlink($filePath);
        }

        return $result;

    }//end removeLighttpdSSLConfFile()


    /**
     * Returns Lighttpd SSL configuration string.
     *
     * @param string $identifier  Domain name or IP address to bind.
     * @param string $pemFileName PEM file name.
     *
     * @return string
     */
    public static function getLighttpdSSLConfString($identifier, $pemFileName)
    {
        $string  = '$SERVER["socket"] == "'.$identifier.':443" {'."\n";
        $string .= '    ssl.engine             = "enable"'."\n";
        $string .= '    ssl.pemfile            = "'.self::$_pemFileDirectoryPath.'/'.$pemFileName.'"'."\n";
        $string .= '    ssl.use-sslv2          = "disable"'."\n";
        $string .= '    setenv.add-environment = ('."\n";
        $string .= '      "HTTPS" => "on" '."\n";
        $string .= '    )'."\n";
        $string .= '}';

        return $string;

    }//end getLighttpdSSLConfString()


    /**
     * Test Lighttpd configuration file.
     *
     * @return boolean
     */
    public static function testLighttpdConf()
    {
        $command   = self::$_lighttpdCommandPath.' -t -f /etc/lighttpd/lighttpd.conf';
        $returnVal = NULL;
        system($command, $returnVal);
        if ($returnVal === 0) {
            return TRUE;
        }

        return FALSE;

    }//end testLighttpdConf()


    /**
     * Restart Lighttpd Web server.
     *
     * @return boolean
     */
    public static function restartLighttpd()
    {
        $command   = self::$_invokeRCCommandPath.' lighttpd restart';
        $returnVal = NULL;
        system($command, $returnVal);
        if ($returnVal === 0) {
            return TRUE;
        }

        return FALSE;

    }//end restartLighttpd()


    /**
     * Returns the existing network interfaces.
     *
     * @return array
     */
    public static function getNetworkInterfaces()
    {
        $pattern  = '/auto eth0:([\d]+)\s+iface ';
        $pattern .= 'eth0:([\d]+) inet static\s+address ';
        $pattern .= '(\d+)\.(\d+)\.(\d+)\.(\d+)\s+netmask ';
        $pattern .= '(\d+)\.(\d+)\.(\d+)\.(\d+)/';

        $interfacesStr = file_get_contents(self::$_interfaceFilePath);
        $matches       = array();
        $interfaces    = array();
        preg_match_all($pattern, $interfacesStr, $matches);
        if (empty($matches[0]) === FALSE) {
            $interfaceLen = count($matches[0]);
            for ($i = 0; $i < $interfaceLen; $i++) {
                $interfaces[] = array(
                                 'alias'   => ($i + 1),
                                 'str'     => $matches[0][$i],
                                 'ip'      => $matches[3][$i].'.'.$matches[4][$i].'.'.$matches[5][$i].'.'.$matches[6][$i],
                                 'netmask' => $matches[7][$i].'.'.$matches[8][$i].'.'.$matches[9][$i].'.'.$matches[10][$i],
                                );
            }
        }

        return $interfaces;

    }//end getNetworkInterfaces()


    /**
     * Returns new network interface string.
     *
     * @param string $ipAddress IP address to bind.
     * @param string $netmask   Netmask address to use.
     *
     * @return string
     */
    public static function createNewNetworkInterface($ipAddress, $netmask)
    {
        $currInterfaces  = self::getNetworkInterfaces();
        $interfacesLen   = count($currInterfaces);
        $interfacesAlias = ($interfacesLen + 1);

        $string  = 'auto eth0:'.$interfacesAlias."\n";
        $string .= '  iface eth0:'.$interfacesAlias.' inet static'."\n";
        $string .= '  address '.$ipAddress."\n";
        $string .= '  netmask '.$netmask;

        $result = array(
                   'interfaceStr'   => $string,
                   'interfaceAlias' => $interfacesAlias,
                  );

        return $result;

    }//end createNewNetworkInterface()


    /**
     * Add the network interface string to system's interfaces file.
     *
     * @param string $interfaceStr Interface string to add.
     *
     * @return mixed
     */
    public static function addNetworkInterface($interfaceStr)
    {
        $result = FALSE;
        if (is_writable(self::$_interfaceFilePath) === TRUE) {
            $interfaceStr = "\n".trim($interfaceStr)."\n";
            $result       = file_put_contents(
                self::$_interfaceFilePath,
                $interfaceStr,
                FILE_APPEND
            );

            if ($result !== FALSE) {
                return TRUE;
            }
        }

        return $result;

    }//end addNetworkInterface()


    /**
     * Remove network interface string from system's interfaces file.
     *
     * @param string $interfaceStr Interface string to remove.
     *
     * @return string
     */
    public static function removeNetworkInterface($interfaceStr)
    {
        $result = FALSE;
        if (is_writable(self::$_interfaceFilePath) === TRUE) {
            $content = file_get_contents(self::$_interfaceFilePath);
            $content = trim($content);
            if (strpos($content, $interfaceStr) !== FALSE) {
                $content = str_replace($interfaceStr, '', $content);
                $content = preg_replace('/\n{3,}/', "\n\n", $content);
                $result  = file_put_contents(
                    self::$_interfaceFilePath,
                    $content
                );

                if ($result !== FALSE) {
                    return TRUE;
                }
            }
        }

        return $result;

    }//end removeNetworkInterface()


    /**
     * Returns the details of the network interface.
     *
     * @param string $interName Name of the interface like eth0.
     *
     * @return array
     */
    public static function getNetworkInterfaceAddresses($interName)
    {
        $command   = self::$_ifconfigCommandPath.' '.$interName;
        $returnVal = NULL;
        $output    = array();
        $result    = array();

        $result[$interName] = array(
                               'ip'        => '',
                               'broadcast' => '',
                               'netmask'   => '',
                              );
        exec($command, $output, $returnVal);

        $pattern  = '/inet addr:([\d]+)\.([\d]+)\.([\d]+)\.([\d]+)\s+';
        $pattern .= 'Bcast:([\d]+)\.([\d]+)\.([\d]+)\.([\d]+)\s+';
        $pattern .= 'Mask:([\d]+)\.([\d]+)\.([\d]+)\.([\d]+)/';
        preg_match_all($pattern, implode("\n", $output), $matches);
        if (empty($matches[0]) === FALSE) {
            $result[$interName]['ip']        = $matches[1][0].'.'.$matches[2][0].'.'.$matches[3][0].'.'.$matches[4][0];
            $result[$interName]['broadcast'] = $matches[5][0].'.'.$matches[6][0].'.'.$matches[7][0].'.'.$matches[8][0];
            $result[$interName]['netmask']   = $matches[9][0].'.'.$matches[10][0].'.'.$matches[11][0].'.'.$matches[12][0];
        }

        return $result;

    }//end getNetworkInterfaceAddresses()


    /**
     * Start up the aliased network interface.
     *
     * @param string $alias Alias for eth0 network interface.
     *
     * @return mixed
     */
    public static function startNetworkInterface($alias)
    {
        $outputFilePath = self::_getCommandOutputFilePath();

        $command   = self::$_ifupCommandPath.' eth0:'.$alias;
        $command  .= ' 1> '.$outputFilePath;
        $command  .= ' 2> '.$outputFilePath;
        $returnVal = NULL;

        system($command, $returnVal);

        $output = file_get_contents($outputFilePath);
        $output = trim($output);

        unlink($outputFilePath);

        if (empty($output) === FALSE) {
            return $output;
        }

        return TRUE;

    }//end startNetworkInterface()


    /**
     * Stop up the aliased network interface.
     *
     * @param string $alias Alias for eth0 network interface.
     *
     * @return string
     */
    public static function stopNetworkInterface($alias)
    {
        $outputFilePath = self::_getCommandOutputFilePath();

        $command   = self::$_ifdownCommandPath.' eth0:'.$alias;
        $command  .= ' 1> '.$outputFilePath;
        $command  .= ' 2> '.$outputFilePath;
        $returnVal = NULL;
        system($command, $returnVal);

        $output = file_get_contents($outputFilePath);
        $output = trim($output);

        unlink($outputFilePath);

        if (empty($output) === FALSE) {
            return $output;
        }

        return TRUE;

    }//end stopNetworkInterface()


    /**
     * Validate the IP address.
     *
     * @param string $ipStr IP address to validate.
     *
     * @return boolean
     */
    public static function isValidIPAddress($ipStr)
    {
        $pattern = '/^([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(\.([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])){3}$/';
        if (preg_match($pattern, $ipStr) === 1) {
            return TRUE;
        }

        return FALSE;

    }//end isValidIPAddress()


    /**
     * Output file path for checking the command execution.
     *
     * @return string
     */
    private static function _getCommandOutputFilePath()
    {
        $path = Init::ROOT_DIR.'/data/output.txt';
        return $path;

    }//end _getCommandOutputFilePath()


}//end class

?>
