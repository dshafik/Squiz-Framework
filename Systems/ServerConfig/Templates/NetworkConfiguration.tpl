<html>
<head>
    <title>Network Configuration</title>
</head>
<body class="NetworkConfiguration">
    <div id="NetworkConfiguration">
        <div class="NetworkConfigurationWrap" style="visibility: visible; margin-left:-262px; margin-top:-163.5px; top: 50%; left: 50%;">
            <div class="NetworkConfiguration-top">
                <div class="NetworkConfiguration-top-headerText">Network Configuration</div>
            </div>
            <div class="NetworkConfiguration-middle">
                <!-- Form Table Starts -->
                <table id="ipTable" class="NetworkConfiguration-formTable">
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Hostname</td>
                        <td>
                            <widget type="GUI/TextBox" id="hostname">
                                <setting name="name">hostname</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getHostname" />
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Mailname</td>
                        <td>
                            <widget type="GUI/TextBox" id="mailname">
                                <setting name="name">mailname</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getMailname" />
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2"><hr /></td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">IP Address</td>
                        <td>
                            <widget type="GUI/TextBox" id="ip1">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">ip1</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>ip</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="ip2">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">ip2</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>ip</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="ip3">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">ip3</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>ip</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="ip4">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">ip4</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>ip</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Netmask</td>
                        <td>
                            <widget type="GUI/TextBox" id="netmask1">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">netmask1</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>netmask</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="netmask2">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">netmask2</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>netmask</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="netmask3">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">netmask3</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>netmask</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="netmask4">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">netmask4</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>netmask</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Broadcast</td>
                        <td>
                            <widget type="GUI/TextBox" id="broadcast1">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">broadcast1</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>broadcast</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="broadcast2">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">broadcast2</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>broadcast</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="broadcast3">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">broadcast3</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>broadcast</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="broadcast4">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">broadcast4</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>broadcast</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Gateway</td>
                        <td>
                            <widget type="GUI/TextBox" id="gateway1">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">gateway1</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>gateway</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="gateway2">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">gateway2</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>gateway</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="gateway3">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">gateway3</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>gateway</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="gateway4">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">gateway4</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>gateway</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2"><hr /></td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">Name Server IP</td>
                        <td>
                            <widget type="GUI/TextBox" id="nameserver11">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver11</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver1</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver12">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver12</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver1</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver13">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver13</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver1</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver14">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver14</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver1</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                    <tr>
                        <td class="NetworkConfiguration-titleCol">2nd Name Server IP</td>
                        <td>
                            <widget type="GUI/TextBox" id="nameserver21">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver21</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver2</arg>
                                        <arg>1</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver22">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver22</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver2</arg>
                                        <arg>2</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver23">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver23</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver2</arg>
                                        <arg>3</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                            <widget type="GUI/TextBox" id="nameserver24">
                                <setting name="inline">true</setting>
                                <setting name="customClass">ipaddress</setting>
                                <setting name="name">nameserver24</setting>
                                <setting name="size">30</setting>
                                <setting name="value">
                                    <dataProvider action="ServerConfig::getNetworkConfigValue">
                                        <arg>nameserver2</arg>
                                        <arg>4</arg>
                                    </dataProvider>
                                </setting>
                            </widget>
                        </td>
                    </tr>
                </table>
                <!-- Form Table Ends -->

                <!-- Progress message Starts -->
                <div id="savingMessage" class="NetworkConfiguration-savingMessage hidden">
                    <text>Please wait while the new network settings are applied</text>
                </div>
                <div id="savingProgress" class="NetworkConfiguration-savingProgress hidden">
                    <div class="NetworkConfiguration-loaderWrap">
                        <img src="/__web/Systems/ServerConfig/Web/Templates/black.gif" class="NetworkConfiguration-image" />
                    </div>
                </div>
                <!-- Progress message Ends -->

                <!-- Complete message Starts -->
                <div id="completeMessage-success" class="NetworkConfiguration-completeMessage success hidden">
                    <text>The new network settings were applied successfully</text>
                </div>
                <div id="completeMessage-failed" class="NetworkConfiguration-completeMessage failed hidden">
                    <text>The new network configuration has been failed. The old configuration has been recovered.</text>
                </div>
                <!-- Complete message Ends -->
            </div>
            <div class="NetworkConfiguration-bottom">
                <widget type="GUI/Button" id="confirm">
                    <setting name="value">Confirm</setting>
                    <setting name="colour">Dark</setting>
                </widget>
            </div>
        </div>
    </div>
    <dataProvider action="ServerConfig::getNetworkConfigurationInitCode">
        <arg></arg>
    </dataProvider>
    <dataProvider action="ServerConfig::printAPIToken">
        <arg></arg>
    </dataProvider>
</body>
</html>
