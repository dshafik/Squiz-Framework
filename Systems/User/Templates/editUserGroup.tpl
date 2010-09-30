<var name="className"><content>UserManagerScreen-editUserGroup</content></var>
<var name="groupInfo"><dataProvider action="User::getUserGroupInfoForTemplate" /></var>

<div id="{className}">
    <div class="{className}-top">
        <h2>{groupInfo:name}</h2>
    </div>
    <div class="{className}-editGroupName">
        <h5><text>Group Name</text></h5>
        <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-groupName">
            <setting name="value">{groupInfo:name}</setting>
        </widget>
    </div>
    <div id="UserManagerScreen-editUserGroup-groupInfo">
        <div class="{className}-childUserDetails">
            <div class="{className}-assetIcon"></div> : {groupInfo:childUserCount}
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addNewUser">
                <setting name="value"><text>+ New</text></setting>
                <setting name="click">UserUserManagerScreen.createNewUser();</setting>
            </widget>
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addExistingUser">
                <setting name="value"><text>+ Existing</text></setting>
                <setting name="click">UserUserManagerScreen.addExistingUser();</setting>
            </widget>
        </div>
        <div class="{className}-childGroupDetails">
            <div class="{className}-assetIcon"></div> : {groupInfo:childGroupCount}
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addNewGroup">
                <setting name="value"><text>+ New</text></setting>
            </widget>
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addExistingGroup">
                <setting name="value"><text>+ Existing</text></setting>
            </widget>
        </div>
        <div class="{className}-parentGroups">
            <h5><text>Belongs to Groups :</text></h5>
        </div>
        <div class="{className}-addToOtherGroup">
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addToGroup">
                <setting name="value"><text>Add to another Group</text></setting>
            </widget>
        </div>
    </div>
    <div id="UserManagerScreen-editUserGroup-createUser" style="display:none;">
        <div class="{className}-createUser-top">
            <div class="{className}-assetIcon"></div><text>Create New User</text>
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-cancelCreateUser">
                <setting name="value"><text>Cancel</text></setting>
                <setting name="colour">Light</setting>
                <setting name="click">UserUserManagerScreen.cancelCreateUser();</setting>
            </widget>
        </div>
        <div class="{className}-createUser-col-left">
            <label for="UserManagerScreen-editUserGroup-newUser-firstName-input"><text>First Name</text></label>
            <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-newUser-firstName">
                <setting name="size">17</setting>
            </widget>
        </div>
        <div class="{className}-createUser-col-right">
            <label for="UserManagerScreen-editUserGroup-newUser-lastName-input"><text>Last Name</text></label>
            <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-newUser-lastName">
                <setting name="size">17</setting>
            </widget>
        </div>
        <label for="UserManagerScreen-editUserGroup-newUser-email-input"><text>Email</text></label>
        <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-newUser-email">
            <setting name="size">38</setting>
        </widget>

        <label for="UserManagerScreen-editUserGroup-newUser-username-input"><text>Username</text></label>
        <widget type="GUI/TextBox" id="UserManagerScreen-editUserGroup-newUser-username" />

        <label for="UserManagerScreen-editUserGroup-newUser-password-input"><text>Password</text></label>
        <widget type="GUI/Password" id="UserManagerScreen-editUserGroup-newUser-password" />

        <div class="{className}-addToOtherGroup">
            <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-newUser-createButton">
                <setting name="value"><text>Create User</text></setting>
                <setting name="click">UserUserManagerScreen.createUser();</setting>
            </widget>
        </div>
    </div>
</div>
