<var name="ldapClassName"><content>LDAPUserManagerScreen</content></var>
<var name="className"><content>UserManagerScreen</content></var>
<var name="userInfo"><dataProvider action="LDAPConnection::getAssetInfoForTemplate" /></var>

<div id="{className}-editor" assetType="LDAPUser">
    <div class="{className}-editUser-top">
        <h2>{userInfo:first_name} {userInfo:last_name}</h2>
    </div>
    <div id="UserManagerScreen-editUser">
        <div class="{className}-col-left">
            <label for="UserManagerScreen-editUser-existingUser-firstName-input"><text>First Name</text></label>
            <br />{userInfo:first_name}
        </div>
        <div class="{className}-col-right">
            <label for="UserManagerScreen-editUser-existingUser-lastName-input"><text>Last Name</text></label>
            <br />{userInfo:last_name}
        </div>

        <div class="{className}-fieldGap"></div>
        <label for="UserManagerScreen-editUser-existingUser-email-input"><text>Email</text></label>
        <br />{userInfo:email}

        <div class="{className}-fieldGap"></div>
        <div>
            <label for="UserManagerScreen-editUser-existingUser-username-input"><text>Username</text></label>
            <br />{userInfo:username}
        </div>

        <div class="{className}-parentGroups">
            <h5><text>Belongs to Groups :</text></h5>
            <widget type="GUI/List" id="UserManagerScreen-parentsList">
                <setting name="items">
                    <dataProvider action="User::getUserGroupsData" />
                </setting>
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
