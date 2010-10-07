<condition>
    <if-not condition="User::isSystemAdministrator()">
        <exception><text>You must be a super user to access this screen</text></exception>
    </if-not>
</condition>

<var name="screenData"><dataProvider action="User::getUserManagerScreenData" /></var>

<div id="UserManagerScreen">
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

<script>
    /* Initialise the screen */
    UserUserManagerScreen.init({screenData});
</script>
