<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<div id="PatchingScreen">
    <div class="PatchingSettingsBoxContainer GUI-screen-left">
        <widget type="GUI/ScreenSettings" id="patchingSettings">
            <setting name="title"><text>System Information</text></setting>
            <setting name="subTitle">
                <content>
                    <text>
                        <value>Current Version: %s</value>
                        <args>
                            <arg><dataProvider action="Patching::getCurrentRevision" /></arg>
                        </args>
                    </text>
                </content>
            </setting>
            <setting name="sections">
                <condition>
                    <!-- Settings box for a demo system. -->
                    <if condition="SquizSuite::isDemoProduct()">
                        <systemid>
                            <label><text>System ID</text></label>
                            <items>
                                <value>
                                    <content>
                                        <text>
                                            <value>
                                                <content>
                                                    <text><![CDATA[
                                                        <div class="PatchingScreen-activateWrap"><span>Demo System</span>%s</div>
                                                        <div class="PatchingScreen-idTextFieldWrap hidden">%s %s</div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg>
                                                    <widget type="GUI/Button" id="PatchingScreen-activateDemoBtn">
                                                        <setting name="value"><text>Activate Demo</text></setting>
                                                        <setting name="click"><![CDATA[PatchingScreen.toggleActivation()]]></setting>
                                                    </widget>
                                                </arg>
                                                <arg>
                                                    <widget type="GUI/TextBox" id="PatchingScreen-activateIdText">
                                                        <setting name="size">12</setting>
                                                        <setting name="maxlength">50</setting>
                                                    </widget>
                                                </arg>
                                                <arg>
                                                    <widget type="GUI/Button" id="PatchingScreen-cancelActivationBtn">
                                                        <setting name="value"><text>Cancel</text></setting>
                                                        <setting name="colour">Dark</setting>
                                                        <setting name="click"><![CDATA[PatchingScreen.toggleActivation()]]></setting>
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
                                                <content>
                                                    <text><![CDATA[<div class="PatchingScreen-demoWarningWrap">
                                                        <span class="PatchingScreen-demoWarning-icon">&nbsp;</span>
                                                        <span class="PatchingScreen-demoWarning-text">%s</span>
                                                    </div>]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg>
                                                    <text>Your system is currently running in demonstration mode. As a result all automatic updating features are disabled until you activate your system</text>
                                                </arg>
                                            </args>
                                        </text>
                                    </content>
                                </value>
                            </items>
                        </systemid>
                    </if>
                    <!-- Settings box for a demo system. -->
                    <else>
                        <systemid>
                            <label><text>System ID</text></label>
                            <items>
                                <value>
                                    <content>
                                        <text>Row 1</text>
                                    </content>
                                </value>
                            </items>
                        </systemid>
                        <nextUpdateCheck>
                            <label><text>Next Update Check</text></label>
                            <items>
                                <value>
                                    <content>
                                        <text>Row 1</text>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <text>Row 2</text>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <text>Row 3</text>
                                    </content>
                                </value>
                            </items>
                        </nextUpdateCheck>
                        <updateNotification>
                            <label><text>Update Notification</text></label>
                            <items>
                                <value>
                                    <content>
                                        <text>Row 1</text>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <text>Row 2</text>
                                    </content>
                                </value>
                            </items>
                        </updateNotification>
                    </else>
                </condition>
            </setting>
        </widget>
    </div>
    <div class="PatchingScreenMainContent GUI-screen-right">
        <!-- Start Cront Screen Jobs List -->
        <widget type="GUI/Box" id="PatchingScreenMainBox">
            <setting name="title"><text>Cron Jobs</text></setting>
            <setting name="content">
                <div><text>hello world</text></div>
            </setting>
        </widget>
        <!-- End Cront Screen Jobs List -->
    </div>
</div>
