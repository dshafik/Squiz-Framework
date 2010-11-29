<var name="productTitle"><dataProvider action="SquizSuite::getProductTitle" /></var>
<var name="systemLogo"><dataProvider action="SquizSuite::getProductLogo" /></var>
<var name="projectSysName"><dataProvider action="SquizSuite::getProductSystemName" /></var>
<html>
    <head>
        <title>Select a project - {productTitle}</title>
    </head>
    <body>
        <widget type="GUI/Toolbar" id="toolbar">
            <setting name="logo">{systemLogo}</setting>
            <setting name="content">
                <content>
                    <condition>
                        <if condition="User::isSystemAdministrator()">
                            <content>
                                <widget type="GUI/ModeSwitcher" id="mode-switcher">
                                    <setting name="items">
                                        <value>
                                            <id>ProjectSelector</id>
                                            <label><text>Dashboard</text></label>
                                            <system>Project</system>
                                        </value>
                                        <value>
                                            <id>SystemConfig</id>
                                            <label><text>System Config</text></label>
                                            <system>{projectSysName}</system>
                                        </value>
                                    </setting>
                                    <setting name="initialItem">ProjectSelector</setting>
                                    <setting name="numberOfStaticButton">2</setting>
                                    <setting name="loadType">dynamic</setting>
                                    <setting name="target">mode-contents</setting>
                                </widget>
                            </content>
                        </if>
                    </condition>
                    <widget type="GUI/SaveButton" id="save" />
                    <widget type="GUI/Toolbar/Container" id="{projectSysName}Main-container-icons">
                        <setting name="content">
                            <content>
                                <widget type="User/LogoutButton" id="userLogoutButton" />
                                <widget type="Help/ToolbarButton" id="helpToolbarButton" />
                                <widget type="Project/SwitcherButton" id="projectSwitcherButton" />
                            </content>
                        </setting>
                    </widget>
                </content>
            </setting>
        </widget>
        <div id="mode-contents">
            <condition>
                <if condition="User::isSystemAdministrator()">
                    <dataProvider action="GUIModeSwitcher::getBodyContent">
                        <arg><widget-settings id="mode-switcher" /></arg>
                    </dataProvider>
                </if>
                <else>
                    <dataProvider action="GUI::printTemplate">
                        <arg>Project</arg>
                        <arg>ProjectSelector.tpl</arg>
                    </dataProvider>
                </else>
            </condition>
        </div>
    </body>
</html>
