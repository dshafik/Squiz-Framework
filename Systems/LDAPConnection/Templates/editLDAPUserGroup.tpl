<var name="ldapClassName"><content>LDAPUserManagerScreen</content></var>
<var name="className"><content>UserManagerScreen</content></var>
<var name="groupInfo"><dataProvider action="LDAPConnection::getAssetInfoForTemplate" /></var>

<div id="{className}-editor" assetType="LDAPUserGroup">
    <div class="{className}-editUserGroup-top">
        <h2>{groupInfo:name}</h2>
    </div>
    <div id="{className}-createUser-wrapper">
        <div class="{className}-editGroupName">
            <h5><text>Group Name</text></h5>
            {groupInfo:name}
        </div>
        <div id="{className}-infoSection">
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
                <widget type="GUI/Button" id="UserManagerScreen-editUserGroup-addToGroup">
                    <setting name="value"><text>Add to another Group</text></setting>
                    <setting name="click">UserUserManagerScreen.addToGroups();</setting>
                </widget>
            </div>
        </div>
    </div>
</div>
