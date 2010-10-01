<var name="className"><content>UserManagerScreen</content></var>
<var name="userInfo"><dataProvider action="User::getUserInfoForTemplate" /></var>
<var name="userStatus">
  <content>
    <condition>
        <if condition="{userInfo:status}">
            <text>Active</text>
        </if>
        <else>
            <text>Inactive</text>
        </else>
    </condition>
  </content>
</var>

<div id="{className}">
    <div class="{className}-editUser-top">
        <h2>{userInfo:first_name} {userInfo:last_name}</h2>
    </div>
    <div id="UserManagerScreen-editUser">
        <div class="{className}-col-left">
            <label for="UserManagerScreen-editUser-existingUser-firstName-input"><text>First Name</text></label>
            <widget type="GUI/TextBox" id="UserManagerScreen-editUser-existingUser-firstName">
                <setting name="size">17</setting>
                <setting name="value">{userInfo:first_name}</setting>
            </widget>
        </div>
        <div class="{className}-col-right">
            <label for="UserManagerScreen-editUser-existingUser-lastName-input"><text>Last Name</text></label>
            <widget type="GUI/TextBox" id="UserManagerScreen-editUser-existingUser-lastName">
                <setting name="size">17</setting>
                <setting name="value">{userInfo:last_name}</setting>
            </widget>
        </div>

        <div class="{className}-fieldGap"></div>
        <label for="UserManagerScreen-editUser-existingUser-email-input"><text>Email</text></label>
        <widget type="GUI/TextBox" id="UserManagerScreen-editUser-existingUser-email">
            <setting name="size">38</setting>
            <setting name="value">{userInfo:email}</setting>
        </widget>

        <div class="{className}-fieldGap"></div>
        <div class="{className}-status">
            <div class="{className}-status-header {userStatus}">
                <div class="{className}-status-header-icon"></div>
                {userStatus}
            </div>
            <div class="{className}-status-content">
                <span><text>Active Account</text></span>
                <widget type="GUI/ToggleButton" id="UserManagerScreen-editUser-toggleStatus">
                    <setting name="toggleAction">UserUserManagerScreen.toggleUserStatus();</setting>
                    <setting name="value">{userInfo:status}</setting>
                </widget>
            </div>
        </div>

        <div class="{className}-fieldGap"></div>
        <div>
            <label for="UserManagerScreen-editUser-existingUser-username-input"><text>Username</text></label>
        </div>
        <widget type="GUI/TextBox" id="UserManagerScreen-editUser-existingUser-username">
            <setting name="size">17</setting>
            <setting name="value">{userInfo:username}</setting>
        </widget>
        <widget type="GUI/Button" id="UserManagerScreen-editUser-resetPassword">
            <setting name="value"><text>Reset Password</text></setting>
            <setting name="colour">Light</setting>
            <setting name="click">UserUserManagerScreen.resetPassword();</setting>
        </widget>

        <div id="{className}-editExistingUser-resetPasswordBox" style="display:none;">
            <div class="{className}-fieldGap"></div>
            <label for="UserManagerScreen-editUser-existingUser-password-input"><text>Password</text></label>
            <widget type="GUI/Password" id="UserManagerScreen-editUser-existingUser-password" />
        </div>


        <div class="{className}-parentGroups">
            <h5><text>Belongs to Groups :</text></h5>
            <widget type="GUI/List" id="UserManagerScreen-parentsList">
                <setting name="items">
                    <dataProvider action="User::getUserGroupsData" />
                </setting>
                <setting name="sortable">true</setting>
                <setting name="allowDelete">true</setting>
                <setting name="itemsGenerator">
                    <system>User</system>
                    <action>getUserGroupData</action>
                </setting>
            </widget>
        </div>

        <div class="{className}-addToOtherGroup">
            <widget type="GUI/Button" id="UserManagerScreen-addToAnotherGroup">
                <setting name="value"><text>Add to another Group</text></setting>
                <setting name="click">UserUserManagerScreen.addToGroups();</setting>
            </widget>
        </div>
    </div>
</div>
