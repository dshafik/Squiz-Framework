<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<var name="screenData"><dataProvider action="User::getUserManagerScreenData" /></var>

<div id="UserManagerScreen" class="GUI-screen-middle">
    <widget type="GUI/Box" id="userManager-box">
        <setting name="title"><text>User Manager</text></setting>
        <setting name="borderRight">false</setting>
        <setting name="content">
          <content>
            <widget id="userManager-assetBrowser" type="GUI/ColumnBrowser">
                <setting name="enableMultiSelect">false</setting>
                <setting name="dynamicLoading">true</setting>
                <setting name="dynamicDataProvider">
                    <system>User</system>
                    <method>getChildren</method>
                </setting>
                <setting name="items">
                    <dataProvider action="User::getChildren"></dataProvider>
                </setting>
            </widget>
          </content>
        </setting>
    </widget>
    <div id="userManager-editPane"></div>
</div>

<widget type="GUI/Intervention" id="UserManagerScreen-lastLinkIntervention-userGroup">
    <setting name="elementContents">
        <content>
            <p><text>This is the last remaining location for this group.
            Removing this link will remove the group and any users that are not linked into other groups from the system.</text></p>
            <widget type="GUI/Button" id="UserManagerScreen-lastLinkIntervention-ok">
                <setting name="value"><text>Remove</text></setting>
                <setting name="colour">Dark</setting>
                <setting name="click">GUI.getWidget('UserManagerScreen-lastLinkIntervention-userGroup').hide();</setting>
            </widget>
            <widget type="GUI/Button" id="UserManagerScreen-lastLinkIntervention-cancel">
                <setting name="value"><text>Cancel</text></setting>
                <setting name="colour">Dark</setting>
                <setting name="click">UserUserManagerScreen.cancelGroupDelete('UserManagerScreen-lastLinkIntervention-userGroup');</setting>
            </widget>
        </content>
    </setting>
</widget>
<widget type="GUI/Intervention" id="UserManagerScreen-lastLinkIntervention-user">
    <setting name="elementContents">
        <content>
            <p><text>This is the last remaining location for this user. Removing this link will remove them from the system. Are you sure you want to do this?</text></p>
            <widget type="GUI/Button" id="UserManagerScreen-lastLinkIntervention-ok">
                <setting name="value"><text>Remove</text></setting>
                <setting name="colour">Dark</setting>
                <setting name="click">GUI.getWidget('UserManagerScreen-lastLinkIntervention-user').hide();</setting>
            </widget>
            <widget type="GUI/Button" id="UserManagerScreen-lastLinkIntervention-cancel">
                <setting name="value"><text>Cancel</text></setting>
                <setting name="colour">Dark</setting>
                <setting name="click">UserUserManagerScreen.cancelGroupDelete('UserManagerScreen-lastLinkIntervention-user');</setting>
            </widget>
        </content>
    </setting>
</widget>

<script>
    /* Initialise the screen */
    UserUserManagerScreen.init({screenData});
</script>
