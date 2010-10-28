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
                                        <div class="PatchingScreen-activateWrap">
                                            <span><text>Demo System</text></span>
                                            <widget type="GUI/Button" id="PatchingScreen-activateDemoBtn">
                                                <setting name="value"><text>Activate Demo</text></setting>
                                                <setting name="click"><![CDATA[PatchingPatchingScreen.toggleActivation()]]></setting>
                                            </widget>
                                        </div>
                                        <div class="PatchingScreen-idTextFieldWrap hidden">
                                            <widget type="GUI/TextBox" id="PatchingScreen-activateIdText">
                                                <setting name="size">12</setting>
                                                <setting name="maxlength">50</setting>
                                            </widget>
                                            <widget type="GUI/Button" id="PatchingScreen-cancelActivationBtn">
                                                <setting name="value"><text>Cancel</text></setting>
                                                <setting name="colour">Dark</setting>
                                                <setting name="click"><![CDATA[PatchingPatchingScreen.toggleActivation()]]></setting>
                                            </widget>
                                        </div>
                                    </content>
                                </value>
                                <value>
                                    <warning>
                                        <text>Your system is currently running in demonstration mode. As a result all automatic updating features are disabled until you activate your system</text>
                                    </warning>
                                </value>
                            </items>
                        </systemid>
                    </if>
                    <!-- Settings box for a demo system. -->
                    <else>
                        <systemid>
                            <!-- System ID Section -->
                            <label><text>System ID</text></label>
                            <items>
                                <value>
                                    <content>
                                        <div class="PatchingScreen-activateWrap">
                                            <div class="PatchingScreen-activateWrap-left">
                                                <div id="PatchingScreen-systemidWrap" class="PatchingScreen-activateWrap">
                                                    <dataProvider action="SquizSuite::getSystemId" />
                                                </div>
                                                <div id="PatchingScreen-newActivationTextWrap" class="PatchingScreen-activateWrap hidden">
                                                    <widget type="GUI/TextBox" id="PatchingScreen-newActivationIdText">
                                                        <setting name="size">20</setting>
                                                        <setting name="maxlength">50</setting>
                                                        <setting name="inline">true</setting>
                                                    </widget>
                                                </div>
                                            </div>
                                            <div class="PatchingScreen-activateWrap-left">
                                                <div id="PatchingScreen-changeIdBtnWrap" class="PatchingScreen-activateWrap">
                                                    <widget type="GUI/Button" id="PatchingScreen-changeIdBtn">
                                                        <setting name="value"><text>Change ID</text></setting>
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.changeActivationID()]]></setting>
                                                    </widget>
                                                </div>
                                                <div id="PatchingScreen-cancel-changeIdBtnWrap" class="PatchingScreen-activateWrap hidden">
                                                    <widget type="GUI/Button" id="PatchingScreen-cancelChangeBtn">
                                                        <setting name="value"><text>Cancel</text></setting>
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.cancelNewActivationID()]]></setting>
                                                    </widget>
                                                </div>
                                            </div>
                                        </div>
                                    </content>
                                </value>
                            </items>
                        </systemid>
                        <!-- Next Update Check Section -->
                        <nextUpdateCheck>
                            <label><text>Next Update Check</text></label>
                            <items>
                                <value>
                                    <content>
                                        <div class="PatchingScreen-activateWrap">
                                            <span class="PatchingScreen-nextCheckString">
                                                <dataProvider action="Patching::getNextUpdateCheckTime" />
                                            </span>
                                            <widget type="GUI/Button" id="PatchingScreen-checkAsapBtn">
                                                <setting name="value"><text>Check ASAP</text></setting>
                                                <setting name="click"><![CDATA[PatchingPatchingScreen.checkUpdateASAP()]]></setting>
                                            </widget>
                                        </div>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <div class="PatchingScreen-activateWrap">
                                            <span><text>Last Updated</text></span>
                                            <span class="PatchingScreen-lastUpdatedString">
                                                <dataProvider action="Patching::getLastUpdatedTime" />
                                            </span>
                                        </div>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <div class="PatchingScreen-activateWrap">
                                            <span>Update Subscription</span>
                                            <span class="PatchingScreen-subscriptionInfo">
                                                <dataProvider action="Patching::getSubscriptionInfo" />
                                            </span>
                                        </div>
                                    </content>
                                </value>
                            </items>
                        </nextUpdateCheck>
                        <!-- Update Notification Section -->
                        <updateNotification>
                            <label><text>Update Notification</text></label>
                            <items>
                                <dataProvider action="Patching::getUpdateNotificationRows" />
                            </items>
                        </updateNotification>
                    </else>
                </condition>
            </setting>
        </widget>
    </div>
    <!-- New/Installed Updates table -->
    <div class="PatchingScreenMainContent GUI-screen-right">
        <widget type="GUI/TabPane" id="updatesBox-switcher">
            <setting name="tabs">
                <value>
                    <id>newUpdates</id>
                    <label><text>New Updates</text></label>
                    <itemContent>
                        <!--text>No updates are available</text-->
                        <widget type="GUI/Table" id="newUpdates-table">
                            <setting name="alternate">false</setting>
                            <setting name="allowDelete">false</setting>
                            <setting name="hideHeader">false</setting>
                            <setting name="columns">
                                <patchId>
                                    <name><text>Patch ID</text></name>
                                    <align>left</align>
                                </patchId>
                                <summary>
                                    <name><text>Summary</text></name>
                                    <align>left</align>
                                </summary>
                                <scheduledT>
                                    <name><text>Scheduled Date/Time</text></name>
                                    <align>left</align>
                                </scheduledT>
                            </setting>
                            <setting name="rows">
                                <dataProvider action="Patching::getNewUpdateRows" />
                            </setting>
                        </widget>
                    </itemContent>
                </value>
                <value>
                    <id>installedUpdates</id>
                    <label><text>Installed Updates</text></label>
                    <itemContent>
                        <!--text>No updates have been installed</text-->
                        <widget type="GUI/Table" id="installedUpdates-table">
                            <setting name="alternate">false</setting>
                            <setting name="allowDelete">false</setting>
                            <setting name="hideHeader">false</setting>
                            <setting name="columns">
                                <patchId>
                                    <name><text>Patch ID</text></name>
                                    <align>left</align>
                                </patchId>
                                <summary>
                                    <name><text>Summary</text></name>
                                    <align>left</align>
                                </summary>
                                <installedT>
                                    <name><text>Installation Date/User</text></name>
                                    <align>left</align>
                                </installedT>
                            </setting>
                            <setting name="rows">
                                <dataProvider action="Patching::getInstalledUpdateRows" />
                            </setting>
                        </widget>
                    </itemContent>
                </value>
            </setting>
        </widget>
    </div>
    <dataProvider action="Patching::getUpToDateInitCode">
        <arg></arg>
    </dataProvider>
</div>
