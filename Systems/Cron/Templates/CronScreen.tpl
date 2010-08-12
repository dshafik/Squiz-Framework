<div id="CronScreen">
    <text>Test</text>
    <div class="CronSettingsBoxContainer GUI-screen-left">
        <!-- Start Cront Screen settings box -->
        <widget type="GUI/ScreenSettings" id="cronSettings">
            <setting name="title">
                <text>Cron Setting</text>
            </setting>
            <setting name="subTitle">
                <content>
                    <text>
                        <value>Current time: %s</value>
                        <args>
                            <arg><dataProvider action="Cron::getCurrentDateTime" /></arg>
                        </args>
                    </text>
                </content>
            </setting>
            <setting name="sections">
                <frequent>
                    <label><text>Frequent</text></label>
                    <items>
                        <value>
                            <content>
                                <text>
                                    <value>Runs every %s minute(s)</value>
                                    <args>
                                        <arg>
                                            <widget type="GUI/TextBox" id="frequentInterval">
                                                <setting name="inline">true</setting>
                                                <setting name="size">2</setting>
                                                <setting name="value">
                                                    <dataProvider action="Cron::getCronInterval">
                                                        <arg>frequent</arg>
                                                    </dataProvider>
                                                </setting>
                                            </widget>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                        <value>
                            <content>
                                 <text>
                                    <value>
                                        <![CDATA[
                                            Last run at <strong>%s</strong>
                                        ]]>
                                    </value>
                                    <args>
                                        <arg>
                                            <dataProvider action="Cron::getLastRunTime">
                                                <arg>frequent</arg>
                                            </dataProvider>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                    </items>
                </frequent>
                <hourly>
                    <label><text>Hourly</text></label>
                    <items>
                        <value>
                            <content>
                                <text>
                                    <value>Runs every %s minute(s) past the hour</value>
                                    <args>
                                        <arg>
                                            <widget type="GUI/TextBox" id="hourlyInterval">
                                                <setting name="inline">true</setting>
                                                <setting name="size">2</setting>
                                                <setting name="value">
                                                    <dataProvider action="Cron::getCronInterval">
                                                        <arg>hourly</arg>
                                                    </dataProvider>
                                                </setting>
                                            </widget>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                        <value>
                            <content>
                                 <text>
                                    <value>Last run at %s</value>
                                    <args>
                                        <arg>
                                            <dataProvider action="Cron::getLastRunTime">
                                                <arg>hourly</arg>
                                            </dataProvider>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                    </items>
                </hourly>
                <daily>
                    <label><text>Daily</text></label>
                    <items>
                        <value>
                            <content>
                                <text>
                                    <value>Runs ever day at %s</value>
                                    <args>
                                        <arg>
                                            <widget type="GUI/TextBox" id="dailyInterval">
                                                <setting name="inline">true</setting>
                                                <setting name="size">2</setting>
                                                <setting name="value">
                                                    <dataProvider action="Cron::getCronInterval">
                                                        <arg>daily</arg>
                                                    </dataProvider>
                                                </setting>
                                            </widget>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                        <value>
                            <content>
                                 <text>
                                    <value>Last run at %s</value>
                                    <args>
                                        <arg>
                                            <dataProvider action="Cron::getLastRunTime">
                                                <arg>daily</arg>
                                            </dataProvider>
                                        </arg>
                                    </args>
                                </text>
                            </content>
                        </value>
                    </items>
                </daily>
            </setting>
        </widget>
        <!-- End Cront Screen settings box -->
    </div>
    <div class="CronScreenMainContent GUI-screen-right">
        <!-- Start Cront Screen Jobs List -->
        <widget type="GUI/Box" id="CronScreenMainBox">
            <setting name="title"><text>Cron Jobs</text></setting>
            <setting name="content">
                <widget type="GUI/Table" id="cronJobsList">
                    <setting name="showBorders">true</setting>
                    <setting name="selectable">false</setting>
                    <setting name="columns">
                        <job>
                            <align>left</align>
                            <name><text>Jobs</text></name>
                        </job>
                        <lastRun>
                            <align>left</align>
                            <name><text>Last Run</text></name>
                        </lastRun>
                        <frequency>
                            <align>center</align>
                            <name><text>Frequency</text></name>
                        </frequency>
                        <enabled>
                            <align>center</align>
                            <name><text>Enabled</text></name>
                        </enabled>
                    </setting>
                    <setting name="rows">
                        <dataProvider action="Cron::getCronScreenJobsList">
                            <arg>cronJobsList</arg>
                        </dataProvider>
                    </setting>
                </widget>
            </setting>
        </widget>
        <!-- End Cront Screen Jobs List -->
    </div>
</div>
