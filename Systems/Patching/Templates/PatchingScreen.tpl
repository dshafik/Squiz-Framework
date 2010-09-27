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
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.toggleActivation()]]></setting>
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
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.toggleActivation()]]></setting>
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
                            <!-- System ID Section -->
                            <label><text>System ID</text></label>
                            <items>
                                <value>
                                    <content>
                                        <text>
                                            <value>
                                                <content>
                                                    <text><![CDATA[
                                                        <div id="PatchingScreen-systemidWrap" class="PatchingScreen-activateWrap">%s</div>
                                                        <div id="PatchingScreen-newActivationTextWrap" class="PatchingScreen-activateWrap hidden">%s</div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg><dataProvider action="SquizSuite::getSystemId" /></arg>
                                                <arg>
                                                    <widget type="GUI/TextBox" id="PatchingScreen-newActivationIdText">
                                                        <setting name="size">30</setting>
                                                        <setting name="maxlength">50</setting>
                                                        <setting name="inline">true</setting>
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
                                                    <text><![CDATA[
                                                        <div id="PatchingScreen-changeIdBtnWrap" class="PatchingScreen-activateWrap">%s</div>
                                                        <div id="PatchingScreen-cancel-changeIdBtnWrap" class="PatchingScreen-activateWrap hidden">%s</div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg>
                                                    <widget type="GUI/Button" id="PatchingScreen-changeIdBtn">
                                                        <setting name="value"><text>Change ID</text></setting>
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.changeActivationID()]]></setting>
                                                    </widget>
                                                </arg>
                                                <arg>
                                                    <widget type="GUI/Button" id="PatchingScreen-cancelChangeBtn">
                                                        <setting name="value"><text>Cancel</text></setting>
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.cancelNewActivationID()]]></setting>
                                                    </widget>
                                                </arg>
                                            </args>
                                        </text>
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
                                        <text>
                                            <value>
                                                <content>
                                                    <text><![CDATA[
                                                        <div class="PatchingScreen-activateWrap"><span>%s</span>%s</div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg>
                                                    <dataProvider action="Patching::getNextUpdateCheckTime" />
                                                </arg>
                                                <arg>
                                                    <widget type="GUI/Button" id="PatchingScreen-checkAsapBtn">
                                                        <setting name="value"><text>Check ASAP</text></setting>
                                                        <setting name="click"><![CDATA[PatchingPatchingScreen.checkAsapBtn()]]></setting>
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
                                                    <text><![CDATA[
                                                        <div class="PatchingScreen-activateWrap">
                                                            <span>Last Updated</span>
                                                            <span class="PatchingScreen-lastUpdatedString">%s</span>
                                                        </div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg><dataProvider action="Patching::getLastUpdatedTime" /></arg>
                                            </args>
                                        </text>
                                    </content>
                                </value>
                                <value>
                                    <content>
                                        <text>
                                            <value>
                                                <content>
                                                    <text><![CDATA[
                                                        <div class="PatchingScreen-activateWrap">
                                                            <span>Update Subscription</span>
                                                            <span class="PatchingScreen-subscriptionInfo">%s</span>
                                                        </div>
                                                    ]]></text>
                                                </content>
                                            </value>
                                            <args>
                                                <arg><dataProvider action="Patching::getSubscriptionInfo" /></arg>
                                            </args>
                                        </text>
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
        <widget type="GUI/Box" id="updatesBox">
            <setting name="headerContent">
                <content>
                    <div style="float:left;">
                        <widget type="GUI/ContentSwitcher" id="updatesBox-switcher">
                            <setting name="items">
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
                            <setting name="loadType">static</setting>
                        </widget>
                    </div>
               </content>
           </setting>
           <setting name="content">
                <content>
                    <dataProvider action="GUIContentSwitcher::getBodyContent">
                        <arg><widget-settings id="updatesBox-switcher" /></arg>
                    </dataProvider>
                </content>
            </setting>
       </widget>
    </div>
    <dataProvider action="Patching::getUpToDateInitCode">
        <arg></arg>
    </dataProvider>
</div>
